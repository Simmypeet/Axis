/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/WindowPch.hpp>

#include <Axis/Enum.hpp>
#include <Axis/Exception.hpp>
#include <Axis/Keyboard.hpp>
#include <Axis/Math.hpp>


namespace Axis
{

namespace Window
{

KeyboardState::KeyboardState(Uint64 keyStates1,
                             Uint64 keyStates2) noexcept :
    _keyStates1(keyStates1),
    _keyStates2(keyStates2) {}

Bool KeyboardState::IsKeyDown(Key key) const
{
    if (System::Enum::GetUnderlyingValue(key) > System::Enum::GetUnderlyingValue(Key::Slash))
        throw System::InvalidArgumentException("The given `key` was invalid!");

    if (System::Enum::GetUnderlyingValue(key) < 64)
        return System::Math::ReadBitPosition(_keyStates1, System::Enum::GetUnderlyingValue(key));
    else
        return System::Math::ReadBitPosition(_keyStates2, System::Enum::GetUnderlyingValue(key) - 64);
}

Bool KeyboardState::IsKeyUp(Key key) const
{
    if (System::Enum::GetUnderlyingValue(key) > System::Enum::GetUnderlyingValue(Key::Slash))
        throw System::InvalidArgumentException("The given `key` was invalid!");

    if (System::Enum::GetUnderlyingValue(key) < 64)
        return !System::Math::ReadBitPosition(_keyStates1, System::Enum::GetUnderlyingValue(key));
    else
        return !System::Math::ReadBitPosition(_keyStates2, System::Enum::GetUnderlyingValue(key) - 64);
}

System::List<Key> KeyboardState::GetPressedKeys() const noexcept
{
    System::List<Key> pressedKeys;

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (System::Math::ReadBitPosition(_keyStates1, i))
            pressedKeys.Append(static_cast<Key>(i));
    }

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (System::Math::ReadBitPosition(_keyStates2, i))
            pressedKeys.Append(static_cast<Key>(i + 64));
    }

    return pressedKeys;
}

System::List<Key> KeyboardState::GetReleasedKeys() const noexcept
{
    System::List<Key> releasedKeys;

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (!System::Math::ReadBitPosition(_keyStates1, i))
            releasedKeys.Append(static_cast<Key>(i));
    }

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (!System::Math::ReadBitPosition(_keyStates2, i))
            releasedKeys.Append(static_cast<Key>(i + 64));
    }

    return releasedKeys;
}

Uint32 KeyboardState::GetPressedKeyCount() const noexcept
{
    Uint32 pressedKeyCount = 0;

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (System::Math::ReadBitPosition(_keyStates1, i))
            ++pressedKeyCount;
    }

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (System::Math::ReadBitPosition(_keyStates2, i))
            ++pressedKeyCount;
    }

    return pressedKeyCount;
}

Uint32 KeyboardState::GetReleasedKeyCount() const noexcept
{
    Uint32 releasedKeyCount = 0;

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (!System::Math::ReadBitPosition(_keyStates1, i))
            ++releasedKeyCount;
    }

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (!System::Math::ReadBitPosition(_keyStates2, i))
            ++releasedKeyCount;
    }

    return releasedKeyCount;
}

} // namespace Window

} // namespace Axis