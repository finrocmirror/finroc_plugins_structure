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
#include "plugins/structure/tTopLevelThreadContainer.h"

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

extern bool make_all_port_links_unique;

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------

void StartUp();
void CreateMainGroup(const std::vector<std::string> &remaining_arguments);


#endif
