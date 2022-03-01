/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_SYSTEM_STREAM
#define AXIS_SYSTEM_STREAM
#pragma once

#include "SmartPointer.hpp"
#include "SystemExport.hpp"


namespace Axis
{

namespace System
{

/// \brief Specifies the position in a stream to use for seeking.
enum class SeekOrigin
{
    /// \brief The origin is at the beginning of the stream.
    Begin,

    /// \brief The origin is at the current position in the stream.
    Current,

    /// \brief The origin is at the end of the stream.
    End
};


/// \brief Provides a generic view of a sequence of bytes. This is an abstract class.
class AXIS_SYSTEM_API Stream : public ISharedFromThis
{
public:
    /// \brief Gets a value indicating whether the current stream is readable.
    virtual Bool CanRead() const = 0;

    /// \brief Gets a value indicating whether the current stream is writable.
    virtual Bool CanWrite() const = 0;

    /// \brief Gets a value indicating whether the current stream can seek.
    virtual Bool CanSeek() const = 0;

    /// \brief Gets the current position in the stream.
    virtual Size GetPosition() const = 0;

    /// \brief Gets the length in bytes of the stream.
    virtual Size GetSize() const = 0;

    /// \brief Reads a sequence of bytes from the current stream and
    ///        advances the position within the stream by the number of bytes read.
    ///
    /// \param buffer The buffer to read the bytes into.
    /// \param offset The zero-based byte offset in buffer at which to begin storing the data read from the current stream.
    /// \param count The maximum number of bytes to be read.
    ///
    /// \return The total number of bytes read into the buffer.
    ///         This can be less than the number of bytes requested if that many bytes are not currently available,
    ///         or zero (0) if the end of the stream has been reached.
    virtual Size Read(PVoid buffer,
                      Size  offset,
                      Size  count) = 0;

    /// \brief Writes a sequence of bytes to the current stream and
    ///        advances the current position within the stream by the number of bytes written.
    ///
    /// \param buffer The buffer to write the bytes from.
    /// \param offset The zero-based byte offset in buffer at which to begin copying bytes to the current stream.
    /// \param count The number of bytes to be written to the current stream.
    ///
    /// \return The total number of bytes written into the buffer.
    ///         This can be less than the number of bytes requested if that many bytes are not currently available,
    ///         or zero (0) if the end of the stream has been reached.
    virtual Size Write(PVoid buffer,
                       Size  offset,
                       Size  count) = 0;

    /// \brief Sets the position within the current stream.
    ///
    /// \param offset A byte offset relative to the origin parameter.
    /// \param origin The origin from which the offset is calculated.
    ///
    /// \return The new position within the stream.
    ///
    virtual Size Seek(Int64      offset,
                      SeekOrigin origin) = 0;
};

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_STREAM