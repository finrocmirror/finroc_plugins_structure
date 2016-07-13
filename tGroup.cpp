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
/*!\file    plugins/structure/tGroup.cpp
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tGroup.h"

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
typedef core::tFrameworkElement::tFlag tFlag;
typedef core::tFrameworkElement::tFlags tFlags;

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

static runtime_construction::tStandardCreateModuleAction<tGroup> cCREATE_ACTION_FOR_GROUP("Group");

const tComponent::tInterfaceInfo tGroup::cINPUT_INTERFACE_INFO = { "Input", tFlags(), tFlag::EMITS_DATA | tFlag::ACCEPTS_DATA | tFlag::PUSH_STRATEGY };
const tComponent::tInterfaceInfo tGroup::cOUTPUT_INTERFACE_INFO = { "Output", tFlags(), tFlag::EMITS_DATA | tFlag::ACCEPTS_DATA | tFlag::OUTPUT_PORT | tFlag::PUSH_STRATEGY };

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
tGroup::tGroup(tFrameworkElement *parent, const std::string &name,
               const std::string &structure_config_file,
               bool share_ports, tFlags extra_flags) :
  tCompositeComponent(parent, name, structure_config_file, extra_flags, share_ports)
{
#ifdef _LIB_FINROC_PLUGINS_RUNTIME_CONSTRUCTION_PRESENT_
  runtime_construction::tEditableInterfaces::AddInterface(GetOutputs(), runtime_construction::tPortCreateOption::SHARED, true);
  runtime_construction::tEditableInterfaces::AddInterface(GetInputs(), runtime_construction::tPortCreateOption::SHARED, true);
#endif
}


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
