//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#include "stdafx.h"


//-----------------------------------------------------------------------------
// Name: handleWindowsMessage
// Desc: Processes Windows messages
//-----------------------------------------------------------------------------
LRESULT EvidyonGUIKeyboardInputSource::handleWindowsMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* returnResult)
{
    // By default, we don't have to return any results
    // TODO: figure out if this is even a necessary parameter (or what happens if we do return
    // from keyboard processing without calling DefWndProc?)
    *returnResult = false;

    switch(uMsg)
    {
        // Handle a character message
        case WM_CHAR:
        {
            typedCharacter((char)wParam);

        } break;

        // Process key presses
        case WM_KEYDOWN:
        {
            keyMoved(translateVKToGUIKey((int)wParam), true);

        } break;

        // Process key releases
        case WM_KEYUP:
        {
            keyMoved(translateVKToGUIKey((int)wParam), false);

        } break;
    }

    // Generic success
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: translateVKToGUIKey
// Desc: Turns a Windows VK-code into a GUIKey
//-----------------------------------------------------------------------------
GUIKey EvidyonGUIKeyboardInputSource::translateVKToGUIKey(int vkCode)
{
    // Make sure the key isn't out of range
    if (vkCode < 0 || vkCode > 255) return GUIKEY_NULL;

    // This table does a single-lookup conversion
    const GUIKey VK_TO_GUIKEY[256] =
    {
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL,    // 0x00 - 0x07
        GUIKEY_BACK, // VK_BACK = 0x08
        GUIKEY_TAB, // VK_TAB = 0x09
        GUIKEY_NULL, GUIKEY_NULL, // 0x0A-0x0B
        GUIKEY_NULL, // VK_CLEAR = 0x0C
        GUIKEY_RETURN, // VK_RETURN = 0x0D
        GUIKEY_NULL, GUIKEY_NULL, // 0x0E - 0x0F
        GUIKEY_RSHIFT, // VK_SHIFT (unspecified) = 0x10
        GUIKEY_RCONTROL, // VK_CONTROL (unspecified) = 0x11
        GUIKEY_RALT, // VK_MENU (alt, unspecified) = 0x12
        GUIKEY_PAUSE, // VK_PAUSE = 0x13
        GUIKEY_CAPITAL, // VK_CAPITAL = 0x14
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, // VK_KANA = 0x15, 0x16, VK_JUNJA, VK_FINAL, VK_HANJA = 0x19
        GUIKEY_NULL, // 0x1A
        GUIKEY_ESCAPE, // VK_ESCAPE = 0x1B
        GUIKEY_CONVERT, // VK_CONVERT = 0x1C
        GUIKEY_NOCONVERT, // VK_NONVONVERT = 0x1D
        GUIKEY_NULL, // VK_ACCEPT = 0x1E
        GUIKEY_NULL, // VK_MODECHANGE = 0x1F
        GUIKEY_SPACE, // VK_SPACE = 0x20
        GUIKEY_PRIOR, // VK_PRIOR = 0x21
        GUIKEY_NEXT, // VK_NEXT = 0x22
        GUIKEY_END, // VK_END = 0x23
        GUIKEY_HOME, // VK_HOME = 0x24
        GUIKEY_LEFT, // VK_LEFT = 0x25
        GUIKEY_UP, // VK_UP = 0x26,
        GUIKEY_RIGHT, // VK_RIGHT = 0x27
        GUIKEY_DOWN, // VK_DOWN = 0x28
        GUIKEY_NULL, // VK_SELECT
        GUIKEY_NULL, // VK_PRINT
        GUIKEY_NULL, // VK_EXECUTE
        GUIKEY_NULL, // VK_SNAPSHOT
        GUIKEY_INSERT, // VK_INSERT
        GUIKEY_DELETE, // VK_DELETE
        GUIKEY_NULL, // GUIKEY_HELP = 0x2F
        GUIKEY_0,
        GUIKEY_1,
        GUIKEY_2,
        GUIKEY_3,
        GUIKEY_4,
        GUIKEY_5,
        GUIKEY_6,
        GUIKEY_7,
        GUIKEY_8,
        GUIKEY_9, // asme as ASCII  '0' - '9' (0x30 - 0x39)
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, // 0x3A-0x3F
        GUIKEY_NULL, // 0x40
        GUIKEY_A,
        GUIKEY_B,
        GUIKEY_C,
        GUIKEY_D,
        GUIKEY_E,
        GUIKEY_F,
        GUIKEY_G,
        GUIKEY_H,
        GUIKEY_I,
        GUIKEY_J,
        GUIKEY_K,
        GUIKEY_L,
        GUIKEY_M,
        GUIKEY_N,
        GUIKEY_O,
        GUIKEY_P,
        GUIKEY_Q,
        GUIKEY_R,
        GUIKEY_S,
        GUIKEY_T,
        GUIKEY_U,
        GUIKEY_V,
        GUIKEY_W,
        GUIKEY_X,
        GUIKEY_Y,
        GUIKEY_Z, // same as ASCII 'A' - 'Z' (0x41 - 0x5A)
        GUIKEY_LWIN, // VK_LWIN = 0x5B
        GUIKEY_RWIN, // VK_RWIN = 0x5C
        GUIKEY_APPS, // VK_APPS = 0x5D
        GUIKEY_NULL, // 0x5E
        GUIKEY_SLEEP, // VK_SLEEP = 0x5F
        GUIKEY_NUMPAD0,
        GUIKEY_NUMPAD1,
        GUIKEY_NUMPAD2,
        GUIKEY_NUMPAD3,
        GUIKEY_NUMPAD4,
        GUIKEY_NUMPAD5,
        GUIKEY_NUMPAD6,
        GUIKEY_NUMPAD7,
        GUIKEY_NUMPAD8,
        GUIKEY_NUMPAD9, // 0x60 - 0x69
        GUIKEY_MULTIPLY, // VK_MULTIPLY = 0x6A
        GUIKEY_ADD, // VK_ADD = 0x6B
        GUIKEY_NULL, // VK_SEPARATOR = 0x6C
        GUIKEY_SUBTRACT, // VK_SUBTRACT = 0x6D
        GUIKEY_DECIMAL, // VK_DECIMAL = 0x6E
        GUIKEY_DIVIDE, // VK_DIVIDE = 0x6F
        GUIKEY_F1,
        GUIKEY_F2,
        GUIKEY_F3,
        GUIKEY_F4,
        GUIKEY_F5,
        GUIKEY_F6,
        GUIKEY_F7,
        GUIKEY_F8,
        GUIKEY_F9,
        GUIKEY_F10,
        GUIKEY_F11,
        GUIKEY_F12,
        GUIKEY_F13,
        GUIKEY_F14,
        GUIKEY_F15, // 0x70-0x7E
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL,
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, // I don't know who has this many function keys (-0x87)
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, // 0x88 - 0x8F
        GUIKEY_NUMLOCK, // VK_NUMLOCK = 0x90
        GUIKEY_SCROLL, // VK_SCROLL = 0x91
        GUIKEY_NUMPADEQUALS, // VK_OEM_NEC_EQUAL = 0x92
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, // 0x93 - 0x9F
        GUIKEY_LSHIFT, // VK_LSHIFT = 0xA0
        GUIKEY_RSHIFT, // VK_RSHIFT = 0xA1
        GUIKEY_LCONTROL, // VK_RCONTROL = 0xA2
        GUIKEY_RCONTROL, // VK_RCONTROL = 0xA3
        GUIKEY_LMENU, // VK_LMENU = 0xA4
        GUIKEY_RMENU, // VK_RMENU = 0xA5
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL,
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, // 0xA6-0xB7: only for higher windows versions
        GUIKEY_SEMICOLON, // VK_OEM_1 = 0xBA (;: for US kbd)
        GUIKEY_EQUALS, // VK_OEM_PLUS = 0xBB (+=)
        GUIKEY_COMMA, // VK_OEM_COMMA = 0xBC
        GUIKEY_MINUS, // VK_OEM_MINUS = 0xBD
        GUIKEY_PERIOD, // VK_OEM_PERIOD = 0xBE
        GUIKEY_SLASH, // VK_OEM_2 = 0xBF (/? on kbd)
        GUIKEY_GRAVE, // VK_OEM_3 = 0xC0 ('~ on kbd)
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL,
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, // 0xC1-0xDA
        GUIKEY_LBRACKET, // VK_OEM_4 = 0xDB ([{ on kbd)
        GUIKEY_BACKSLASH, // VK_OEM_5 = 0xDC (\| on kbd)
        GUIKEY_RBRACKET, // VK_OEM_6 = 0xDD (]} on kbd)
        GUIKEY_APOSTROPHE, // VK_OEM_7 = 0xDE ('" on kbd)
        GUIKEY_NULL, // VK_OEM_8 = 0xDF
        GUIKEY_NULL, // 0xE0
        GUIKEY_AX, // VK_OEM_AX = 0xE1
        GUIKEY_OEM_102, // VK_OEM_102 = 0xE2
        GUIKEY_NULL, // VK_ICO_HELP = 0xE3
        GUIKEY_NULL, // VK_ICO_00 = 0xE4
        GUIKEY_NULL, // VK_PROCESSKEY = 0xE5
        GUIKEY_NULL, // VK_ICO_CLEAR = 0xE6
        GUIKEY_NULL, // VK_PACKET = 0xE7
        GUIKEY_NULL, // 0xE8

        // 0xE9-0xFE:  Nokia/Ericsson definitions (unused)
        GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL, GUIKEY_NULL,

        // reserved: 0xFF
        GUIKEY_NULL,
    };

    // Look up the code in the table and return it
    return VK_TO_GUIKEY[vkCode];
}

