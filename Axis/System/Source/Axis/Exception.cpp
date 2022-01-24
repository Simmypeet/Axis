/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/SystemPch.hpp>

#include <Axis/Exception.hpp>
#include <Axis/String.hpp>

namespace Axis
{

inline const Char* CopyMessageValue(const Char* message) noexcept
{
    auto length = ::Axis::GetStringLength(message);

    auto pointer = (Char*)std::malloc(((length + 1) * sizeof(Char)));

    if (!pointer) // Out of memory / no message specified.
        return pointer;

    std::memcpy(pointer, message, (length * sizeof(Char)));

    pointer[length] = '\0';

    return pointer;
}

inline void FreeMessage(const Char* message) noexcept
{
    std::free((PVoid)message);
}

Exception::Exception(const Exception& other) noexcept :
    _message(CopyMessageValue(other._message)) {}

Exception::Exception(Exception&& other) noexcept :
    _message(other._message)
{
    other._message = nullptr;
}

Exception::Exception(const Char* message) noexcept :
    _message(CopyMessageValue(message)) {}

Exception::~Exception() noexcept
{
    if (_message)
        FreeMessage(_message);
}

Exception& Exception::operator=(const Exception& other) noexcept
{
    if (this != std::addressof(other))
    {
        if (_message)
            FreeMessage(_message);

        _message = CopyMessageValue(other._message);
    }

    return *this;
}

Exception& Exception::operator=(Exception&& other) noexcept
{
    if (this != std::addressof(other))
    {
        if (_message)
            FreeMessage(_message);

        _message = other._message;

        other._message = nullptr;
    }

    return *this;
}

} // namespace Axis