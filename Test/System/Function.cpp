#include <Axis/System>
#include <doctest.h>

DOCTEST_TEST_CASE("Function object : [Axis::System]")
{
    using namespace Axis;

    static Size InstanceCount = 0;

    /// \brief A functor that increments the instance count.
    struct LeakDetector
    {
        LeakDetector() noexcept
        {
            ++InstanceCount;
        }

        LeakDetector(const LeakDetector&) noexcept
        {
            ++InstanceCount;
        }

        LeakDetector(LeakDetector&&) noexcept
        {
            ++InstanceCount;
        }

        ~LeakDetector()
        {
            --InstanceCount;
        }

        Int32 operator()() const noexcept
        {
            return 32;
        }
    };


    auto lambda = [](Int32 num) -> Int32 {
        return num;
    };

    // Constructs Axis::Function object with a lambda function
    Function<Int32(Int32)> function1 = lambda;

    // Constructs Axis::Function object with nullptr
    Function<Int32(Int32)> function2 = nullptr;

    // Constructs Axis::Function object with default constructor
    Function<Int32(Int32)> function3;

    DOCTEST_CHECK(function1);
    DOCTEST_CHECK(!function2);
    DOCTEST_CHECK(!function3);

    // Creates new scope
    {
        Function<Int32()> leakTesting = LeakDetector();

        // We should have an instance of LeakDetector now
        DOCTEST_CHECK(InstanceCount == 1);

        Function<Int32()> leakTestingCopy = leakTesting;

        // We should have 2 instances of LeakDetector now
        DOCTEST_CHECK(InstanceCount == 2);

        Function<Int32()> leakTestingMove = std::move(leakTesting);

        // We should have 2 instances of LeakDetector now
        // Because the move constructor is called and the original object is destroyed
        DOCTEST_CHECK(InstanceCount == 2);
    }

    // No leak
    DOCTEST_CHECK(InstanceCount == 0);
}