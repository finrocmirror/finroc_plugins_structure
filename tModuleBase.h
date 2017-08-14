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
/*!\file    plugins/structure/tModuleBase.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * \brief   Contains tModuleBase
 *
 * \b tModuleBase
 *
 * Base class for different types of modules
 * with common functionality.
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tModuleBase_h__
#define __plugins__structure__tModuleBase_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/thread/tTask.h"
#include "plugins/data_ports/tOutputPort.h"
#include "plugins/parameters/tParameter.h"
#include "plugins/parameters/tStaticParameter.h"
#ifdef _LIB_FINROC_PLUGINS_RPC_PORTS_PRESENT_
#include "plugins/rpc_ports/tClientPort.h"
#include "plugins/rpc_ports/tServerPort.h"
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
//! Base class for modules
/*!
 * Base class for different types of modules -
 * with common functionality.
 */
class tModuleBase : public tComponent
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * \param parent Parent
   * \param name Name of component
   * \param extra_flags Any extra flags for component
   * \param share_ports Share ports in interfaces so that they can be connected to remote components?
   */
  tModuleBase(core::tFrameworkElement *parent, const std::string &name, tFlags extra_flags = tFlags(), bool share_ports = false);

  /*!
   * \return Returns 'Parameters' interface of this component
   */
  inline tInterface& GetParameters()
  {
    return GetInterface(parameters_interface, cPARAMETERS_INTERFACE_INFO, false);
  }
  inline tInterface& GetParameterParent() __attribute__((deprecated("USE GetParameters()")))
  {
    return GetParameters();
  }

  /*!
   * \return Returns 'Services' interface of this component
   */
  inline tInterface& GetServices()
  {
    return GetInterface(cSERVICES_INTERFACE_INFO, GetFlag(tFlag::SHARED));
  }

  /**
   * RPC port classes to use in modules.
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
  template <typename T>
  using tServer = tConveniencePort<rpc_ports::tServerPort<T>, tModuleBase, tInterface, &tModuleBase::GetServices>;

  template <typename T>
  using tClient = tConveniencePort<rpc_ports::tClientPort<T>, tModuleBase, tInterface, &tModuleBase::GetServices>;
#endif

  /**
   * Parameter classes to use in modules.
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
  class tParameter : public tConveniencePort<parameters::tParameter<T>, tModuleBase, tInterface, &tModuleBase::GetParameters>
  {
  public:
    template<typename ... ARGS>
    explicit tParameter(const ARGS&... args)
      : tConveniencePort<parameters::tParameter<T>, tModuleBase, tInterface, &tModuleBase::GetParameters>(args...)
    {
      if (this->GetWrapped())
      {
        assert(this->GetWrapped()->GetParent()->GetName() == "Parameters");
        this->AddListenerSimple(static_cast<tModuleBase*>(this->GetWrapped()->GetParent()->GetParent())->parameters_changed);
      }
    }
  };

  template <typename T>
  using tStaticParameter = tConveniencePort<parameters::tStaticParameter<T>, tComponent, core::tFrameworkElement, &tComponent::GetThis>;

  /*! Static interface info on common interfaces of composite components */
  static const tInterfaceInfo cVISUALIZATION_INTERFACE_INFO, cSERVICES_INTERFACE_INFO, cPARAMETERS_INTERFACE_INFO;

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  /*!
   * (Should only be called by abstract module classes such as tModule and tSenseControlModule)
   *
   * Calls OnParameterChange() if a parameter change was detected and resets change flag
   */
  void CheckParameters();

  /*!
   * (Automatically called)
   * Checks and resets all changed flags of ports in specified port group
   * and set custom API changed flags accordingly.
   *
   * This way, all changed flags can be reset automatically without the risk
   * of missing a change
   * (which could happen when resetting after Update()/Sense()/Control() call).
   *
   * \param port_group Port group to process
   * \return Has any port changed since last call?
   */
  bool ProcessChangedFlags(tFrameworkElement& port_group);

  virtual core::tFrameworkElement& GetVisualizationParent() override;

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Stores pointer to parameters interface */
  tInterface* parameters_interface;

  /*! Introduced this helper class to remove ambiguities when derived classes add listeners to ports */
  class tParameterChangeDetector
  {
    friend class tModuleBase;

    /*! Changed flag that is set whenever a parameter change is detected */
    volatile bool parameters_changed;

    tParameterChangeDetector() : parameters_changed(true) {}

  public:
    /*! Implementation of tPortListenerRaw */
    void OnPortChange(data_ports::tChangeContext& change_context);
  };

  /*! Changed flag that is set whenever a parameter change is detected */
  tParameterChangeDetector parameters_changed;

  /*! Called whenever parameters have changed */
  virtual void OnParameterChange()
  {}
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
