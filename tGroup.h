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
 * Group with sensor input, sensor output, controller input and controller
 * output interfaces.
 * Used to structure finroc applications.
 * Its contents can be edited and saved using finstruct.
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tGroup_h__
#define __plugins__structure__tGroup_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/port/tPortGroup.h"
#include "plugins/data_ports/tProxyPort.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/tGroupBase.h"

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
class tGroup : public tGroupBase
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
  tGroup(core::tFrameworkElement *parent, const std::string &name, const std::string &structure_config_file = "",
         bool share_so_and_ci_ports = false, tFlags extra_flags = tFlags());

  /*!
   * \return Parent port group of all controller inputs
   */
  inline core::tPortGroup& GetControllerInputs()
  {
    return *controller_input;
  }

  /*!
   * \return Parent port group of all controller outputs
   */
  inline core::tPortGroup& GetControllerOutputs()
  {
    return *controller_output;
  }

  /*!
   * \return Parent port group of all sensor inputs
   */
  inline core::tPortGroup& GetSensorInputs()
  {
    return *sensor_input;
  }

  /*!
   * \return Parent port group of all sensor outputs
   */
  inline core::tPortGroup& GetSensorOutputs()
  {
    return *sensor_output;
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
   * tUnit argument is port's unit.
   * const T& is interpreted as port's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when T is a string type. There we have these rules:
   * A String not provided as first argument is interpreted as default value.
   * Any further string is interpreted as config entry.
   */
  template <typename T>
  class tControllerInput : public tConveniencePort<data_ports::tProxyPort<T, false>, tGroup, core::tPortGroup, &tGroup::GetControllerInputs>
  {
  public:
    template<typename ... ARGS>
    explicit tControllerInput(const ARGS&... args)
      : tConveniencePort<data_ports::tProxyPort<T, false>, tGroup, core::tPortGroup, &tGroup::GetControllerInputs>(args...)
    {}
  };

  template <typename T>
  class tControllerOutput : public tConveniencePort<data_ports::tProxyPort<T, true>, tGroup, core::tPortGroup, &tGroup::GetControllerOutputs>
  {
  public:
    template<typename ... ARGS>
    explicit tControllerOutput(const ARGS&... args)
      : tConveniencePort<data_ports::tProxyPort<T, true>, tGroup, core::tPortGroup, &tGroup::GetControllerOutputs>(args...)
    {}
  };

  template <typename T>
  class tSensorInput : public tConveniencePort<data_ports::tProxyPort<T, false>, tGroup, core::tPortGroup, &tGroup::GetSensorInputs>
  {
  public:
    template<typename ... ARGS>
    explicit tSensorInput(const ARGS&... args)
      : tConveniencePort<data_ports::tProxyPort<T, false>, tGroup, core::tPortGroup, &tGroup::GetSensorInputs>(args...)
    {}
  };

  template <typename T>
  class tSensorOutput : public tConveniencePort<data_ports::tProxyPort<T, true>, tGroup, core::tPortGroup, &tGroup::GetSensorOutputs>
  {
  public:
    template<typename ... ARGS>
    explicit tSensorOutput(const ARGS&... args)
      : tConveniencePort<data_ports::tProxyPort<T, true>, tGroup, core::tPortGroup, &tGroup::GetSensorOutputs>(args...)
    {}
  };

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Group's interfaces */
  finroc::core::tPortGroup* controller_input;
  finroc::core::tPortGroup* controller_output;
  finroc::core::tPortGroup* sensor_input;
  finroc::core::tPortGroup* sensor_output;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
