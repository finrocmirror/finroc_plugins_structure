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
 * \brief
 *
 * Provides an optional main function for Finroc applications
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
#include "plugins/structure/tThreadContainer.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
typedef finroc::core::tFrameworkElement::tFlags tFlags;

extern const std::string cPROGRAM_DESCRIPTION;
extern const std::string cCOMMAND_LINE_ARGUMENTS;
extern const std::string cADDITIONAL_HELP_TEXT;
extern const std::string cMAIN_THREAD_CONTAINER_NAME;

extern bool make_all_port_links_unique;

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------

void StartUp();
void InitMainGroup(finroc::structure::tThreadContainer *main_thread, const std::vector<std::string> &remaining_arguments);


#endif
