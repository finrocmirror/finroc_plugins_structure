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
/*!\file    plugins/structure/tCompositeComponent.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2013-05-19
 *
 */
//----------------------------------------------------------------------
#include "plugins/structure/tCompositeComponent.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#ifdef _LIB_FINROC_PLUGINS_RUNTIME_CONSTRUCTION_PRESENT_
#include "plugins/runtime_construction/tFinstructable.h"
#endif

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
/*! We do not want to have this prefix in XML file names, as this will not be found when a system installation is used */
static const char* cUNWANTED_XML_FILE_PREFIX = "sources/cpp/";

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

tCompositeComponent::tCompositeComponent(core::tFrameworkElement *parent, const std::string &name, const std::string &structure_config_file, tFlags extra_flags) :
  tComponent(parent, name, extra_flags | tFlag::FINSTRUCTABLE_GROUP),
  structure_config_file_parameter(structure_config_file.length() > 0 ? NULL : new tStaticParameter<std::string>("XML file", this, "")),
  structure_config_file(rrlib::util::StartsWith(structure_config_file, cUNWANTED_XML_FILE_PREFIX) ? structure_config_file.substr(strlen(cUNWANTED_XML_FILE_PREFIX)) : structure_config_file)
{
  core::tFrameworkElementTags::AddTag(*this, "group");
  if (structure_config_file.length() > 0) // Fixed name? => we can help finstruct by adding annotation
  {
    core::tFrameworkElementTags::AddTag(*this, "finstructable structure file: " + this->structure_config_file);
  }

#ifdef _LIB_FINROC_PLUGINS_RUNTIME_CONSTRUCTION_PRESENT_
  // Create annotation to make composite component finstructable
  this->EmplaceAnnotation<runtime_construction::tFinstructable>(this->structure_config_file);
#endif
}

core::tPortGroup* tCompositeComponent::CreateInterface(const std::string& name, bool share_ports, tFlags extra_flags, tFlags default_port_flags)
{
  core::tPortGroup* created_group = new core::tPortGroup(this, name, tFlag::INTERFACE | extra_flags, default_port_flags | (share_ports ? tFlags(tFlag::SHARED) : tFlags()));
  if (IsReady())
  {
    created_group->Init();
  }
  return created_group;
}

void tCompositeComponent::OnStaticParameterChange()
{
  if (structure_config_file_parameter && structure_config_file_parameter->HasChanged())
  {
    this->structure_config_file = structure_config_file_parameter->Get();
    if (this->structure_config_file.length() > 0)
    {
      //if (this.childCount() == 0) { // TODO: original intension: changing xml files to mutliple existing ones in finstruct shouldn't load all of them
#ifdef _LIB_FINROC_PLUGINS_RUNTIME_CONSTRUCTION_PRESENT_
      if (core::FinrocFileExists(this->structure_config_file))
      {
        this->GetAnnotation<runtime_construction::tFinstructable>()->LoadXml();
      }
      else
      {
        FINROC_LOG_PRINT(DEBUG, "Cannot find XML file ", this->structure_config_file, ". Creating empty group. You may edit and save this group using finstruct.");
      }
#endif
    }
  }
}

void tCompositeComponent::PostChildInit()
{
  if (!structure_config_file_parameter)
  {
    if (this->structure_config_file.length() > 0)
    {
#ifdef _LIB_FINROC_PLUGINS_RUNTIME_CONSTRUCTION_PRESENT_
      if (core::FinrocFileExists(this->structure_config_file))
      {
        this->GetAnnotation<runtime_construction::tFinstructable>()->LoadXml();
      }
#endif
    }
  }
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
