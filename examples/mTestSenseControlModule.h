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
/*!\file    plugins/structure/examples/mTestSenseControlModule.h
 *
 * \author  Tobias Foehst
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * \brief   Contains mTestSenseControlModule
 *
 * \b mTestSenseControlModule
 *
 * Simple SenseControlModule for test program
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__examples__mTestSenseControlModule_h__
#define __plugins__structure__examples__mTestSenseControlModule_h__

#include "plugins/structure/tSenseControlModule.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace structure
{
namespace examples
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Simple test module
/*!
 * Simple SenseControlModule for test program
 */
class mTestSenseControlModule : public structure::tSenseControlModule
{

//----------------------------------------------------------------------
// Ports (These are the only variables that may be declared public)
//----------------------------------------------------------------------
public:

  tControllerInput<int> ci_signal_1;
  tControllerOutput<int> co_signal_2;
  tSensorInput<int> si_signal_3;
  tSensorOutput<int> so_signal_4;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  mTestSenseControlModule(core::tFrameworkElement *parent, const std::string &name = "TestSenseControlModule");

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  /*! Destructor
   *
   * The destructor of modules is declared private to avoid accidental deletion. Deleting
   * modules is already handled by the framework.
   */
  ~mTestSenseControlModule();

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  int counter;

  virtual void Sense() override;
  virtual void Control() override;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}



#endif
