# Coding style

## General

**Formatting:** Use the provided `.clang-format` to format the source files.

**Spaces vs tab:** Use 4 spaces instead of a tab for the consistency of the code appearance across multiple IDEs and settings.

**Documentation:** Use Doxygen documentation comment style for public API codes.

**Prefer verbosity over conciseness:** Prefer lengthy function name but self-explanatory and can be understood by reading its name.

**Uses `using` integer with size:** Int8, Int16, Int32, Int64, Uint8, Uint16, Uint32, Uint64.

**Bracing:** Open brace should always be at the start of the new line after the statement that begins the block.

## File

**Filename:** Uses Pascal case to name your file or conveniently names the files after the classes.

**Public header files**: Public header files should be in the `Include` directory and the file extension should be `.hpp` for C++ headers and `.h` for C headers.

**Private header files**: Any header files that the clients shouldn't include should be in the `Private` directory.

**Source files:** Source files should be in the `Source` directory and the file extension should be `.cpp` for C++ source files and `.c` for C source files.

## Naming

### Class, function, and public class members

Use Pascal case naming convention, examples

```cxx
class IGraphicsDevice
{
public:
    void WaitDeviceIdle();
    const GraphicsAdapter Adapter;
}
```

### Private / protected class members

Use camel case prefixes with an underscore, example:

```cxx
class IGraphicsDevice
{
public:
    void WaitDeviceIdle();
    const GraphicsAdapter Adapter;

private:
    Int32 _deviceId;
}
```

### Local variables and parameters

Use camel case naming convention, example:

```cxx
Int32 Multiply(Int32 num, Int32 num1)
{
    Int32 result = num * num1;
    return result;
}
```

### Namespace

-   Use Pascal case naming convention.
-   Don't indent the contents inside the namespace, for example:

```CXX
namespace Axis
{

Int32 Multiply(Int32 num, Int32 num1)
{
    Int32 result = num * num1;
    return result;
}

} // namespace Axis
```

### Nested namespace

-   Don't indent any nested namespaces and contents:

```cxx
namespace Axis
{

namespace Math
{

Int32 Multiply(Int32 num, Int32 num1)
{
    Int32 result = num * num1;
    return result;
}

} // namespace Math

} // namespace Axis
```

## Other

### Class visibility orders

All the public methods/members should appear first before private or protected methods/members.

```cxx
namespace Axis
{

class SpriteBatch
{
public:
    void Draw();
    void Flush();

private:
    SharedPointer<IBuffer> _vertexBuffer;
    SharedPointer<IBuffer> _indexBuffer;
};

} // namespace Axis
```

### Default member initialization

Prefer using [default member initializers](https://en.cppreference.com/w/cpp/language/constructor) rather than default constructor full with member initializer lists.

```cxx
namespace Axis
{

class SpriteBatch
{
public:
    void Draw();
    void Flush();

private:
    SharedPointer<IBuffer> _vertexBuffer = nullptr; // default member initializer
    SharedPointer<IBuffer> _indexBuffer = nullptr;  // default member initializer
    Size _batchCount = 0;                 // default member initializer
};

} // namespace Axis
```
