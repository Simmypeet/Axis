/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_CORE_COMPONENTCOLLECTION_HPP
#define AXIS_CORE_COMPONENTCOLLECTION_HPP
#pragma once

#include "../../System/Include/Axis/Event.hpp"
#include "../../System/Include/Axis/List.hpp"
#include "../../System/Include/Axis/SmartPointer.hpp"
#include "ApplicationComponent.hpp"

namespace Axis
{

namespace Core
{

/// \brief A collection of `Axis::ApplicationComponent` objects,
///        the collection provides event handlers for different
///        events happening in the collection.
class ComponentCollection final
{
public:
    /// \brief Destructor
    ~ComponentCollection() noexcept = default;

    /// \brief Adds a new component to the collection.
    ///
    /// \param[in] component The component to add.
    void Append(System::SharedPointer<ApplicationComponent>&& component);

    /// \brief Adds a component to the collection and adjusts its update and render order
    ///        to the next highest value.
    ///
    /// \param[in] component The component to add.
    ///
    /// \note If the exception is thrown, the component's update and render order will
    ///       be set to the previous value.
    void AppendHighest(System::SharedPointer<ApplicationComponent>&& component);

    /// \brief Removes a component from the collection.
    ///
    /// \param[in] component The component to remove.
    ///
    /// \return True if the component was removed, false otherwise.
    Bool Remove(const ApplicationComponent& component);

    /// \brief Updates all the components in the collection.
    ///
    /// \param[in] timeStep The time since the last update.
    void UpdateAll(const System::TimePeriod& timeStep);

    /// \brief Renders all the components in the collection.
    ///
    /// \param[in] timeStep The time since the last update.
    void RenderAll(const System::TimePeriod& timeStep);

    /// \brief Gets the list of components in the collection.
    ///
    /// \return The list of components.
    inline const System::List<System::SharedPointer<ApplicationComponent>>& GetComponents() const noexcept { return _components; }

    /// \brief The event raised when a component is added to the collection.
    inline System::Event<void(ApplicationComponent&)>::Register& GetComponentAddedEventHandler() noexcept { return _componentAddedEvent.EventRegister; }

    /// \brief The event raised when a component is removed from the collection.
    inline System::Event<void(ApplicationComponent&)>::Register& GetComponentRemovedEventHandler() noexcept { return _componentRemovedEvent.EventRegister; }

    ComponentCollection(const ComponentCollection&) = delete;            ///< Deleted copy constructor
    ComponentCollection(ComponentCollection&&)      = delete;            ///< Deleted move constructor
    ComponentCollection& operator=(const ComponentCollection&) = delete; ///< Deleted copy assignment operator
    ComponentCollection& operator=(ComponentCollection&&) = delete;      ///< Deleted move assignment operator

private:
    /// \brief Private constructor
    explicit ComponentCollection(Application& application) noexcept;

    /// Private members
    System::List<System::SharedPointer<ApplicationComponent>> _components;            ///< The collection of components.
    System::List<System::SharedPointer<ApplicationComponent>> _componentsDrawOrder;   ///< The collection of components in draw order.
    System::List<System::SharedPointer<ApplicationComponent>> _componentsUpdateOrder; ///< The collection of components in update order.
    Application&                                              _application;           ///< The application.
    System::Event<void(ApplicationComponent&)>                _componentAddedEvent;   ///< The event raised when a component is added to the collection.
    System::Event<void(ApplicationComponent&)>                _componentRemovedEvent; ///< The event raised when a component is removed from the collection.

    /// Friend declaration
    friend class Application;
};

} // namespace Core

} // namespace Axis

#endif // AXIS_CORE_COMPONENTCOLLECTION_HPP