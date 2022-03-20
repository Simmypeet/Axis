/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include "LeakTester.hpp"
#include <Axis/Variant.hpp>
#include <doctest/doctest.h>

namespace Axis::System
{

DOCTEST_TEST_CASE("Variant : [Axis::System]")
{
    using LeakTesterType = LeakTester<Size, true, true>;

    DOCTEST_SUBCASE("Default constructor")
    {
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        {
            Variant<LeakTesterType, Bool> variant = {};

            DOCTEST_CHECK(variant.IsValueless() == true);
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
    }

    DOCTEST_SUBCASE("Construct function")
    {
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        {
            Variant<LeakTesterType, Bool> variant = {};

            DOCTEST_CHECK(variant.IsValueless() == true);
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

            variant.Construct<0>(1);

            DOCTEST_CHECK(variant.IsValueless() == false);
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

            variant.Construct<1>(true);

            DOCTEST_CHECK(variant.IsValueless() == false);
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
    }

    DOCTEST_SUBCASE("Copy constructor")
    {
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        {
            Variant<LeakTesterType, Bool> variant = {};

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

            DOCTEST_CHECK(variant.IsValueless() == true);

            variant.Construct<0>(1);

            DOCTEST_CHECK(variant.IsValueless() == false);
            DOCTEST_CHECK(variant.GetTypeIndex() == 0);
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

            Variant<LeakTesterType, Bool> anotherVariant = variant;

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 2);

            DOCTEST_CHECK(anotherVariant.IsValueless() == false);
            DOCTEST_CHECK(anotherVariant.GetTypeIndex() == 0);
        }
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
    }
}

} // namespace Axis::System
