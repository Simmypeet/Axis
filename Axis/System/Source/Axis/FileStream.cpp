/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/SystemPch.hpp>

#include <Axis/Enum.hpp>
#include <Axis/Exception.hpp>
#include <Axis/FileStream.hpp>
#include <corecrt_wstdio.h>
#include <cstdio>
#include <errno.h>


namespace Axis
{

namespace System
{

FileStream::FileStream(const StringView<WChar>& filePath,
                       FileModeFlags            fileModes) :
    _filePath(filePath),
    _fileModes(fileModes)
{
    // Checks if the file path is valid.
    if (filePath == nullptr)
        throw InvalidArgumentException("`filePath` was nullptr!");

    // If both truncate and append are specified, throw an exception.
    if ((Bool)(_fileModes & FileMode::Append) && (Bool)(_fileModes & FileMode::Truncate))
        throw InvalidArgumentException("Invalid file mode, both `Append` and `Truncate` were specified!");

    // If truncate is specified, and write is not specified, throw an exception.
    if ((Bool)(_fileModes & FileMode::Truncate) && !(Bool)(_fileModes & FileMode::Write))
        throw InvalidArgumentException("Invalid file mode, `Truncate` was specified, but `Write` was not!");

    // If append is specified, and write is not specified, throw an exception.
    if ((Bool)(_fileModes & FileMode::Append) && !(Bool)(_fileModes & FileMode::Write))
        throw InvalidArgumentException("Invalid file mode, `Append` was specified, but `Write` was not!");

#ifdef AXIS_PLATFORM_WINDOWS
    const wchar_t* mode = nullptr;
#    define AXIS_STRING_LITERALS(str) L#    str
#else
    const char* mode      = nullptr;
#    define AXIS_STRING_LITERALS(str) #    str
#endif

    // clang-format off

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#endif

    switch (fileModes)
    {
        case FileMode::Read:
            mode = AXIS_STRING_LITERALS(r);
            break;

        case FileMode::Write:
        case FileMode::Write | FileMode::Truncate:
            mode = AXIS_STRING_LITERALS(w);
            break;

        case FileMode::Write | FileMode::Append:
            mode = AXIS_STRING_LITERALS(a);
            break;

        case FileMode::Read | FileMode::Write:
            mode = AXIS_STRING_LITERALS(r+);
            break;

        case FileMode::Read | FileMode::Write | FileMode::Truncate:
            mode = AXIS_STRING_LITERALS(w+);
            break;

        case FileMode::Read | FileMode::Write | FileMode::Append:
            mode = AXIS_STRING_LITERALS(a+);
            break;

        case FileMode::Binary | FileMode::Read:
            mode = AXIS_STRING_LITERALS(rb);
            break;

        case FileMode::Binary | FileMode::Write:
        case FileMode::Binary | FileMode::Write | FileMode::Truncate:
            mode = AXIS_STRING_LITERALS(wb);
            break;

        case FileMode::Binary | FileMode::Write | FileMode::Append:
            mode = AXIS_STRING_LITERALS(ab);
            break;

        case FileMode::Binary | FileMode::Write | FileMode::Read:
            mode = AXIS_STRING_LITERALS(r+b);
            break;

        case FileMode::Binary | FileMode::Write | FileMode::Truncate | FileMode::Read:
            mode = AXIS_STRING_LITERALS(w+b);
            break;

        case FileMode::Binary | FileMode::Write | FileMode::Append | FileMode::Read:
            mode = AXIS_STRING_LITERALS(a+b);
            break;

        default:
            throw InvalidArgumentException("Invalid file mode.");
    }

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

        // clang-format on

#ifdef AXIS_PLATFORM_WINDOWS
    auto errorCode = _wfopen_s(&_fileHandle, _filePath.GetCString(), mode);
#else
    auto        errorCode = fopen_s(&_fileHandle, _filePath, mode);
#endif
    if (errorCode == EACCES)
        throw UnauthorizedAccessException("Access denied!");
    else if (errorCode == ENOENT)
        throw FileNotFoundException("File not found!");
    else if (errorCode != 0)
        throw IOException("Failed to open file!");
}

FileStream::~FileStream() noexcept
{
    if (_fileHandle != nullptr)
        fclose(_fileHandle);
}

FileStream::FileStream(FileStream&& other) noexcept :
    _fileHandle(other._fileHandle),
    _filePath(std::move(other._filePath)),
    _fileModes(other._fileModes)
{
    other._fileHandle = nullptr;
    other._fileModes  = (FileMode)0;
}

FileStream& FileStream::operator=(FileStream&& other) noexcept
{
    if (this != &other)
        return *this;

    if (_fileHandle != nullptr)
        fclose(_fileHandle);

    _fileHandle = other._fileHandle;
    _filePath   = std::move(other._filePath);
    _fileModes  = other._fileModes;

    other._fileHandle = nullptr;
    other._fileModes  = (FileMode)0;

    return *this;
}

Bool FileStream::CanRead() const
{
    if (_fileHandle == nullptr)
        throw InvalidOperationException("File stream was not opened!");

    return (Bool)(_fileModes & FileMode::Read);
}

Bool FileStream::CanWrite() const
{
    if (_fileHandle == nullptr)
        throw InvalidOperationException("File stream was not opened!");

    return (Bool)(_fileModes & FileMode::Write);
}

Bool FileStream::CanSeek() const
{
    if (_fileHandle == nullptr)
        throw InvalidOperationException("File stream was not opened!");

    return true;
}

Size FileStream::GetPosition() const
{
    if (_fileHandle == nullptr)
        throw InvalidOperationException("FileStream was not opened!");

    auto result = ftell(_fileHandle);

    if (result == -1)
        throw IOException("Failed to get file position!");
    else
        return result;
}

Size FileStream::GetLength() const
{
    if (_fileHandle == nullptr)
        throw InvalidOperationException("FileStream was not opened!");

    // Seeks to the end of the file. If successful, returns the position of the end of the file.
    // Also have to seek back to the original position.

    auto originalPosition = ftell(_fileHandle);

    if (originalPosition == -1)
        throw IOException("Failed to get file position!");

    auto result = fseek(_fileHandle, 0, SEEK_END);

    if (result != 0)
        throw IOException("Failed to seek to end of file!");

    auto length = ftell(_fileHandle);

    if (length == -1)
        throw IOException("Failed to get file length!");

    result = fseek(_fileHandle, originalPosition, SEEK_SET);

    if (result != 0)
        throw IOException("Failed to seek back to original position!");

    return length;
}

Size FileStream::Read(PVoid buffer,
                      Size  offset,
                      Size  count)
{
    if (_fileHandle == nullptr)
        throw InvalidOperationException("File stream was not opened!");

    auto result = fread(buffer, 1, count, _fileHandle);

    if (result == -1)
        throw IOException("Failed to read from file!");

    return result;
}

Size FileStream::Write(PVoid buffer,
                       Size  offset,
                       Size  count)
{
    if (_fileHandle == nullptr)
        throw InvalidOperationException("File stream was not opened!");

    auto result = fwrite(buffer, 1, count, _fileHandle);

    if (result == -1)
        throw IOException("Failed to write to file!");

    return result;
}

Size FileStream::Seek(Int64      offset,
                      SeekOrigin origin)
{
    if (_fileHandle == nullptr)
        throw InvalidOperationException("File stream was not opened!");

    int originInt = 0;

    switch (origin)
    {
        case SeekOrigin::Begin:
            originInt = SEEK_SET;
            break;

        case SeekOrigin::Current:
            originInt = SEEK_CUR;
            break;

        case SeekOrigin::End:
            originInt = SEEK_END;
            break;
    }

    auto result = fseek(_fileHandle, (long)offset, originInt);

    if (result != 0)
        throw IOException("Failed to seek in file!");

    return GetPosition();
}

} // namespace System

} // namespace Axis
