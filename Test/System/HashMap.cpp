#include <Axis/System>
#include <doctest.h>

using namespace Axis;
using namespace Axis::System;

DOCTEST_TEST_CASE("Hash map : [Axis::System]")
{
    DOCTEST_SUBCASE("Insert and Remove")
    {
        HashMap<Int32, Bool> map;

        map.Insert({1, true});
        map.Insert({2, false});
        map.Insert({3, true});
        map.Insert({4, false});
        map.Insert({5, true});

        // Inserts duplicates
        map.Insert({1, true});

        // We should have only 5 elements
        CHECK(map.GetSize() == 5);

        // Element 3 should be in the set
        CHECK(map.Find(3) != map.end());

        // Element 6 should not be in the set
        CHECK(map.Find(6) == map.end());

        // Key 1 should have value of true
        CHECK(map.Find(1)->Second == true);

        // Key 2 should have value of false
        CHECK(map.Find(2)->Second == false);

        map.Remove(1);

        // We should have only 4 elements
        CHECK(map.GetSize() == 4);

        // Element 1 should not be in the set
        CHECK(map.Find(1) == map.end());

        DOCTEST_SUBCASE("Copy constructor")
        {
            HashMap<Int32, Bool> map2(map);

            // We should have only 4 elements
            CHECK(map2.GetSize() == 4);

            // Element 1 should not be in the set
            CHECK(map2.Find(1) == map2.end());
        }

        DOCTEST_SUBCASE("Move constructor")
        {
            HashMap<Int32, Bool> map2(std::move(map));

            // We should have only 4 elements
            CHECK(map2.GetSize() == 4);

            // Element 1 should not be in the set
            CHECK(map2.Find(1) == map2.end());
        }
    }
}
