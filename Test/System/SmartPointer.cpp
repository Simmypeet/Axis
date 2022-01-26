#include <Axis/SmartPointer.hpp>
#include <doctest.h>

DOCTEST_TEST_CASE("Axis smart pointers : [Axis::System]")
{
    using namespace Axis;
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

    DOCTEST_SUBCASE("`Axis::UniquePointer`")
    {
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
                UniquePointer<TestStruct> ptr(Axis::New<TestStruct>());

                // instance count should be 1
                CHECK(Instances == 1);

                // Should be true
                CHECK(ptr);
            }

            // Should be zero
            CHECK(Instances == 0);
        }
    }

    DOCTEST_SUBCASE("`Axis::SharedPointer`")
    {
        DOCTEST_SUBCASE("Constructor")
        {
            // Default constructor
            {
                SharedPointer<TestStruct> ptr;

                // Should be false
                CHECK(!ptr);

                // Should be zero
                CHECK(Instances == 0);
            }

            // Nullptr constructor
            {
                SharedPointer<TestStruct> ptr(nullptr);

                // Should be false
                CHECK(!ptr);

                // Should be zero
                CHECK(Instances == 0);
            }

            // Constructs shared pointer of TestStruct
            {
                SharedPointer<TestStruct> ptr(Axis::New<TestStruct>());

                // instance count should be 1
                CHECK(Instances == 1);

                // Should be true
                CHECK(ptr);
            }

            // Constructs shared pointer of array of TestStruct
            {
                SharedPointer<TestStruct[]> ptr(Axis::NewArray<TestStruct>(10));

                // instance count should be 10
                CHECK(Instances == 10);

                // Should be true
                CHECK(ptr);
            }

            // Copy constructor
            {
                SharedPointer<TestStruct> ptr(Axis::New<TestStruct>());

                // instance count should be 1
                CHECK(Instances == 1);

                // Should be true
                CHECK(ptr);

                // Copy constructor
                SharedPointer<TestStruct> ptr2(ptr);

                // instance count should be 2
                CHECK(Instances == 1);

                // Should be true
                CHECK(ptr2);

                // ptr1 and ptr2 should be equal
                CHECK(ptr == ptr2);

                SharedPointer<TestStruct> null1 = nullptr;
                SharedPointer<TestStruct> null2 = null1;
            }

            // Move constructor
            {
                SharedPointer<TestStruct> ptr(Axis::New<TestStruct>());

                // instance count should be 1
                CHECK(Instances == 1);

                // Should be true
                CHECK(ptr);

                // Move constructor
                SharedPointer<TestStruct> ptr2(std::move(ptr));

                // instance count should be 1
                CHECK(Instances == 1);

                // ptr1 should be null
                CHECK(!ptr);

                // ptr2 should be not null
                CHECK(ptr2);

                // ptr1 and ptr2 should not be equal
                CHECK(ptr != ptr2);


                SharedPointer<TestStruct> null1 = nullptr;
                SharedPointer<TestStruct> null2 = std::move(null1);
            }
        }

        DOCTEST_SUBCASE("Reference counting")
        {
            // Reference count test
            {
                {
                    SharedPointer<TestStruct> testStruct;

                    // No instances
                    CHECK(Instances == 0);

                    {
                        // Constructs shared pointer of TestStruct
                        SharedPointer<TestStruct> ptr(Axis::New<TestStruct>());

                        // instance count should be 1
                        CHECK(Instances == 1);

                        // Should be true
                        CHECK(ptr);

                        // Assigns shared pointer of TestStruct
                        testStruct = ptr;

                        // instance count should be 1
                        CHECK(Instances == 1);

                        // Should be true
                        CHECK(testStruct);

                        // ptr and testStruct should be equal
                        CHECK(ptr == testStruct);
                    }

                    // instance count should still be 1 (shared)
                    CHECK(Instances == 1);

                    // testStruct should be not null
                    CHECK(testStruct);
                }

                // instance count should be zero
                CHECK(Instances == 0);
            }

            // Nullptr assignment should reduce the reference count
            {
                SharedPointer<TestStruct> testStruct(Axis::New<TestStruct>());

                // instance count should be 1
                CHECK(Instances == 1);

                // Should be true
                CHECK(testStruct);

                // Assigns nullptr
                testStruct = nullptr;

                // instance count should be 0
                CHECK(Instances == 0);

                // testStruct should be null
                CHECK(!testStruct);
            }
        }

        DOCTEST_SUBCASE("MakeShared")
        {
            // Uses `SharedPtr<T>::MakeShared` instead of `Axis::New`
            {
                SharedPointer<TestStruct> ptr = Axis::MakeShared<TestStruct>();

                // instance count should be 1
                CHECK(Instances == 1);

                // Should be true
                CHECK(ptr);
            }

            // Uses `SharedPtr<T>::MakeShared` instead of `Axis::New` to create array of instances
            {
                SharedPointer<TestStruct[]> ptr = Axis::MakeShared<TestStruct[]>(10);

                // instance count should be 10
                CHECK(Instances == 10);

                // Should be true
                CHECK(ptr);
            }
        }
    }
}