#include <Axis/SmartPointer.hpp>
#include <doctest.h>
#include <iostream>
#include <memory>


using namespace Axis;

DOCTEST_TEST_CASE("Axis smart pointers : [Axis::System]")
{
    DOCTEST_SUBCASE("`Axis::UniquePointer`")
    {
        static Size Instances = 0;

        struct TestStruct
        {
            TestStruct() noexcept { Instances++; }
            virtual ~TestStruct() noexcept { Instances--; }

            Size num1 = 64;
            Size num2 = 128;
        };

        struct TestStructDerived : public TestStruct
        {
            using TestStruct::TestStruct;

            ~TestStructDerived() noexcept override = default;
        };

        DOCTEST_SUBCASE("Constructors")
        {
            // Should be zero
            CHECK(Instances == 0);

            {
                // Constructs unique pointer of array of TestStruct
                UniquePointer<TestStruct[]> ptr(Axis::NewArray<TestStruct>(10));

                // instance count should be 10
                CHECK(Instances == 10);

                // Should be true
                CHECK(ptr);
            }

            // Should be zero
            CHECK(Instances == 0);
        }

        DOCTEST_SUBCASE("Conversion constructors")
        {
            // Converts from const qualified unique pointer
            {
                UniquePointer<const TestStruct> ptr(Axis::New<TestStruct>());

                // instance count should be 1
                CHECK(Instances == 1);

                // Should be true
                CHECK(ptr);
            }

            // Should be zero
            CHECK(Instances == 0);
        }
    }
}