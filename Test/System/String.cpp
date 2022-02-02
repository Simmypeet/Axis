#include <Axis/System>
#include <doctest.h>

DOCTEST_TEST_CASE("Axis string data structure : [Axis-System]")
{
    DOCTEST_SUBCASE("`Axis::String` constructors")
    {
        DOCTEST_SUBCASE("Constructor")
        {
            using namespace Axis;

            // Default string (empty)
            String8 str;

            // Checks if the string is empty
            DOCTEST_CHECK(str == nullptr);

            // Constructs a string
            String8 helloWorld = "Hello World";

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(helloWorld.GetLength() == 11);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(helloWorld == "Hello World");

            // Constructs a string with a nullptr
            String8 nullString = nullptr;

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(nullString.GetLength() == 0);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(nullString == "");

            // Compares string with a nullptr
            DOCTEST_CHECK(nullString == nullptr);

            // Constructs a string with a nullptr
            String8 emptyString = "";

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(emptyString.GetLength() == 0);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(emptyString == "");

            // Compares string with a nullptr
            DOCTEST_CHECK(emptyString == nullptr);

            // Creates a string which probably uses dynamic memory
            String8 longString = "This is a long string which probably uses dynamic memory";

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(longString.GetLength() == 56);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(longString == "This is a long string which probably uses dynamic memory");

            // Compares string with a nullptr
            DOCTEST_CHECK(longString != nullptr);

            // Compares string with an empty string
            DOCTEST_CHECK(longString != "");

            // Creates `Axis::String8` from WChar string
            String8 helloWorldCross = L"Hello World";

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(helloWorldCross.GetLength() == 11);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(helloWorldCross == "Hello World");
        }

        DOCTEST_SUBCASE("Move constructor")
        {
            using namespace Axis;

            // Constructs a string
            String8 longString = "This is a long string which probably uses dynamic memory";

            // Moves a string
            String8 movedLongString = std::move(longString);

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(movedLongString.GetLength() == 56);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(movedLongString == "This is a long string which probably uses dynamic memory");

            // Compares string with a nullptr
            DOCTEST_CHECK(movedLongString != nullptr);

            // Compares string with an empty string
            DOCTEST_CHECK(movedLongString != "");

            // Moved string is empty
            DOCTEST_CHECK(longString == nullptr);
        }

        DOCTEST_SUBCASE("Copy constructor")
        {
            using namespace Axis;

            // Constructs a string
            String8 helloWorld = "Hello World";

            // Copy constructor
            String8 copyHelloWorld = helloWorld;

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(copyHelloWorld.GetLength() == 11);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(copyHelloWorld == "Hello World");

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(helloWorld.GetLength() == 11);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(helloWorld == "Hello World");

            // Constructs a long string
            String8 longString = "This is a long string which probably uses dynamic memory";

            // Copy constructor
            String8 copyLongString = longString;

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(copyLongString.GetLength() == 56);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(copyLongString == "This is a long string which probably uses dynamic memory");

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(longString.GetLength() == 56);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(longString == "This is a long string which probably uses dynamic memory");
        }
    }

    DOCTEST_SUBCASE("`Axis::String` assignment operator")
    {
        DOCTEST_SUBCASE("Copy assignment operator")
        {
            using namespace Axis;

            // Constructs a string
            String8 helloWorld = "Hello World";

            // Constructs another string
            String8 anotherString = "Another string";

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(helloWorld.GetLength() == 11);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(helloWorld == "Hello World");

            // Copy assignment operator
            helloWorld = anotherString;

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(helloWorld.GetLength() == 14);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(helloWorld == "Another string");

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(anotherString.GetLength() == 14);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(anotherString == "Another string");
        }

        DOCTEST_SUBCASE("Move assignment operator")
        {
            using namespace Axis;

            // Constructs a string
            String8 helloWorld = "Hello World";

            // Constructs another string
            String8 anotherString = "Another string";

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(helloWorld.GetLength() == 11);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(helloWorld == "Hello World");

            // Move assignment operator
            helloWorld = std::move(anotherString);

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(helloWorld.GetLength() == 14);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(helloWorld == "Another string");

            // Checks if the string contains the correct amount of characters
            DOCTEST_CHECK(anotherString.GetLength() == 0);

            // Checks if the string contains the correct characters
            DOCTEST_CHECK(anotherString == "");
        }
    }

    DOCTEST_SUBCASE("`Axis::String` comparison operators")
    {
        using namespace Axis;

        // Compares two strings
        DOCTEST_CHECK(String8("Hello World") == String8("Hello World"));

        // Compares two strings
        DOCTEST_CHECK(String8("Hello World") != String8("Hello World!"));

        // Compares two strings with different character types
        DOCTEST_CHECK(String8("Hello World") == WString(L"Hello World"));

        // Compares with nullptr
        DOCTEST_CHECK(String8("Hello World") != nullptr);

        // Compares with empty string
        DOCTEST_CHECK(String8("Hello World") != "");

        // Compares with a nullptr
        DOCTEST_CHECK(String8() == nullptr);

        // Compares with an empty string
        DOCTEST_CHECK(String8() == "");
    }

    DOCTEST_SUBCASE("`Axis::String` subscript operator")
    {
        using namespace Axis;

        // Constructs a string
        String8 helloWorld = "Hello World";

        // Subscripts the string with an index and checks if the character is correct
        DOCTEST_CHECK(helloWorld[0] == 'H');
        DOCTEST_CHECK(helloWorld[1] == 'e');
        DOCTEST_CHECK(helloWorld[2] == 'l');
        DOCTEST_CHECK(helloWorld[3] == 'l');
        DOCTEST_CHECK(helloWorld[4] == 'o');
        DOCTEST_CHECK(helloWorld[5] == ' ');
        DOCTEST_CHECK(helloWorld[6] == 'W');
        DOCTEST_CHECK(helloWorld[7] == 'o');
        DOCTEST_CHECK(helloWorld[8] == 'r');
        DOCTEST_CHECK(helloWorld[9] == 'l');
        DOCTEST_CHECK(helloWorld[10] == 'd');

        // Out of range subscript, should throws an exception
        try
        {
            helloWorld[11] = '!';
        }
        catch (::Axis::Exception&)
        {
            DOCTEST_CHECK(true);
        }
    }
}