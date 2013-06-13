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
/*!\file    plugins/structure/tGroupBase.h
 *
 * \author  Max Reichardt
 *
 * \date    2013-05-19
 *
 * \brief   Contains tGroupBase
 *
 * \b tGroupBase
 *
 * Base class for different types of groups -
 * with common functionality.
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tGroupBase_h__
#define __plugins__structure__tGroupBase_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/port/tPortGroup.h"
#include "plugins/runtime_construction/tFinstructableGroup.h"

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
//! Base class for groups
/*!
 * Base class for different types of groups -
 * with common functionality.
 */
class tGroupBase : public runtime_construction::tFinstructableGroup
{
  /*! GetContainer function for tStaticParameter */
  tFrameworkElement& GetThis()
  {
    return *this;
  }

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
  tGroupBase(core::tFrameworkElement *parent, const std::string &name, const std::string &structure_config_file = "",
             tFlags extra_flags = tFlags());

  /**
   * Parameter class to use in group.
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
  class tStaticParameter : public tConveniencePort<parameters::tStaticParameter<T>, tGroupBase, core::tFrameworkElement, &tGroupBase::GetThis>
  {
  public:
    template<typename ... ARGS>
    explicit tStaticParameter(const ARGS&... args)
      : tConveniencePort<parameters::tStaticParameter<T>, tGroupBase, core::tFrameworkElement, &tGroupBase::GetThis>(args...)
    {}
  };

  /*!
   * Checks the module's static parameters for changes
   * and calls OnStaticParameterChange() if any static parameters changed.
   *
   * (Not thread-safe: Should not be called in parallel to an active thread already executing the module.)
   */
  void CheckStaticParameters();

  // operator new is overloaded for auto-port naming feature
  void* operator new(size_t size);
  void* operator new[](size_t size); // not allowed

//----------------------------------------------------------------------
// Protected destructor (framework elements have their own memory management and are deleted with ManagedDelete)
//----------------------------------------------------------------------
protected:

  virtual ~tGroupBase();

  /*!
   * Creates interface for this group
   *
   * \param name Name of interface
   * \param share_ports Should ports in this interfaces be shared? (so that they can be accessed from other runtime environments)
   * \param extra_flags Any extra flags to assign to interface
   * \param extra_flags Any extra flags to assign to all ports
   */
  core::tPortGroup* CreateInterface(const std::string& name, bool share_ports, tFlags extra_interface_flags = tFlags(), tFlags default_port_flags = tFlags());

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  template <typename TPort, typename TElement, typename TContainer, TContainer& (TElement::*GET_CONTAINER)()>
  friend class tConveniencePort;

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
