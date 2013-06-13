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

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

tModuleBase::tModuleBase(tFrameworkElement *parent, const std::string &name)
  : tFrameworkElement(parent, name),
    parameters(NULL),
    parameters_changed(),
    auto_name_port_count(0),
    count_for_type(NULL)
{
  internal::AddModule(this);
  if (!internal::FindParent(this, false))
  {
    FINROC_LOG_PRINT(ERROR, "Module ", GetQualifiedName(), " was not created using new().");
    abort();
  }
  core::tFrameworkElementTags::AddTag(*this, "module");
}

tModuleBase::~tModuleBase()
{
  internal::RemoveModule(this);
}

void tModuleBase::CheckParameters()
{
  if (parameters_changed.parameters_changed && parameters)
  {
    this->ProcessChangedFlags(*parameters);
    parameters_changed.parameters_changed = false;
    this->OnParameterChange();
  }
}

void tModuleBase::CheckStaticParameters()
{
  parameters::internal::tStaticParameterList::DoStaticParameterEvaluation(*this);
}

core::tFrameworkElement& tModuleBase::GetParameterParent()
{
  if (!parameters)
  {
    parameters = new tFrameworkElement(this, "Parameters");
  }
  return *parameters;
}


core::tPortGroup* tModuleBase::CreateInterface(const std::string& name, bool share_ports, tFlags extra_flags)
{
  if (IsReady())
  {
    std::string name_without_space = name;
    if (name.find(' ') != std::string::npos)
    {
      name_without_space.erase(name.find(' '), 1);
    }
    FINROC_LOG_PRINT(WARNING, "Interface ", name, " created after module has been initialized. Data dependencies via this interface will not be considered for scheduling. Call Get", name_without_space, "s() in constructor or OnStaticParameterChange() to avoid this.");
  }
  return new core::tPortGroup(this, name, tFlag::INTERFACE | extra_flags, share_ports ? tFlags(tFlag::SHARED) : tFlags());
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
    if (it->IsReady() || (!this->IsReady()))
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

void* tModuleBase::operator new(size_t size)
{
  void* result = ::operator new(size);
  internal::AddMemoryBlock(result, size);
  return result;
}

void* tModuleBase::operator new[](size_t size)
{
  assert(false && "Allocating (non-pointer) array of framework elements is not allowed.");
  throw std::bad_alloc();
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
