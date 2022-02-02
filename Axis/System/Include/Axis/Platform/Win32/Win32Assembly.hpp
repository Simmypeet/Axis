/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

/// \file Win32Assembly.hpp
///
/// \brief Contains definition of `Axis::Assembly` class in Win32 platform.

#ifndef AXIS_SYSTEM_WIN32ASSEMBLY_HPP
#define AXIS_SYSTEM_WIN32ASSEMBLY_HPP
#pragma once

#include "../../SmartPointer.hpp"
#include "../../SystemExport.hpp"

namespace Axis
{

/// \brief Represents the dynamically loaded shared library.
///
/// In win32 platform this represents "dynamic loaded library" (DLL)
class AXIS_SYSTEM_API Assembly final : public ISharedFromThis
{
public:
    /// \brief Loads the assembly from the specified filePath.
    ///
    /// \param[in] filePath Path to load the DLL
    /// \param[out] resultCode ResultCode for reporting errors
    ///
    /// \pre filePath should not be nullptr
    explicit Assembly(const WChar* filePath);

    /// \brief Destructor
    ~Assembly() noexcept;

    /// \brief Loads the function symbol contained within the assembly.
    ///
    /// \param[in] symbol Symbol name to load
    AXIS_NODISCARD PVoid LoadSymbol(const Char* symbol);

private:
    PVoid _libraryHandle = nullptr;
};

} // namespace Axis

#endif // AXIS_SYSTEM_WIN32ASSEMBLY_HPP