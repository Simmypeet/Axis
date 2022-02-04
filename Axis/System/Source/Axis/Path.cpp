/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/SystemPch.hpp>

#include <Axis/Path.hpp>

namespace Axis
{

namespace Path
{

Bool IsPathValid(const StringView<WChar>& path)
{
    Bool foundSeperators = false;

    const wchar_t* current = path.GetCString();

// In Win32 platform if the path is rooted, it contains drive letters (A-Z followed by : and directory seperator)
#ifdef AXIS_PLATFORM_WINDOWS
    if ((*current >= 65 && *current <= 90 && *(current + 1) == L':') && (*(current + 2) == L'\\' || *(current + 2) == L'/'))
        current += 3;
#endif

    while (*current != 0)
    {
        const wchar_t currentChar = *current;

        // Found invalid characters in path
        for (Size i = 0; i < InvalidPathCharacters.GetLength(); i++)
        {
            if (currentChar == InvalidPathCharacters[i])
                return false;
        }

        // Two consecutive directory seperators are invalid
        if (currentChar == L'\\' || currentChar == L'/')
        {
            if (foundSeperators)
                return false;

            foundSeperators = true;
        }
        else
            foundSeperators = false;

        current++;
    }

    return true;
}

WString CombinePath(const Span<WString>& paths)
{
    // nullptr span, returns empty string
    if (!paths)
        return {};

    Size    pathSizeToReserve = 0;
    WString combinedPath      = {};

    for (const auto& path : paths)
    {
        if (!IsPathValid(path))
            return {};

        pathSizeToReserve = path.GetLength();
    }

    // Reserve heap memory
    combinedPath.ReserveFor(pathSizeToReserve);

    for (const auto& path : paths)
    {
        auto pathToCombine = path.GetCString();

        // Trailing directory seperator at the start
        if (pathToCombine[0] == '/' || pathToCombine[0] == '\\')
            pathToCombine++;

        // Adds to the path
        combinedPath += path;

        if (combinedPath[combinedPath.GetLength() - 1] != '\\' && combinedPath[combinedPath.GetLength() - 1] != '/')
        {
            combinedPath += DirectorySeparatorString;
        }
    }

    if (combinedPath[combinedPath.GetLength() - 1] == '\\' || combinedPath[combinedPath.GetLength() - 1] == '/')
    {
        // Removes trailing directory seperator
        combinedPath.RemoveAt(combinedPath.GetLength() - 1);
    }


    return combinedPath;
}

} // namespace Path

} // namespace Axis
