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
/*!\file    plugins/structure/tSenseControlGroup.cpp
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tSenseControlGroup.h"

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

static runtime_construction::tStandardCreateModuleAction<tSenseControlGroup> cCREATE_ACTION_FOR_SENSE_CONTROL_GROUP("SenseControlGroup");

const tComponent::tInterfaceInfo tSenseControlGroup::cSENSOR_INPUT_INTERFACE_INFO = { "Sensor Input", tFlag::PROXY_INTERFACE | tFlag::INTERFACE_FOR_DATA_PORTS | tFlag::SENSOR_DATA | tFlag::INTERFACE_FOR_INPUTS };
const tComponent::tInterfaceInfo tSenseControlGroup::cSENSOR_OUTPUT_INTERFACE_INFO = { "Sensor Output", tFlag::PROXY_INTERFACE | tFlag::INTERFACE_FOR_DATA_PORTS | tFlag::SENSOR_DATA | tFlag::INTERFACE_FOR_OUTPUTS };
const tComponent::tInterfaceInfo tSenseControlGroup::cCONTROLLER_INPUT_INTERFACE_INFO = { "Controller Input", tFlag::PROXY_INTERFACE | tFlag::INTERFACE_FOR_DATA_PORTS | tFlag::CONTROLLER_DATA | tFlag::INTERFACE_FOR_INPUTS };
const tComponent::tInterfaceInfo tSenseControlGroup::cCONTROLLER_OUTPUT_INTERFACE_INFO = { "Controller Output", tFlag::PROXY_INTERFACE | tFlag::INTERFACE_FOR_DATA_PORTS | tFlag::CONTROLLER_DATA | tFlag::INTERFACE_FOR_OUTPUTS };

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
tSenseControlGroup::tSenseControlGroup(tFrameworkElement *parent, const std::string &name,
                                       const std::string &structure_config_file,
                                       bool share_so_and_ci_ports, tFlags extra_flags) :
  tCompositeComponent(parent, name, structure_config_file, extra_flags, share_so_and_ci_ports)
{
#ifdef _LIB_FINROC_PLUGINS_RUNTIME_CONSTRUCTION_PRESENT_
  runtime_construction::tEditableInterfaces::AddInterface(GetControllerOutputs(), runtime_construction::tPortCreateOption::SHARED, true);
  runtime_construction::tEditableInterfaces::AddInterface(GetControllerInputs(), runtime_construction::tPortCreateOption::SHARED, true);
  runtime_construction::tEditableInterfaces::AddInterface(GetSensorOutputs(), runtime_construction::tPortCreateOption::SHARED, true);
  runtime_construction::tEditableInterfaces::AddInterface(GetSensorInputs(), runtime_construction::tPortCreateOption::SHARED, true);
#endif
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
