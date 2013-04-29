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
/*!\file    plugins/structure/tModule.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * \brief   Contains tModule
 *
 * \b tModule
 *
 * Plain module with an input and an output port interface
 * and a single periodic update task.
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tModule_h__
#define __plugins__structure__tModule_h__

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
//! Plain module with periodic update task
/*!
 * Plain module with an input and an output port interface
 * and a single periodic update task.
 */
class tModule : public tModuleBase
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tModule(core::tFrameworkElement *parent, const std::string &name, bool shared_output_ports = false, bool share_input_ports = false);

  /*!
   * \return Parent port group of all inputs
   */
  inline core::tPortGroup& GetInputs()
  {
    return *input;
  }

  /*!
   * \return Parent port group of all outputs
   */
  inline core::tPortGroup& GetOutputs()
  {
    return *output;
  }

  /**
   * Port and parameter classes to use in module.
   *
   * Constructors take a variadic argument list... just any properties you want to assign to port/parameter.
   *
   * Unlike tPort, port name and parent are usually determined automatically (however, only possible when port is direct class member).
   * If this is not possible/desired, name needs to be provided as first constructor argument - parent as arbitrary one.
   *
   * So...
   *
   * The first string is interpreted as port name, the second possibly as config entry (relevant for parameters only).
   * A framework element pointer is interpreted as parent.
   * tFrameworkElement::tFlags arguments are interpreted as flags.
   * int argument is interpreted as queue length.
   * tBounds<T> are port's bounds.
   * tUnit argument is port's unit.
   * int16/short argument is interpreted as minimum network update interval.
   * const T& is interpreted as port's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when port has numeric or string type.
   * There we have these rules:
   *
   * string type: The second string argument is interpreted as default_value. The third as config entry.
   * numeric type: The first numeric argument is interpreted as default_value.
   */
  template <typename T>
  class tInput : public tConveniencePort<data_ports::tInputPort<T>, tModule, core::tPortGroup, &tModule::GetInputs>
  {
  public:
    template<typename ... ARGS>
    explicit tInput(const ARGS&... args)
      : tConveniencePort<data_ports::tInputPort<T>, tModule, core::tPortGroup, &tModule::GetInputs>(args...)
    {}
  };

  template < typename T = double >
  class tOutput : public tConveniencePort<data_ports::tOutputPort<T>, tModule, core::tPortGroup, &tModule::GetOutputs>
  {
  public:
    template<typename ... ARGS>
    explicit tOutput(const ARGS&... args)
      : tConveniencePort<data_ports::tOutputPort<T>, tModule, core::tPortGroup, &tModule::GetOutputs>(args...)
    {}
  };

//----------------------------------------------------------------------
// Protected destructor (framework elements have their own memory management and are deleted with ManagedDelete)
//----------------------------------------------------------------------
protected:

  virtual ~tModule();

  /*!
   * May be called in Update() method to check
   * whether any input port has changed, since last call to Update().
   *
   * (Changed flags are reset automatically)
   */
  bool InputChanged()
  {
    return input_changed;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Module's interfaces */
  finroc::core::tPortGroup *input;
  finroc::core::tPortGroup *output;

  /*! Task that calls Update() regularly */
  class UpdateTask : public rrlib::thread::tTask
  {
    tModule& module;
  public:
    UpdateTask(tModule& module);
    virtual void ExecuteTask();
  };

  UpdateTask update_task;

  /*! Has any input port changed since last cycle? */
  bool input_changed;

  /*! Called periodically with cycle time of thread container this module belongs to */
  virtual void Update() = 0;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
