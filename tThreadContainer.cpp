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
/*!\file    plugins/structure/tThreadContainer.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-12-23
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tThreadContainer.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

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

static runtime_construction::tStandardCreateModuleAction<tThreadContainer<>> cCREATE_ACTION_1("ThreadContainer");
static runtime_construction::tStandardCreateModuleAction<tThreadContainer<tGroup>> cCREATE_ACTION_2("ThreadContainer<Group>");

// This create action is basically the same as the first, but preferred as it is more explicit (we have the first for backward-compatibility)
static runtime_construction::tStandardCreateModuleAction<tThreadContainer<tSenseControlGroup>> cCREATE_ACTION_3("ThreadContainer<SenseControlGroup>");

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}

template class scheduling::tThreadContainerElement<structure::tGroup>;
template class scheduling::tThreadContainerElement<structure::tSenseControlGroup>;

}
