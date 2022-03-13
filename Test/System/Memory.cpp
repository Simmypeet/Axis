/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include "LeakTester.hpp"
#include <Axis/Memory.hpp>
#include <doctest/doctest.h>

namespace Axis::System
{

template <Size ThrowAt>
struct Thrower
{
    using TargetLeakTester = LeakTester<Thrower<ThrowAt>, true, true>;

    Thrower()
    {
        if (TargetLeakTester::GetInstanceCount() == ThrowAt)
        {
            throw ThrowAt;
        }
    }
};

DOCTEST_TEST_CASE("MemoryResource : [Voltaic::System]")
{

    DOCTEST_SUBCASE("New and Delete")
    {
        using Type = LeakTester<Size, true, true>;

        DOCTEST_CHECK(Type::GetInstanceCount() == 0);
        {
            Type* instance = New<Type>(2);
            DOCTEST_CHECK(instance->Instance == 2);
            DOCTEST_CHECK(Type::GetInstanceCount() == 1);
            Delete<Type>(instance);
        }
        DOCTEST_CHECK(Type::GetInstanceCount() == 0);
    }

    DOCTEST_SUBCASE("NewArray and DeleteArray")
    {
        using Type = LeakTester<Size, true, true>;

        DOCTEST_CHECK(Type::GetInstanceCount() == 0);
        {
            constexpr Size ArraySize = 32;

            Type* instance = NewArray<Type>(ArraySize, 0);

            DOCTEST_CHECK(Type::GetInstanceCount() == 32);

            for (Size i = 0; i < ArraySize; ++i)
            {
                DOCTEST_CHECK(instance[i].Instance == 0);
            }

            DeleteArray<Type>(instance);
        }
        DOCTEST_CHECK(Type::GetInstanceCount() == 0);
    }

    DOCTEST_SUBCASE("NewArray with exceptions")
    {
        constexpr Size ThrowAt = 5;
        using ThrowerType      = Thrower<ThrowAt>;
        using Type             = ThrowerType::TargetLeakTester;

        DOCTEST_CHECK(Type::GetInstanceCount() == 0);
        {
            constexpr Size ArraySize = 32;

            try
            {
                auto _ = NewArray<Type>(ArraySize);
            }
            catch (Size& val)
            {
                DOCTEST_CHECK(val == ThrowAt);
            }
        }
        DOCTEST_CHECK(Type::GetInstanceCount() == 0);
    }
}

} // namespace Axis::System
