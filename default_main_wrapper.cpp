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
/*!\file    plugins/structure/default_main_wrapper.cpp
 *
 * \author  Tobias Foehst
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/default_main_wrapper.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <cstdlib>

#include "core/tRuntimeEnvironment.h"

#include "rrlib/logging/configuration.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/main_utilities.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
typedef finroc::core::tFrameworkElement::tFlag tFlag;
typedef finroc::core::tFrameworkElement::tFlags tFlags;

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------
int main(int argc, char **argv)
{
  if (!finroc::structure::InstallSignalHandler())
  {
    FINROC_LOG_PRINT(ERROR, "Error installing signal handler. Exiting...");
    return EXIT_FAILURE;
  }

  rrlib::logging::default_log_description = basename(argv[0]);
  rrlib::logging::SetLogFilenamePrefix(basename(argv[0]));

  finroc::structure::RegisterCommonOptions();

  StartUp();

  std::vector<std::string> remaining_arguments = rrlib::getopt::ProcessCommandLine(argc, argv, cPROGRAM_DESCRIPTION, cCOMMAND_LINE_ARGUMENTS, cADDITIONAL_HELP_TEXT);

  finroc::structure::InstallCrashHandler();
  finroc::structure::ConnectTCPPeer(basename(argv[0]));

  finroc::structure::tThreadContainer *main_thread = new finroc::structure::tThreadContainer(
    &finroc::core::tRuntimeEnvironment::GetInstance(), cMAIN_THREAD_CONTAINER_NAME, "", true, make_all_port_links_unique ? tFlags(tFlag::GLOBALLY_UNIQUE_LINK) : tFlags()); // TODO: Put part xml here?

  InitMainGroup(main_thread, remaining_arguments);

  return finroc::structure::InitializeAndRunMainLoop(basename(argv[0]));
}
