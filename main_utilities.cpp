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
#include "plugins/parameters/tConfigurablePlugin.h"
#include "plugins/scheduling/tExecutionControl.h"
#include "plugins/scheduling/scheduling.h"
#include "plugins/scheduling/tThreadContainerThread.h"

#ifdef _LIB_FINROC_PLUGINS_TCP_PRESENT_
#include "plugins/tcp/tPeer.h"
#endif

#include "rrlib/getopt/parser.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/tComponent.h"

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
#ifdef NDEBUG
bool enable_crash_handler = false;
#else
bool enable_crash_handler = true;
#endif

// We do not use stuff from rrlib_thread, because we have the rare case that in signal handler
// waiting thread does something else, which is problematic with respect to enforcing lock order
std::mutex main_thread_wait_mutex;
#ifndef RRLIB_SINGLE_THREADED
std::condition_variable main_thread_wait_variable;
#endif

#ifdef _LIB_FINROC_PLUGINS_TCP_PRESENT_
tcp::tPeer *tcp_peer;
#endif

/*!
 * Shuts down program cleanly by stopping all threads and deleting all framework elements.
 *
 * \return True if function is called for the first time
 */
bool Shutdown(const char* signal_name)
{
  static int call_count = 0; // How many time has function been called?
  call_count++;
  if (call_count == 1)
  {
    FINROC_LOG_PRINT_STATIC(USER, "\nCaught ", signal_name, ". Exiting...");
    run_main_loop = false;
    std::unique_lock<std::mutex> l(main_thread_wait_mutex);
#ifndef RRLIB_SINGLE_THREADED
    main_thread_wait_variable.notify_all();
#endif

    if (definitions::cSINGLE_THREADED && scheduling::tThreadContainerThread::CurrentThread())
    {
      scheduling::tThreadContainerThread::CurrentThread()->StopThread();
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------
// HandleSignalSIGINT
//----------------------------------------------------------------------
void HandleSignalSIGINT(int signal)
{
  static int call_count = 0; // How many time has function been called?
  assert(signal == SIGINT);
  call_count++;
  if ((!Shutdown("SIGINT")) && call_count < 5)
  {
    FINROC_LOG_PRINT_STATIC(USER, "\nCaught SIGINT again. Unfortunately, the program still has not terminated. Program will be aborted at fifth SIGINT.");
  }
  else if (call_count >= 5)
  {
    FINROC_LOG_PRINT_STATIC(USER, "\nCaught SIGINT for the fifth time. Aborting program.");
    abort();
  }
}

//----------------------------------------------------------------------
// HandleSignalSIGINT
//----------------------------------------------------------------------
void HandleSignalSIGTERM(int signal)
{
  assert(signal == SIGTERM);
  if (!Shutdown("SIGTERM"))
  {
    FINROC_LOG_PRINT_STATIC(USER, "\nCaught SIGTERM while shutting down. Aborting program.");
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
    if (!core::FinrocFileExists(file))
    {
      FINROC_LOG_PRINT_STATIC(ERROR, "Could not find specified config file ", file);
      return false;
    }
    else
    {
      FINROC_LOG_PRINT_STATIC(DEBUG, "Loading config file ", file);
    }
    parameters::tConfigurablePlugin::SetConfigFile(file);
    core::tRuntimeEnvironment::GetInstance().AddAnnotation(*new parameters::tConfigFile(file));
  }

  // pause
  rrlib::getopt::tOption pause(name_to_option_map.at("pause"));
  pause_at_startup = pause->IsActive();

  // port
  rrlib::getopt::tOption port_option(name_to_option_map.at("port"));
  if (port_option->IsActive())
  {
#ifdef _LIB_FINROC_PLUGINS_TCP_PRESENT_
    int port = atoi(rrlib::getopt::EvaluateValue(port_option).c_str());
    if (port < 1 || port > 65535)
    {
      FINROC_LOG_PRINT_STATIC(ERROR, "Invalid port '", port, "'. Using default: ", tcp::tOptions::GetDefaultOptions().preferred_server_port);
    }
    else
    {
      FINROC_LOG_PRINT_STATIC(DEBUG, "Listening on user defined port ", port, ".");
      tcp::tOptions::GetDefaultOptions().preferred_server_port = port;
    }
#endif
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
#ifdef _LIB_FINROC_PLUGINS_TCP_PRESENT_
    for (auto & address : rrlib::getopt::EvaluateValueList(connect_option))
    {
      tcp::tOptions::GetDefaultOptions().connect_to.push_back(address);
      FINROC_LOG_PRINT_STATIC(DEBUG, "Connecting to ", address);
    }
#endif
  }

  // listen-address
  rrlib::getopt::tOption listen_address_option(name_to_option_map.at("listen-address"));
  if (listen_address_option->IsActive())
  {
#ifdef _LIB_FINROC_PLUGINS_TCP_PRESENT_
    tcp::tOptions::GetDefaultOptions().server_listen_address = rrlib::getopt::EvaluateValue(listen_address_option);
    FINROC_LOG_PRINT_STATIC(DEBUG, "Listening on ", tcp::tOptions::GetDefaultOptions().server_listen_address);
#endif
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
      FINROC_LOG_PRINT_STATIC(ERROR, "Option --crash-handler needs be either 'on' or 'off' (not '", s, "').");
      return false;
    }
  }

  // profiling
  rrlib::getopt::tOption profiling(name_to_option_map.at("profiling"));
  if (profiling->IsActive())
  {
    scheduling::SetProfilingEnabled(true);
  }

  // component visualization
  rrlib::getopt::tOption disable_component_visualization(name_to_option_map.at("disable-component-visualization"));
  if (disable_component_visualization->IsActive())
  {
    tComponent::SetComponentVisualizationEnabled(false);
  }

  return true;
}

}

//----------------------------------------------------------------------
// InstallSignalHandler
//----------------------------------------------------------------------
bool InstallSignalHandler()
{
#if __linux__
  struct sigaction signal_action;
  signal_action.sa_handler = HandleSignalSIGINT;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = 0;

  if (sigaction(SIGINT, &signal_action, NULL) != 0)
  {
    perror("Could not install signal handler");
    return false;
  }

  signal_action.sa_handler = HandleSignalSIGTERM;
  if (sigaction(SIGTERM, &signal_action, NULL) != 0)
  {
    perror("Could not install signal handler");
    return false;
  }

#else
  signal(SIGINT, HandleSignalSIGINT);
  signal(SIGTERM, HandleSignalSIGTERM);
#endif

  return true;
}

//----------------------------------------------------------------------
// RegisterCommonOptions
//----------------------------------------------------------------------
void RegisterCommonOptions()
{
  rrlib::getopt::AddValue("log-config", 'l', "Log config file", &OptionsHandler, true);
  rrlib::getopt::AddValue("config-file", 'c', "Parameter config file", &OptionsHandler, true);
  rrlib::getopt::AddValue("listen-address", 0, "Address on which to listen for connections (default: 0.0.0.0), set this to :: to enable IPv6", &OptionsHandler);
  rrlib::getopt::AddValue("port", 'p', "Network port to use", &OptionsHandler, true);
  rrlib::getopt::AddValue("connect", 0, "TCP address of finroc application to connect to (default: localhost:<port>)", &OptionsHandler);
  rrlib::getopt::AddValue("crash-handler", 0, "Enable/disable crash handler (default: 'on' in debug mode - 'off' in release mode).", &OptionsHandler, true);
  rrlib::getopt::AddFlag("pause", 0, "Pause program at startup", &OptionsHandler);
  rrlib::getopt::AddFlag("port-links-are-not-unique", 0, "Port links in this part are not unique in P2P network (=> host name is prepended in GUI, for instance).", &OptionsHandler);
  rrlib::getopt::AddFlag("profiling", 0, "Enables profiling (creates additional ports with profiling information)", &OptionsHandler);
  rrlib::getopt::AddFlag("disable-component-visualization", 0, "Disables component visualization (no dedicated visualization ports will be created)", &OptionsHandler);
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
      FINROC_LOG_PRINT_STATIC(ERROR, "Error installing crash handler. Crashes will simply terminate the program.");
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
  tcp::tOptions::GetDefaultOptions().peer_name = peer_name;
  tcp_peer = new tcp::tPeer();
  tcp_peer->Init();
  try
  {
    tcp_peer->Connect();
  }
  catch (const std::exception& e1)
  {
    FINROC_LOG_PRINT_STATIC(WARNING, "Error connecting Peer: ", e1);
  }
#endif
}

//----------------------------------------------------------------------
// InitializeAndRunMainLoop
//----------------------------------------------------------------------
int InitializeAndRunMainLoop(const std::string &program_name)
{
  core::tRuntimeEnvironment &runtime_environment = core::tRuntimeEnvironment::GetInstance();

  typedef core::tFrameworkElement::tFlag tFlag;

  // Have any top-level framework elements containing threads already been created?
  // In this case, we won't create an extra thread container (finstructed part does not need one for example)
  std::vector<core::tFrameworkElement*> executables;
  for (auto it = runtime_environment.ChildrenBegin(); it != runtime_environment.ChildrenEnd(); ++it)
  {
    if (it->GetAnnotation<scheduling::tExecutionControl>() && (it->GetFlag(tFlag::FINSTRUCTABLE_GROUP) || it->GetFlag(tFlag::EDGE_AGGREGATOR)))
    {
      executables.push_back(&(*it));
    }
  }

  for (size_t i = 0; i < executables.size(); i++)
  {
    core::tFrameworkElement* fe = executables[i];
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
    core::tFrameworkElement* fe = executables[i];
    if (pause_at_startup)
    {
      scheduling::tExecutionControl::PauseAll(*fe); // Shouldn't be necessary, but who knows what people might implement
    }
    else
    {
      scheduling::tExecutionControl::StartAll(*fe);
      FINROC_LOG_PRINT_STATIC(USER, "Finroc program '", program_name, "' is now running.");
    }
  }

  run_main_loop = true;
  {
    if (definitions::cSINGLE_THREADED && scheduling::tThreadContainerThread::CurrentThread())
    {
      scheduling::tThreadContainerThread::CurrentThread()->Run();
    }
    else
    {
      std::unique_lock<std::mutex> l(main_thread_wait_mutex);
      while (run_main_loop)
      {
#ifndef RRLIB_SINGLE_THREADED
        main_thread_wait_variable.wait_for(l, std::chrono::seconds(10));
#else
        std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
      }
    }
  }
  FINROC_LOG_PRINT_STATIC(DEBUG, "Left main loop");

  // In many cases this is not necessary.
  // However, doing this before static deinitialization can avoid issues with external libraries and thread container threads still running.
  core::tRuntimeEnvironment::Shutdown();

  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
// End of declaration
//----------------------------------------------------------------------
}
}
