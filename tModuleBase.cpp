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
/*!\file    plugins/structure/tModuleBase.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tModuleBase.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/tFrameworkElementTags.h"

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
static constexpr uint cMANDATORY_PORT_FLAGS_FOR_CHANGED_CHECK = (core::tFrameworkElementFlag::READY | core::tFrameworkElementFlag::PUSH_STRATEGY).Raw();

const tComponent::tInterfaceInfo tModuleBase::cVISUALIZATION_INTERFACE_INFO = { "Visualization", tFlag::INTERFACE_FOR_DATA_PORTS | tFlag::INTERFACE_FOR_OUTPUTS };
const tComponent::tInterfaceInfo tModuleBase::cSERVICES_INTERFACE_INFO = { "Services", tFlag::INTERFACE_FOR_RPC_PORTS };
const tComponent::tInterfaceInfo tModuleBase::cPARAMETERS_INTERFACE_INFO = { "Parameters", tFlag::INTERFACE_FOR_DATA_PORTS | tFlag::PARAMETER_INTERFACE | tFlag::INTERFACE_FOR_INPUTS };

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

tModuleBase::tModuleBase(tFrameworkElement *parent, const std::string &name, tFlags extra_flags, bool share_ports)
  : tComponent(parent, name, extra_flags | (share_ports ? tFlags(tFlag::SHARED) : tFlags())),
    parameters_interface(nullptr),
    parameters_changed()
{
  core::tFrameworkElementTags::AddTag(*this, "module");
}

void tModuleBase::CheckParameters()
{
  if (parameters_changed.parameters_changed && parameters_interface)
  {
    this->ProcessChangedFlags(*parameters_interface);
    parameters_changed.parameters_changed = false;
    this->OnParameterChange();
  }
}

void tModuleBase::tParameterChangeDetector::OnPortChange(data_ports::tChangeContext& change_context)
{
  parameters_changed = true;
}

bool tModuleBase::ProcessChangedFlags(core::tFrameworkElement& port_group)
{
  bool any_changed = false;
  for (auto it = port_group.ChildPortsBegin(); it != port_group.ChildPortsEnd(); ++it)
  {
    if ((it->GetAllFlags().Raw() & cMANDATORY_PORT_FLAGS_FOR_CHANGED_CHECK) == cMANDATORY_PORT_FLAGS_FOR_CHANGED_CHECK || (!this->IsReady()))
    {
      bool changed = static_cast<data_ports::common::tAbstractDataPort&>(*it).HasChanged();
      static_cast<data_ports::common::tAbstractDataPort&>(*it).ResetChanged();
      any_changed |= changed;
      static_cast<data_ports::common::tAbstractDataPort&>(*it).SetCustomChangedFlag(changed ?
          data_ports::tChangeStatus::CHANGED : data_ports::tChangeStatus::NO_CHANGE);
    }
  }
  return any_changed;
}

core::tFrameworkElement& tModuleBase::GetVisualizationParent()
{
  if (CreateComponentVisualizationPorts())
  {
    return GetInterface(cVISUALIZATION_INTERFACE_INFO, false);
  }
  return *this;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
