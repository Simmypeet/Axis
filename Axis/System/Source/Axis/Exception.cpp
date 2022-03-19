/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/SystemPch.hpp>

#include <Axis/Exception.hpp>
#include <cstring>

namespace Axis::System
{

inline const Char* CopyMessageValue(const Char* message) noexcept
{
    auto stringLength = std::strlen(message);

    auto* newMessage = new (std::nothrow) Char[stringLength + 1];

    if (newMessage == nullptr)
        return nullptr;

    std::memcpy(newMessage, message, (stringLength * sizeof(Char)));

    newMessage[stringLength] = '\0';

    return newMessage;
}

inline void FreeMessage(const Char* message) noexcept
{
    delete[] message;
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

} // namespace Axis::System
