#include "LeakTester.hpp"
#include <Axis/SmartPointer.hpp>
#include <doctest/doctest.h>

using namespace Axis;
using namespace Axis::System;

namespace Axis::System
{

struct Base
{};

struct Derived : public Base
{};

using LeakTesterType = LeakTester<Size, true, true>;

DOCTEST_TEST_CASE("Axis smart pointers : [Axis::System]")
{
    DOCTEST_SUBCASE("UniquePointer")
    {
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        {
            UniquePointer<LeakTesterType> pointer(New<LeakTesterType>(1));

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

            DOCTEST_CHECK(pointer != nullptr);
            DOCTEST_CHECK((*pointer).Instance == 1);
            DOCTEST_CHECK(pointer->Instance == 1);
        }
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

        DOCTEST_SUBCASE("Nullptr constructor")
        {
            UniquePointer<LeakTesterType> pointer = nullptr;

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

            DOCTEST_CHECK(pointer == nullptr);
        }

        DOCTEST_SUBCASE("Move constructor")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                UniquePointer<LeakTesterType> pointer(New<LeakTesterType>(1));

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK((*pointer).Instance == 1);
                DOCTEST_CHECK(pointer->Instance == 1);

                UniquePointer<LeakTesterType> anotherPointer = Move(pointer);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(anotherPointer != nullptr);
                DOCTEST_CHECK((*anotherPointer).Instance == 1);
                DOCTEST_CHECK(anotherPointer->Instance == 1);

                DOCTEST_CHECK(pointer == nullptr);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Move with cast constructor")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                UniquePointer<Derived> pointer(New<Derived>());
                UniquePointer<Base>    anotherPointer = Move(pointer);

                DOCTEST_CHECK(pointer == nullptr);
                DOCTEST_CHECK(anotherPointer != nullptr);

                UniquePointer<const Base> anotherConstPointer = Move(anotherPointer);

                DOCTEST_CHECK(pointer == nullptr);
                DOCTEST_CHECK(anotherPointer == nullptr);
                DOCTEST_CHECK(anotherConstPointer != nullptr);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Constructor with custom deleter")
        {
            Int32 value = 0;

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                struct CustomDeleter
                {
                    void operator()(const LeakTesterType* leakTester) noexcept
                    {
                        *ValuePointer = 69;
                        Axis::System::Delete(leakTester);
                    }

                    Int32* ValuePointer = nullptr;
                };

                CustomDeleter deleter = {AddressOf(value)};

                UniquePointer<LeakTesterType, CustomDeleter> pointer(New<LeakTesterType>(1),
                                                                     deleter);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK((*pointer).Instance == 1);
                DOCTEST_CHECK(pointer->Instance == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

            DOCTEST_CHECK(value == 69);
        }

        DOCTEST_SUBCASE("Construct with void pointer")
        {
            UniquePointer<int> pointer(New<int>(32));

            UniquePointer<void> voidPointer(Move(pointer));
        }

        DOCTEST_SUBCASE("Move assignment operator")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                UniquePointer<LeakTesterType> pointer1(New<LeakTesterType>(1));
                UniquePointer<LeakTesterType> pointer2(nullptr);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer1 != nullptr);
                DOCTEST_CHECK(pointer2 == nullptr);

                DOCTEST_CHECK(pointer1->Instance == 1);

                pointer2 = Move(pointer1);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer1 == nullptr);
                DOCTEST_CHECK(pointer2 != nullptr);

                DOCTEST_CHECK(pointer2->Instance == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                UniquePointer<LeakTesterType> pointer1(New<LeakTesterType>(1));
                UniquePointer<LeakTesterType> pointer2(New<LeakTesterType>(2));

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 2);

                DOCTEST_CHECK(pointer1 != nullptr);
                DOCTEST_CHECK(pointer2 != nullptr);

                DOCTEST_CHECK(pointer1->Instance == 1);
                DOCTEST_CHECK(pointer2->Instance == 2);

                pointer2 = Move(pointer1);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer1 == nullptr);
                DOCTEST_CHECK(pointer2 != nullptr);

                DOCTEST_CHECK(pointer2->Instance == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Move assignment with case")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                UniquePointer<Derived> pointer1(New<Derived>());
                UniquePointer<Base>    pointer2(nullptr);

                DOCTEST_CHECK(pointer1 != nullptr);
                DOCTEST_CHECK(pointer2 == nullptr);

                pointer2 = Move(pointer1);

                DOCTEST_CHECK(pointer1 == nullptr);
                DOCTEST_CHECK(pointer2 != nullptr);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }
    }

    DOCTEST_SUBCASE("SharedPointer")
    {
        DOCTEST_SUBCASE("Array type")
        {
            auto pointer = SharedPointer<LeakTesterType[]>::MakeShared<DefaultMemoryResource>(10, 1);
        }

        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        {
            auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

            DOCTEST_CHECK(pointer != nullptr);

            DOCTEST_CHECK(pointer->Instance == 1);
        }
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

        DOCTEST_SUBCASE("Copy constructor")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointer->Instance == 1);

                SharedPointer<LeakTesterType> anotherPointer(pointer);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(anotherPointer != nullptr);
                DOCTEST_CHECK(anotherPointer->Instance == 1);

                DOCTEST_CHECK(pointer == anotherPointer);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Copy constructor with cast")
        {
            SharedPointer<Derived> pointer(SharedPointer<Derived>::MakeShared<DefaultMemoryResource>());
            SharedPointer<Base>    anotherPointer(pointer);

            DOCTEST_CHECK(pointer != nullptr);
            DOCTEST_CHECK(anotherPointer != nullptr);

            DOCTEST_CHECK(pointer == anotherPointer);
        }

        DOCTEST_SUBCASE("Move constructor")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointer->Instance == 1);

                SharedPointer<LeakTesterType> anotherPointer(Move(pointer));

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer == nullptr);
                DOCTEST_CHECK(anotherPointer != nullptr);

                DOCTEST_CHECK(anotherPointer->Instance == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Move constructor with cast")
        {
            SharedPointer<Derived> pointer(SharedPointer<Derived>::MakeShared<DefaultMemoryResource>());
            SharedPointer<Base>    anotherPointer(Move(pointer));

            DOCTEST_CHECK(pointer == nullptr);
            DOCTEST_CHECK(anotherPointer != nullptr);
        }

        DOCTEST_SUBCASE("Copy assignment operator")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer1 = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);
                auto pointer2 = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(2);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 2);

                DOCTEST_CHECK(pointer1 != nullptr);
                DOCTEST_CHECK(pointer2 != nullptr);

                DOCTEST_CHECK(pointer1->Instance == 1);
                DOCTEST_CHECK(pointer2->Instance == 2);

                pointer2 = pointer1;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer1 != nullptr);
                DOCTEST_CHECK(pointer2 != nullptr);

                DOCTEST_CHECK(pointer1->Instance == 1);
                DOCTEST_CHECK(pointer2->Instance == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Move assignment operator")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer1 = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);
                auto pointer2 = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(2);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 2);

                DOCTEST_CHECK(pointer1 != nullptr);
                DOCTEST_CHECK(pointer2 != nullptr);

                DOCTEST_CHECK(pointer1->Instance == 1);
                DOCTEST_CHECK(pointer2->Instance == 2);

                pointer2 = Move(pointer1);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer1 == nullptr);
                DOCTEST_CHECK(pointer2 != nullptr);

                DOCTEST_CHECK(pointer2->Instance == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Reference counting")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                SharedPointer<LeakTesterType> pointer = nullptr;
                {
                    auto anotherPointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

                    DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                    DOCTEST_CHECK(anotherPointer != nullptr);
                    DOCTEST_CHECK(anotherPointer->Instance == 1);

                    pointer = anotherPointer;
                }

                // The object is still alive because the `pointer` still holds a reference to it.
                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointer->Instance == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Reset and nullptr assignment")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointer->Instance == 1);

                pointer.Reset();

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

                DOCTEST_CHECK(pointer == nullptr);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointer->Instance == 1);

                pointer = nullptr;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

                DOCTEST_CHECK(pointer == nullptr);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

            DOCTEST_SUBCASE("Explicit cast conversion")
            {
                SharedPointer<Base>    pointer        = SharedPointer<Derived>::MakeShared<DefaultMemoryResource>();
                SharedPointer<Derived> pointerAnother = (SharedPointer<Derived>)pointer;

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointerAnother != nullptr);

                DOCTEST_CHECK(pointer == pointerAnother);
            }
        }
    }

    DOCTEST_SUBCASE("WeakPointer")
    {
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        {
            auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

            DOCTEST_CHECK(pointer != nullptr);
            DOCTEST_CHECK(pointer->Instance == 1);

            WeakPointer<LeakTesterType> weakPointer = (WeakPointer<LeakTesterType>)pointer;

            pointer.Reset();

            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

            DOCTEST_CHECK(weakPointer != nullptr);

            DOCTEST_CHECK(weakPointer.GetStrongReferenceCount() == 0);

            // Should give nullptr shared pointer
            auto newSharedPointer = weakPointer.Generate();

            DOCTEST_CHECK(newSharedPointer == nullptr);
        }
        DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);

        DOCTEST_SUBCASE("Copy constructor")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

                auto weakPointer = (WeakPointer<LeakTesterType>)pointer;

                auto anotherPointer = weakPointer;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointer->Instance == 1);

                DOCTEST_CHECK(weakPointer != nullptr);
                DOCTEST_CHECK(anotherPointer != nullptr);

                DOCTEST_CHECK(weakPointer == anotherPointer);

                DOCTEST_CHECK(weakPointer.GetStrongReferenceCount() == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Move constructor")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

                auto weakPointer = (WeakPointer<LeakTesterType>)pointer;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointer->Instance == 1);

                DOCTEST_CHECK(weakPointer != nullptr);

                DOCTEST_CHECK(weakPointer.GetStrongReferenceCount() == 1);

                auto anotherWeakPointer = Move(weakPointer);

                DOCTEST_CHECK(anotherWeakPointer != nullptr);
                DOCTEST_CHECK(weakPointer == nullptr);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Copy assignment operator")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

                auto                        weakPointer    = (WeakPointer<LeakTesterType>)pointer;
                WeakPointer<LeakTesterType> anotherPointer = nullptr;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointer->Instance == 1);

                DOCTEST_CHECK(weakPointer != nullptr);
                DOCTEST_CHECK(anotherPointer == nullptr);

                DOCTEST_CHECK(weakPointer != anotherPointer);

                DOCTEST_CHECK(weakPointer.GetStrongReferenceCount() == 1);

                anotherPointer = weakPointer;

                DOCTEST_CHECK(weakPointer != nullptr);
                DOCTEST_CHECK(anotherPointer != nullptr);

                DOCTEST_CHECK(weakPointer == anotherPointer);

                DOCTEST_CHECK(weakPointer.GetStrongReferenceCount() == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }

        DOCTEST_SUBCASE("Move assignmnet oeprator")
        {
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
            {
                auto pointer = SharedPointer<LeakTesterType>::MakeShared<DefaultMemoryResource>(1);

                auto                        weakPointer    = (WeakPointer<LeakTesterType>)pointer;
                WeakPointer<LeakTesterType> anotherPointer = nullptr;

                DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 1);

                DOCTEST_CHECK(pointer != nullptr);
                DOCTEST_CHECK(pointer->Instance == 1);

                DOCTEST_CHECK(weakPointer != nullptr);
                DOCTEST_CHECK(anotherPointer == nullptr);

                DOCTEST_CHECK(weakPointer != anotherPointer);

                DOCTEST_CHECK(weakPointer.GetStrongReferenceCount() == 1);

                anotherPointer = Move(weakPointer);

                DOCTEST_CHECK(weakPointer == nullptr);
                DOCTEST_CHECK(anotherPointer != nullptr);

                DOCTEST_CHECK(anotherPointer.GetStrongReferenceCount() == 1);
            }
            DOCTEST_CHECK(LeakTesterType::GetInstanceCount() == 0);
        }
    }
}

} // namespace Axis::System
