#include <Axis/Event.hpp>
#include <doctest.h>

DOCTEST_TEST_CASE("Axis event : [Axis::System]")
{
    using namespace Axis;

    // Counts the number of times the event is invoked.
    Int32 counter = 0;

    // Event that will be raised.
    Event<void(Int32&)> event;

    // Subscribes a handler to the event.
    Size token1 = event.EventRegister.Add([&](Int32& value) { value++; });

    // Invokes the event.
    event(counter);

    // Checks if the event was invoked.
    CHECK(counter == 1);

    // Subscribes another handler to the event.
    Size token2 = event.EventRegister.Add([&](Int32& value) { value += 2; });

    // Invokes the event.
    event(counter);

    // Checks if the event was invoked.
    CHECK(counter == 4);

    // Unsubscribes the first handler.
    event.EventRegister.Remove(token1);

    // Invokes the event.
    event(counter);

    // Checks if the event was invoked.
    CHECK(counter == 6);

    // Unsubscribes the second handler.
    event.EventRegister.Remove(token2);

    // Invokes the event.
    event(counter);

    // Counter should be the same as it was before unsubscribing.
    CHECK(counter == 6);

    DOCTEST_SUBCASE("Token generation")
    {
        Event<void()> event;

        // Subscribes a handler to the event.
        Size token1 = event.EventRegister.Add([&]() {});

        // Subscribes another handler to the event.
        Size token2 = event.EventRegister.Add([&]() {});

        // Checks if the tokens are different.
        CHECK(token1 != token2);

        // Unsubscribes the first handler.
        Bool result = event.EventRegister.Remove(token1);

        // Checks if the handler was removed.
        CHECK(result);

        // Checks if the token is still valid.
        CHECK(!event.EventRegister.TokenExists(token1));
        CHECK(event.EventRegister.TokenExists(token2));

    }
}