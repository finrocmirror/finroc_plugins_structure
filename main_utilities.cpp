//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    plugins/structure/main_utilities.cpp
 *
 * \author  Tobias Foehst
 * \author  Max Reichardt
 * \author  Michael Arndt
 *
 * \date    2013-05-16
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/main_utilities.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <cstdlib>
#include <csignal>
#include <mutex>
#include <condition_variable>

extern "C"
{
#include <libgen.h>
}

#include "rrlib/logging/configuration.h"
#ifdef _LIB_RRLIB_CRASH_HANDLER_PRESENT_
#include "rrlib/crash_handler/crash_handler.h"
#endif

#include "core/file_lookup.h"
#include "core/tRuntimeEnvironment.h"
#include "plugins/parameters/tConfigFile.h"
#include "plugins/scheduling/tExecutionControl.h"

#ifdef _LIB_FINROC_PLUGINS_TCP_PRESENT_
#include "plugins/tcp/tPeer.h"
#endif

#include "rrlib/getopt/parser.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
extern bool make_all_port_links_unique;

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace structure
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

namespace
{

bool run_main_loop = false;
bool pause_at_startup = false;
std::string listen_address = "0.0.0.0";
int network_port = 4444;
std::string connect_to;
#ifdef NDEBUG
bool enable_crash_handler = false;
#else
bool enable_crash_handler = true;
#endif

// We do not use stuff from rrlib_thread, because we have the rare case that in signal handler
// waiting thread does something else, which is problematic with respect to enforcing lock order
std::mutex main_thread_wait_mutex;
std::condition_variable main_thread_wait_variable;

#ifdef _LIB_FINROC_PLUGINS_TCP_PRESENT_
finroc::tcp::tPeer *tcp_peer;
#endif

//----------------------------------------------------------------------
// HandleSignalSIGINT
//----------------------------------------------------------------------
void HandleSignalSIGINT(int signal)
{
  static int call_count = 0; // How many time has function been called?
  assert(signal == SIGINT);
  call_count++;
  if (call_count == 1)
  {
    FINROC_LOG_PRINT(USER, "\nCaught SIGINT. Exiting...");
    run_main_loop = false;
    std::unique_lock<std::mutex> l(main_thread_wait_mutex);
    main_thread_wait_variable.notify_all();
  }
  else if (call_count < 5)
  {
    FINROC_LOG_PRINT(USER, "\nCaught SIGINT again. Unfortunately, the program still has not terminated. Program will be aborted at fifth SIGINT.");
  }
  else
  {
    FINROC_LOG_PRINT(USER, "\nCaught SIGINT for the fifth time. Aborting program.");
    abort();
  }
}

//----------------------------------------------------------------------
// OptionsHandler
//----------------------------------------------------------------------
bool OptionsHandler(const rrlib::getopt::tNameToOptionMap &name_to_option_map)
{
  // log-config
  rrlib::getopt::tOption log_config(name_to_option_map.at("log-config"));
  if (log_config->IsActive())
  {
    rrlib::logging::ConfigureFromFile(rrlib::getopt::EvaluateValue(log_config));
  }

  // config-file
  rrlib::getopt::tOption parameter_config(name_to_option_map.at("config-file"));
  if (parameter_config->IsActive())
  {
    const std::string &file = rrlib::getopt::EvaluateValue(parameter_config);
    if (!finroc::core::FinrocFileExists(file))
    {
      FINROC_LOG_PRINT(ERROR, "Could not find specified config file ", file);
      return false;
    }
    else
    {
      FINROC_LOG_PRINT(DEBUG, "Loading config file ", file);
    }
    finroc::core::tRuntimeEnvironment::GetInstance().AddAnnotation(*new finroc::parameters::tConfigFile(file));
  }

  // pause
  rrlib::getopt::tOption pause(name_to_option_map.at("pause"));
  pause_at_startup = pause->IsActive();

  // port
  rrlib::getopt::tOption port_option(name_to_option_map.at("port"));
  if (port_option->IsActive())
  {
    int port = atoi(rrlib::getopt::EvaluateValue(port_option).c_str());
    if (port < 1 || port > 65535)
    {
      FINROC_LOG_PRINT(ERROR, "Invalid port '", port, "'. Using default: ", network_port);
    }
    else
    {
      FINROC_LOG_PRINT(DEBUG, "Listening on user defined port ", port, ".");
      network_port = port;
    }
  }

  // port-links-are-not-unique
  rrlib::getopt::tOption opt(name_to_option_map.at("port-links-are-not-unique"));
  if (opt->IsActive())
  {
    make_all_port_links_unique = false;
  }

  // connect
  rrlib::getopt::tOption connect_option(name_to_option_map.at("connect"));
  if (connect_option->IsActive())
  {
    connect_to = rrlib::getopt::EvaluateValue(connect_option);
    FINROC_LOG_PRINT(DEBUG, "Connecting to ", connect_to);
  }

  // listen-address
  rrlib::getopt::tOption listen_address_option(name_to_option_map.at("listen-address"));
  if (listen_address_option->IsActive())
  {
    listen_address = rrlib::getopt::EvaluateValue(listen_address_option);
    FINROC_LOG_PRINT(DEBUG, "Listening on ", listen_address);
  }

  // crash-handler
  rrlib::getopt::tOption crash_config(name_to_option_map.at("crash-handler"));
  if (crash_config->IsActive())
  {
    std::string s(rrlib::getopt::EvaluateValue(crash_config));
    if (s.compare("on") == 0)
    {
      enable_crash_handler = true;
    }
    else if (s.compare("off") == 0)
    {
      enable_crash_handler = false;
    }
    else
    {
      FINROC_LOG_PRINT(ERROR, "Option --crash-handler needs be either 'on' or 'off' (not '", s, "').");
      return false;
    }
  }

  return true;
}

}

//----------------------------------------------------------------------
// InstallSignalHandler
//----------------------------------------------------------------------
bool InstallSignalHandler()
{
  struct sigaction signal_action;
  signal_action.sa_handler = HandleSignalSIGINT;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = 0;

  if (sigaction(SIGINT, &signal_action, NULL) != 0)
  {
    perror("Could not install signal handler");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
// RegisterCommonOptions
//----------------------------------------------------------------------
void RegisterCommonOptions()
{
  rrlib::getopt::AddValue("log-config", 'l', "Log config file", &OptionsHandler);
  rrlib::getopt::AddValue("config-file", 'c', "Parameter config file", &OptionsHandler);
  rrlib::getopt::AddValue("listen-address", 0, "Address on which to listen for connections (default: 0.0.0.0), set this to :: to enable IPv6", &OptionsHandler);
  rrlib::getopt::AddValue("port", 'p', "Network port to use", &OptionsHandler);
  rrlib::getopt::AddValue("connect", 0, "TCP address of finroc application to connect to (default: localhost:<port>)", &OptionsHandler);
  rrlib::getopt::AddValue("crash-handler", 0, "Enable/disable crash handler (default: 'on' in debug mode - 'off' in release mode).", &OptionsHandler);
  rrlib::getopt::AddFlag("pause", 0, "Pause program at startup", &OptionsHandler);
  rrlib::getopt::AddFlag("port-links-are-not-unique", 0, "Port links in this part are not unique in P2P network (=> host name is prepended in GUI, for instance).", &OptionsHandler);
}

//----------------------------------------------------------------------
// InstallCrashHandler
//----------------------------------------------------------------------
void InstallCrashHandler()
{
#ifdef _LIB_RRLIB_CRASH_HANDLER_PRESENT_
  if (enable_crash_handler)
  {
    if (!rrlib::crash_handler::InstallCrashHandler())
    {
      FINROC_LOG_PRINT(ERROR, "Error installing crash handler. Crashes will simply terminate the program.");
    }
  }
#endif
}

//----------------------------------------------------------------------
// ConnectTCPPeer
//----------------------------------------------------------------------
void ConnectTCPPeer(const std::string &peer_name)
{
#ifdef _LIB_FINROC_PLUGINS_TCP_PRESENT_
  // Create and connect TCP peer
  tcp_peer = new finroc::tcp::tPeer(peer_name, connect_to, network_port, true, true, listen_address);
  tcp_peer->Init();
  try
  {
    tcp_peer->Connect();
  }
  catch (const std::exception& e1)
  {
    FINROC_LOG_PRINT(WARNING, "Error connecting Peer: ", e1);
  }
#endif
}

//----------------------------------------------------------------------
// InitializeAndRunMainLoop
//----------------------------------------------------------------------
int InitializeAndRunMainLoop(const std::string &program_name)
{
  finroc::core::tRuntimeEnvironment &runtime_environment = finroc::core::tRuntimeEnvironment::GetInstance();

  typedef finroc::core::tFrameworkElement::tFlag tFlag;

  // Have any top-level framework elements containing threads already been created?
  // In this case, we won't create an extra thread container (finstructed part does not need one for example)
  std::vector<finroc::core::tFrameworkElement*> executables;
  for (auto it = runtime_environment.ChildrenBegin(); it != runtime_environment.ChildrenEnd(); ++it)
  {
    if (it->GetAnnotation<finroc::scheduling::tExecutionControl>() && (it->GetFlag(tFlag::FINSTRUCTABLE_GROUP) || it->GetFlag(tFlag::EDGE_AGGREGATOR)))
    {
      executables.push_back(&(*it));
    }
  }

  for (size_t i = 0; i < executables.size(); i++)
  {
    finroc::core::tFrameworkElement* fe = executables[i];
    if (!fe->IsReady())
    {
      fe->Init();
    }
  }

#ifdef _LIB_FINROC_PLUGINS_TCP_PRESENT_
  if (tcp_peer->IsReady())
  {
    tcp_peer->StartServingStructure();
  }
#endif

  for (size_t i = 0; i < executables.size(); i++)
  {
    finroc::core::tFrameworkElement* fe = executables[i];
    if (pause_at_startup)
    {
      finroc::scheduling::tExecutionControl::PauseAll(*fe); // Shouldn't be necessary, but who knows what people might implement
    }
    else
    {
      finroc::scheduling::tExecutionControl::StartAll(*fe);
      FINROC_LOG_PRINT(USER, "Finroc program '", program_name, "' is now running.");
    }
  }

  run_main_loop = true;
  {
    std::unique_lock<std::mutex> l(main_thread_wait_mutex);
    while (run_main_loop)
    {
      main_thread_wait_variable.wait_for(l, std::chrono::seconds(10));
    }
  }
  FINROC_LOG_PRINT(DEBUG, "Left main loop");

  // In many cases this is not necessary.
  // However, doing this before static deinitialization can avoid issues with external libraries and thread container threads still running.
  finroc::core::tRuntimeEnvironment::Shutdown();

  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
// End of declaration
//----------------------------------------------------------------------
}
}
