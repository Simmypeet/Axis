#include "Axis/Assert.hpp"
#include "LeakTester.hpp"
#include <Axis/List.hpp>
#include <doctest.h>

using namespace Axis;
using namespace Axis::System;

DOCTEST_TEST_CASE("Axis list : [Axis::System]")
{
    using LeakTesterType = LeakTester<Size, false, false>;

    DOCTEST_SUBCASE("Constructors")
    {
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        {
            List<LeakTesterType> list = {1, 2, 3, 4, 5};

            DOCTEST_CHECK(list.GetSize() == 5);

            DOCTEST_CHECK(list[0].Instance == 1);
            DOCTEST_CHECK(list[1].Instance == 2);
            DOCTEST_CHECK(list[2].Instance == 3);
            DOCTEST_CHECK(list[3].Instance == 4);
            DOCTEST_CHECK(list[4].Instance == 5);

            DOCTEST_CHECK_THROWS_AS(Axis::System::Ignore = list[5], ArgumentOutOfRangeException);

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 5);
        }
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        {
            List<LeakTesterType> list(5, 1);

            DOCTEST_CHECK(list.GetSize() == 5);

            DOCTEST_CHECK(list[0].Instance == 1);
            DOCTEST_CHECK(list[1].Instance == 1);
            DOCTEST_CHECK(list[2].Instance == 1);
            DOCTEST_CHECK(list[3].Instance == 1);
            DOCTEST_CHECK(list[4].Instance == 1);

            DOCTEST_CHECK_THROWS_AS(Axis::System::Ignore = list[5], ArgumentOutOfRangeException);

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 5);
        }
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);


        DOCTEST_SUBCASE("Move constructor")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                List<LeakTesterType> list  = {1, 2, 3, 4, 5};
                List<LeakTesterType> list1 = Axis::System::Move(list);

                DOCTEST_CHECK(list.GetSize() == 0);
                DOCTEST_CHECK(list1.GetSize() == 5);

                DOCTEST_CHECK(list1[0].Instance == 1);
                DOCTEST_CHECK(list1[1].Instance == 2);
                DOCTEST_CHECK(list1[2].Instance == 3);
                DOCTEST_CHECK(list1[3].Instance == 4);
                DOCTEST_CHECK(list1[4].Instance == 5);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Copy constructor")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                List<LeakTesterType> list  = {1, 2, 3, 4, 5};
                List<LeakTesterType> list1 = list;

                DOCTEST_CHECK(list.GetSize() == 5);
                DOCTEST_CHECK(list1.GetSize() == 5);

                DOCTEST_CHECK(list1[0].Instance == 1);
                DOCTEST_CHECK(list1[1].Instance == 2);
                DOCTEST_CHECK(list1[2].Instance == 3);
                DOCTEST_CHECK(list1[3].Instance == 4);
                DOCTEST_CHECK(list1[4].Instance == 5);

                DOCTEST_CHECK(list[0].Instance == 1);
                DOCTEST_CHECK(list[1].Instance == 2);
                DOCTEST_CHECK(list[2].Instance == 3);
                DOCTEST_CHECK(list[3].Instance == 4);
                DOCTEST_CHECK(list[4].Instance == 5);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 10);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }
    }

    DOCTEST_SUBCASE("Assignments")
    {
        DOCTEST_SUBCASE("Copy assignment")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                List<LeakTesterType> list1 = {1, 2, 3, 4, 5};
                List<LeakTesterType> list2 = {6, 7, 8, 9, 0};

                list1 = list2;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 10);

                constexpr auto CheckCase1 = [](const List<LeakTesterType>& list) {
                    DOCTEST_CHECK(list.GetSize() == 5);

                    DOCTEST_CHECK(list[0].Instance == 6);
                    DOCTEST_CHECK(list[1].Instance == 7);
                    DOCTEST_CHECK(list[2].Instance == 8);
                    DOCTEST_CHECK(list[3].Instance == 9);
                    DOCTEST_CHECK(list[4].Instance == 0);
                };

                CheckCase1(list1);
                CheckCase1(list2);

                List<LeakTesterType> list3 = {10, 11, 12, 13, 14};
                List<LeakTesterType> list4 = {15, 16, 17, 18, 19, 20};

                list3 = list4;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 22);

                constexpr auto CheckCase2 = [](const List<LeakTesterType>& list) {
                    DOCTEST_CHECK(list.GetSize() == 6);

                    DOCTEST_CHECK(list[0].Instance == 15);
                    DOCTEST_CHECK(list[1].Instance == 16);
                    DOCTEST_CHECK(list[2].Instance == 17);
                    DOCTEST_CHECK(list[3].Instance == 18);
                    DOCTEST_CHECK(list[4].Instance == 19);
                    DOCTEST_CHECK(list[5].Instance == 20);
                };

                CheckCase2(list3);
                CheckCase2(list4);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Move assignment")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                List<LeakTesterType> list  = {0, 1, 2, 3, 4, 5};
                List<LeakTesterType> list1 = {6, 7, 8, 9, 0};

                list = Axis::System::Move(list1);

                DOCTEST_CHECK(list.GetSize() == 5);

                DOCTEST_CHECK(list[0].Instance == 6);
                DOCTEST_CHECK(list[1].Instance == 7);
                DOCTEST_CHECK(list[2].Instance == 8);
                DOCTEST_CHECK(list[3].Instance == 9);
                DOCTEST_CHECK(list[4].Instance == 0);

                DOCTEST_CHECK(list1.GetSize() + list.GetSize() == LeakTesterType::GetInstanceCount());
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }
    }

    DOCTEST_SUBCASE("Iterators")
    {
        DOCTEST_SUBCASE("Range-based for loop")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                List<LeakTesterType> list = {0, 1, 2, 3, 4, 5};

                List<LeakTesterType>::Iterator it;

                Size i = 0;
                for (auto& item : list)
                {
                    DOCTEST_CHECK(item.Instance == i);
                    ++i;
                }
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }
    }

    DOCTEST_SUBCASE("Modification")
    {
        DOCTEST_SUBCASE("Reserve")
        {
            constexpr auto CheckReserve = [](List<LeakTesterType>::SizeType reserveSize) {
                List<LeakTesterType> list = {0, 1, 2, 3, 4, 5};

                try
                {
                    list.Reserve(reserveSize);
                }
                catch (...)
                {
                    AXIS_DEBUG_TRAP();
                }

                DOCTEST_CHECK(list.GetSize() == 6);

                DOCTEST_CHECK(list[0].Instance == 0);
                DOCTEST_CHECK(list[1].Instance == 1);
                DOCTEST_CHECK(list[2].Instance == 2);
                DOCTEST_CHECK(list[3].Instance == 3);
                DOCTEST_CHECK(list[4].Instance == 4);
                DOCTEST_CHECK(list[5].Instance == 5);

                AXIS_DEBUG_TRAP();
            };

            // Reserves for 0 elements
            CheckReserve(0);

            // Reserves for less than the current size
            CheckReserve(3);

            // Reserves for more than the current size
            CheckReserve(10);

            // Reserves for the maximum size (probably throws an exception)
            CheckReserve(10000000000);
        }
    }
}