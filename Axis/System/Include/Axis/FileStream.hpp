/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_FILESTREAM_HPP
#define AXIS_FILESTREAM_HPP
#pragma once

#include "Config.hpp"
#include "Stream.hpp"
#include "StringView.hpp"
#include "SystemExport.hpp"
#include "Utility.hpp"

namespace Axis
{

namespace System
{

/// \brief Specifies how the operating system should open the file.
enum class FileMode : Uint8
{
    /// \brief Opens a file for reading.
    Read = AXIS_BIT(1),

    /// \brief Opens a file for writing.
    Write = AXIS_BIT(2),

    /// \brief Operation is performed in binary mode rather than text.
    Binary = AXIS_BIT(3),

    /// \brief The position of the stream pointer is set to the end of the file.
    AtTheEnd = AXIS_BIT(4),

    /// \brief All write operations would be done at the end of the file, appending to its existing contents. Can't be used with \a `Truncate`.
    ///        Supports only \a `Write`.
    Append = AXIS_BIT(5),

    /// \brief Any contents that existed in the file before it is open are discarded. Can't be used with \a `Append`.
    ///        Requires \a `Write`.
    Truncate = AXIS_BIT(6),
};

/// \brief Specifies how the operating system should open the file. (Bit mask)
typedef FileMode FileModeFlags;

/// \brief Provides a Stream for a file, supporting both synchronous
///        and asynchronous read and write operations.
class AXIS_SYSTEM_API FileStream final : public Stream
{
public:
    /// \brief Creates a new instance of the FileStream class.
    ///
    /// \param[in] fileName The name of the file to open or create.
    /// \param[in] fileMode Specifies how a file is to be opened .
    ///
    /// \see Axis::FileMode, Axis::FileAccess
    FileStream(const StringView<WChar>& filePath,
               FileModeFlags            fileMode);

    /// \brief Destructor
    ~FileStream() noexcept override final;

    /// \brief Move Constructor
    ///
    /// \param[in] other The other FileStream object to move from.
    ///
    /// This causes the other FileStream object to be moved into this object and become invalid.
    FileStream(FileStream&&) noexcept;

    /// \brief Move Assignment Operator
    ///
    /// \param[in] other The other FileStream object to move from.
    ///
    /// This causes the other FileStream object to be moved into this object and become invalid.
    FileStream& operator=(FileStream&&) noexcept;

    /// \brief Gets a value indicating whether the current stream is readable.
    Bool CanRead() const override final;

    /// \brief Gets a value indicating whether the current stream is writable.
    Bool CanWrite() const override final;

    /// \brief Gets a value indicating whether the current stream can seek.
    Bool CanSeek() const override final;

    /// \brief Gets the current position in the stream.
    Size GetPosition() const override final;

    /// \brief Gets the length in bytes of the stream.
    Size GetSize() const override final;

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
    Size Read(PVoid buffer,
              Size  offset,
              Size  count) override final;

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
    Size Write(PVoid buffer,
               Size  offset,
               Size  count) override final;

    /// \brief Sets the position within the current stream.
    ///
    /// \param offset A byte offset relative to the origin parameter.
    /// \param origin The origin from which the offset is calculated.
    ///
    /// \return The new position within the stream.
    Size Seek(Int64      offset,
              SeekOrigin origin) override final;

    /// \brief Checks if the stream is open.
    ///
    /// \return True if the stream is open, false otherwise.
    inline Bool IsOpen() const noexcept { return _fileHandle != nullptr; }

    /// \brief Gets \a `FILE*` handle of the stream.
    ///
    /// \return \a `FILE*` handle of the stream.
    inline FILE* GetFileHandle() const noexcept { return _fileHandle; }

    /// \brief Gets mode flags of the stream.
    ///
    /// \return Mode flags of the stream.
    inline FileModeFlags GetFileModes() const noexcept { return _fileModes; }

private:
    FILE*         _fileHandle = nullptr; ///< Low-level C file handle.
    WString       _filePath   = {};      ///< File path.
    FileModeFlags _fileModes  = {};      ///< File mode used during construction.
};

} // namespace System

} // namespace Axis

#endif // AXIS_FILESTREAM_HPP
