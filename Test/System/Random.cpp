#include <Axis/System>
#include <doctest.h>

DOCTEST_TEST_CASE("Random data structure : [Axis-System]")
{
    DOCTEST_SUBCASE("`Axis::Random` constructors")
    {
        using namespace Axis;

        // Constructs a random number generator with same seed
        // should yield the same results

        Random random1(1);
        Random random2(1);

        DOCTEST_CHECK(random1.Next() == random2.Next()); // Attemp 1
        DOCTEST_CHECK(random1.Next() == random2.Next()); // Attemp 2
        DOCTEST_CHECK(random1.Next() == random2.Next()); // Attemp 3
        DOCTEST_CHECK(random1.Next() == random2.Next()); // Attemp 4
    }
}