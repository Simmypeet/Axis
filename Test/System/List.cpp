/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include "LeakTester.hpp"
#include <Axis/List.hpp>
#include <doctest/doctest.h>

namespace Axis::System
{

template <Bool EnableCopyAssignment, Bool EnableMoveAssignment>
void RunTestList()
{
    using LeakTesterType = LeakTester<Size, EnableCopyAssignment, EnableMoveAssignment>;
    using ListType       = List<LeakTesterType, DefaultAllocator>;

    DOCTEST_SUBCASE("Constructors")
    {
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        {
            ListType list = {1, 2, 3, 4, 5};

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
            ListType list(5, 1);

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
                ListType list  = {1, 2, 3, 4, 5};
                ListType list1 = Axis::System::Move(list);

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
                ListType list  = {1, 2, 3, 4, 5};
                ListType list1 = list;

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
                ListType list1 = {1, 2, 3, 4, 5};
                ListType list2 = {6, 7, 8, 9, 0};

                list1 = list2;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 10);

                constexpr auto CheckCase1 = [](const ListType& list) {
                    DOCTEST_CHECK(list.GetSize() == 5);

                    DOCTEST_CHECK(list[0].Instance == 6);
                    DOCTEST_CHECK(list[1].Instance == 7);
                    DOCTEST_CHECK(list[2].Instance == 8);
                    DOCTEST_CHECK(list[3].Instance == 9);
                    DOCTEST_CHECK(list[4].Instance == 0);
                };

                CheckCase1(list1);
                CheckCase1(list2);

                ListType list3 = {10, 11, 12, 13, 14};
                ListType list4 = {15, 16, 17, 18, 19, 20};

                list3 = list4;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 22);

                constexpr auto CheckCase2 = [](const ListType& list) {
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
                ListType list  = {0, 1, 2, 3, 4, 5};
                ListType list1 = {6, 7, 8, 9, 0};

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
                ListType list = {0, 1, 2, 3, 4, 5};

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
            constexpr auto CheckReserve = [](typename ListType::SizeType reserveSize) {
                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
                {
                    ListType list = {0, 1, 2, 3, 4, 5};

                    try
                    {
                        list.Reserve(reserveSize);
                    }
                    catch (...)
                    {}

                    DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 6);

                    DOCTEST_CHECK(list.GetSize() == 6);

                    DOCTEST_CHECK(list[0].Instance == 0);
                    DOCTEST_CHECK(list[1].Instance == 1);
                    DOCTEST_CHECK(list[2].Instance == 2);
                    DOCTEST_CHECK(list[3].Instance == 3);
                    DOCTEST_CHECK(list[4].Instance == 4);
                    DOCTEST_CHECK(list[5].Instance == 5);
                }
                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            };

            // Reserves for 0 elements
            CheckReserve(0);

            // Reserves for less than the current size
            CheckReserve(3);

            // Reserves for more than the current size
            CheckReserve(10);

            // Reserves more than the maximum size
            CheckReserve(std::numeric_limits<typename ListType::SizeType>::max());
        }

        DOCTEST_SUBCASE("Append")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                ListType list = {0, 1, 2, 3, 4};

                list.Append(5);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 6);

                DOCTEST_CHECK(list.GetSize() == 6);

                DOCTEST_CHECK(list[0].Instance == 0);
                DOCTEST_CHECK(list[1].Instance == 1);
                DOCTEST_CHECK(list[2].Instance == 2);
                DOCTEST_CHECK(list[3].Instance == 3);
                DOCTEST_CHECK(list[4].Instance == 4);
                DOCTEST_CHECK(list[5].Instance == 5);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("AppendRange")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                ListType list  = {0, 1, 2, 3, 4};
                ListType list2 = {};

                list2.AppendRange(list.begin(), list.end());

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 10);

                DOCTEST_CHECK(list2.GetSize() == 5);

                DOCTEST_CHECK(list2[0].Instance == 0);
                DOCTEST_CHECK(list2[1].Instance == 1);
                DOCTEST_CHECK(list2[2].Instance == 2);
                DOCTEST_CHECK(list2[3].Instance == 3);
                DOCTEST_CHECK(list2[4].Instance == 4);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("RemoveAt")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                ListType list = {0, 1, 2, 3, 4, 5};

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 6);

                list.RemoveAt(5);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 5);

                DOCTEST_CHECK(list.GetSize() == 5);

                DOCTEST_CHECK(list[0].Instance == 0);
                DOCTEST_CHECK(list[1].Instance == 1);
                DOCTEST_CHECK(list[2].Instance == 2);
                DOCTEST_CHECK(list[3].Instance == 3);
                DOCTEST_CHECK(list[4].Instance == 4);

                list.RemoveAt(1, 3);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 2);

                DOCTEST_CHECK(list.GetSize() == 2);

                DOCTEST_CHECK(list[0].Instance == 0);
                DOCTEST_CHECK(list[1].Instance == 4);

                list.RemoveAt(0, 2);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

                DOCTEST_CHECK(list.GetSize() == 0);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Insert and Emplace")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                ListType list = {0, 1, 4, 5};

                DOCTEST_CHECK(list.GetSize() == 4);

                list.Insert(2, 3); /// This function will be redirected to the Emplace function using the copy / move constructor as
                                   /// variadic template arguments

                list.Emplace(2, 2);

                list.Emplace(6, 6); // Typically this function will be redirected to Append / EmplaceBack

                DOCTEST_CHECK(list.GetSize() == 7);

                DOCTEST_CHECK(list[0].Instance == 0);
                DOCTEST_CHECK(list[1].Instance == 1);
                DOCTEST_CHECK(list[2].Instance == 2);
                DOCTEST_CHECK(list[3].Instance == 3);
                DOCTEST_CHECK(list[4].Instance == 4);
                DOCTEST_CHECK(list[5].Instance == 5);
                DOCTEST_CHECK(list[6].Instance == 6);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("InsertRange")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                ListType list  = {0, 5};
                ListType list1 = {1, 2};
                ListType list2 = {3, 4};

                list.InsertRange(1, list2.cbegin(), list2.cend());

                list.InsertRange(1, list1.cbegin(), list1.cend());

                DOCTEST_CHECK(list.GetSize() == 6);

                DOCTEST_CHECK(list[0].Instance == 0);
                DOCTEST_CHECK(list[1].Instance == 1);
                DOCTEST_CHECK(list[2].Instance == 2);
                DOCTEST_CHECK(list[3].Instance == 3);
                DOCTEST_CHECK(list[4].Instance == 4);
                DOCTEST_CHECK(list[5].Instance == 5);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Clear")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                constexpr auto RunTest = [](Bool deallocateMemory) {
                    ListType list = {0, 1, 2, 3, 4};

                    auto capacity = list.GetCapacity();

                    if (deallocateMemory)
                        list.template Clear<true>();
                    else
                        list.template Clear<false>();

                    DOCTEST_CHECK(list.GetSize() == 0);
                    DOCTEST_CHECK(list.GetCapacity() == (deallocateMemory ? 0 : capacity));
                };

                RunTest(true);
                RunTest(false);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Resize")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                ListType list = {0, 1, 2, 3, 4, 5};

                DOCTEST_CHECK(list.GetSize() == 6);

                list.Resize(4);

                DOCTEST_CHECK(list.GetSize() == 4);

                DOCTEST_CHECK(list[0].Instance == 0);
                DOCTEST_CHECK(list[1].Instance == 1);
                DOCTEST_CHECK(list[2].Instance == 2);
                DOCTEST_CHECK(list[3].Instance == 3);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                ListType list = {0, 1, 2, 3, 4, 5};

                DOCTEST_CHECK(list.GetSize() == 6);

                list.Resize(8, LeakTesterType(0));

                DOCTEST_CHECK(list.GetSize() == 8);

                DOCTEST_CHECK(list[0].Instance == 0);
                DOCTEST_CHECK(list[1].Instance == 1);
                DOCTEST_CHECK(list[2].Instance == 2);
                DOCTEST_CHECK(list[3].Instance == 3);
                DOCTEST_CHECK(list[4].Instance == 4);
                DOCTEST_CHECK(list[5].Instance == 5);
                DOCTEST_CHECK(list[6].Instance == 0);
                DOCTEST_CHECK(list[7].Instance == 0);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Reset")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                ListType list = {0, 1, 2, 3, 4, 5};

                DOCTEST_CHECK(list.GetSize() == 6);

                list.Reset(0);

                DOCTEST_CHECK(list.GetSize() == 6);

                DOCTEST_CHECK(list[0].Instance == 0);
                DOCTEST_CHECK(list[1].Instance == 0);
                DOCTEST_CHECK(list[2].Instance == 0);
                DOCTEST_CHECK(list[3].Instance == 0);
                DOCTEST_CHECK(list[4].Instance == 0);
                DOCTEST_CHECK(list[5].Instance == 0);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }
    }
}

DOCTEST_TEST_CASE("Axis list : [Axis::System]")
{
    DOCTEST_SUBCASE("EnableCopyAssignment: true, EnableMoveAssignment: true")
    {
        RunTestList<true, true>();
    }

    DOCTEST_SUBCASE("EnableCopyAssignment: false, EnableMoveAssignment: true")
    {
        RunTestList<false, true>();
    }

    DOCTEST_SUBCASE("EnableCopyAssignment: true, EnableMoveAssignment: false")
    {
        RunTestList<true, false>();
    }

    DOCTEST_SUBCASE("EnableCopyAssignment: false, EnableMoveAssignment: false")
    {
        RunTestList<false, false>();
    }
}

} // namespace Axis::System
