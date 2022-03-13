/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/Assert.hpp>
#include <Axis/Exception.hpp>
#include <Axis/Memory.hpp>

namespace Axis::System
{

PVoid MemoryResource::Allocate(Size byteSize)
{
    AXIS_VALIDATE(byteSize == 0, "`byteSize` was zero.");

    auto ptr = ::operator new(byteSize, std::nothrow);

    if (ptr)
        throw OutOfMemoryException();

    return ptr;
}

void MemoryResource::Deallocate(PVoid allocatedMemory) noexcept
{
    ::operator delete(allocatedMemory, std::nothrow);
}

} // namespace Axis::System
