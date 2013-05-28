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
/*!\file    plugins/structure/tGroupBase.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2013-05-19
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tGroupBase.h"

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

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

tGroupBase::tGroupBase(core::tFrameworkElement *parent, const std::string &name, const std::string &structure_config_file, tFlags extra_flags) :
  tFinstructableGroup(parent, name, structure_config_file, extra_flags),
  auto_name_port_count(0),
  count_for_type(NULL)
{
  internal::AddModule(this);
  if (!internal::FindParent(this, false))
  {
    FINROC_LOG_PRINT(ERROR, "Group ", GetQualifiedName(), " was not created using new().");
    abort();
  }
}

tGroupBase::~tGroupBase()
{}

core::tPortGroup* tGroupBase::CreateInterface(const std::string& name, bool share_ports, tFlags extra_flags)
{
  core::tPortGroup* created_group = new core::tPortGroup(this, name, tFlag::INTERFACE | extra_flags, share_ports ? tFlags(tFlag::SHARED) : tFlags());
  if (IsReady())
  {
    created_group->Init();
  }
  return created_group;
}

void* tGroupBase::operator new(size_t size)
{
  void* result = ::operator new(size);
  internal::AddMemoryBlock(result, size);
  return result;
}

void* tGroupBase::operator new[](size_t size)
{
  FINROC_LOG_PRINT_STATIC(ERROR, "Allocating (non-pointer) array of framework elements is not allowed.");
  throw std::bad_alloc();
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
