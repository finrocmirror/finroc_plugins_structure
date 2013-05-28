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
#include "plugins/parameters/tConfigFile.h"
#include "plugins/parameters/tConfigNode.h"
#include "plugins/parameters/tParameter.h"
#include "plugins/parameters/tStaticParameter.h"
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
//! Base class for modules
/*!
 * Base class for different types of modules -
 * with common functionality.
 */
class tModuleBase : public core::tFrameworkElement
{
  /*! GetContainer function for tParameter */
  tFrameworkElement& GetParameterParent();

  /*! GetContainer function for tStaticParameter */
  tFrameworkElement& GetThis()
  {
    return *this;
  }

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tModuleBase(core::tFrameworkElement *parent, const std::string &name);

  /**
   * Parameter classes to use in module.
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
  class tParameter : public tConveniencePort<parameters::tParameter<T>, tModuleBase, tFrameworkElement, &tModuleBase::GetParameterParent>
  {
  public:
    template<typename ... ARGS>
    explicit tParameter(const ARGS&... args)
      : tConveniencePort<parameters::tParameter<T>, tModuleBase, tFrameworkElement, &tModuleBase::GetParameterParent>(args...)
    {
      assert(this->GetWrapped()->GetParent()->NameEquals("Parameters"));
      this->AddPortListenerSimple(static_cast<tModuleBase*>(this->GetWrapped()->GetParent()->GetParent())->parameters_changed);
    }
  };

  template <typename T>
  class tStaticParameter : public tConveniencePort<parameters::tStaticParameter<T>, tModuleBase, core::tFrameworkElement, &tModuleBase::GetThis>
  {
  public:
    template<typename ... ARGS>
    explicit tStaticParameter(const ARGS&... args)
      : tConveniencePort<parameters::tStaticParameter<T>, tModuleBase, core::tFrameworkElement, &tModuleBase::GetThis>(args...)
    {}
  };

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
  void* operator new[](size_t size); // not allowed => exception

  /*!
   * \param node Common parent config file node for all child parameter config entries (starting with '/' => absolute link - otherwise relative).
   */
  void SetConfigNode(const std::string& node)
  {
    parameters::tConfigNode::SetConfigNode(*this, node);
  }

  /*!
   * \return Config file for module
   */
  parameters::tConfigFile* GetConfigFile() const
  {
    return parameters::tConfigFile::Find(*this);
  }

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  virtual ~tModuleBase();


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
   */
  core::tPortGroup* CreateInterface(const std::string& name, bool share_ports, tFlags extra_flags = tFlags());

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

  template <typename TPort, typename TElement, typename TContainer, TContainer& (TElement::*GET_CONTAINER)()>
  friend class tConveniencePort;

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

  /*! Element aggregating parameters */
  core::tFrameworkElement* parameters;

  /*! Changed flag that is set whenever a parameter change is detected */
  tParameterChangeDetector parameters_changed;

  /*! Number of ports already created that have auto-generated names */
  int auto_name_port_count;

  /*! Counter should be reset for every module class in type hierarchy. This helper variable is used to detect this. */
  const char* count_for_type;


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
