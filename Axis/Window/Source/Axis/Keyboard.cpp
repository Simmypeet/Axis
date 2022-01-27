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

KeyboardState::KeyboardState(Uint64 keyStates1,
                             Uint64 keyStates2) noexcept :
    _keyStates1(keyStates1),
    _keyStates2(keyStates2) {}

Bool KeyboardState::IsKeyDown(Key key) const
{
    if (Enum::GetUnderlyingValue(key) > Enum::GetUnderlyingValue(Key::Slash))
        throw InvalidArgumentException("The given `key` was invalid!");

    if (Enum::GetUnderlyingValue(key) < 64)
        return Math::ReadBitPosition(_keyStates1, Enum::GetUnderlyingValue(key));
    else
        return Math::ReadBitPosition(_keyStates2, Enum::GetUnderlyingValue(key) - 64);
}

Bool KeyboardState::IsKeyUp(Key key) const
{
    if (Enum::GetUnderlyingValue(key) > Enum::GetUnderlyingValue(Key::Slash))
        throw InvalidArgumentException("The given `key` was invalid!");

    if (Enum::GetUnderlyingValue(key) < 64)
        return !Math::ReadBitPosition(_keyStates1, Enum::GetUnderlyingValue(key));
    else
        return !Math::ReadBitPosition(_keyStates2, Enum::GetUnderlyingValue(key) - 64);
}

List<Key> KeyboardState::GetPressedKeys() const noexcept
{
    List<Key> pressedKeys;

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (Math::ReadBitPosition(_keyStates1, i))
            pressedKeys.Append(static_cast<Key>(i));
    }

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (Math::ReadBitPosition(_keyStates2, i))
            pressedKeys.Append(static_cast<Key>(i + 64));
    }

    return pressedKeys;
}

List<Key> KeyboardState::GetReleasedKeys() const noexcept
{
    List<Key> releasedKeys;

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (!Math::ReadBitPosition(_keyStates1, i))
            releasedKeys.Append(static_cast<Key>(i));
    }

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (!Math::ReadBitPosition(_keyStates2, i))
            releasedKeys.Append(static_cast<Key>(i + 64));
    }

    return releasedKeys;
}

Uint32 KeyboardState::GetPressedKeyCount() const noexcept
{
    Uint32 pressedKeyCount = 0;

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (Math::ReadBitPosition(_keyStates1, i))
            ++pressedKeyCount;
    }

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (Math::ReadBitPosition(_keyStates2, i))
            ++pressedKeyCount;
    }

    return pressedKeyCount;
}

Uint32 KeyboardState::GetReleasedKeyCount() const noexcept
{
    Uint32 releasedKeyCount = 0;

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (!Math::ReadBitPosition(_keyStates1, i))
            ++releasedKeyCount;
    }

    for (Uint8 i = 0; i < 64; ++i)
    {
        if (!Math::ReadBitPosition(_keyStates2, i))
            ++releasedKeyCount;
    }

    return releasedKeyCount;
}

} // namespace Axis