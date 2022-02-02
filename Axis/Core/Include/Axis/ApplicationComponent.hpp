/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

/// \file ApplicationComponent.hpp
///
/// \brief Contains the definition of `Axis::ApplicationComponent` interface class.

#ifndef AXIS_CORE_APPLICATIONCOMPONENT_HPP
#define AXIS_CORE_APPLICATIONCOMPONENT_HPP
#pragma once

#include "../../System/Include/Axis/Event.hpp"
#include "../../System/Include/Axis/SmartPointer.hpp"

namespace Axis
{

/// Forward declarations
struct TimePeriod;
class Application;

/// \brief Defines the interface for an application component.
class ApplicationComponent : public ISharedFromThis
{
public:
    /// \brief Event that is triggered when the component's priority is changed.
    ///
    /// - Argument \a `Axis::ApplicationComponent&` specifies which component has changed its priority.
    /// - Argument \a `Axis::Int32` specifies the new priority.
    using PriorityChangedEvent = Event<void(ApplicationComponent&, Int32)>;

    /// \brief Event that is triggered when the component activation state is changed.
    ///
    /// - Argument \a `Axis::ApplicationComponent&` specifies which component has changed its activation state.
    /// - Argument \a `Axis::Bool` specifies the new activation state.
    using ActivationChangedEvent = Event<void(ApplicationComponent&, Bool)>;

    /// \brief Default constructor
    ApplicationComponent() noexcept = default;

    /// \brief Constructs an application component with initial
    ///        priority and activation state.
    ///
    /// \param[in] updateOrder The update order of the component.
    /// \param[in] drawOrder The draw order of the component.
    /// \param[in] isActive The initial activation state of the component.
    /// \param[in] isVisible The initial visibility state of the component.
    ApplicationComponent(Int32 updateOrder,
                         Int32 renderOrder,
                         Bool  isActive  = true,
                         Bool  isVisible = true) noexcept;

    /// \brief Default virtual destructor
    virtual ~ApplicationComponent() noexcept = default;

    /// \brief Gets the component's update order. The lower the value, the earlier the component will be updated.
    ///
    /// \return The component's update order.
    inline Int32 GetUpdateOrder() const noexcept { return _updateOrder; }

    /// \brief Gets the component's render order. The lower the value, the earlier the component will be rendered.
    ///
    /// \return The component's render order.
    inline Int32 GetRenderOrder() const noexcept { return _renderOrder; }

    /// \brief Checks if the component is activated. If the component is not activated, it will not be updated.
    ///
    /// \return True if the component is activated, false otherwise.
    inline Bool IsActive() const noexcept { return _isActive; }

    /// \brief Checks if the component is visible. If the component is not visible, it will not be rendered.
    ///
    /// \return True if the component is visible, false otherwise.
    inline Bool IsVisible() const noexcept { return _isVisible; }

    /// \brief Sets the component's update order. The lower the value, the earlier the component will be updated.
    ///
    /// \param [in] order The component's update order.
    void SetUpdateOrder(Int32 updateOrder);

    /// \brief Sets the component's render order. The lower the value, the earlier the component will be rendered.
    ///
    /// \param [in] order The component's render order.
    void SetRenderOrder(Int32 renderOrder);

    /// \brief Sets the component's activation state. If the component is not activated, it will not be updated.
    ///
    /// \param [in] isActive True if the component is activated, false otherwise.
    void SetActive(Bool isActive);

    /// \brief Sets the component's visibility state. If the component is not visible, it will not be rendered.
    ///
    /// \param [in] isVisible True if the component is visible, false otherwise.
    void SetVisible(Bool isVisible);

    /// \brief Component's update method. The method is called by the application when it is time to update the component.
    ///
    /// \param [in] timePeriod Time period since the last update.
    virtual void Update(const TimePeriod& timePeriod) = 0;

    /// \brief Component's render method. The method is called by the application when it is time to render the component.
    ///
    /// \param [in] timePeriod Time period since the last update.
    virtual void Render(const TimePeriod& timePeriod) = 0;

    /// \brief The function is called by the application when the component is added to the application.
    ///
    /// \param [in] application The application that owns the component.
    virtual void OnAttach(Application& application) = 0;

    /// \brief The function is called by the application when the component is removed from the application.
    ///
    /// \param [in] application The application that owns the component.
    virtual void OnDetach(Application& application) = 0;

    /// \brief The event that is triggered when the component's update order is changed.
    inline PriorityChangedEvent::Register& GetUpdateOrderChangedEventHandler() noexcept { return _updateOrderChangedEvent.EventRegister; }

    /// \brief The event that is triggered when the component's render order is changed.
    inline PriorityChangedEvent::Register& GetRenderOrderChangedEventHandler() noexcept { return _renderOrderChangedEvent.EventRegister; }

    /// \brief The event that is triggered when the component's activation state is changed.
    inline ActivationChangedEvent::Register& GetActiveChangedEventHandler() noexcept { return _activeChangedEvent.EventRegister; }

    /// \brief The event that is triggered when the component's visibility state is changed.
    inline ActivationChangedEvent::Register& GetVisibleChangedEventHandler() noexcept { return _visibleChangedEvent.EventRegister; }

private:
    Int32                  _updateOrder = {};        ///< The component's update order.
    Int32                  _renderOrder = {};        ///< The component's render order.
    Bool                   _isActive    = {};        ///< True if the component is activated, false otherwise.
    Bool                   _isVisible   = {};        ///< True if the component is visible, false otherwise.
    PriorityChangedEvent   _updateOrderChangedEvent; ///< Event that is triggered when the component's update order is changed.
    PriorityChangedEvent   _renderOrderChangedEvent; ///< Event that is triggered when the component's render order is changed.
    ActivationChangedEvent _activeChangedEvent;      ///< Event that is triggered when the component's activation state is changed.
    ActivationChangedEvent _visibleChangedEvent;     ///< Event that is triggered when the component's visibility state is changed.
};

} // namespace Axis

#endif // AXIS_CORE_APPLICATIONCOMPONENT_HPP