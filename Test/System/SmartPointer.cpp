#include <Axis/System>
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
            DOCTEST_CHECK(Instances == 0);

            {
                // Constructs unique pointer of array of TestStruct
                UniquePointer<TestStruct[]> ptr(Axis::NewArray<TestStruct>(10));

                // instance count should be 10
                DOCTEST_CHECK(Instances == 10);

                // Should be true
                DOCTEST_CHECK(ptr);
            }

            // Should be zero
            DOCTEST_CHECK(Instances == 0);
        }

        DOCTEST_SUBCASE("Conversion constructors")
        {
            // Converts from const qualified unique pointer
            {
                UniquePointer<TestStruct> ptr(Axis::New<TestStruct>());

                // instance count should be 1
                DOCTEST_CHECK(Instances == 1);

                // Should be true
                DOCTEST_CHECK(ptr);
            }

            // Should be zero
            DOCTEST_CHECK(Instances == 0);
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
                DOCTEST_CHECK(!ptr);

                // Should be zero
                DOCTEST_CHECK(Instances == 0);
            }

            // Nullptr constructor
            {
                SharedPointer<TestStruct> ptr(nullptr);

                // Should be false
                DOCTEST_CHECK(!ptr);

                // Should be zero
                DOCTEST_CHECK(Instances == 0);
            }

            // Constructs shared pointer of TestStruct
            {
                SharedPointer<TestStruct> ptr(Axis::New<TestStruct>());

                // instance count should be 1
                DOCTEST_CHECK(Instances == 1);

                // Should be true
                DOCTEST_CHECK(ptr);
            }

            // Constructs shared pointer of array of TestStruct
            {
                SharedPointer<TestStruct[]> ptr(Axis::NewArray<TestStruct>(10));

                // instance count should be 10
                DOCTEST_CHECK(Instances == 10);

                // Should be true
                DOCTEST_CHECK(ptr);
            }

            // Copy constructor
            {
                SharedPointer<TestStruct> ptr(Axis::New<TestStruct>());

                // instance count should be 1
                DOCTEST_CHECK(Instances == 1);

                // Should be true
                DOCTEST_CHECK(ptr);

                // Copy constructor
                SharedPointer<TestStruct> ptr2(ptr);

                // instance count should be 2
                DOCTEST_CHECK(Instances == 1);

                // Should be true
                DOCTEST_CHECK(ptr2);

                // ptr1 and ptr2 should be equal
                DOCTEST_CHECK(ptr == ptr2);

                SharedPointer<TestStruct> null1 = nullptr;
                SharedPointer<TestStruct> null2 = null1;
            }

            // Move constructor
            {
                SharedPointer<TestStruct> ptr(Axis::New<TestStruct>());

                // instance count should be 1
                DOCTEST_CHECK(Instances == 1);

                // Should be true
                DOCTEST_CHECK(ptr);

                // Move constructor
                SharedPointer<TestStruct> ptr2(std::move(ptr));

                // instance count should be 1
                DOCTEST_CHECK(Instances == 1);

                // ptr1 should be null
                DOCTEST_CHECK(!ptr);

                // ptr2 should be not null
                DOCTEST_CHECK(ptr2);

                // ptr1 and ptr2 should not be equal
                DOCTEST_CHECK(ptr != ptr2);


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
                    DOCTEST_CHECK(Instances == 0);

                    {
                        // Constructs shared pointer of TestStruct
                        SharedPointer<TestStruct> ptr(Axis::New<TestStruct>());

                        // instance count should be 1
                        DOCTEST_CHECK(Instances == 1);

                        // Should be true
                        DOCTEST_CHECK(ptr);

                        // Assigns shared pointer of TestStruct
                        testStruct = ptr;

                        // instance count should be 1
                        DOCTEST_CHECK(Instances == 1);

                        // Should be true
                        DOCTEST_CHECK(testStruct);

                        // ptr and testStruct should be equal
                        DOCTEST_CHECK(ptr == testStruct);
                    }

                    // instance count should still be 1 (shared)
                    DOCTEST_CHECK(Instances == 1);

                    // testStruct should be not null
                    DOCTEST_CHECK(testStruct);
                }

                // instance count should be zero
                DOCTEST_CHECK(Instances == 0);
            }

            // Nullptr assignment should reduce the reference count
            {
                SharedPointer<TestStruct> testStruct(Axis::New<TestStruct>());

                // instance count should be 1
                DOCTEST_CHECK(Instances == 1);

                // Should be true
                DOCTEST_CHECK(testStruct);

                // Assigns nullptr
                testStruct = nullptr;

                // instance count should be 0
                DOCTEST_CHECK(Instances == 0);

                // testStruct should be null
                DOCTEST_CHECK(!testStruct);
            }
        }

        DOCTEST_SUBCASE("MakeShared")
        {
            // Uses `SharedPtr<T>::MakeShared` instead of `Axis::New`
            {
                SharedPointer<TestStruct> ptr = Axis::MakeShared<TestStruct>();

                // instance count should be 1
                DOCTEST_CHECK(Instances == 1);

                // Should be true
                DOCTEST_CHECK(ptr);
            }

            // Uses `SharedPtr<T>::MakeShared` instead of `Axis::New` to create array of instances
            {
                SharedPointer<TestStruct[]> ptr = Axis::MakeShared<TestStruct[]>(10);

                // instance count should be 10
                DOCTEST_CHECK(Instances == 10);

                // Should be true
                DOCTEST_CHECK(ptr);
            }
        }

        DOCTEST_SUBCASE("ISharedFromThis")
        {
            struct SharedFromThisDerived : public ISharedFromThis
            {};

            SharedPointer<SharedFromThisDerived> ptr = Axis::MakeShared<SharedFromThisDerived>();

            auto anotherPtr = ISharedFromThis::CreateSharedPointerFromThis(*ptr);

            DOCTEST_CHECK(ptr == anotherPtr);
        }
    }
}