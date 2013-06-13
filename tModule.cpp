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
/*!\file    plugins/structure/tModule.cpp
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tModule.h"

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

tModule::tModule(tFrameworkElement *parent, const std::string &name, bool share_output_ports, bool share_input_ports)
  : tModuleBase(parent, name),

    input(NULL),
    output(NULL),
    share_output_ports(share_output_ports),
    share_input_ports(share_input_ports),
    update_task(*this),
    input_changed(true)
{
}

tModule::~tModule()
{}

void tModule::PostChildInit()
{
  CheckStaticParameters(); // evaluate static parameters before we create the task
  this->AddAnnotation(*new scheduling::tPeriodicFrameworkElementTask(this->input, this->output, this->update_task));
}

tModule::UpdateTask::UpdateTask(tModule& module)
  : module(module)
{}

void tModule::UpdateTask::ExecuteTask()
{
  this->module.CheckParameters();
  if (this->module.input)
  {
    this->module.input_changed = this->module.ProcessChangedFlags(*this->module.input);
  }
  this->module.Update();
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
