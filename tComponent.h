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
/*!\file    plugins/structure/tComponent.h
 *
 * \author  Max Reichardt
 *
 * \date    2014-03-19
 *
 * \brief   Contains tComponent
 *
 * \b tComponent
 *
 * Base class for all components (e.g. modules, groups)
 * Contains common functionality
 *
 */
//----------------------------------------------------------------------
#ifndef __plugins__structure__tComponent_h__
#define __plugins__structure__tComponent_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "core/port/tPortGroup.h"
#include "plugins/data_ports/tOutputPort.h"
#include "plugins/parameters/tConfigFile.h"
#include "plugins/parameters/tConfigNode.h"
#include "plugins/runtime_construction/tStandardCreateModuleAction.h"

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
//! Component base class
/*!
 * Base class for all components (e.g. modules, groups)
 * Contains common functionality
 */
class tComponent : public core::tFrameworkElement
{
  /*!
   * GetContainer function for e.g. visualization ports
   * (returns this component if no visualization ports are to be created)
   */
  tFrameworkElement& GetVisualizationParent();

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * Level of detail for component visualization
   *
   * Defines for which level-of-detail settings (in e.g. finstruct) a port
   * is suitable for visualization.
   */
  enum class tLevelOfDetail : int
  {
    LOW,          //<! Suitable for low setting (up to 80x60 pixel)
    MID,          //<! Suitable for medium setting (up to 200x150 pixel)
    HIGH,         //<! Suitable for high setting (max. details)

    ALL,          //<! Suitable for all levels
    MID_AND_HIGH, //<! Suitable for high and medium setting
    LOW_AND_MID   //<! Suitable for low and medium setting
  };

  /*!
   * Can be passed to constructors of convenience port classes to not create a port
   * (calling the default-constructor during component initialization, typically creates a port).
   * Doing so, no port will be created and GetWrapped() will return nullptr.
   * A port may be assigned later (note that name and parent need to be provided for construction).
   */
  static tFlag cDO_NOT_CREATE_NOW;


  /*!
   * \param parent Parent
   * \param name Name of module
   * \param structure_config_file XML
   * \param share_so_and_ci_ports Share sensor output and controller input ports so that they can be accessed from other runtime environments?
   * \param extra_flags Any extra flags for component
   */
  tComponent(core::tFrameworkElement *parent, const std::string &name, tFlags extra_flags = tFlags());

  /*!
   * Checks the module's static parameters for changes
   * and calls OnStaticParameterChange() if any static parameters changed.
   *
   * (Not thread-safe: Should not be called in parallel to an active thread already executing the module.)
   */
  void CheckStaticParameters();

  /*!
   * \return Config file for module
   */
  parameters::tConfigFile* GetConfigFile() const
  {
    return parameters::tConfigFile::Find(*this);
  }

  /*!
   * \param node Common parent config file node for all child parameter config entries (starting with '/' => absolute link - otherwise relative).
   */
  void SetConfigNode(const std::string& node)
  {
    parameters::tConfigNode::SetConfigNode(*this, node);
  }

  /*!
   * Defines that specified port can be used for component visualization.
   * Must be called before port has been initialized
   * (otherwise and error message is printed and call is ignored)
   *
   * \param port Port containing data for visualization
   * \param level_of_detail Defines for which level of details port is suitable
   */
  static void SetVisualizationPort(core::tPortWrapperBase port, tLevelOfDetail level_of_detail);


  /**
   * Port class for component visualization purposes only.
   *
   * The first template parameter T is the data type of the port and should be a
   * data type that finstruct can visualize (e.g. tImage oder tCanvas).
   * The seconds template parameter defines for which level of details the port is suitable.
   *
   * If create_component_visualization_ports is false, no port will be created.
   * So before doing anything with a visualization port (apart from constructing),
   * IsConnected() should be called to determine whether creating a visualization
   * is necessary.
   * IsConnected() will always return false, if no port was created.
   *
   * Constructor takes a variadic argument list... just any properties you want to assign to port.
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
   * tUnit argument is port's unit.
   * const T& is interpreted as port's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when T is a string type. There we have these rules:
   * A String not provided as first argument is interpreted as default value.
   * Any further string is interpreted as config entry.
   */
  template <typename T, tLevelOfDetail Tlevel_of_detail>
  class tVisualizationOutput : public tConveniencePort<data_ports::tOutputPort<T>, tComponent, tFrameworkElement, &tComponent::GetVisualizationParent>
  {
    typedef tConveniencePort<data_ports::tOutputPort<T>, tComponent, tFrameworkElement, &tComponent::GetVisualizationParent> tBase;

  public:
    template<typename ... ARGS>
    explicit tVisualizationOutput(const ARGS&... args)
      : tBase(args..., create_component_visualization_ports ? tFlag::PORT : cDO_NOT_CREATE_NOW)
    {
      if (create_component_visualization_ports)
      {
        SetVisualizationPort(*this, Tlevel_of_detail);
      }
    }

    /*!
     * \return Is anyone interested in visualization from this port? If not, port must not be used for publishing any data
     */
    inline bool IsConnected() const
    {
      if (tBase::GetWrapped())
      {
        return tBase::IsConnected();
      }
      return false;
    }
  };

  /*!
   * Globally sets whether dedicated component visualization outputs (tVisualizationOutput)
   * should be created (enabled by default).
   * Must be set at program startup - before components are created - to have an effect.
   *
   * \param enabled Whether to create visualization outputs
   */
  static void SetComponentVisualizationEnabled(bool enabled);

  /*!
   * When storing convenience ports in std::unique pointers, this class can be used as deleter so that
   * actual wrapped port is also deleted.
   */
  class tPortDeleter
  {
  public:
    void operator()(core::tPortWrapperBase* ptr) const
    {
      ptr->GetWrapped()->ManagedDelete();
      delete ptr;
    }
  };

  // operator new is overloaded for auto-port naming feature
  void* operator new(size_t size);
  void* operator new[](size_t size); // not allowed

//----------------------------------------------------------------------
// Protected destructor (framework elements have their own memory management and are deleted with ManagedDelete)
//----------------------------------------------------------------------
protected:

  virtual ~tComponent();


  /*! GetContainer function for e.g. tParameter */
  tFrameworkElement& GetParameterParent();

  /*! GetContainer function for services */
  tFrameworkElement& GetServicesParent();

  /*! GetContainer function for e.g. tStaticParameter */
  tFrameworkElement& GetThis()
  {
    return *this;
  }

  /*!
   * \return Has framework element "Parameters" for aggregating parameters has been created?
   */
  bool ParameterParentCreated()
  {
    return parameters;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  template <typename TPort, typename TElement, typename TContainer, TContainer& (TElement::*GET_CONTAINER)()>
  friend class tConveniencePort;

  /*! Element aggregating parameters */
  core::tFrameworkElement* parameters;

  /*! Number of ports already created that have auto-generated names */
  int auto_name_port_count;

  /*! Counter should be reset for every module class in type hierarchy. This helper variable is used to detect this. */
  const char* count_for_type;

  /*!
   * Global setting as to whether dedicated component visualization outputs (tVisualizationOutput)
   * should be created.
   * Must be set at program startup - before components are created - to have an effect.
   */
  static bool create_component_visualization_ports;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
