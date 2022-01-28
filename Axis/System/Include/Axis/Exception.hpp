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

/// \brief Raised when external error occurs.
class AXIS_SYSTEM_API ExternalException : public Exception
{
public:
    using Exception::Exception;
};

/// \brief Raised when a method call is invalid in an object's current state.
class AXIS_SYSTEM_API InvalidOperationException : public Exception
{
public:
    using Exception::Exception;
};

/// \brief Raised when passed argument is invalid.
class AXIS_SYSTEM_API InvalidArgumentException : public Exception
{
public:
    using Exception::Exception;
};

/// \brief Raised when passed argument is out of expected range.
class AXIS_SYSTEM_API ArgumentOutOfRangeException : public InvalidArgumentException
{
public:
    using InvalidArgumentException::InvalidArgumentException;
};

/// \brief Raised when requested memory is not sufficient.
class AXIS_SYSTEM_API OutOfMemoryException : public Exception
{
public:
    using Exception::Exception;
};

/// \brief The exception that is thrown when an I/O error occurs.
class AXIS_SYSTEM_API IOException : public Exception
{
public:
    using Exception::Exception;
};

/// \brief The exception that is thrown when the operating system denies access because
///        of an I/O error or a specific type of security error.
class AXIS_SYSTEM_API UnauthorizedAccessException : public IOException
{
public:
    using IOException::IOException;
};

/// \brief The exception that is thrown when an attempt to access a file that does not exist on disk fails.
class AXIS_SYSTEM_API FileNotFoundException : public IOException
{
public:
    using IOException::IOException;
};

} // namespace Axis

#endif // AXIS_SYSTEM_EXCEPTION_HPP