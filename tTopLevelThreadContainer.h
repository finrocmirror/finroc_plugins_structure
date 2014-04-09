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
/*!\file    plugins/structure/tTopLevelThreadContainer.h
 *
 * \author  Max Reichardt
 *
 * \date    2014-04-08
 *
 * \brief   Contains tTopLevelThreadContainer
 *
 * \b tTopLevelThreadContainer
 *
 * Convenience class for top-level ThreadContainers.
 * The parent (the tRuntimeEnvironment instance) is automatically set.
 * Furthermore, ports are shared by default and it can be easily specified
 * whether port names are unique (the respective command line argument
 * will also be processed).
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tTopLevelThreadContainer_h__
#define __plugins__structure__tTopLevelThreadContainer_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/tRuntimeEnvironment.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/tThreadContainer.h"

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
//! Top-Level ThreadContainer
/*!
 * Convenience class for top-level ThreadContainers.
 * The parent (the tRuntimeEnvironment instance) is automatically set.
 * Furthermore, ports are shared by default and it can be easily specified
 * whether port names are unique (the respective command line argument
 * will also be processed).
 */
template <typename T = tSenseControlGroup>
class tTopLevelThreadContainer : public tThreadContainer<T>
{
  typedef core::tFrameworkElement::tFlag tFlag;
  typedef core::tFrameworkElement::tFlags tFlags;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * This constructor only works if T has a constructor with the same signature
   * as tSenseControlGroup and tGroup.
   */
  tTopLevelThreadContainer(const std::string& name, const std::string &structure_config_file = "",
                           bool share_ports = true, bool port_links_are_unique = true) :
    tThreadContainer<T>(&core::tRuntimeEnvironment::GetInstance(), name, structure_config_file, share_ports, port_links_are_unique ? tFlags(tFlag::GLOBALLY_UNIQUE_LINK) : tFlags())
  {
    this->InitiallyShowInTools();
  }

  /*!
   * Constructor to cope with any
   */
  template <typename ... TArgs>
  tTopLevelThreadContainer(const TArgs &... args, typename std::enable_if<sizeof...(args) >= 4, bool >::type port_links_are_unique = true) :
    tThreadContainer<T>(&core::tRuntimeEnvironment::GetInstance(), args..., port_links_are_unique ? tFlags(tFlag::GLOBALLY_UNIQUE_LINK) : tFlags())
  {
    this->InitiallyShowInTools();
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
