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
/*!\file    plugins/structure/tComponent.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2014-03-19
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tComponent.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tTraceableException.h"
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
const tComponent::tInterfaceInfo tComponent::cPROFILING_INTERFACE_INFO = { "Profiling", tFlag::INTERFACE_FOR_DATA_PORTS | tFlag::INTERFACE_FOR_OUTPUTS };

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
bool tComponent::create_component_visualization_ports = true;
core::tFrameworkElement::tFlag tComponent::cDO_NOT_CREATE_NOW = core::tFrameworkElement::tFlag::DELETED;

tComponent::tComponent(core::tFrameworkElement *parent, const std::string &name, tFlags extra_flags) :
  tFrameworkElement(parent, name, extra_flags),
  auto_name_port_count(0),
  count_for_type(nullptr)
{
  internal::AddModule(this);
  if (!internal::FindParent(this, false))
  {
    FINROC_LOG_PRINT(ERROR, "Component ", *this, " was not created using new().");
    abort();
  }
}

tComponent::~tComponent()
{
  internal::RemoveModule(this);
}

void tComponent::CheckStaticParameters()
{
  parameters::internal::tStaticParameterList::DoStaticParameterEvaluation(*this);
}

tInterface& tComponent::CreateInterface(const tInterfaceInfo& interface_info, bool shared_ports)
{
  tFlags port_flags = interface_info.default_port_flags | (shared_ports ? tFlag::SHARED : tFlag::PORT);
  if (interface_info.interface_flags.Get(tFlag::SENSOR_DATA))
  {
    port_flags |= tFlag::SENSOR_DATA;
  }
  if (interface_info.interface_flags.Get(tFlag::CONTROLLER_DATA))
  {
    port_flags |= tFlag::CONTROLLER_DATA;
  }
  bool input_interface = interface_info.interface_flags.Get(tFlag::INTERFACE_FOR_INPUTS) || interface_info.interface_flags.Get(tFlag::PARAMETER_INTERFACE);
  if (input_interface && interface_info.interface_flags.Get(tFlag::INTERFACE_FOR_OUTPUTS))
  {
    port_flags |= tFlag::PUSH_STRATEGY;
  }
  else if (input_interface)
  {
    port_flags |= data_ports::cDEFAULT_INPUT_PORT_FLAGS;
  }
  else if (interface_info.interface_flags.Get(tFlag::INTERFACE_FOR_OUTPUTS))
  {
    port_flags |= data_ports::cDEFAULT_OUTPUT_PORT_FLAGS;
  }
  if (interface_info.interface_flags.Get(tFlag::PROXY_INTERFACE))
  {
    port_flags |= tFlag::EMITS_DATA | tFlag::ACCEPTS_DATA | tFlag::PUSH_STRATEGY;
  }

  tInterface* result = new tInterface(this, interface_info.name, interface_info.interface_flags | tFlag::INTERFACE, port_flags);
  if (IsReady())
  {
    result->Init();
  }
  return *result;
}

tInterface& tComponent::GetInterface(const tInterfaceInfo& interface_info, bool shared_ports)
{
  core::tFrameworkElement* interface = this->GetChild(interface_info.name);
  if (!interface)
  {
    return CreateInterface(interface_info, shared_ports);
  }
  if (!interface->GetFlag(tFlag::INTERFACE))
  {
    std::stringstream stream;
    stream << (*interface) << " is no interface";
    throw std::runtime_error(stream.str());
  }
  return static_cast<tInterface&>(*interface);
}

void tComponent::SetComponentVisualizationEnabled(bool enabled)
{
  create_component_visualization_ports = enabled;
}

void tComponent::SetVisualizationPort(core::tPortWrapperBase port, tLevelOfDetail level_of_detail)
{
  // Tags for ports - depending on level of detail
  static const std::vector<std::string> cTAGS =
  {
    "visualization-low",
    "visualization-mid",
    "visualization-high",
    "visualization-all",
    "visualization-more",
    "visualization-less"
  };

  if (!port.GetWrapped())
  {
    throw rrlib::util::tTraceableException<std::runtime_error>("Port wrapper does not contain port");
  }
  else if (port.IsReady())
  {
    FINROC_LOG_PRINT_STATIC(ERROR, "Port '", port, "' has already been initialized. Ignoring SetVisualizationPort() call.");
    return;
  }

  if (create_component_visualization_ports)
  {
    core::tFrameworkElementTags::AddTag(*port.GetWrapped(), cTAGS[static_cast<int>(level_of_detail)]);
  }
}

void* tComponent::operator new(size_t size)
{
  void* result = ::operator new(size);
  internal::AddMemoryBlock(result, size);
  return result;
}

void* tComponent::operator new[](size_t size)
{
  FINROC_LOG_PRINT_STATIC(ERROR, "Allocating (non-pointer) array of framework elements is not allowed.");
  throw std::bad_alloc();
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
