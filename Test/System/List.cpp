#include <Axis/System>
#include <doctest.h>

using namespace Axis;
using namespace Axis::System;

DOCTEST_TEST_CASE("Axis list data structure : [Axis::System]")
{
    DOCTEST_SUBCASE("`Axis::List` constructors")
    {
        DOCTEST_SUBCASE("Constructor")
        {
            // Default list (empty)
            List<Int32> list;

            // Length should be 0
            CHECK(list.GetLength() == 0);

            // Begin and end should be equal
            CHECK(list.begin() == list.end());

            // Creates a list from initializer list
            List<Int32> list2 = {1, 2, 3, 4, 5};

            // Length should be 5
            CHECK(list2.GetLength() == 5);

            // Checks the value of each element
            CHECK(list2[0] == 1);
            CHECK(list2[1] == 2);
            CHECK(list2[2] == 3);
            CHECK(list2[3] == 4);
            CHECK(list2[4] == 5);

            // Creates a list with specified size and default value 
            List<Int32> list3(3, 0);

            // Length should be 3
            CHECK(list3.GetLength() == 3);

            // Checks the value of each element
            CHECK(list3[0] == 0);
            CHECK(list3[1] == 0);
            CHECK(list3[2] == 0);
        }

        DOCTEST_SUBCASE("Copy constructor")
        {

            // Creates a list from initializer list
            List<Int32> list = {1, 2, 3, 4, 5};

            // Creates a copy of the list
            List<Int32> list2(list);

            // Length should be 5
            CHECK(list.GetLength() == 5);

            // Checks the value of each element
            CHECK(list[0] == 1);
            CHECK(list[1] == 2);
            CHECK(list[2] == 3);
            CHECK(list[3] == 4);
            CHECK(list[4] == 5);

            // Length should be 5
            CHECK(list2.GetLength() == 5);

            // Checks the value of each element
            CHECK(list2[0] == 1);
            CHECK(list2[1] == 2);
            CHECK(list2[2] == 3);
            CHECK(list2[3] == 4);
            CHECK(list2[4] == 5);
        }

        DOCTEST_SUBCASE("Move constructor")
        {
            // Creates a list from initializer list
            List<Int32> list = {1, 2, 3, 4, 5};

            // Creates a copy of the list
            List<Int32> list2(std::move(list));

            // Length of moved list should be 0
            CHECK(list.GetLength() == 0);

            // Length should be 5
            CHECK(list2.GetLength() == 5);

            // Checks the value of each element
            CHECK(list2[0] == 1);
            CHECK(list2[1] == 2);
            CHECK(list2[2] == 3);
            CHECK(list2[3] == 4);
            CHECK(list2[4] == 5);
        }
    }

    DOCTEST_SUBCASE("Assignment operator")
    {
        DOCTEST_SUBCASE("Copy assignment")
        {
            // Creates a list from initializer list
            List<Int32> list = {1, 2, 3, 4, 5};

            // Creates an another list from initializer list
            List<Int32> list2 = {6, 7, 8, 9, 10};

            // Checks the validity of list and list2
            CHECK(list.GetLength() == 5);
            CHECK(list2.GetLength() == 5);

            // Checks the value of each element
            CHECK(list[0] == 1);
            CHECK(list[1] == 2);
            CHECK(list[2] == 3);
            CHECK(list[3] == 4);
            CHECK(list[4] == 5);
            CHECK(list2[0] == 6);
            CHECK(list2[1] == 7);
            CHECK(list2[2] == 8);
            CHECK(list2[3] == 9);
            CHECK(list2[4] == 10);

            // Assigns list2 to list
            list = list2;

            // Checks the validity of list and list2
            CHECK(list.GetLength() == 5);
            CHECK(list2.GetLength() == 5);

            // Checks the value of each element
            CHECK(list[0] == 6);
            CHECK(list[1] == 7);
            CHECK(list[2] == 8);
            CHECK(list[3] == 9);
            CHECK(list[4] == 10);
            CHECK(list2[0] == 6);
            CHECK(list2[1] == 7);
            CHECK(list2[2] == 8);
            CHECK(list2[3] == 9);
            CHECK(list2[4] == 10);
        }

        DOCTEST_SUBCASE("Move assignment")
        {
            // Creates a list from initializer list
            List<Int32> list = {1, 2, 3, 4, 5};

            // Creates an another list from initializer list
            List<Int32> list2 = {6, 7, 8, 9, 10};

            // Checks the validity of list and list2
            CHECK(list.GetLength() == 5);
            CHECK(list2.GetLength() == 5);

            // Checks the value of each element
            CHECK(list[0] == 1);
            CHECK(list[1] == 2);
            CHECK(list[2] == 3);
            CHECK(list[3] == 4);
            CHECK(list[4] == 5);
            CHECK(list2[0] == 6);
            CHECK(list2[1] == 7);
            CHECK(list2[2] == 8);
            CHECK(list2[3] == 9);
            CHECK(list2[4] == 10);

            // Assigns list2 to list
            list = std::move(list2);

            // Checks the validity of list and list2
            CHECK(list.GetLength() == 5);
            CHECK(list2.GetLength() == 0);

            // Checks the value of each element
            CHECK(list[0] == 6);
            CHECK(list[1] == 7);
            CHECK(list[2] == 8);
            CHECK(list[3] == 9);
            CHECK(list[4] == 10);
        }
    }

    DOCTEST_SUBCASE("Functions")
    {
        DOCTEST_SUBCASE("EmplaceBack")
        {
            struct Number
            {
                Number(Int32 value)
                {
                    if (value < 0)
                        throw value;

                    Value = value;
                }

                Bool operator==(const Number& other) const noexcept { return Value == other.Value; }

                Int32 Value = 0;
            };

            // Creates a list from initializer list
            List<Number> list = {1, 2, 3, 4, 5};

            // Checks the validity of list
            CHECK(list.GetLength() == 5);

            // Checks the value of each element
            CHECK(list[0].Value == 1);
            CHECK(list[1].Value == 2);
            CHECK(list[2].Value == 3);
            CHECK(list[3].Value == 4);
            CHECK(list[4].Value == 5);

            // Emplaces a new element at the end of the list
            list.EmplaceBack(6);
            list.EmplaceBack(7);
            list.EmplaceBack(8);
            list.EmplaceBack(9);
            list.EmplaceBack(10);

            // Checks the validity of list
            CHECK(list.GetLength() == 10);

            // Checks the value of each element
            CHECK(list[0].Value == 1);
            CHECK(list[1].Value == 2);
            CHECK(list[2].Value == 3);
            CHECK(list[3].Value == 4);
            CHECK(list[4].Value == 5);
            CHECK(list[5].Value == 6);
            CHECK(list[6].Value == 7);
            CHECK(list[7].Value == 8);
            CHECK(list[8].Value == 9);
            CHECK(list[9].Value == 10);

            try
            {
                list.EmplaceBack(-1);
            }
            catch (Int32 value)
            {
                CHECK(value == -1);
            }

            // The list should not be changed

            // Checks the validity of list
            CHECK(list.GetLength() == 10);

            // Checks the value of each element
            CHECK(list[0].Value == 1);
            CHECK(list[1].Value == 2);
            CHECK(list[2].Value == 3);
            CHECK(list[3].Value == 4);
            CHECK(list[4].Value == 5);
            CHECK(list[5].Value == 6);
            CHECK(list[6].Value == 7);
            CHECK(list[7].Value == 8);
            CHECK(list[8].Value == 9);
            CHECK(list[9].Value == 10);
        }

        DOCTEST_SUBCASE("Emplace")
        {
            struct Number
            {
                Number(Int32 value)
                {
                    if (value < 0)
                        throw value;

                    Value = value;
                }

                Number(const Number&) = default;

                Number(Number&&) = delete;

                Bool operator==(const Number& other) const noexcept { return Value == other.Value; }

                Int32 Value = 0;
            };

            constexpr auto val = std::is_copy_constructible_v<Number>;

            // Creates a list from initializer list
            List<Number> list = {1, 2, 3, 4, 5};

            // Checks the validity of list
            CHECK(list.GetLength() == 5);

            // Checks the value of each element
            CHECK(list[0].Value == 1);
            CHECK(list[1].Value == 2);
            CHECK(list[2].Value == 3);
            CHECK(list[3].Value == 4);
            CHECK(list[4].Value == 5);

            // Emplaces a new element at the end of the list
            list.Emplace(5, 10);
            list.Emplace(5, 9);
            list.Emplace(5, 8);
            list.Emplace(5, 7);
            list.Emplace(5, 6);

            // Checks the validity of list
            CHECK(list.GetLength() == 10);

            // Checks the value of each element
            CHECK(list[0].Value == 1);
            CHECK(list[1].Value == 2);
            CHECK(list[2].Value == 3);
            CHECK(list[3].Value == 4);
            CHECK(list[4].Value == 5);
            CHECK(list[5].Value == 6);
            CHECK(list[6].Value == 7);
            CHECK(list[7].Value == 8);
            CHECK(list[8].Value == 9);
            CHECK(list[9].Value == 10);

            try
            {
                list.Emplace(1, -1);
            }
            catch (Int32 value)
            {
                CHECK(value == -1);
            }

            // The list should not be changed

            // Checks the validity of list
            CHECK(list.GetLength() == 10);

            // Checks the value of each element
            CHECK(list[0].Value == 1);
            CHECK(list[1].Value == 2);
            CHECK(list[2].Value == 3);
            CHECK(list[3].Value == 4);
            CHECK(list[4].Value == 5);
            CHECK(list[5].Value == 6);
            CHECK(list[6].Value == 7);
            CHECK(list[7].Value == 8);
            CHECK(list[8].Value == 9);
            CHECK(list[9].Value == 10);
        }

        DOCTEST_SUBCASE("RemoveAt")
        {
            // Creates a list from initializer list
            List<Int32> list = {1, 2, 3, 4, 5};

            // Checks the validity of list
            CHECK(list.GetLength() == 5);

            // Checks the value of each element
            CHECK(list[0] == 1);
            CHECK(list[1] == 2);
            CHECK(list[2] == 3);
            CHECK(list[3] == 4);
            CHECK(list[4] == 5);

            // Removes the element at index 2
            list.RemoveAt(2);

            // Checks the validity of list
            CHECK(list.GetLength() == 4);

            // Checks the value of each element
            CHECK(list[0] == 1);
            CHECK(list[1] == 2);
            CHECK(list[2] == 4);
            CHECK(list[3] == 5);

            // Removes the element at index 0
            list.RemoveAt(0);

            // Checks the validity of list
            CHECK(list.GetLength() == 3);

            // Checks the value of each element
            CHECK(list[0] == 2);
            CHECK(list[1] == 4);
            CHECK(list[2] == 5);

            // Removes the element at index 1
            list.RemoveAt(1);

            // Checks the validity of list
            CHECK(list.GetLength() == 2);

            // Checks the value of each element
            CHECK(list[0] == 2);
            CHECK(list[1] == 5);

            // Removes the element at index 0
            list.RemoveAt(0);

            // Checks the validity of list
            CHECK(list.GetLength() == 1);

            // Checks the value of each element
            CHECK(list[0] == 5);

            // Removes the element at index 0
            list.RemoveAt(0);

            // Checks the validity of list
            CHECK(list.GetLength() == 0);
        }
    }
}