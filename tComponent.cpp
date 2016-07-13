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
const tComponent::tInterfaceInfo tComponent::cPROFILING_INTERFACE_INFO = { "Profiling", core::tFrameworkElementFlags(), data_ports::cDEFAULT_OUTPUT_PORT_FLAGS };

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
    FINROC_LOG_PRINT(ERROR, "Component ", GetQualifiedName(), " was not created using new().");
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
  tInterface* result = new tInterface(this, interface_info.name, interface_info.extra_interface_flags | tFlag::INTERFACE, interface_info.default_port_flags | (shared_ports ? tFlag::SHARED : tFlag::PORT));
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
    throw std::runtime_error(interface->GetQualifiedName() + " is no interface");
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
    FINROC_LOG_PRINT_STATIC(ERROR, "Port '", port.GetWrapped()->GetQualifiedName(), "' has already been initialized. Ignoring SetVisualizationPort() call.");
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
