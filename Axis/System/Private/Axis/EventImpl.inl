/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_EVENTIMPL_INL
#define AXIS_SYSTEM_EVENTIMPL_INL
#pragma once

#include "../../Include/Axis/Event.hpp"
#include "../../Include/Axis/Random.hpp"

namespace Axis
{

namespace System
{

template <class ReturnType, class... Args>
inline void Event<ReturnType(Args...)>::operator()(Args&&... args)
{
    for (auto& handler : EventRegister._handlers)
        handler.Second(std::forward<Args>(args)...);
}

template <class ReturnType, class... Args>
inline void Event<ReturnType(Args...)>::Invoke(Args&&... args)
{
    for (auto& handler : EventRegister._handlers)
        handler.Second(std::forward<Args>(args)...);
}

template <class ReturnType, class... Args>
inline Size Event<ReturnType(Args...)>::Register::Add(const Function<ReturnType(Args...)>& handler,
                                                      Size                                 token)
{
    static Random s_Random = {};

    // Generates a new token if the given token is collided with another handler.
    auto it = _handlers.Find(token);

    while (it != _handlers.end())
    {
        token = s_Random.NextSize();
        it    = _handlers.Find(token);
    }

    _handlers.Insert({token, handler});

    return token;
}

template <class ReturnType, class... Args>
inline Bool Event<ReturnType(Args...)>::Register::Remove(Size token) noexcept
{
    auto result = _handlers.Remove(token);

    return result.First;
}

template <class ReturnType, class... Args>
inline Bool Event<ReturnType(Args...)>::Register::TokenExists(Size token) const noexcept
{
    return _handlers.Find(token) != _handlers.end();
}

template <class ReturnType, class... Args>
inline EventToken<ReturnType(Args...)>::EventToken(typename Event<ReturnType(Args...)>::Register& event,
                                                   const Function<ReturnType(Args...)>&           handler,
                                                   Size                                           token) :
    _event(std::addressof(event))
{
    _token = _event->Add(handler, token);
}

template <class ReturnType, class... Args>
inline EventToken<ReturnType(Args...)>::EventToken(typename Event<ReturnType(Args...)>::Register& event,
                                                   Function<ReturnType(Args...)>&&                handler,
                                                   Size                                           token) :
    _event(std::addressof(event))
{
    _token = _event->Add(std::move(handler), token);
}

template <class ReturnType, class... Args>
inline EventToken<ReturnType(Args...)>::~EventToken() noexcept
{
    if (_event != nullptr)
        _event->Remove(_token);
}

template <class ReturnType, class... Args>
inline EventToken<ReturnType(Args...)>::EventToken(EventToken<ReturnType(Args...)>&& other) noexcept :
    _token(other._token),
    _event(other._event)
{
    other._token = 0;
    other._event = nullptr;
}

template <class ReturnType, class... Args>
inline EventToken<ReturnType(Args...)>& EventToken<ReturnType(Args...)>::operator=(EventToken<ReturnType(Args...)>&& other) noexcept
{
    if (_event != nullptr)
        _event->Remove(_token);

    _token = other._token;
    _event = other._event;

    other._token = 0;
    other._event = nullptr;

    return *this;
}

template <class ReturnType, class... Args>
inline void EventToken<ReturnType(Args...)>::Unsubscribe() noexcept
{
    if (_event != nullptr)
        _event->Remove(_token);

    _token = 0;
    _event = nullptr;
}

template <class ReturnType, class... Args>
inline Bool EventToken<ReturnType(Args...)>::IsSubscribed() const noexcept
{
    return _event != nullptr;
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_EVENTIMPL_INL