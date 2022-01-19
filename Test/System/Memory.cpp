#include <Axis/Memory.hpp>
#include <doctest.h>

TEST_CASE("Axis memory allocation : [Axis-System]")
{
    SUBCASE("Allocators")
    {
        SUBCASE("Allocate memory with `Axis::MallocAllocator`")
        {
            using namespace Axis;

            // Allocates memory for 16 bytes with alignment of 16 bytes
            auto allocatedMemory = MallocAllocator::Allocate(16, 16);

            // Checks if the allocated memory is not null
            CHECK(allocatedMemory != nullptr);

            // Checks if the allocated memory is aligned
            CHECK(reinterpret_cast<uintptr_t>(allocatedMemory) % 16 == 0);

            // Frees the allocated memory
            MallocAllocator::Deallocate(allocatedMemory);
        }

        SUBCASE("Allocate memory with `Axis::PoolAllocator`")
        {
            using namespace Axis;

            // Allocates memory for 16 bytes with alignment of 16 bytes
            auto allocatedMemory = PoolAllocator::Allocate(16, 16);

            // Checks if the allocated memory is not null
            CHECK(allocatedMemory != nullptr);

            // Checks if the allocated memory is aligned
            CHECK(reinterpret_cast<uintptr_t>(allocatedMemory) % 16 == 0);

            // Frees the allocated memory
            PoolAllocator::Deallocate(allocatedMemory);
        }
    }

    SUBCASE("`Axis::New` and `Axis::Delete`")
    {
        SUBCASE("Creates an instance in dynamic memory with `Axis::New`")
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
            auto instace = Axis::New<DefaultAllocator, TestClass>(&objectAlive);

            // Checks if the instance is not null
            CHECK(instace != nullptr);

            // Checks if the instance is alive
            CHECK(objectAlive == true);

            // Destroys the instance
            Axis::Delete<DefaultAllocator>(instace);

            // Checks if the instance is dead
            CHECK(objectAlive == false);
        }
    }

    SUBCASE("`Axis::NewArray` and `Axis::DeleteArray`")
    {
        SUBCASE("Creates an array of instances in dynamic memory with `Axis::NewArray`")
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
            };

            // Creates an array of instances of `TestClass`
            auto array = Axis::NewArray<DefaultAllocator, TestClass>(10);

            // Checks if the array is not null
            CHECK(array != nullptr);

            // Checks if the array has 10 elements
            CHECK(elementCount == 10);

            // Destroys the array
            Axis::DeleteArray<DefaultAllocator>(array);

            // Checks if the array has 0 elements
            CHECK(elementCount == 0);
        }
    }
}