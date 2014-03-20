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
/*!\file    plugins/structure/tCompositeComponent.h
 *
 * \author  Max Reichardt
 *
 * \date    2013-05-19
 *
 * \brief   Contains tCompositeComponent
 *
 * \b tCompositeComponent
 *
 * Base class for different types of composite components (e.g. "groups")
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tCompositeComponent_h__
#define __plugins__structure__tCompositeComponent_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/string.h"
#include "core/tFrameworkElementTags.h"
#include "core/file_lookup.h"
#include "plugins/data_ports/tOutputPort.h"
#include "plugins/parameters/tParameter.h"
#include "plugins/parameters/tStaticParameter.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/tComponent.h"

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
//! Base class for composite components
/*!
 * Base class for different types of composite components (e.g. "groups")
 */
class tCompositeComponent : public tComponent
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
   * \param extra_flags Any extra flags for group
   */
  tCompositeComponent(core::tFrameworkElement *parent, const std::string &name, const std::string &structure_config_file = "",
                      tFlags extra_flags = tFlags());

  /**
   * Parameter class to use in composite component.
   *
   * Constructors take a variadic argument list... just any properties you want to assign to parameter.
   *
   * Unlike tPort, port name and parent are usually determined automatically (however, only possible when port is direct class member).
   * If this is not possible/desired, name needs to be provided as first constructor argument - parent as arbitrary one.
   *
   * A string as first parameter is interpreted as port name; Any other string as config entry
   * A framework element pointer is interpreted as parent.
   * tFrameworkElement::tFlags arguments are interpreted as flags.
   * A tQueueSettings argument creates an input queue with the specified settings (not to be used with parameters)
   * tBounds<T> are parameters's bounds.
   * tUnit argument is parameters's unit.
   * const T& is interpreted as parameters's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when T is a string type. There we have these rules:
   * A String not provided as first argument is interpreted as default value.
   * Any further string is interpreted as config entry.
   */
  template <typename T>
  class tParameter : public tConveniencePort<parameters::tParameter<T>, tComponent, tFrameworkElement, &tCompositeComponent::GetParameterParent>
  {
  public:
    template<typename ... ARGS>
    explicit tParameter(const ARGS&... args)
      : tConveniencePort<parameters::tParameter<T>, tComponent, tFrameworkElement, &tCompositeComponent::GetParameterParent>(args..., core::tFrameworkElement::tFlag::EMITS_DATA)
    {
      assert(this->GetWrapped()->GetParent()->NameEquals("Parameters"));
    }

    /*!
     * Attach this parameter to another one.
     * If this parameter is changed, the change is also propagated to the attached parameter.
     */
    void AttachTo(parameters::tParameter<T>& other)
    {
      this->GetWrapped()->ConnectTo(*other.GetWrapped());
    }
  };

  template <typename T>
  using tStaticParameter = tConveniencePort<parameters::tStaticParameter<T>, tComponent, core::tFrameworkElement, &tComponent::GetThis>;


  /*!
   * Contains name of XML file to use
   * Parameter only exists if no (fixed) XML file was provided via constructor
   */
  std::unique_ptr<tStaticParameter<std::string>> structure_config_file_parameter;

//----------------------------------------------------------------------
// Protected fields and methods
//----------------------------------------------------------------------
protected:

  /*! Local variable with current XML file to use - reference to this is passed to tFinstructable */
  std::string structure_config_file;

  /*!
   * Creates interface for this composite component
   *
   * \param name Name of interface
   * \param share_ports Should ports in this interfaces be shared? (so that they can be accessed from other runtime environments)
   * \param extra_flags Any extra flags to assign to interface
   * \param extra_flags Any extra flags to assign to all ports
   */
  core::tPortGroup* CreateInterface(const std::string& name, bool share_ports, tFlags extra_interface_flags = tFlags(), tFlags default_port_flags = tFlags());

  virtual void OnStaticParameterChange() override;
  virtual void PostChildInit() override;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
