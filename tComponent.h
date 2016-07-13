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

/*! Typedef for component interfaces */
typedef core::tPortGroup tInterface;

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
  virtual core::tFrameworkElement& GetVisualizationParent() = 0;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*! Typedefs for convenvience for subclasses in different namespaces */
  typedef finroc::structure::tInterface tInterface;

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
   * \return Global setting as to whether dedicated component visualization outputs (tVisualizationOutput) should be created.
   */
  static bool CreateComponentVisualizationPorts()
  {
    return create_component_visualization_ports;
  }

  /*!
   * \return Config file for module
   */
  parameters::tConfigFile* GetConfigFile() const
  {
    return parameters::tConfigFile::Find(*this);
  }

  /*!
   * \return Returns profiling interface
   */
  tInterface& GetProfilingInterface()
  {
    return GetInterface(cPROFILING_INTERFACE_INFO, false);
  }

  /*!
   * Components may have a std::vector of ports in their interfaces.
   * This is a convenience method for adjusting the number of ports in such vectors.
   * If the number of ports is equals to the 'number_of_ports' parameters, nothing is done.
   * If the number of ports is greater than the 'number_of_ports' parameters, ports at the back are deleted until the vector has the desired size.
   * If the number of ports is smaller than the 'number_of_ports' parameters, ports at the back are appended.
   * Ports are named: <common_prefix><start_index><common_postfix>, <common_prefix><start_index + 1><common_postfix>, ..., <common_prefix><start_index + number_of_ports - 1><common_postfix>
   *
   * \param port_vector Port vector to resize
   * \param number_of_ports Number of ports the vector should have
   * \param common_prefix Common prefix of all ports names (see port naming above; add a pending space if you like to have a space between prefix and space)
   * \param start_index Number in name of first port in vector (incremented for each further port)
   * \param start_index Common postfix of all ports names (see port naming above; could be e.g. ']')
   */
  template <typename TPort>
  void ResizePortVector(std::vector<TPort>& port_vector, int number_of_ports, const std::string& common_prefix, size_t start_index = 1, const std::string& common_postfix = "")
  {
    while (port_vector.size() > number_of_ports)
    {
      port_vector.rbegin()->GetWrapped()->ManagedDelete();
      port_vector.pop_back();
    }
    while (port_vector.size() < number_of_ports)
    {
      size_t port_index = port_vector.size() + start_index;
      std::stringstream stream;
      stream << common_prefix << port_index << common_postfix;
      port_vector.push_back(TPort(stream.str(), this));
      port_vector.back().Init();
    }
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

  /*!
   * Construction information on interface
   */
  struct tInterfaceInfo
  {
    /*! Interface name */
    const char* name;

    /*! Any extra flags to assign to interface */
    core::tFrameworkElement::tFlags extra_interface_flags;

    /*! Any extra flags to assign to all ports */
    core::tFrameworkElement::tFlags default_port_flags;
  };

  /*!
   * Returns specified interface - creates it if this has not been done already.
   *
   * \param interface_variable Variable that stores pointer to interface
   * \param interface_info Construction information on interface (typically stored in static variable)
   * \param shared_ports Should all ports in interface be shared?
   */
  inline tInterface& GetInterface(tInterface*& interface_variable, const tInterfaceInfo& interface_info, bool shared_ports)
  {
    if (!interface_variable)
    {
      interface_variable = &CreateInterface(interface_info, shared_ports);
    }
    return *interface_variable;
  }

  /*!
   * Returns specified interface - creates it if this has not been done already.
   * (This variant looks up interface by string, which is more expensive, but often not critical)
   *
   * \param interface_info Construction information on interface (typically stored in static variable)
   * \param shared_ports Should all ports in interface be shared?
   */
  tInterface& GetInterface(const tInterfaceInfo& interface_info, bool shared_ports);

  /*! Static info on profiling interface */
  static const tInterfaceInfo cPROFILING_INTERFACE_INFO;


  // operator new is overloaded for auto-port naming feature
  void* operator new(size_t size);
  void* operator new[](size_t size); // not allowed

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
protected:

  /*! Protected destructor (framework elements have their own memory management and are deleted with ManagedDelete) */
  virtual ~tComponent();


  /*! GetContainer function for e.g. tStaticParameter */
  tFrameworkElement& GetThis()
  {
    return *this;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  template <typename TPort, typename TElement, typename TContainer, TContainer& (TElement::*GET_CONTAINER)()>
  friend class tConveniencePort;

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

  /*!
   * Creates interface
   *
   * \param interface_info Construction information on interface (typically stored in static variable)
   * \param shared_ports Should all ports in interface be shared?
   */
  virtual tInterface& CreateInterface(const tInterfaceInfo& interface_info, bool shared_ports);
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
