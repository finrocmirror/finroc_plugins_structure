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
    sensor_input(NULL),
    sensor_output(NULL),
    controller_input(NULL),
    controller_output(NULL),
    share_so_and_ci_ports(share_so_and_ci_ports),
    sense_task(*this),
    control_task(*this),
    sensor_input_changed(true),
    controller_input_changed(true)
{
  //controller_input->AddAnnotation(*new scheduling::tPeriodicFrameworkElementTask(*this->controller_input, *this->controller_output, this->control_task));
  //sensor_input->AddAnnotation(*new scheduling::tPeriodicFrameworkElementTask(*this->sensor_input, *this->sensor_output, this->sense_task));
}

tSenseControlModule::~tSenseControlModule()
{}

void tSenseControlModule::PostChildInit()
{
  CheckStaticParameters(); // evaluate static parameters before we create the tasks
  tFrameworkElement* controller_task_parent = controller_input ? controller_input : (controller_output ? controller_output : NULL);
  if (controller_task_parent)
  {
    data_ports::tOutputPort<rrlib::time::tDuration> execution_duration;
    if (scheduling::IsProfilingEnabled())
    {
      execution_duration = data_ports::tOutputPort<rrlib::time::tDuration>(&GetProfilingPortGroup(), "Control() Duration");
      execution_duration.Init();
    }
    controller_task_parent->AddAnnotation(*new scheduling::tPeriodicFrameworkElementTask(this->controller_input, this->controller_output, this->control_task, execution_duration));
  }
  else
  {
    FINROC_LOG_PRINT(WARNING, "Module has no controller interfaces. Control() will not be called!");
  }

  tFrameworkElement* sensor_task_parent = sensor_input ? sensor_input : (sensor_output ? sensor_output : NULL);
  if (sensor_task_parent)
  {
    data_ports::tOutputPort<rrlib::time::tDuration> execution_duration;
    if (scheduling::IsProfilingEnabled())
    {
      execution_duration = data_ports::tOutputPort<rrlib::time::tDuration>(&GetProfilingPortGroup(), "Sense() Duration");
      execution_duration.Init();
    }
    sensor_task_parent->AddAnnotation(*new scheduling::tPeriodicFrameworkElementTask(this->sensor_input, this->sensor_output, this->sense_task, execution_duration));
  }
  else
  {
    FINROC_LOG_PRINT(WARNING, "Module has no sensor interfaces. Sense() will not be called!");
  }
}

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
  if (this->module.controller_input)
  {
    this->module.controller_input_changed = this->module.ProcessChangedFlags(*this->module.controller_input);
  }
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
  if (this->module.sensor_input)
  {
    this->module.sensor_input_changed = this->module.ProcessChangedFlags(*this->module.sensor_input);
  }
  this->module.Sense();
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
