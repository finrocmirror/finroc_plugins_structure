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
    parameters(new tFrameworkElement(this, "Parameters")),
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
  if (parameters_changed.parameters_changed)
  {
    this->ProcessChangedFlags(*parameters);
    parameters_changed.parameters_changed = false;
    this->EvaluateParameters();
  }
}

void tModuleBase::tParameterChangeDetector::PortChanged(data_ports::common::tAbstractDataPort& origin)
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
