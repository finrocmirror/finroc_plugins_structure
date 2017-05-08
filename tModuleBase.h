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

  tModuleBase(core::tFrameworkElement *parent, const std::string &name);

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
  using tServer = tConveniencePort<rpc_ports::tServerPort<T>, tComponent, tFrameworkElement, &tModuleBase::GetServicesParent>;

  template <typename T>
  using tClient = tConveniencePort<rpc_ports::tClientPort<T>, tComponent, tFrameworkElement, &tModuleBase::GetServicesParent>;
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
  class tParameter : public tConveniencePort<parameters::tParameter<T>, tComponent, tFrameworkElement, &tModuleBase::GetParameterParent>
  {
  public:
    template<typename ... ARGS>
    explicit tParameter(const ARGS&... args)
      : tConveniencePort<parameters::tParameter<T>, tComponent, tFrameworkElement, &tModuleBase::GetParameterParent>(args...)
    {
      if (this->GetWrapped())
      {
        assert(this->GetWrapped()->GetParent()->NameEquals("Parameters"));
        this->AddListenerSimple(static_cast<tModuleBase*>(this->GetWrapped()->GetParent()->GetParent())->parameters_changed);
      }
    }
  };

  template <typename T>
  using tStaticParameter = tConveniencePort<parameters::tStaticParameter<T>, tComponent, core::tFrameworkElement, &tComponent::GetThis>;

  /*!
   * \return Port group with profiling ports. Created with first call to this function.
   */
  core::tPortGroup& GetProfilingPortGroup();

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
   * Creates interface for this module
   *
   * \param name Name of interface
   * \param share_ports Should ports in this interfaces be shared? (so that they can be accessed from other runtime environments)
   * \param extra_flags Any extra flags to assign to interface
   * \param default_port_flags Default flags for ports in this interface
   */
  core::tPortGroup* CreateInterface(const std::string& name, bool share_ports, tFlags extra_flags = tFlags(), tFlags default_port_flags = tFlags());

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

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

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
