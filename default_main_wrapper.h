//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    plugins/structure/default_main_wrapper.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * Provides an optional main method for Finroc applications
 * with various command line parameters.
 * It is customizable and in fact used for most existing Finroc applications.
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__default_main_wrapper_h__
#define __plugins__structure__default_main_wrapper_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/getopt/parser.h"
#include "plugins/scheduling/tThreadContainerElement.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
extern const char * const cPROGRAM_VERSION;
extern const char * const cPROGRAM_DESCRIPTION;
extern int finroc_argc_copy;     // copy of argc for 'finroc' part. TODO: remove when rrlib_getopt supports prioritized evaluation of -m option
extern char ** finroc_argv_copy; // copy of argv for 'finroc' part. TODO: remove when rrlib_getopt supports prioritized evaluation of -m option
extern bool links_are_unique;

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------

void StartUp();
void InitMainGroup(finroc::scheduling::tThreadContainer *main_thread, std::vector<char*> remaining_args);


#endif
