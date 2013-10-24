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
/*!\file    plugins/structure/internal/register.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/internal/register.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"

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
namespace internal
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

struct tStructureElementStorage
{
  rrlib::thread::tMutex mutex;
  std::vector<tInstantiatedModule> reg;
  std::vector<tModulePorts> module_type_reg;
};
static inline unsigned int GetLongevity(internal::tStructureElementStorage*)
{
  return 20; // can be deleted after runtime environment
}

typedef rrlib::design_patterns::tSingletonHolder<tStructureElementStorage, rrlib::design_patterns::singleton::Longevity> tStructureElementStorageInstance;

static rrlib::thread::tMutex& GetMutex()
{
  return tStructureElementStorageInstance::Instance().mutex;
}

static std::vector<tInstantiatedModule>& GetRegister()
{
  return tStructureElementStorageInstance::Instance().reg;
}

static std::vector<tModulePorts>& GetModuleTypeRegister()
{
  return tStructureElementStorageInstance::Instance().module_type_reg;
}

void AddMemoryBlock(void* address, size_t size)
{
  rrlib::thread::tLock lock(GetMutex());
  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Adding memory block at ", address, " with size ", size);
  tInstantiatedModule m = {(char*)address, size, NULL };
  GetRegister().push_back(m);
}

void AddModule(core::tFrameworkElement* module)
{
  rrlib::thread::tLock lock(GetMutex());
  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Adding module ", module, "...");
  std::vector<tInstantiatedModule>& reg = GetRegister();
  void* ptr = module;
  for (int i = reg.size() - 1; i >= 0; i--) // reverse direction, because relevant module usually is at the end (the most recently added)
  {
    if (ptr >= reg[i].address && ptr < reg[i].address + reg[i].size)
    {
      assert(reg[i].module == NULL);
      FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Module resides in memory block ", reg[i].address);
      reg[i].module = module;
    }
  }
}

void RemoveModule(core::tFrameworkElement* module)
{
  rrlib::thread::tLock lock(GetMutex());
  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Removing module ", module);
  std::vector<tInstantiatedModule>& reg = GetRegister();
  for (int i = reg.size() - 1; i >= 0; i--) // reverse direction, because relevant module is possibly rather at the end
  {
    if (module == reg[i].module)
    {
      reg.erase(reg.begin() + i);
      return;
    }
  }
}

core::tFrameworkElement* FindParent(void* ptr, bool abort_if_not_found)
{
  rrlib::thread::tLock lock(GetMutex());
  std::vector<tInstantiatedModule>& reg = GetRegister();
  for (int i = reg.size() - 1; i >= 0; i--) // reverse direction, because relevant module usually is at the end (the most recently added)
  {
    if (ptr >= reg[i].address && ptr < reg[i].address + reg[i].size)
    {
      assert(reg[i].module != NULL);
      return reg[i].module;
    }
  }
  if (abort_if_not_found)
  {
    FINROC_LOG_PRINT(ERROR, "Could not find parent for port (or parameter). Please provide port name as first and parent as second constructor parameter for all ports that are not plain module/group class members (e.g. pushed in a std::vector).");
    abort();
  }
  return NULL;
}

std::string& GetAutoGeneratedPortName(core::tFrameworkElement* parent, size_t port_index)
{
  static std::string unresolved("(unresolved port name)");

  rrlib::thread::tLock lock(GetMutex());
  std::vector<tModulePorts>& reg = GetModuleTypeRegister();

  // normalize type String (remove any template arguments)
  std::string s(rrlib::rtti::Demangle(typeid(*parent).name()));
  if (s.find('<') != std::string::npos)
  {
    s = s.substr(0, s.find('<'));
  }

  for (size_t i = 0; i < reg.size(); i++)
  {
    if (s.compare(reg[i].name) == 0) // Equals?
    {
      if (port_index < reg[i].ports.size())
      {
        return reg[i].ports[port_index];
      }
      else
      {
        break;
      }
    }
  }

  FINROC_LOG_PRINT_STATIC(WARNING, "Cannot resolve port name for module type ", s, " index ", port_index, ". Automatic port names are only available for a module's plain member variables. For other ports, the name needs to be explicitly specified when calling the constructor. If this is a template module, it is possibly not included in the 'make.xml'.");
  return unresolved;
}

/*!
 * Add port names for a module type
 * (typically called by auto-generated code)
 */
void AddPortNamesForModuleType(const std::string& name, const std::vector<std::string>& names)
{
  rrlib::thread::tLock lock(GetMutex());
  std::vector<tModulePorts>& reg = GetModuleTypeRegister();

  tModulePorts mp;
  mp.name = name;
  mp.ports = names;
  reg.push_back(mp);
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
