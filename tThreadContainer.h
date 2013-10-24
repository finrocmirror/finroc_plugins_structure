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
/*!\file    plugins/structure/tThreadContainer.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-12-23
 *
 * \brief   Contains tThreadContainer
 *
 * \b tThreadContainer
 *
 * Group containing thread that executes periodic tasks of all children.
 * Execution is performed in the order of the graph.
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tThreadContainer_h__
#define __plugins__structure__tThreadContainer_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "plugins/scheduling/tThreadContainerElement.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/tSenseControlGroup.h"

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
//! Group containing thread for execution
/*!
 * Group containing thread that executes periodic tasks of all children.
 * Execution is performed in the order of the graph.
 */
typedef scheduling::tThreadContainerElement<tSenseControlGroup> tThreadContainer;

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}

extern template class scheduling::tThreadContainerElement<structure::tSenseControlGroup>;

}


#endif
