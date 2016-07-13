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
/*!\file    plugins/structure/tGroup.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * \brief   Contains tGroup
 *
 * \b tGroup
 *
 * Plain group with an input and an output port interface.
 * Used to structure finroc applications.
 * Its contents can be edited and saved using finstruct.
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tGroup_h__
#define __plugins__structure__tGroup_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "plugins/data_ports/tProxyPort.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/tCompositeComponent.h"

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
//! Plain Group in default Finroc API
/*!
 * Plain group with an input and an output port interface.
 * Used to structure finroc applications.
 * Its contents can be edited and saved using finstruct.
 */
class tGroup : public tCompositeComponent
{

//----------------------------------------------------------------------
// Ports (These are the only variables that may be declared public)
//----------------------------------------------------------------------
public:

  /*!
   * \return Input interface
   */
  inline tInterface& GetInputs()
  {
    return GetInterface(cINPUT_INTERFACE_INFO, GetFlag(tFlag::SHARED));
  }

  /*!
   * \return Output interface
   */
  inline tInterface& GetOutputs()
  {
    return GetInterface(cOUTPUT_INTERFACE_INFO, GetFlag(tFlag::SHARED));
  }

  /**
   * Port classes to use in group.
   * (see base class for static parameters)
   *
   * Constructors take a variadic argument list... just any properties you want to assign to port.
   *
   * Unlike tPort, port name and parent are usually determined automatically (however, only possible when port is direct class member).
   * If this is not possible/desired, name needs to be provided as first constructor argument - parent as arbitrary one.
   *
   * So...
   *
   * A string as first parameter is interpreted as port name; Any other or further string as config entry (irrelevant for ports).
   * A framework element pointer is interpreted as parent.
   * tFrameworkElement::tFlags arguments are interpreted as flags.
   * A tQueueSettings argument creates an input queue with the specified settings.
   * tBounds<T> are port's bounds.
   * const T& is interpreted as port's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when T is a string type. There we have these rules:
   * A String not provided as first argument is interpreted as default value.
   * Any further string is interpreted as config entry.
   */
  template <typename T>
  using tInput = tConveniencePort<data_ports::tProxyPort<T, false>, tGroup, core::tPortGroup, &tGroup::GetInputs>;

  template <typename T>
  using tOutput = tConveniencePort<data_ports::tProxyPort<T, true>, tGroup, core::tPortGroup, &tGroup::GetOutputs>;

  /*! Static interface info on data port interfaces */
  static const tInterfaceInfo cOUTPUT_INTERFACE_INFO, cINPUT_INTERFACE_INFO;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tGroup(core::tFrameworkElement *parent, const std::string &name,
         const std::string &structure_config_file = "", bool share_ports = false,
         tFlags extra_flags = tFlags());

  tGroup(core::tFrameworkElement *parent, const std::string &name,
         const std::string &structure_config_file, bool share_ports, bool outdated_second_share_parameter,
         tFlags extra_flags = tFlags()) __attribute__((deprecated)) :
    tGroup(parent, name, structure_config_file, share_ports | outdated_second_share_parameter, extra_flags)
  {}

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
