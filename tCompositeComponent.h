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
#ifdef _LIB_FINROC_PLUGINS_RPC_PORTS_PRESENT_
#include "plugins/rpc_ports/tClientPort.h"
#include "plugins/rpc_ports/tProxyPort.h"
#include "plugins/rpc_ports/tServerPort.h"
#endif
#ifdef _LIB_FINROC_PLUGINS_RUNTIME_CONSTRUCTION_PRESENT_
#include "plugins/runtime_construction/tEditableInterfaces.h"
#endif

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

  template <typename T>
  using tStaticParameter = tConveniencePort<parameters::tStaticParameter<T>, tComponent, core::tFrameworkElement, &tComponent::GetThis>;

  /*!
   * Whether this composite component stores and loads configuration of parameters below (config file links in particular)
   * (For parameters that are below another sub-composite-component that manages parameter configuration, this component is responsible for their configuration)
   */
  std::unique_ptr<tStaticParameter<bool>> par_manages_parameter_configuration;

  /*!
   * Contains name of XML file to use
   * Parameter only exists if no (fixed) XML file was provided via constructor
   */
  std::unique_ptr<tStaticParameter<std::string>> structure_config_file_parameter;


  /*!
   * \param parent Parent
   * \param name Name of component
   * \param structure_config_file Structure config XML file
   * \param extra_flags Any extra flags for component
   * \param share_ports Share ports in interfaces so that they can be connected to remote components?
   */
  tCompositeComponent(core::tFrameworkElement *parent, const std::string &name, const std::string &structure_config_file = "",
                      tFlags extra_flags = tFlags(), bool share_ports = false);

  /*!
   * \return Returns 'Parameters' interface of this component
   */
  inline tInterface& GetParameters()
  {
    return GetInterface(cPARAMETERS_INTERFACE_INFO, false);
  }

  /*!
   * \return Returns 'Services' interface of this component
   */
  inline tInterface& GetServices()
  {
    return GetInterface(cSERVICES_INTERFACE_INFO, GetFlag(tFlag::SHARED));
  }

  /**
   * RPC port classes to use in composite components.
   * Usually, RPC ports in composite components simply forward RPC ports of inner components.
   * If, however, a composite component shall process RPCs directly, the Tend_point template parameter has to be set to true.
   *
   * Constructors take a variadic argument list... just any properties you want to assign to port/parameter.
   *
   * Unlike tPort, port name and parent are usually determined automatically (however, only possible when port is direct class member).
   * If this is not possible/desired, name needs to be provided as first constructor argument - parent as arbitrary one.
   *
   * A string is interpreted as port name.
   * A framework element pointer is interpreted as parent.
   * tFrameworkElement::tFlags arguments are interpreted as flags.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   */
#ifdef _LIB_FINROC_PLUGINS_RPC_PORTS_PRESENT_
  template <typename T, bool Tend_point = false>
  using tServer = tConveniencePort<typename std::conditional<Tend_point, rpc_ports::tServerPort<T>, rpc_ports::tProxyPort<T, true>>::type, tCompositeComponent, tInterface, &tCompositeComponent::GetServices>;

  template <typename T, bool Tend_point = false>
  using tClient = tConveniencePort<typename std::conditional<Tend_point, rpc_ports::tClientPort<T>, rpc_ports::tProxyPort<T, false>>::type, tCompositeComponent, tInterface, &tCompositeComponent::GetServices>;
#endif

  /**
   * Parameter classes to use in composite components.
   *
   * Constructors take a variadic argument list... just any properties you want to assign to port/parameter.
   *
   * Parameter name and parent are usually determined automatically (however, only possible when parameter is direct class member).
   * If this is not possible/desired, name needs to be provided as first constructor argument - parent as arbitrary one.
   *
   * A string as first parameter is interpreted as parameter name; Any other string as config entry
   * A framework element pointer is interpreted as parent.
   * tFrameworkElement::tFlags arguments are interpreted as flags.
   * tBounds<T> are parameters' bounds.
   * const T& is interpreted as parameters's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when T is a string type. There we have these rules:
   * A String not provided as first argument is interpreted as default value.
   * Any further string is interpreted as config entry.
   */
  template <typename T>
  class tParameter : public tConveniencePort<parameters::tParameter<T>, tCompositeComponent, tInterface, &tCompositeComponent::GetParameters>
  {
  public:
    template<typename ... ARGS>
    explicit tParameter(const ARGS&... args)
      : tConveniencePort<parameters::tParameter<T>, tCompositeComponent, tInterface, &tCompositeComponent::GetParameters>(args..., core::tFrameworkElement::tFlag::EMITS_DATA)
    {
      assert(this->GetWrapped()->GetParent()->GetName() == "Parameters");
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

  /*! Static interface info on common interfaces of composite components */
  static const tInterfaceInfo cVISUALIZATION_INTERFACE_INFO, cSERVICES_INTERFACE_INFO, cPARAMETERS_INTERFACE_INFO;

//----------------------------------------------------------------------
// Protected fields and methods
//----------------------------------------------------------------------
protected:

  virtual core::tFrameworkElement& GetVisualizationParent() override;
  virtual void OnStaticParameterChange() override;
  virtual void OnInitialization() override;

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Local variable with current XML file to use - reference to this is passed to tFinstructable */
  std::string structure_config_file;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
