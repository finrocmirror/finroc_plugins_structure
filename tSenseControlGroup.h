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
/*!\file    plugins/structure/tSenseControlGroup.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * \brief   Contains tSenseControlGroup
 *
 * \b tSenseControlGroup
 *
 * Group with sensor input, sensor output, controller input and controller
 * output interfaces.
 * Used to structure finroc applications.
 * Its contents can be edited and saved using finstruct.
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tSenseControlGroup_h__
#define __plugins__structure__tSenseControlGroup_h__

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
//! Group in default Finroc API
/*!
 * Group with sensor input, sensor output, controller input and controller
 * output interfaces.
 * Used to structure finroc applications.
 * Its contents can be edited and saved using finstruct.
 */
class tSenseControlGroup : public tCompositeComponent
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
  tSenseControlGroup(core::tFrameworkElement *parent, const std::string &name,
                     const std::string &structure_config_file = "",
                     bool share_so_and_ci_ports = false, tFlags extra_flags = tFlags());

  /*!
   * \return Controller Input Interface
   */
  inline tInterface& GetControllerInputs()
  {
    return GetInterface(cCONTROLLER_INPUT_INTERFACE_INFO, GetFlag(tFlag::SHARED));
  }

  /*!
   * \return Controller Output Interface
   */
  inline tInterface& GetControllerOutputs()
  {
    return GetInterface(cCONTROLLER_OUTPUT_INTERFACE_INFO, false);
  }

  /*!
   * \return Sensor Input Interface
   */
  inline tInterface& GetSensorInputs()
  {
    return GetInterface(cSENSOR_INPUT_INTERFACE_INFO, false);
  }

  /*!
   * \return Sensor Output Interface
   */
  inline tInterface& GetSensorOutputs()
  {
    return GetInterface(cSENSOR_OUTPUT_INTERFACE_INFO, GetFlag(tFlag::SHARED));
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
  using tControllerInput = tConveniencePort<data_ports::tProxyPort<T, false>, tSenseControlGroup, core::tPortGroup, &tSenseControlGroup::GetControllerInputs>;

  template <typename T>
  using tControllerOutput = tConveniencePort<data_ports::tProxyPort<T, true>, tSenseControlGroup, core::tPortGroup, &tSenseControlGroup::GetControllerOutputs>;

  template <typename T>
  using tSensorInput = tConveniencePort<data_ports::tProxyPort<T, false>, tSenseControlGroup, core::tPortGroup, &tSenseControlGroup::GetSensorInputs>;

  template <typename T>
  using tSensorOutput = tConveniencePort<data_ports::tProxyPort<T, true>, tSenseControlGroup, core::tPortGroup, &tSenseControlGroup::GetSensorOutputs>;


  /*! Static interface info on data port interfaces */
  static const tInterfaceInfo cSENSOR_INPUT_INTERFACE_INFO, cSENSOR_OUTPUT_INTERFACE_INFO, cCONTROLLER_INPUT_INTERFACE_INFO, cCONTROLLER_OUTPUT_INTERFACE_INFO;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
