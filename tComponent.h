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
/*!\file    plugins/structure/tComponent.h
 *
 * \author  Max Reichardt
 *
 * \date    2014-03-19
 *
 * \brief   Contains tComponent
 *
 * \b tComponent
 *
 * Base class for all components (e.g. modules, groups)
 * Contains common functionality
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tComponent_h__
#define __plugins__structure__tComponent_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/port/tPortGroup.h"
#include "plugins/parameters/tConfigFile.h"
#include "plugins/parameters/tConfigNode.h"
#include "plugins/runtime_construction/tStandardCreateModuleAction.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/tConveniencePort.h"

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
// Class declaration
//----------------------------------------------------------------------
//! Component base class
/*!
 * Base class for all components (e.g. modules, groups)
 * Contains common functionality
 */
class tComponent : public core::tFrameworkElement
{
//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * \param parent Parent
   * \param name Name of module
   * \param structure_config_file XML
   * \param share_so_and_ci_ports Share sensor output and controller input ports so that they can be accessed from other runtime environments?
   * \param extra_flags Any extra flags for component
   */
  tComponent(core::tFrameworkElement *parent, const std::string &name, tFlags extra_flags = tFlags());

  /*!
   * Checks the module's static parameters for changes
   * and calls OnStaticParameterChange() if any static parameters changed.
   *
   * (Not thread-safe: Should not be called in parallel to an active thread already executing the module.)
   */
  void CheckStaticParameters();

  /*!
   * \return Config file for module
   */
  parameters::tConfigFile* GetConfigFile() const
  {
    return parameters::tConfigFile::Find(*this);
  }

  /*!
   * \param node Common parent config file node for all child parameter config entries (starting with '/' => absolute link - otherwise relative).
   */
  void SetConfigNode(const std::string& node)
  {
    parameters::tConfigNode::SetConfigNode(*this, node);
  }


  /*!
   * When storing convenience ports in std::unique pointers, this class can be used as deleter so that
   * actual wrapped port is also deleted.
   */
  class tPortDeleter
  {
  public:
    void operator()(core::tPortWrapperBase* ptr) const
    {
      ptr->GetWrapped()->ManagedDelete();
      delete ptr;
    }
  };

  // operator new is overloaded for auto-port naming feature
  void* operator new(size_t size);
  void* operator new[](size_t size); // not allowed

//----------------------------------------------------------------------
// Protected destructor (framework elements have their own memory management and are deleted with ManagedDelete)
//----------------------------------------------------------------------
protected:

  virtual ~tComponent();


  /*! GetContainer function for e.g. tParameter */
  tFrameworkElement& GetParameterParent();

  /*! GetContainer function for e.g. tStaticParameter */
  tFrameworkElement& GetThis()
  {
    return *this;
  }

  /*!
   * \return Has framework element "Parameters" for aggregating parameters has been created?
   */
  bool ParameterParentCreated()
  {
    return parameters;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  template <typename TPort, typename TElement, typename TContainer, TContainer& (TElement::*GET_CONTAINER)()>
  friend class tConveniencePort;

  /*! Element aggregating parameters */
  core::tFrameworkElement* parameters;

  /*! Number of ports already created that have auto-generated names */
  int auto_name_port_count;

  /*! Counter should be reset for every module class in type hierarchy. This helper variable is used to detect this. */
  const char* count_for_type;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
