#include <Axis/Rectangle.hpp>
#include <doctest.h>

DOCTEST_TEST_CASE("Rectangle data structure : [Axis::System]")
{
    DOCTEST_SUBCASE("`Axis::Rectangle `Constructors")
    {
        DOCTEST_SUBCASE("Default constructor")
        {
            using namespace Axis;

            Rectangle<Int32> rectangle;

            DOCTEST_CHECK(rectangle.X == 0);
            DOCTEST_CHECK(rectangle.Y == 0);
            DOCTEST_CHECK(rectangle.Width == 0);
            DOCTEST_CHECK(rectangle.Height == 0);
        }

        DOCTEST_SUBCASE("Constructor with defined values")
        {
            using namespace Axis;

            Rectangle<Int32> rectangle(1, 2, 3, 4);

            DOCTEST_CHECK(rectangle.X == 1);
            DOCTEST_CHECK(rectangle.Y == 2);
            DOCTEST_CHECK(rectangle.Width == 3);
            DOCTEST_CHECK(rectangle.Height == 4);
        }

        DOCTEST_SUBCASE("Copy constructor")
        {
            using namespace Axis;

            Rectangle<Int32> rectangle(1, 2, 3, 4);
            Rectangle<Int32> rectangleCopy(rectangle);

            DOCTEST_CHECK(rectangleCopy.X == 1);
            DOCTEST_CHECK(rectangleCopy.Y == 2);
            DOCTEST_CHECK(rectangleCopy.Width == 3);
            DOCTEST_CHECK(rectangleCopy.Height == 4);
        }
    }

    DOCTEST_SUBCASE("Equality comparisons")
    {
        using namespace Axis;

        Rectangle<Int32> rectangle(1, 2, 3, 4);
        Rectangle<Int32> rectangleCopy(rectangle);
        Rectangle<Int32> anotherRect(2, 3, 4, 5);

        DOCTEST_CHECK(rectangle == rectangleCopy);
        DOCTEST_CHECK(!(rectangle == anotherRect));
    }

    DOCTEST_SUBCASE("Inequality comparisons")
    {
        using namespace Axis;

        Rectangle<Int32> rectangle(1, 2, 3, 4);
        Rectangle<Int32> rectangleCopy(rectangle);
        Rectangle<Int32> anotherRect(2, 3, 4, 5);

        DOCTEST_CHECK(!(rectangle != rectangleCopy));
        DOCTEST_CHECK(rectangle != anotherRect);
    }
}