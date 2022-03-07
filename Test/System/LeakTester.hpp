#include <Axis/Utility.hpp>

namespace Axis
{

namespace System
{

namespace Test
{
template <typename T, bool EnableCopyAssignment, bool EnableMoveAssignment>
struct LeakTester
{
public:
    using ThisType = LeakTester<T, EnableCopyAssignment, EnableMoveAssignment>;

    // Default constructor
    LeakTester() noexcept(Axis::System::IsDefaultConstructible<T>) = default;

    // Constructor increments; the counter
    LeakTester(const T& arg) noexcept(Axis::System::IsNoThrowCopyConstructible<T>) :
        Instance(arg)
    {
        s_InstanceCount++;
    }

    // Destructor decrements the counter
    ~LeakTester() noexcept
    {
        s_InstanceCount--;
    }

    // Copy constructor; increments the counter
    LeakTester(const ThisType& other) noexcept(Axis::System::IsNoThrowCopyConstructible<T>) :
        Instance(other.Instance)
    {
        s_InstanceCount++;
    }

    // Move constructor; increments the counter
    LeakTester(ThisType&& other) noexcept(Axis::System::IsNoThrowMoveConstructible<T>) :
        Instance(Axis::System::Move(other.Instance))
    {
        s_InstanceCount++;
    }

    // Copy assignment operator; doesn't increment / decrement the counter
    ThisType& operator=(const ThisType& other) noexcept(Axis::System::IsNoThrowCopyAssignable<T>) requires(EnableCopyAssignment)
    {
        if (this != Axis::System::AddressOf(other))
            Instance = other.Instance;

        return *this;
    }

    // Move assignment operator; doesn't increment / decrement the counter
    ThisType& operator=(ThisType&& other) noexcept(Axis::System::IsNoThrowMoveAssignable<T>) requires(EnableMoveAssignment)
    {
        if (this != Axis::System::AddressOf(other))
            Instance = Axis::System::Move(other.Instance);

        return *this;
    }

    // Value
    T Instance;

    // Gets the total living elements
    static Axis::Size GetInstanceCount() noexcept { return s_InstanceCount; }

private:
    // Total number of constructed elements
    static Axis::Size s_InstanceCount;
};

} // namespace Test

} // namespace System

} // namespace Axis

template <class T, bool EnableCopyAssignment, bool EnableMoveAssignment>
Axis::Size Axis::System::Test::LeakTester<T, EnableCopyAssignment, EnableMoveAssignment>::s_InstanceCount = 0;
