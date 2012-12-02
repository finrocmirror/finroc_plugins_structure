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
/*!\file    plugins/structure/internal/register.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * Internal helper functions:
 * Can be used to determine which is the parent module or group of a port
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__internal__register_h__
#define __plugins__structure__internal__register_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace structure
{
namespace internal
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//! Contains information about auto-generated port name of a single module type
struct tModulePorts
{
  /*! demangled RTTI name of module type (without template arguments) */
  std::string name;

  /*! port name in module */
  std::vector<std::string> ports;
};

//! Info on a single instantiated module */
struct tInstantiatedModule
{
  /*! Pointer to beginning of memory block */
  char* address;

  /*! Size of memory block */
  size_t size;

  /*! Pointer to module (usually == address, but not always) */
  core::tFrameworkElement* module;
};

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------

/*!
 * Add memory block in which a tModuleBase or tGroup will be constructed
 * (should only be called by tModuleBase and tGroup)
 */
void AddMemoryBlock(void* address, size_t size);

/*!
 * Add Module to register
 * (should only be called by tModuleBase and tGroup)
 */
void AddModule(core::tFrameworkElement* module);

/*!
 * Add Module to register
 * (should only be called by tModuleBase and tGroup)
 */
void RemoveModule(core::tFrameworkElement* module);

/*!
 * Add port names for a module type
 * (typically called by auto-generated code)
 */
void AddPortNamesForModuleType(const std::string& name, const std::vector<std::string>& names);

/*!
 * \return Parent module/group of port class at address ptr
 */
core::tFrameworkElement* FindParent(void* ptr, bool abort_if_not_found = true);

/*!
 * Retrieves auto-generated port name
 *
 * \param parent Parent Module of port
 * \param port_index Index of port
 * \param data_type Type of port (for consistency check)
 * \return auto-generated port name
 */
std::string& GetAutoGeneratedPortName(core::tFrameworkElement* parent, size_t port_index);

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
