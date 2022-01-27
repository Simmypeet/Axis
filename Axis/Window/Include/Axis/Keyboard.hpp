/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Keyboard.hpp
/// 
/// \brief Contains the functions and classes for retrieving keyboard inputs.

#ifndef AXIS_WINDOW_KEYBOARD_HPP
#define AXIS_WINDOW_KEYBOARD_HPP
#pragma once

#include "../../../System/Include/Axis/List.hpp"
#include "Input.hpp"
#include "WindowExport.hpp"

namespace Axis
{

/// \brief Enumerations of all keys on the keyboard.
enum class Key : Uint8
{
    /// \brief Escape key
    Escape,

    /// \brief F1 key
    F1,

    /// \brief F2 key
    F2,

    /// \brief F3 key
    F3,

    /// \brief F4 key
    F4,

    /// \brief F5 key
    F5,

    /// \brief F6 key
    F6,

    /// \brief F7 key
    F7,

    /// \brief F8 key
    F8,

    /// \brief F9 key
    F9,

    /// \brief F10 key
    F10,

    /// \brief F11 key
    F11,

    /// \brief F12 key
    F12,

    /// \brief PrintScreen key
    PrintScreen,

    /// \brief ScrollLock key
    ScrollLock,

    /// \brief PauseBreak key
    PauseBreak,

    /// \brief Tilde key
    Tilde,

    /// \brief Number one key
    One,

    /// \brief Number two key
    Two,

    /// \brief Number three key
    Three,

    /// \brief Number four key
    Four,

    /// \brief Number five key
    Five,

    /// \brief Number six key
    Six,

    /// \brief Number seven key
    Seven,

    /// \brief Number eight key
    Eight,

    /// \brief Number nine key
    Nine,

    /// \brief Number zero key
    Zero,

    /// \brief Hyphen key
    Hyphen,

    /// \brief Equal key
    Equal,

    /// \brief Back key (Backspace)
    Back,

    /// \brief Insert key
    Insert,

    /// \brief Home key
    Home,

    /// \brief Page up key
    PageUp,

    /// \brief Num lock key
    NumLock,

    /// \brief Numpad divide key (/)
    Divide,

    /// \brief Numpad multiply key (*)
    Multiply,

    /// \brief Numpad minus key (-)
    Minus,

    /// \brief Tab key
    Tab,

    /// \brief Delete key
    Delete,

    /// \brief End key
    End,

    /// \brief Page down key
    PageDown,

    /// \brief Numpad plus key (+)
    Plus,

    /// \brief Caps lock key
    CapsLock,

    /// \brief Enter (return) key
    Enter,

    /// \brief Left shift key
    LeftShift,

    /// \brief Right shift key
    RightShift,

    /// \brief Numpad decimal key (.)
    Decimal,

    /// \brief Numpad enter (return) key
    NumpadEnter,

    /// \brief Left control key
    LeftCtrl,

    /// \brief Left alt key
    LeftAlt,

    /// \brief Space (space bar) key
    Space,

    /// \brief Right alt key
    RightAlt,

    /// \brief Right control key
    RightCtrl,

    /// \brief Arrow up key
    ArrowUp,

    /// \brief Arrow up left
    ArrowLeft,

    /// \brief Arrow up down
    ArrowDown,

    /// \brief Arrow up right
    ArrowRight,

    /// \brief Numpad number zero key
    Numpad0,

    /// \brief Numpad number one key
    Numpad1,

    /// \brief Numpad number two key
    Numpad2,

    /// \brief Numpad number three key
    Numpad3,

    /// \brief Numpad number four key
    Numpad4,

    /// \brief Numpad number five key
    Numpad5,

    /// \brief Numpad number six key
    Numpad6,

    /// \brief Numpad number seven key
    Numpad7,

    /// \brief Numpad number eight key
    Numpad8,

    /// \brief Numpad number nine key
    Numpad9,

    /// \brief Character `A` key
    A,

    /// \brief Character `B` key
    B,

    /// \brief Character `C` key
    C,

    /// \brief Character `D` key
    D,

    /// \brief Character `E` key
    E,

    /// \brief Character `F` key
    F,

    /// \brief Character `G` key
    G,

    /// \brief Character `H` key
    H,

    /// \brief Character `I` key
    I,

    /// \brief Character `J` key
    J,

    /// \brief Character `K` key
    K,

    /// \brief Character `L` key
    L,

    /// \brief Character `M` key
    M,

    /// \brief Character `N` key
    N,

    /// \brief Character `O` key
    O,

    /// \brief Character `P` key
    P,

    /// \brief Character `Q` key
    Q,

    /// \brief Character `R` key
    R,

    /// \brief Character `S` key
    S,

    /// \brief Character `T` key
    T,

    /// \brief Character `U` key
    U,

    /// \brief Character `V` key
    V,

    /// \brief Character `W` key
    W,

    /// \brief Character `X` key
    X,

    /// \brief Character `Y` key
    Y,

    /// \brief Character `Z` key
    Z,

    /// \brief Left bracket key
    LeftBracket,

    /// \brief Right bracket key
    RightBracket,

    /// \brief Back slash key
    BackSlash,

    /// \brief Semi colon key
    SemiColon,

    /// \brief Apostrophe key
    Apostrophe,

    /// \brief Comma key
    Comma,

    /// \brief Perido key
    Period,

    /// \brief Slash key
    Slash,
};

/// \brief Holds key strokes information.
/// 
/// \warning To be implemented.
class AXIS_WINDOW_API KeyboardState final
{
public:
    /// Constructor
    KeyboardState(Uint64 keyStates1,
                  Uint64 keyStates2) noexcept;

    /// \brief Checks if the given key is pressed/down.
    AXIS_NODISCARD Bool IsKeyDown(Key key) const;

    /// \brief Checks if the given key is released/up.
    AXIS_NODISCARD Bool IsKeyUp(Key key) const;

    /// \brief Gets all the keys that are pressed.
    AXIS_NODISCARD List<Key> GetPressedKeys() const noexcept;

    /// \brief Gets all the keys that are released.
    AXIS_NODISCARD List<Key> GetReleasedKeys() const noexcept;

    /// \brief Gets the number of keys that are pressed.
    AXIS_NODISCARD Uint32 GetPressedKeyCount() const noexcept;

    /// \brief Gets the number of keys that are released.
    AXIS_NODISCARD Uint32 GetReleasedKeyCount() const noexcept;

    /// \brief Equals operator
    AXIS_NODISCARD inline Bool operator==(const KeyboardState& other) const noexcept { return _keyStates1 == other._keyStates1 && _keyStates2 == other._keyStates2; }

    /// \brief Not equals operator.
    AXIS_NODISCARD inline Bool operator!=(const KeyboardState& other) const noexcept { return !(*this == other); }

private:
    /// Private member
    Uint64 _keyStates1; ///< Holds the first 64 bits of the key states.
    Uint64 _keyStates2; ///< Holds the second 64 bits of the key states.
};

namespace Keyboard
{

/// \brief Retrieves keystrokes information.
/// 
/// \returns \a `Axis::KeyboardState` object containing keystrokes.
AXIS_NODISCARD KeyboardState AXIS_WINDOW_API GetKeyboardState();

} // namespace Keyboard

} // namespace Axis

#endif // AXIS_WINDOW_KEYBOARD_HPP