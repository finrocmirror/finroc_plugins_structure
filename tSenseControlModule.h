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
/*!\file    plugins/structure/tSenseControlModule.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * \brief   Contains tSenseControlModule
 *
 * \b tSenseControlModule
 *
 * Module with a SensorInput, SensorOutput, ControllerInput and ControllerOutput
 * interface and two periodic tasks (Sense() and Control())
 *
 * Use this module, if the data it processes can be classified in sensor
 * or controller data.
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tSenseControlModule_h__
#define __plugins__structure__tSenseControlModule_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/tModuleBase.h"

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
//! Module with sense and control tasks + respective port interfaces
/*!
 * Module with a SensorInput, SensorOutput, ControllerInput and ControllerOutput
 * interface and two periodic tasks (Sense() and Control())
 *
 * Use this module, if the data it processes can be classified in sensor
 * or controller data.
 */
class tSenseControlModule : public tModuleBase
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * \param parent Parent
   * \param name Name of module
   * \param share_so_and_ci_ports Share sensor output and controller input ports so that they can be accessed from other runtime environments?
   */
  tSenseControlModule(core::tFrameworkElement *parent, const std::string &name, bool share_so_and_ci_ports = false);

  /*!
   * \return Controller Input Interface
   */
  inline tInterface& GetControllerInputs()
  {
    return GetInterface(controller_input, cCONTROLLER_INPUT_INTERFACE_INFO, GetFlag(tFlag::SHARED));
  }

  /*!
   * \return Controller Output Interface
   */
  inline tInterface& GetControllerOutputs()
  {
    return GetInterface(controller_output, cCONTROLLER_OUTPUT_INTERFACE_INFO, false);
  }

  /*!
   * \return Sensor Input Interface
   */
  inline tInterface& GetSensorInputs()
  {
    return GetInterface(sensor_input, cSENSOR_INPUT_INTERFACE_INFO, false);
  }

  /*!
   * \return Sensor Output Interface
   */
  inline tInterface& GetSensorOutputs()
  {
    return GetInterface(sensor_output, cSENSOR_OUTPUT_INTERFACE_INFO, GetFlag(tFlag::SHARED));
  }

  /**
   * Port classes to use in module.
   *
   * Constructors take a variadic argument list... just any properties you want to assign to port.
   *
   * Unlike tPort, port name and parent are usually determined automatically (however, only possible when port is direct class member).
   * If this is not possible/desired, name needs to be provided as first constructor argument - parent as arbitrary one.
   *
   * So...
   *
   * A string as first parameter is interpreted as port name; Any other string as config entry
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
  using tControllerInput = tConveniencePort<data_ports::tInputPort<T>, tSenseControlModule, core::tPortGroup, &tSenseControlModule::GetControllerInputs>;

  template <typename T>
  using tControllerOutput = tConveniencePort<data_ports::tOutputPort<T>, tSenseControlModule, core::tPortGroup, &tSenseControlModule::GetControllerOutputs>;

  template <typename T>
  using tSensorInput = tConveniencePort<data_ports::tInputPort<T>, tSenseControlModule, core::tPortGroup, &tSenseControlModule::GetSensorInputs>;

  template <typename T>
  using tSensorOutput = tConveniencePort<data_ports::tOutputPort<T>, tSenseControlModule, core::tPortGroup, &tSenseControlModule::GetSensorOutputs>;


  /*! Static interface info on data port interfaces */
  static const tInterfaceInfo cSENSOR_INPUT_INTERFACE_INFO, cSENSOR_OUTPUT_INTERFACE_INFO, cCONTROLLER_INPUT_INTERFACE_INFO, cCONTROLLER_OUTPUT_INTERFACE_INFO;

//----------------------------------------------------------------------
// Protected fields and methods
//----------------------------------------------------------------------
protected:


  /*! Protected destructor (framework elements have their own memory management and are deleted with ManagedDelete) */
  virtual ~tSenseControlModule();

  /*!
   * May be called in Sense() method to check
   * whether any sensor input port has changed, since last call to Sense().
   *
   * (Changed flags are reset automatically)
   */
  bool SensorInputChanged()
  {
    return sensor_input_changed;
  }

  /*!
   * May be called in Control() method to check
   * whether any controller input port has changed, since last call to Control().
   *
   * (Changed flags are reset automatically)
   */
  bool ControllerInputChanged()
  {
    return controller_input_changed;
  }

  virtual void PostChildInit() override;

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Module's interfaces */
  tInterface *sensor_input;
  tInterface *sensor_output;
  tInterface *controller_input;
  tInterface *controller_output;

  /*! Task that calls Control() regularly */
  class ControlTask : public rrlib::thread::tTask
  {
    tSenseControlModule& module;
  public:
    ControlTask(tSenseControlModule& module);
    virtual void ExecuteTask() override;
  };

  /*! Task that calls Sense() regularly */
  class SenseTask : public rrlib::thread::tTask
  {
    tSenseControlModule& module;
  public:
    SenseTask(tSenseControlModule& module);
    virtual void ExecuteTask() override;
  };

  SenseTask sense_task;
  ControlTask control_task;

  /*! Has any sensor input port changed since last cycle? */
  bool sensor_input_changed;

  /*! Has any controller input port changed since last cycle? */
  bool controller_input_changed;

  /*! Called periodically with cycle time of thread container this module belongs to */
  virtual void Sense() = 0;

  /*! Called periodically with cycle time of thread container this module belongs to */
  virtual void Control() = 0;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
