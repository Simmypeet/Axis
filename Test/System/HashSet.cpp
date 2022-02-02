#include <Axis/System>
#include <doctest.h>

DOCTEST_TEST_CASE("Hash set : [Axis::System]")
{
    using namespace Axis;
    DOCTEST_SUBCASE("Insert and Remove")
    {
        HashSet<Int32> set;

        set.Insert(1);
        set.Insert(2);
        set.Insert(3);
        set.Insert(4);
        set.Insert(5);

        // Inserts duplicates
        set.Insert(1);
        set.Insert(2);

        // We should have only 5 elements
        CHECK(set.GetSize() == 5);

        // Element 3 should be in the set
        CHECK(set.Find(3) != set.end());

        // Element 6 should not be in the set
        CHECK(set.Find(6) == set.end());

        set.Remove(1);

        // We should have only 4 elements
        CHECK(set.GetSize() == 4);

        // Element 1 should not be in the set
        CHECK(set.Find(1) == set.end());

        DOCTEST_SUBCASE("Copy constructor")
        {
            HashSet<Int32> set2(set);

            // We should have only 4 elements
            CHECK(set2.GetSize() == 4);

            // Element 1 should not be in the set
            CHECK(set2.Find(1) == set2.end());
        }

        DOCTEST_SUBCASE("Move constructor")
        {
            HashSet<Int32> set2(std::move(set));

            // We should have only 4 elements
            CHECK(set2.GetSize() == 4);

            // Element 1 should not be in the set
            CHECK(set2.Find(1) == set2.end());
        }
    }
}