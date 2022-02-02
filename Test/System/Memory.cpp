#include <Axis/System>
#include <doctest.h>

DOCTEST_TEST_CASE("Axis memory allocation : [Axis-System]")
{
    DOCTEST_SUBCASE("Allocators")
    {
        DOCTEST_SUBCASE("Allocate memory with `Axis::MallocAllocator`")
        {
            using namespace Axis;

            // Allocates memory for 16 bytes with alignment of 16 bytes
            auto allocatedMemory = MallocAllocator::Allocate(16, 16);

            // Checks if the allocated memory is not null
            DOCTEST_CHECK(allocatedMemory != nullptr);

            // Checks if the allocated memory is aligned
            DOCTEST_CHECK(reinterpret_cast<uintptr_t>(allocatedMemory) % 16 == 0);

            // Frees the allocated memory
            MallocAllocator::Deallocate(allocatedMemory);
        }

        DOCTEST_SUBCASE("Allocate memory with `Axis::PoolAllocator`")
        {
            using namespace Axis;

            // Allocates memory for 16 bytes with alignment of 16 bytes
            auto allocatedMemory = PoolAllocator::Allocate(16, 16);

            // Checks if the allocated memory is not null
            DOCTEST_CHECK(allocatedMemory != nullptr);

            // Checks if the allocated memory is aligned
            DOCTEST_CHECK(reinterpret_cast<uintptr_t>(allocatedMemory) % 16 == 0);

            // Frees the allocated memory
            PoolAllocator::Deallocate(allocatedMemory);
        }
    }

    DOCTEST_SUBCASE("`Axis::New` and `Axis::Delete`")
    {
        DOCTEST_SUBCASE("Creates an instance in dynamic memory with `Axis::New`")
        {
            using namespace Axis;

            // Checks if `Axis::New` creates an instance in dynamic memory and
            // destroys the instance (destructor) in `Axis::Delete`.

            Bool objectAlive = false;

            struct TestClass
            {
                TestClass(Bool* indicator) noexcept :
                    Indicator(indicator)
                {
                    // Sets the indicator to true
                    *Indicator = true;
                }

                ~TestClass() noexcept
                {
                    // Sets the indicator to false
                    *Indicator = false;
                }

                Bool* Indicator = nullptr;
            };

            // Creates an instance of `TestClass`
            auto instace = Axis::AllocatedNew<DefaultAllocator, TestClass>(&objectAlive);

            // Checks if the instance is not null
            DOCTEST_CHECK(instace != nullptr);

            // Checks if the instance is alive
            DOCTEST_CHECK(objectAlive == true);

            // Destroys the instance
            Axis::AllocatedDelete<DefaultAllocator>(instace);

            // Checks if the instance is dead
            DOCTEST_CHECK(objectAlive == false);
        }

        DOCTEST_SUBCASE("Creates an instance in dynamic memory with `Axis::New` which throws exception")
        {
            using namespace Axis;

            struct TestClass
            {
                TestClass()
                {
                    // Throws number 1
                    throw Int32(1);
                }
            };

            // Creates an instance of `TestClass` with `Axis::New`,
            // which throws an exception

            try
            {
                auto instace = Axis::AllocatedNew<DefaultAllocator, TestClass>();
            }
            catch (Int32& exception)
            {
                // Checks if the exception is number 1
                DOCTEST_CHECK(exception == 1);
            }
        }

        DOCTEST_SUBCASE("Creates an instance in dynamic memory with `Axis::NewArray` which throws exception")
        {
            using namespace Axis;

            static Size instanceCount = 0;

            struct TestClass
            {
                TestClass()
                {
                    // Throws number 1

                    if (instanceCount == 20)
                        throw Int32(1);

                    ++instanceCount;
                }

                ~TestClass() noexcept
                {
                    --instanceCount;
                }
            };

            // Creates an instance of `TestClass` with `Axis::NewArray`,
            // which throws an exception

            try
            {
                auto instances = Axis::AllocatedNewArray<DefaultAllocator, TestClass>(20);
            }
            catch (Int32& exception)
            {
                // Checks if the exception is number 1
                DOCTEST_CHECK(exception == 1);

                // Checks if the instance count is 0
                DOCTEST_CHECK(instanceCount == 0);
            }
        }
    }

    DOCTEST_SUBCASE("`Axis::NewArray` and `Axis::DeleteArray`")
    {
        DOCTEST_SUBCASE("Creates an array of instances in dynamic memory with `Axis::NewArray`")
        {
            using namespace Axis;

            // Checks if `Axis::NewArray` creates an array of instances in dynamic memory and
            // destroys the instances (destructors) in `Axis::DeleteArray`.

            static Size elementCount = 0;

            struct TestClass
            {
                TestClass() noexcept
                {
                    elementCount++;
                }
                ~TestClass() noexcept
                {
                    elementCount--;
                }

                Int32 Value = {};
            };

            // Creates an array of instances of `TestClass`
            auto array = Axis::NewArray<TestClass>(10);

            // Checks if the array is not null
            DOCTEST_CHECK(array != nullptr);

            // Checks if the array has 10 elements
            DOCTEST_CHECK(elementCount == 10);

            // Destroys the array
            Axis::DeleteArray(array);

            // Checks if the array has 0 elements
            DOCTEST_CHECK(elementCount == 0);
        }
    }
}