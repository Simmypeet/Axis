/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Exception.hpp
///
/// \brief Contains difinitions for the exception classes.

#ifndef AXIS_SYSTEM_EXCEPTION_HPP
#define AXIS_SYSTEM_EXCEPTION_HPP
#pragma once

#include "SystemExport.hpp"
#include "Trait.hpp"

namespace Axis
{

/// \brief Defines the base class for all exceptions.
class AXIS_SYSTEM_API Exception
{
public:
    /// \brief Default constructor
    Exception() noexcept = default;

    /// \brief Copy constructor
    Exception(const Exception& other) noexcept;

    /// \brief Move constructor
    Exception(Exception&& other) noexcept;

    /// \brief Constructs exception object with custom message.
    ///
    /// \param[in] message Pointer to null terminated message.
    Exception(const Char* message) noexcept;

    /// \brief Virtual destructor
    virtual ~Exception() noexcept;

    /// \brief Copy assignment operator
    Exception& operator=(const Exception& other) noexcept;

    /// \brief Move assignment operator
    Exception& operator=(Exception&& other) noexcept;

    /// \brief Returns the message of the exception.
    inline const Char* GetMessage() const noexcept { return _message; }

private:
    const Char* _message = nullptr; // The message of the exception.
};

/// \brief Raised when a null value pointer is de-referenced.
class AXIS_SYSTEM_API NullPointerException : public Exception
{
public:
    /// \brief Constructs exception object with custom message.
    NullPointerException(const Char* message) noexcept;
};

/// \brief Raised when external error occurs.
class AXIS_SYSTEM_API ExternalException : public Exception
{
public:
    /// \brief Constructs exception object with custom message.
    ExternalException(const Char* message) noexcept;
};

/// \brief Raised when a method call is invalid in an object's current state.
class AXIS_SYSTEM_API InvalidOperationException : public Exception
{
public:
    /// \brief Constructs exception object with custom message.
    InvalidOperationException(const Char* message) noexcept;
};

/// \brief Raised when passed argument is invalid.
class AXIS_SYSTEM_API InvalidArgumentException : public Exception
{
public:
    /// \brief Constructs exception object with custom message.
    InvalidArgumentException(const Char* message) noexcept;
};

/// \brief Raised when passed argument is out of expected range.
class AXIS_SYSTEM_API ArgumentOutOfRangeException : public Exception
{
public:
    /// \brief Constructs exception object with custom message.
    ArgumentOutOfRangeException(const Char* message) noexcept;
};

/// \brief Raised when requested memory is not sufficient.
class AXIS_SYSTEM_API OutOfMemoryException : public Exception
{};

} // namespace Axis

#endif // AXIS_SYSTEM_EXCEPTION_HPP