#include <Axis/Vector3.hpp>
#include <doctest.h>

DOCTEST_TEST_CASE("Vector3 template class : [Axis::System]")
{
    DOCTEST_SUBCASE("`Axis::Vector3` constructors")
    {
        DOCTEST_SUBCASE("Default constructor")
        {
            using namespace Axis;

            // Default vector
            Vector3<Int32> vector;

            // Checks if the vector is empty
            DOCTEST_CHECK(vector.X == 0);
            DOCTEST_CHECK(vector.Y == 0);
            DOCTEST_CHECK(vector.Z == 0);

            // Default vector in float
            Vector3<Float32> vectorF;

            // Checks if the vector is empty
            DOCTEST_CHECK(vectorF.X == 0.0f);
            DOCTEST_CHECK(vectorF.Y == 0.0f);
            DOCTEST_CHECK(vectorF.Z == 0.0f);
        }

        DOCTEST_SUBCASE("Coordinate constructor")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == 1);
            DOCTEST_CHECK(vector.Y == 2);
            DOCTEST_CHECK(vector.Z == 3);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vectorF.X == 1.0f);
            DOCTEST_CHECK(vectorF.Y == 2.0f);
            DOCTEST_CHECK(vectorF.Z == 3.0f);
        }

        DOCTEST_SUBCASE("Conversion constructor")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(Vector3<Float32>(1.0f, 2.0f, 3.0f));

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == 1);
            DOCTEST_CHECK(vector.Y == 2);
            DOCTEST_CHECK(vector.Z == 3);
        }
    }

    DOCTEST_SUBCASE("Arithmetic operators")
    {
        DOCTEST_SUBCASE("Addition")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Adds a vector
            Vector3<Int32> vector2 = vector + Vector3<Int32>(4, 5, 6);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2.X == 5);
            DOCTEST_CHECK(vector2.Y == 7);
            DOCTEST_CHECK(vector2.Z == 9);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Adds a vector
            Vector3<Float32> vector2F = vectorF + Vector3<Float32>(4.0f, 5.0f, 6.0f);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2F.X == 5.0f);
            DOCTEST_CHECK(vector2F.Y == 7.0f);
            DOCTEST_CHECK(vector2F.Z == 9.0f);
        }

        DOCTEST_SUBCASE("Subtraction")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Subtracts a vector
            Vector3<Int32> vector2 = vector - Vector3<Int32>(4, 5, 6);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2.X == -3);
            DOCTEST_CHECK(vector2.Y == -3);
            DOCTEST_CHECK(vector2.Z == -3);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Subtracts a vector
            Vector3<Float32> vector2F = vector - Vector3<Float32>(4.0f, 5.0f, 6.0f);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2F.X == -3.0f);
            DOCTEST_CHECK(vector2F.Y == -3.0f);
            DOCTEST_CHECK(vector2F.Z == -3.0f);
        }

        DOCTEST_SUBCASE("Multiplication")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Multiplies a vector
            Vector3<Int32> vector2 = vector * Vector3<Int32>(4, 5, 6);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2.X == 4);
            DOCTEST_CHECK(vector2.Y == 10);
            DOCTEST_CHECK(vector2.Z == 18);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Multiplies a vector
            Vector3<Float32> vector2F = vectorF * Vector3<Float32>(4.0f, 5.0f, 6.0f);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2F.X == 4.0f);
            DOCTEST_CHECK(vector2F.Y == 10.0f);
            DOCTEST_CHECK(vector2F.Z == 18.0f);
        }

        DOCTEST_SUBCASE("Division")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Divides a vector
            Vector3<Int32> vector2 = vector / Vector3<Int32>(4, 5, 6);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2.X == 0);
            DOCTEST_CHECK(vector2.Y == 0);
            DOCTEST_CHECK(vector2.Z == 0);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Divides a vector
            Vector3<Float32> vector2F = vectorF / Vector3<Float32>(4.0f, 5.0f, 6.0f);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2F.X == 1.0f / 4.0f);
            DOCTEST_CHECK(vector2F.Y == 2.0f / 5.0f);
            DOCTEST_CHECK(vector2F.Z == 3.0f / 6.0f);
        }

        DOCTEST_SUBCASE("Multiplication by a scalar value")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Multiplies a vector
            Vector3<Int32> vector2 = vector * 4;

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2.X == 4);
            DOCTEST_CHECK(vector2.Y == 8);
            DOCTEST_CHECK(vector2.Z == 12);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Multiplies a vector
            Vector3<Float32> vector2F = vectorF * 4.0f;

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2F.X == 4.0f);
            DOCTEST_CHECK(vector2F.Y == 8.0f);
            DOCTEST_CHECK(vector2F.Z == 12.0f);
        }

        DOCTEST_SUBCASE("Division by a scalar value")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Divides a vector
            Vector3<Int32> vector2 = vector / 4;

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2.X == 0);
            DOCTEST_CHECK(vector2.Y == 0);
            DOCTEST_CHECK(vector2.Z == 0);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Divides a vector
            Vector3<Float32> vector2F = vectorF / 4.0f;

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector2F.X == 1.0f / 4.0f);
            DOCTEST_CHECK(vector2F.Y == 2.0f / 4.0f);
            DOCTEST_CHECK(vector2F.Z == 3.0f / 4.0f);
        }
    }

    DOCTEST_SUBCASE("Compound arithmetic operators")
    {
        DOCTEST_SUBCASE("Addition")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Adds a vector
            vector += Vector3<Int32>(4, 5, 6);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == 5);
            DOCTEST_CHECK(vector.Y == 7);
            DOCTEST_CHECK(vector.Z == 9);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Adds a vector
            vectorF += Vector3<Float32>(4.0f, 5.0f, 6.0f);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vectorF.X == 5.0f);
            DOCTEST_CHECK(vectorF.Y == 7.0f);
            DOCTEST_CHECK(vectorF.Z == 9.0f);
        }

        DOCTEST_SUBCASE("Subtraction")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Subtracts a vector
            vector -= Vector3<Int32>(4, 5, 6);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == -3);
            DOCTEST_CHECK(vector.Y == -3);
            DOCTEST_CHECK(vector.Z == -3);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Subtracts a vector
            vectorF -= Vector3<Float32>(4.0f, 5.0f, 6.0f);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vectorF.X == -3.0f);
            DOCTEST_CHECK(vectorF.Y == -3.0f);
            DOCTEST_CHECK(vectorF.Z == -3.0f);
        }

        DOCTEST_SUBCASE("Multiplication")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Multiplies a vector
            vector *= Vector3<Int32>(4, 5, 6);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == 4);
            DOCTEST_CHECK(vector.Y == 10);
            DOCTEST_CHECK(vector.Z == 18);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Multiplies a vector
            vectorF *= Vector3<Float32>(4.0f, 5.0f, 6.0f);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vectorF.X == 4.0f);
            DOCTEST_CHECK(vectorF.Y == 10.0f);
            DOCTEST_CHECK(vectorF.Z == 18.0f);
        }

        DOCTEST_SUBCASE("Division")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Divides a vector
            vector /= Vector3<Int32>(4, 5, 6);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == 0);
            DOCTEST_CHECK(vector.Y == 0);
            DOCTEST_CHECK(vector.Z == 0);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Divides a vector
            vectorF /= Vector3<Float32>(4.0f, 5.0f, 6.0f);

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vectorF.X == 1.0f / 4.0f);
            DOCTEST_CHECK(vectorF.Y == 2.0f / 5.0f);
            DOCTEST_CHECK(vectorF.Z == 3.0f / 6.0f);
        }

        DOCTEST_SUBCASE("Multiplication by a scalar value")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Multiplies a vector
            vector *= 4;

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == 4);
            DOCTEST_CHECK(vector.Y == 8);
            DOCTEST_CHECK(vector.Z == 12);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Multiplies a vector
            vectorF *= 4.0f;

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vectorF.X == 4.0f);
            DOCTEST_CHECK(vectorF.Y == 8.0f);
            DOCTEST_CHECK(vectorF.Z == 12.0f);
        }

        DOCTEST_SUBCASE("Division by a scalar value")
        {
            using namespace Axis;

            // Constructs a vector
            Vector3<Int32> vector(1, 2, 3);

            // Divides a vector
            vector /= 4;

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vector.X == 0);
            DOCTEST_CHECK(vector.Y == 0);
            DOCTEST_CHECK(vector.Z == 0);

            // Constructs a vector in float
            Vector3<Float32> vectorF = {1.0f, 2.0f, 3.0f};

            // Divides a vector
            vectorF /= 4.0f;

            // Checks if the vector contains the correct amount of coordinates
            DOCTEST_CHECK(vectorF.X == 1.0f / 4.0f);
            DOCTEST_CHECK(vectorF.Y == 2.0f / 4.0f);
            DOCTEST_CHECK(vectorF.Z == 3.0f / 4.0f);
        }
    }

    DOCTEST_SUBCASE("Comparison operators")
    {
        DOCTEST_SUBCASE("Equality")
        {
            using namespace Axis;

            // Constructs two vectors
            Vector3<Int32> vector1(1, 2, 3);
            Vector3<Int32> vector2(1, 2, 3);

            // Checks if the two vectors are equal
            DOCTEST_CHECK(vector1 == vector2);
        }

        DOCTEST_SUBCASE("Inequality")
        {
            using namespace Axis;

            // Constructs two vectors
            Vector3<Int32> vector1(1, 2, 3);
            Vector3<Int32> vector2(1, 3, 3);

            // Checks if the two vectors are not equal
            DOCTEST_CHECK(vector1 != vector2);
        }
    }

    DOCTEST_SUBCASE("Magnitude")
    {
        using namespace Axis;

        Vector3<Int32> vector(2, 3, 4);

        // Checks if the magnitude of the vector is correct
        DOCTEST_CHECK(Math::IsFloatEqual(vector.GetMagnitude(), std::sqrt(29)));
    }

    DOCTEST_SUBCASE("Normalize")
    {
        using namespace Axis;

        Vector3<Float32> vector(2, 3, 4);

        // Normalizes the vector
        vector.Normalize();

        // Checks if the vector is normalized
        DOCTEST_CHECK(Math::IsFloatEqual(vector.GetMagnitude(), 1.0f));
    }

    DOCTEST_SUBCASE("DotProduct")
    {
        using namespace Axis;

        Vector3<Int32> vector1(2, 3, 4);
        Vector3<Int32> vector2(3, 4, 5);

        // Checks if the dot product of the two vectors is correct
        DOCTEST_CHECK(vector1.DotProduct(vector2) == 38);
    }

    DOCTEST_SUBCASE("CrossProduct")
    {
        using namespace Axis;

        Vector3<Int32> vector1(2, 3, 4);
        Vector3<Int32> vector2(3, 4, 5);

        // Checks if the cross product of the two vectors is correct
        DOCTEST_CHECK(vector1.CrossProduct(vector2) == Vector3<Int32>(-1, 2, -1));
    }
}