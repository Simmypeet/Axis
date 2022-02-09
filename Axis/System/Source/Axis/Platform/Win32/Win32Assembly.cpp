/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/SystemPch.hpp>

#include <Axis/Exception.hpp>
#include <Axis/Platform/Win32/Win32Assembly.hpp>
#include <Windows.h>

namespace Axis
{

namespace System
{

Assembly::Assembly(const StringView<WChar>& filePath)
{
    if (filePath == nullptr)
        throw InvalidArgumentException("filePath cannot be nullptr!");

    if (filePath.IsNullTerminated())
    {
        _libraryHandle = LoadLibraryW(filePath.GetCString());
    }
    else
    {
        WString filePathCopy(filePath);
        _libraryHandle = LoadLibraryW(filePathCopy.GetCString());
    }


    if (_libraryHandle == nullptr)
        throw ExternalException("Failed to load library!");
}

Assembly::~Assembly() noexcept
{
    if (_libraryHandle)
        FreeLibrary((HMODULE)_libraryHandle);
}

PVoid Assembly::LoadSymbol(const StringView<Char>& symbol)
{
    PVoid procAddress = nullptr;

    if (symbol.IsNullTerminated())
        procAddress = (PVoid)GetProcAddress((HMODULE)_libraryHandle, symbol.GetCString());
    else
    {
        String8 symbolString = symbol;
        procAddress          = (PVoid)GetProcAddress((HMODULE)_libraryHandle, symbolString.GetCString());
    }

    if (!procAddress)
        throw ExternalException("Failed to load symbol!");

    return (PVoid)procAddress;
}

} // namespace System

} // namespace Axis
