/////////////////////////////////////////////////////////////////
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/SystemPch.hpp>

#include <Axis/Path.hpp>
#include <Shlwapi.h>
#include <Windows.h>

namespace Axis
{

namespace Path
{

WString GetExecutableFilePath()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return buffer;
}

WString GetExecutableDirectoryPath()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    PathRemoveFileSpecW(buffer);
    return buffer;
}

} // namespace Path

} // namespace Axis
