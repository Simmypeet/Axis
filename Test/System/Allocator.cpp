/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include "LeakTester.hpp"
#include <Axis/Allocator.hpp>
#include <doctest/doctest.h>

namespace Axis::System
{

DOCTEST_TEST_CASE("Allocator : [Axis::System]")
{
    using ValueType           = LeakTester<Size, true, true>;
    using AllocatorType       = Allocator<ValueType, DefaultMemoryResource>;
    using AllocatorTraitsType = AllocatorTraits<AllocatorType>;

    AllocatorType allocator = {};

    constexpr Size ElementCount = 32;

    DOCTEST_CHECK(ValueType::GetInstanceCount() == 0);

    // Allocates memory for 32 elements
    auto pointer = AllocatorTraitsType::Allocate(allocator, 32);

    for (AllocatorType::SizeType i = 0; i < ElementCount; ++i)
    {
        AllocatorTraitsType::Construct(allocator, pointer + i, i);
    }

    DOCTEST_CHECK(ValueType::GetInstanceCount() == 32);

    for (AllocatorType::SizeType i = 0; i < ElementCount; ++i)
    {
        DOCTEST_CHECK(pointer[i].Instance == i);
    }

    for (AllocatorType::SizeType i = 0; i < ElementCount; ++i)
    {
        AllocatorTraitsType::Destruct(allocator, pointer + i);
    }

    DOCTEST_CHECK(ValueType::GetInstanceCount() == 0);

    AllocatorTraitsType::Deallocate(allocator,
                                    pointer,
                                    ElementCount);
}

} // namespace Axis::System
