/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Path.hpp
///
/// \brief Contains utility functions for working with paths and directories.

#ifndef AXIS_SYSTEM_PATH_HPP
#define AXIS_SYSTEM_PATH_HPP
#pragma once

#include "Span.hpp"
#include "StaticArray.hpp"
#include "String.hpp"
#include "StringView.hpp"
#    include "SystemExport.hpp"

namespace Axis
{

namespace Path
{

#ifdef AXIS_PLATFORM_WINDOWS

/// \brief Character use to separate the directory.
constexpr WChar DirectorySeparator = L'\\';

/// \brief Character use to separate the directory.
constexpr const WChar* DirectorySeparatorString = L"\\";

/// \brief List of invalid characters that should not appear in the
///        directory and file names.
constexpr StaticArray<WChar, 7> InvalidPathCharacters = {L':', L'*', L'?', L'\"', L'<', L'>', L'|'};

#else

/// \brief Character use to separator the directory.
constexpr WChar DirectorySeparator = L'\\';

/// \brief Character use to separator the directory.
constexpr WChar* DirectorySeparatorString = L"\\";

#endif

/// \brief Gets the full path to the executable file.
AXIS_NODISCARD WString AXIS_SYSTEM_API GetExecutableFilePath();

/// \brief Gets the full path to the executable directory.
AXIS_NODISCARD WString AXIS_SYSTEM_API GetExecutableDirectoryPath();

/// \brief Checks if the specified file path is valid or not.
///        (e.g., contains invalid characters, Float64 directory seperations)
///
/// \return true if file path is valid, false otherwise
AXIS_NODISCARD Bool AXIS_SYSTEM_API IsPathValid(const StringView<WChar>& path);

/// \brief Combines the file paths.
///
/// \param[in] paths Paths to combine.
///
/// \return Returns a valid file path upon successful combination, empty string otherwise.
AXIS_NODISCARD WString AXIS_SYSTEM_API CombinePath(const Span<WString>& paths);

} // namespace Path

} // namespace Axis

#endif // AXIS_SYSTEM_PATH_HPP
