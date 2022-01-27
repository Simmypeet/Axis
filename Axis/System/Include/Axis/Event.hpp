/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Event.hpp
///
/// \brief Contains `Axis::Event` template class.

#ifndef AXIS_SYSTEM_EVENT_HPP
#define AXIS_SYSTEM_EVENT_HPP
#pragma once

#include "Function.hpp"
#include "HashMap.hpp"
#include "Trait.hpp"

namespace Axis
{

/// \brief Defines an event notification class. When event is raised it will
///        notify all handlers whose interest in this event.
template <class T>
class Event;

/// \brief Defines an event notification class. When event is raised it will
///        notify all handlers that subscribed to this event.
template <class ReturnType, class... Args>
class Event<ReturnType(Args...)>
{
public:
    /// \brief Default constructor
    Event() noexcept = default;

    /// \brief Invokes all handlers that subscribed to this event
    ///        with the given arguments.
    void operator()(Args&&... args);

    /// \brief Invokes all handlers that subscribed to this event
    ///        with the given arguments.
    void Invoke(Args&&... args);

    /// \brief Interface for subscribing and unsubscribing to an event.
    class Register
    {
    public:
        /// \brief Adds a handler to the event
        ///
        /// \param[in] handler Callable object to subscribe to the event.
        /// \param[in] token Token to identify the handler. If the token is collided with
        ///                  another handler, new token will be generated.
        ///
        /// \return Returns the token that was used to subscribe to the event.
        Size Add(const Function<ReturnType(Args...)>& handler,
                 Size                                 token = 0);

        /// \brief Removes a handler from the event
        ///
        /// \param[in] token Token to identify the handler.
        ///
        /// \return Returns true if the handler was removed successfully.
        Bool Remove(Size token);

        /// \brief Checks if the event has any handlers with the given token
        ///        subscribed to it.
        ///
        /// \param[in] token Token to identify the handler.
        ///
        /// \return Returns true if the event has any handlers with the given token
        Bool TokenExists(Size token) const noexcept;

    private:
        /// \brief Private default constructor
        Register() noexcept = default;

        /// \brief Private destructor
        ~Register() noexcept = default;

        /// \brief Private copy constructor
        Register(const Register& other) = default;

        /// \brief Private move constructor
        Register(Register&& other) = default;

        /// \brief Private assignment operator
        Register& operator=(const Register& other) = default;

        /// \brief Private move assignment operator
        Register& operator=(Register&& other) = default;

        /// Contains handlers that subscribed to this event.
        HashMap<Size, Function<ReturnType(Args...)>> _handlers;

        // Friend declaration
        friend class Event;
    };

    Register EventRegister; ///< Register instance

    // ReturnType must be void
    static_assert(std::is_same_v<ReturnType, void>);
};

} // namespace Axis

#include "../../Private/Axis/EventImpl.inl"

#endif // AXIS_SYSTEM_EVENT_HPP