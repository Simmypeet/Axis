/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/SystemPch.hpp>

#include <Axis/Exception.hpp>
#include <Axis/Platform/Win32/Win32Assembly.hpp>
#include <Axis/Utility.hpp>
#include <Windows.h>

namespace Axis
{

Assembly::Assembly(const WChar* filePath)
{
    if (filePath == nullptr)
        throw InvalidArgumentException("filePath cannot be nullptr!");

    _libraryHandle = LoadLibraryW(filePath);

    if (_libraryHandle == nullptr)
        throw ExternalException("Failed to load library!");
}

Assembly::~Assembly() noexcept
{
    if (_libraryHandle)
        FreeLibrary((HMODULE)_libraryHandle);
}

PVoid Assembly::LoadSymbol(const Char* symbol)
{
    auto procAddress = GetProcAddress((HMODULE)_libraryHandle, symbol);

    if (!procAddress)
        throw ExternalException("Failed to load symbol!");

    return (PVoid)procAddress;
}

} // namespace Axis