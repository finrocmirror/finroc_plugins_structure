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
/*!\file    plugins/structure/tConveniencePort.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * \brief   Contains tConveniencePort
 *
 * \b tConveniencePort
 *
 * Base class for ports that are typically used inside
 * groups and modules from an application developer.
 *
 * In many cases (if plain member variable of a group or module)
 * it is able to determine its name and parent automatically.
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tConveniencePort_h__
#define __plugins__structure__tConveniencePort_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/port/tPortGroup.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "plugins/structure/internal/register.h"

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
//! Convenience port
/*!
 * Base class for ports that are typically used inside
 * groups and modules from an application developer.
 *
 * In many cases (if plain member variable of a group or module)
 * it is able to determine its name and parent automatically.
 *
 * \tparam TPort  Port class that this class derives from
 * \tparam TElement Framework element class these ports belong to (typically group or module)
 * \tparam TContainer  Framework element type this port will be child of (typically port group)
 * \tparam GET_CONTAINER  Function that returns framework element to add port to (typically port group)
 */
template <typename TPort, typename TElement, typename TContainer, TContainer& (TElement::*GET_CONTAINER)()>
class tConveniencePort : public TPort
{
  typedef typename TPort::tDataType tDataType;
  typedef typename TPort::tConstructorParameters tConstructorParameters;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tConveniencePort() : TPort(MakeStandardCreationInfo(this->GetPortName(), &(this->FindParent()->*GET_CONTAINER)())) {}

  /*!
   * Constructor takes variadic argument list... just any properties you want to assign to port.
   *
   * Unlike tPort, port name and parent are usually determined automatically (however, only possible when port is direct class member).
   * If this is not possible/desired, name needs to be provided as first constructor argument - parent as arbitrary one.
   *
   * So...
   *
   * A string as first parameter is interpreted as port name; Any other string as config entry (relevant for parameters only).
   * A framework element pointer is interpreted as parent.
   * tFrameworkElement::tFlags arguments are interpreted as flags.
   * A tQueueSettings argument creates an input queue with the specified settings.
   * tBounds<T> are port's bounds.
   * tUnit argument is port's unit.
   * const T& is interpreted as port's default value.
   * tConstructorParameters argument is copied.
   *
   * This becomes a little tricky when T is a string type. There we have these rules:
   * A String not provided as first argument is interpreted as default value.
   * Any further string is interpreted as config entry.
   */
  template<typename A1, typename ... ARest>
  tConveniencePort(const A1& arg1, const ARest&... rest) : TPort(MakeCreationInfo(arg1, rest...)) {}

  /*!
   * (relevant for input ports and parameters only)
   *
   * \return Has port changed since last reset?
   */
  bool HasChanged()
  {
    return HasChangedImpl(this);
  }

  /*!
   * Resets port's changed flag
   * (both "real" port's and the one for the custom API)
   * (usually does not need to called - only if you want to ignore more changes)
   */
  void ResetChanged()
  {
    ResetChangedImpl(this);
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! \return Parent module of parameter */
  TElement* FindParent()
  {
    return static_cast<TElement*>(internal::FindParent(this));
  }

  /*! Get auto-generated port name */
  std::string GetPortName()
  {
    TElement* parent = FindParent();
    return internal::GetAutoGeneratedPortName(parent, UpdateCurrentPortNameIndex());
  }

  /*! Get & update current index for auto-generated port names */
  int UpdateCurrentPortNameIndex(TElement* parent = NULL)
  {
    if (parent == NULL)
    {
      parent = FindParent();
    }
    if (typeid(*parent).name() != parent->count_for_type) // detect class change when traversing module type hierarchy
    {
      parent->count_for_type = typeid(*parent).name();
      parent->auto_name_port_count = 0;
    }
    int current_index = parent->auto_name_port_count;
    parent->auto_name_port_count++;
    return current_index;
  }

  /*!
   * Create port creation info for this convenience port (non-template constructor)
   */
  tConstructorParameters MakeStandardCreationInfo(const std::string& name, core::tFrameworkElement* parent)
  {
    tConstructorParameters result;
    result.name = name;
    result.parent = parent;
    if (result.parent && typeid(*result.parent) == typeid(core::tPortGroup))
    {
      result.flags |= static_cast<core::tPortGroup*>(result.parent)->GetDefaultPortFlags();
    }
    return result;
  }

  /*!
   * Create port creation info for this convenience port (template constructor)
   */
  template<typename A1, typename ... ARest>
  tConstructorParameters MakeCreationInfo(const A1& arg1, const ARest&... rest)
  {
    tConstructorParameters result;
    if (data_ports::tIsString<A1>::value)
    {
      result = core::tPortWrapperBase::tConstructorArguments<tConstructorParameters>(arg1, rest...);
      if (result.name.length() == 0)
      {
        result.name = this->GetPortName();
      }
      else
      {
        if (result.parent == NULL)
        {
          this->UpdateCurrentPortNameIndex(this->FindParent());
        }
        else
        {
          this->UpdateCurrentPortNameIndex(static_cast<TElement*>(result.parent));
        }
      }
    }
    else
    {
      result = core::tPortWrapperBase::tConstructorArguments<tConstructorParameters>(this->GetPortName(), arg1, rest...);
    }
    if (result.parent == NULL)
    {
      result.parent = &(this->FindParent()->*GET_CONTAINER)();
    }
    else
    {
      result.parent = &(static_cast<TElement*>(result.parent)->*GET_CONTAINER)();
    }
    if (result.parent && typeid(*result.parent) == typeid(core::tPortGroup))
    {
      result.flags |= static_cast<core::tPortGroup*>(result.parent)->GetDefaultPortFlags();
    }
    return result;
  }

  /*! Helper methods for different kinds of base classes */
  bool HasChangedImpl(...) // Ports and parameters
  {
    return TPort::GetWrapped()->GetCustomChangedFlag() != data_ports::tChangeStatus::NO_CHANGE;
  }
  template <typename T>
  bool HasChangedImpl(parameters::tStaticParameter<T>* p)
  {
    return TPort::HasChanged();
  }

  void ResetChangedImpl(...) // Ports and parameters
  {
    TPort::GetWrapped()->ResetChanged();
    TPort::GetWrapped()->SetCustomChangedFlag(data_ports::tChangeStatus::NO_CHANGE);
  }
  template <typename T>
  void ResetChangedImpl(parameters::tStaticParameter<T>* p)
  {
    TPort::ResetChanged();
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
