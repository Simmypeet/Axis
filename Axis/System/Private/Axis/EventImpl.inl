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
inline Bool Event<ReturnType(Args...)>::Register::Remove(Size token)
{
    auto result = _handlers.Remove(token);

    return result.First;
}

template <class ReturnType, class... Args>
inline Bool Event<ReturnType(Args...)>::Register::TokenExists(Size token) const noexcept
{
    return _handlers.Find(token) != _handlers.end();
}

} // namespace Axis

#endif // AXIS_SYSTEM_EVENTIMPL_INL