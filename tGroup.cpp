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
/*!\file    plugins/structure/tGroup.cpp
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tGroup.h"

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
tGroup::tGroup(tFrameworkElement *parent, const std::string &name, const std::string &structure_config_file, bool share_so_and_ci_ports)
  : tFinstructableGroup(parent, name, structure_config_file),
    controller_input(new core::tPortGroup(this, "Controller Input", tFlag::INTERFACE | tFlag::CONTROLLER_DATA, share_so_and_ci_ports ? tFlags(tFlag::SHARED) : tFlags())),
    controller_output(new core::tPortGroup(this, "Controller Output", tFlag::INTERFACE | tFlag::CONTROLLER_DATA, tFlags())),
    sensor_input(new core::tPortGroup(this, "Sensor Input", tFlag::INTERFACE | tFlag::SENSOR_DATA, tFlags())),
    sensor_output(new core::tPortGroup(this, "Sensor Output", tFlag::INTERFACE | tFlag::SENSOR_DATA, share_so_and_ci_ports ? tFlags(tFlag::SHARED) : tFlags())),
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

tGroup::~tGroup()
{
  internal::RemoveModule(this);
}

void* tGroup::operator new(size_t size)
{
  void* result = ::operator new(size);
  internal::AddMemoryBlock(result, size);
  return result;
}

void* tGroup::operator new[](size_t size)
{
  FINROC_LOG_PRINT_STATIC(ERROR, "Allocating (non-pointer) array of framework elements is not allowed.");
  throw std::bad_alloc();
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
