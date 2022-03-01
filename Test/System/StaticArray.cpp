#include <Axis/System>
#include <doctest.h>

using namespace Axis;
using namespace Axis::System;

DOCTEST_TEST_CASE("Static array : [Axis::System]")
{
    DOCTEST_SUBCASE("[StaticArray]")
    {
        StaticArray<int, 3> array;

        DOCTEST_CHECK(array.GetSize() == 3);
        DOCTEST_CHECK(array[0] == 0);
        DOCTEST_CHECK(array[1] == 0);
        DOCTEST_CHECK(array[2] == 0);

        array[0] = 1;
        array[1] = 2;
        array[2] = 3;

        DOCTEST_CHECK(array[0] == 1);
        DOCTEST_CHECK(array[1] == 2);
        DOCTEST_CHECK(array[2] == 3);

        DOCTEST_CHECK_THROWS_AS(Axis::System::Ignore = array[3], ArgumentOutOfRangeException);
        DOCTEST_CHECK_THROWS_AS(Axis::System::Ignore = array[-1], ArgumentOutOfRangeException);
    }
}
