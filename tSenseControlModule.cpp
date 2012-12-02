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
/*!\file    plugins/structure/tSenseControlModule.cpp
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tSenseControlModule.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "plugins/scheduling/tPeriodicFrameworkElementTask.h"

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

tSenseControlModule::tSenseControlModule(tFrameworkElement *parent, const std::string &name, bool share_so_and_ci_ports)
  : tModuleBase(parent, name),
    sensor_input(new core::tPortGroup(this, "Sensor Input", tFlag::INTERFACE | tFlag::SENSOR_DATA, tFlags())),
    sensor_output(new core::tPortGroup(this, "Sensor Output", tFlag::INTERFACE | tFlag::SENSOR_DATA, share_so_and_ci_ports ? tFlags(tFlag::SHARED) : tFlags())),
    controller_input(new core::tPortGroup(this, "Controller Input", tFlag::INTERFACE | tFlag::CONTROLLER_DATA, share_so_and_ci_ports ? tFlags(tFlag::SHARED) : tFlags())),
    controller_output(new core::tPortGroup(this, "Controller Output", tFlag::INTERFACE | tFlag::CONTROLLER_DATA, tFlags())),
    sense_task(*this),
    control_task(*this),
    sensor_input_changed(true),
    controller_input_changed(true)
{
  controller_input->AddAnnotation(*new scheduling::tPeriodicFrameworkElementTask(*this->controller_input, *this->controller_output, this->control_task));
  sensor_input->AddAnnotation(*new scheduling::tPeriodicFrameworkElementTask(*this->sensor_input, *this->sensor_output, this->sense_task));
}

tSenseControlModule::~tSenseControlModule()
{}

//----------------------------------------------------------------------
// tSenseControlModule::ControlTask constructors
//----------------------------------------------------------------------
tSenseControlModule::ControlTask::ControlTask(tSenseControlModule& module)
  : module(module)
{}

//----------------------------------------------------------------------
// tSenseControlModule::ControlTask ExecuteTask
//----------------------------------------------------------------------
void tSenseControlModule::ControlTask::ExecuteTask()
{
  this->module.CheckParameters();
  this->module.controller_input_changed = this->module.ProcessChangedFlags(this->module.GetControllerInputs());
  this->module.Control();
}

//----------------------------------------------------------------------
// tSenseControlModule::SenseTask constructors
//----------------------------------------------------------------------
tSenseControlModule::SenseTask::SenseTask(tSenseControlModule& module)
  : module(module)
{}

//----------------------------------------------------------------------
// tSenseControlModule::ControlTask ExecuteTask
//----------------------------------------------------------------------
void tSenseControlModule::SenseTask::ExecuteTask()
{
  this->module.CheckParameters();
  this->module.sensor_input_changed = this->module.ProcessChangedFlags(this->module.GetSensorInputs());
  this->module.Sense();
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
