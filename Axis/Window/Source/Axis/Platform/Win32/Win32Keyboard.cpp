/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include "Axis/Math.hpp"
#include <Axis/WindowPch.hpp>

#include <Axis/Enum.hpp>
#include <Axis/Keyboard.hpp>
#include <Windows.h>

namespace Axis
{

namespace Window
{

namespace Keyboard
{

KeyboardState GetKeyboardState()
{
    constexpr auto AxisKeyToWindowsAPIKey = [](Key key) -> int {
        switch (key)
        {
            // clang-format off
            case Key::Escape:       return VK_ESCAPE;
            case Key::F1:           return VK_F1;
            case Key::F2:           return VK_F2;
            case Key::F3:           return VK_F3;
            case Key::F4:           return VK_F4;
            case Key::F5:           return VK_F5;
            case Key::F6:           return VK_F6;
            case Key::F7:           return VK_F7;
            case Key::F8:           return VK_F8;
            case Key::F9:           return VK_F9;
            case Key::F10:          return VK_F10;
            case Key::F11:          return VK_F11;
            case Key::F12:          return VK_F12;
            case Key::PrintScreen:  return VK_SNAPSHOT;
            case Key::ScrollLock:   return VK_SCROLL;
            case Key::PauseBreak:   return VK_PAUSE;
            case Key::Tilde:        return VK_OEM_3;
            case Key::One:          return 1;
            case Key::Two:          return 2;
            case Key::Three:        return 3;
            case Key::Four:         return 4;
            case Key::Five:         return 5;
            case Key::Six:          return 6;
            case Key::Seven:        return 7;
            case Key::Eight:        return 8;
            case Key::Nine:         return 9;
            case Key::Zero:         return 0;
            case Key::Hyphen:       return VK_OEM_MINUS;
            case Key::Equal:        return VK_OEM_PLUS;
            case Key::Back:         return VK_BACK;
            case Key::Insert:       return VK_INSERT;
            case Key::Home:         return VK_HOME;
            case Key::PageUp:       return VK_PRIOR;
            case Key::NumLock:      return VK_NUMLOCK;
            case Key::Divide:       return VK_DIVIDE;
            case Key::Multiply:     return VK_MULTIPLY;
            case Key::Minus:        return VK_SUBTRACT;
            case Key::Tab:          return VK_TAB;
            case Key::Delete:       return VK_DELETE;
            case Key::End:          return VK_END;
            case Key::PageDown:     return VK_NEXT;
            case Key::Plus:         return VK_ADD;
            case Key::CapsLock:     return VK_CAPITAL;
            case Key::Enter:        return VK_RETURN;
            case Key::LeftShift:    return VK_LSHIFT;
            case Key::RightShift:   return VK_RSHIFT;
            case Key::Decimal:      return VK_DECIMAL;
            case Key::NumpadEnter:  return VK_RETURN;
            case Key::LeftCtrl:     return VK_LCONTROL;
            case Key::LeftAlt:      return VK_LMENU;
            case Key::Space:        return VK_SPACE;
            case Key::RightAlt:     return VK_RMENU;
            case Key::RightCtrl:    return VK_RCONTROL;
            case Key::ArrowUp:      return VK_UP;
            case Key::ArrowLeft:    return VK_LEFT;
            case Key::ArrowDown:    return VK_DOWN;
            case Key::ArrowRight:   return VK_RIGHT;
            case Key::Numpad0:      return VK_NUMPAD0;
            case Key::Numpad1:      return VK_NUMPAD1;
            case Key::Numpad2:      return VK_NUMPAD2;
            case Key::Numpad3:      return VK_NUMPAD3;
            case Key::Numpad4:      return VK_NUMPAD4;
            case Key::Numpad5:      return VK_NUMPAD5;
            case Key::Numpad6:      return VK_NUMPAD6;
            case Key::Numpad7:      return VK_NUMPAD7;
            case Key::Numpad8:      return VK_NUMPAD8;
            case Key::Numpad9:      return VK_NUMPAD9;
            case Key::A:            return 'A';
            case Key::B:            return 'B';
            case Key::C:            return 'C';
            case Key::D:            return 'D';
            case Key::E:            return 'E';
            case Key::F:            return 'F';
            case Key::G:            return 'G';
            case Key::H:            return 'H';
            case Key::I:            return 'I';
            case Key::J:            return 'J';
            case Key::K:            return 'K';
            case Key::L:            return 'L';
            case Key::M:            return 'M';
            case Key::N:            return 'N';
            case Key::O:            return 'O';
            case Key::P:            return 'P';
            case Key::Q:            return 'Q';
            case Key::R:            return 'R';
            case Key::S:            return 'S';
            case Key::T:            return 'T';
            case Key::U:            return 'U';
            case Key::V:            return 'V';
            case Key::W:            return 'W';
            case Key::X:            return 'X';
            case Key::Y:            return 'Y';
            case Key::Z:            return 'Z';
            case Key::LeftBracket:  return VK_OEM_4;
            case Key::RightBracket: return VK_OEM_6;
            case Key::BackSlash:    return VK_OEM_5;
            case Key::SemiColon:    return VK_OEM_1;
            case Key::Apostrophe:   return VK_OEM_7;
            case Key::Comma:        return VK_OEM_COMMA;
            case Key::Period:       return VK_OEM_PERIOD;
            case Key::Slash:        return VK_OEM_2;
                // clang-format on
        }

        return 0;
    };

    Uint64 keyStates1 = 0;
    Uint64 keyStates2 = 0;

    // Loops through all the keys and sets the state of the key
    for (int i = 0; i < System::Enum::GetUnderlyingValue(Key::Slash); i++)
    {
        // For the first 64 keys store the state in the first 64 bits
        if (i < 64)
            keyStates1 = System::Math::AssignBitToPosition(keyStates1, i, (GetAsyncKeyState(AxisKeyToWindowsAPIKey(static_cast<Key>(i))) & 0x8000) != 0);
        // For the second 64 keys store the state in the second 64 bits
        else
            keyStates2 = System::Math::AssignBitToPosition(keyStates2, i - 64, (GetAsyncKeyState(AxisKeyToWindowsAPIKey(static_cast<Key>(i))) & 0x8000) != 0);
    }

    return KeyboardState(keyStates1, keyStates2);
}

} // namespace Keyboard

} // namespace Window

} // namespace Axis