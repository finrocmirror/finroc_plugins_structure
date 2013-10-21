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
#include "plugins/runtime_construction/tEditableInterfaces.h"

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

static runtime_construction::tStandardCreateModuleAction<tSenseControlGroup> cCREATE_ACTION_FOR_SENSE_CONTROL_GROUP("SenseControlGroup");

typedef runtime_construction::tEditableInterfaces::tStaticInterfaceInfo tStaticInterfaceInfo;
typedef core::tFrameworkElement::tFlag tFlag;

const std::vector<tStaticInterfaceInfo>& cSTATIC_INTERFACE_INFO_SENSE_CONTROL_GROUP =
{
  tStaticInterfaceInfo { "Sensor Input", tFlag::SENSOR_DATA, tFlag::EMITS_DATA | tFlag::ACCEPTS_DATA | tFlag::PUSH_STRATEGY, runtime_construction::tPortCreateOption::SHARED },
  tStaticInterfaceInfo { "Sensor Output", tFlag::SENSOR_DATA, tFlag::EMITS_DATA | tFlag::ACCEPTS_DATA | tFlag::OUTPUT_PORT | tFlag::PUSH_STRATEGY, runtime_construction::tPortCreateOption::SHARED },
  tStaticInterfaceInfo { "Controller Input", tFlag::CONTROLLER_DATA, tFlag::EMITS_DATA | tFlag::ACCEPTS_DATA | tFlag::PUSH_STRATEGY, runtime_construction::tPortCreateOption::SHARED },
  tStaticInterfaceInfo { "Controller Output", tFlag::CONTROLLER_DATA, tFlag::EMITS_DATA | tFlag::ACCEPTS_DATA | tFlag::OUTPUT_PORT | tFlag::PUSH_STRATEGY, runtime_construction::tPortCreateOption::SHARED }
};

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
tSenseControlGroup::tSenseControlGroup(tFrameworkElement *parent, const std::string &name,
                                       const std::string &structure_config_file,
                                       bool share_so_and_ci_ports, tFlags extra_flags) :
  tGroupBase(parent, name, structure_config_file, extra_flags)
{
  interface_array.fill(NULL);
  this->EmplaceAnnotation<runtime_construction::tEditableInterfaces>(cSTATIC_INTERFACE_INFO_SENSE_CONTROL_GROUP, interface_array.begin(), share_so_and_ci_ports ? 6 : 0); // 6 => bits 2 and 3 are set (Sensor Output and Controller Input)
}

core::tPortGroup& tSenseControlGroup::GetInterface(tInterfaceEnumeration desired_interface)
{
  if (!interface_array[desired_interface])
  {
    runtime_construction::tEditableInterfaces* editable_interfaces = this->GetAnnotation<runtime_construction::tEditableInterfaces>();
    editable_interfaces->CreateInterface(this, desired_interface, IsReady());
  }
  return *interface_array[desired_interface];
}

core::tPortGroup& tSenseControlGroup::GetInterface(const std::string& interface_name)
{
  for (size_t i = 0; i < cSTATIC_INTERFACE_INFO_SENSE_CONTROL_GROUP.size(); i++)
  {
    if (interface_name.compare(cSTATIC_INTERFACE_INFO_SENSE_CONTROL_GROUP[i].name) == 0)
    {
      return GetInterface(static_cast<tInterfaceEnumeration>(i));
    }
  }
  throw std::runtime_error("No interface with name '" + interface_name + "' is meant to be added to a group.");
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
