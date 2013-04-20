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
#ifndef __GUIKEYBOARD_H__
#define __GUIKEYBOARD_H__
#pragma once




/**
 * To make things easy, these key codes are synonomous with the DirectInput key codes.  They
 * have already been mapped to a [0,255] range, and it makes support for DI a snap.
 */
enum GUIKey
{
  GUIKEY_NULL      = 0x00,
  GUIKEY_ESCAPE      = 0x01,
  GUIKEY_1         = 0x02,
  GUIKEY_2         = 0x03,
  GUIKEY_3         = 0x04,
  GUIKEY_4         = 0x05,
  GUIKEY_5         = 0x06,
  GUIKEY_6         = 0x07,
  GUIKEY_7         = 0x08,
  GUIKEY_8         = 0x09,
  GUIKEY_9         = 0x0A,
  GUIKEY_0         = 0x0B,
  GUIKEY_MINUS       = 0x0C,  /* - on main keyboard */
  GUIKEY_EQUALS      = 0x0D,
  GUIKEY_BACK      = 0x0E,  /* backspace */
  GUIKEY_TAB       = 0x0F,
  GUIKEY_Q         = 0x10,
  GUIKEY_W         = 0x11,
  GUIKEY_E         = 0x12,
  GUIKEY_R         = 0x13,
  GUIKEY_T         = 0x14,
  GUIKEY_Y         = 0x15,
  GUIKEY_U         = 0x16,
  GUIKEY_I         = 0x17,
  GUIKEY_O         = 0x18,
  GUIKEY_P         = 0x19,
  GUIKEY_LBRACKET    = 0x1A,
  GUIKEY_RBRACKET    = 0x1B,
  GUIKEY_RETURN      = 0x1C,  /* Enter on main keyboard */
  GUIKEY_LCONTROL    = 0x1D,
  GUIKEY_A         = 0x1E,
  GUIKEY_S         = 0x1F,
  GUIKEY_D         = 0x20,
  GUIKEY_F         = 0x21,
  GUIKEY_G         = 0x22,
  GUIKEY_H         = 0x23,
  GUIKEY_J         = 0x24,
  GUIKEY_K         = 0x25,
  GUIKEY_L         = 0x26,
  GUIKEY_SEMICOLON     = 0x27,
  GUIKEY_APOSTROPHE    = 0x28,
  GUIKEY_GRAVE       = 0x29,  /* accent grave */
  GUIKEY_LSHIFT      = 0x2A,
  GUIKEY_BACKSLASH     = 0x2B,
  GUIKEY_Z         = 0x2C,
  GUIKEY_X         = 0x2D,
  GUIKEY_C         = 0x2E,
  GUIKEY_V         = 0x2F,
  GUIKEY_B         = 0x30,
  GUIKEY_N         = 0x31,
  GUIKEY_M         = 0x32,
  GUIKEY_COMMA       = 0x33,
  GUIKEY_PERIOD      = 0x34,  /* . on main keyboard */
  GUIKEY_SLASH       = 0x35,  /* / on main keyboard */
  GUIKEY_RSHIFT      = 0x36,
  GUIKEY_MULTIPLY    = 0x37,  /* * on numeric keypad */
  GUIKEY_LMENU       = 0x38,  /* left Alt */
  GUIKEY_SPACE       = 0x39,
  GUIKEY_CAPITAL     = 0x3A,
  GUIKEY_F1        = 0x3B,
  GUIKEY_F2        = 0x3C,
  GUIKEY_F3        = 0x3D,
  GUIKEY_F4        = 0x3E,
  GUIKEY_F5        = 0x3F,
  GUIKEY_F6        = 0x40,
  GUIKEY_F7        = 0x41,
  GUIKEY_F8        = 0x42,
  GUIKEY_F9        = 0x43,
  GUIKEY_F10       = 0x44,
  GUIKEY_NUMLOCK     = 0x45,
  GUIKEY_SCROLL      = 0x46,  /* Scroll Lock */
  GUIKEY_NUMPAD7     = 0x47,
  GUIKEY_NUMPAD8     = 0x48,
  GUIKEY_NUMPAD9     = 0x49,
  GUIKEY_SUBTRACT    = 0x4A,  /* - on numeric keypad */
  GUIKEY_NUMPAD4     = 0x4B,
  GUIKEY_NUMPAD5     = 0x4C,
  GUIKEY_NUMPAD6     = 0x4D,
  GUIKEY_ADD       = 0x4E,  /* + on numeric keypad */
  GUIKEY_NUMPAD1     = 0x4F,
  GUIKEY_NUMPAD2     = 0x50,
  GUIKEY_NUMPAD3     = 0x51,
  GUIKEY_NUMPAD0     = 0x52,
  GUIKEY_DECIMAL     = 0x53,  /* . on numeric keypad */
  GUIKEY_OEM_102     = 0x56,  /* <> or \| on RT 102-key keyboard (Non-U.S.) */
  GUIKEY_F11       = 0x57,
  GUIKEY_F12       = 0x58,
  GUIKEY_F13       = 0x64,  /*           (NEC PC98) */
  GUIKEY_F14       = 0x65,  /*           (NEC PC98) */
  GUIKEY_F15       = 0x66,  /*           (NEC PC98) */
  GUIKEY_KANA      = 0x70,  /* (Japanese keyboard)      */
  GUIKEY_ABNT_C1     = 0x73,  /* /? on Brazilian keyboard */
  GUIKEY_CONVERT     = 0x79,  /* (Japanese keyboard)      */
  GUIKEY_NOCONVERT     = 0x7B,  /* (Japanese keyboard)      */
  GUIKEY_YEN       = 0x7D,  /* (Japanese keyboard)      */
  GUIKEY_ABNT_C2     = 0x7E,  /* Numpad . on Brazilian keyboard */
  GUIKEY_NUMPADEQUALS  = 0x8D,  /* = on numeric keypad (NEC PC98) */
  GUIKEY_PREVTRACK     = 0x90,  /* Previous Track (KEY_CIRCUMFLEX on Japanese keyboard) */
  GUIKEY_AT        = 0x91,  /*           (NEC PC98) */
  GUIKEY_COLON       = 0x92,  /*           (NEC PC98) */
  GUIKEY_UNDERLINE     = 0x93,  /*           (NEC PC98) */
  GUIKEY_KANJI       = 0x94,  /* (Japanese keyboard)      */
  GUIKEY_STOP      = 0x95,  /*           (NEC PC98) */
  GUIKEY_AX        = 0x96,  /*           (Japan AX) */
  GUIKEY_UNLABELED     = 0x97,  /*            (J3100) */
  GUIKEY_NEXTTRACK     = 0x99,  /* Next Track */
  GUIKEY_NUMPADENTER   = 0x9C,  /* Enter on numeric keypad */
  GUIKEY_RCONTROL    = 0x9D,
  GUIKEY_MUTE      = 0xA0,  /* Mute */
  GUIKEY_CALCULATOR    = 0xA1,  /* Calculator */
  GUIKEY_PLAYPAUSE     = 0xA2,  /* Play / Pause */
  GUIKEY_MEDIASTOP     = 0xA4,  /* Media Stop */
  GUIKEY_VOLUMEDOWN    = 0xAE,  /* Volume - */
  GUIKEY_VOLUMEUP    = 0xB0,  /* Volume + */
  GUIKEY_WEBHOME     = 0xB2,  /* Web home */
  GUIKEY_NUMPADCOMMA   = 0xB3,  /* , on numeric keypad (NEC PC98) */
  GUIKEY_DIVIDE      = 0xB5,  /* / on numeric keypad */
  GUIKEY_SYSRQ       = 0xB7,
  GUIKEY_RMENU       = 0xB8,  /* right Alt */
  GUIKEY_PAUSE       = 0xC5,  /* Pause */
  GUIKEY_HOME      = 0xC7,  /* Home on arrow keypad */
  GUIKEY_UP        = 0xC8,  /* UpArrow on arrow keypad */
  GUIKEY_PRIOR       = 0xC9,  /* PgUp on arrow keypad */
  GUIKEY_LEFT      = 0xCB,  /* LeftArrow on arrow keypad */
  GUIKEY_RIGHT       = 0xCD,  /* RightArrow on arrow keypad */
  GUIKEY_END       = 0xCF,  /* End on arrow keypad */
  GUIKEY_DOWN      = 0xD0,  /* DownArrow on arrow keypad */
  GUIKEY_NEXT      = 0xD1,  /* PgDn on arrow keypad */
  GUIKEY_INSERT      = 0xD2,  /* Insert on arrow keypad */
  GUIKEY_DELETE      = 0xD3,  /* Delete on arrow keypad */
  GUIKEY_LWIN      = 0xDB,  /* Left Windows key */
  GUIKEY_RWIN      = 0xDC,  /* Right Windows key */
  GUIKEY_APPS      = 0xDD,  /* AppMenu key */
  GUIKEY_POWER       = 0xDE,  /* System Power */
  GUIKEY_SLEEP       = 0xDF,  /* System Sleep */
  GUIKEY_WAKE      = 0xE3,  /* System Wake */
  GUIKEY_WEBSEARCH     = 0xE5,  /* Web Search */
  GUIKEY_WEBFAVORITES  = 0xE6,  /* Web Favorites */
  GUIKEY_WEBREFRESH    = 0xE7,  /* Web Refresh */
  GUIKEY_WEBSTOP     = 0xE8,  /* Web Stop */
  GUIKEY_WEBFORWARD    = 0xE9,  /* Web Forward */
  GUIKEY_WEBBACK     = 0xEA,  /* Web Back */
  GUIKEY_MYCOMPUTER    = 0xEB,  /* My Computer */
  GUIKEY_MAIL      = 0xEC,  /* Mail */
  GUIKEY_MEDIASELECT   = 0xED,  /* Media Select */

  /*
   *  Alternate names for keys, to facilitate transition from DOS.
   */
  GUIKEY_BACKSPACE    = GUIKEY_BACK,      /* backspace */
  GUIKEY_NUMPADSTAR   = GUIKEY_MULTIPLY,    /* * on numeric keypad */
  GUIKEY_LALT       = GUIKEY_LMENU,       /* left Alt */
  GUIKEY_CAPSLOCK     = GUIKEY_CAPITAL,     /* CapsLock */
  GUIKEY_NUMPADMINUS  = GUIKEY_SUBTRACT,    /* - on numeric keypad */
  GUIKEY_NUMPADPLUS   = GUIKEY_ADD,       /* + on numeric keypad */
  GUIKEY_NUMPADPERIOD   = GUIKEY_DECIMAL,     /* . on numeric keypad */
  GUIKEY_NUMPADSLASH  = GUIKEY_DIVIDE,      /* / on numeric keypad */
  GUIKEY_RALT       = GUIKEY_RMENU,       /* right Alt */
  GUIKEY_UPARROW    = GUIKEY_UP,        /* UpArrow on arrow keypad */
  GUIKEY_PGUP       = GUIKEY_PRIOR,       /* PgUp on arrow keypad */
  GUIKEY_LEFTARROW    = GUIKEY_LEFT,      /* LeftArrow on arrow keypad */
  GUIKEY_RIGHTARROW   = GUIKEY_RIGHT,       /* RightArrow on arrow keypad */
  GUIKEY_DOWNARROW    = GUIKEY_DOWN,      /* DownArrow on arrow keypad */
  GUIKEY_PGDN       = GUIKEY_NEXT,      /* PgDn on arrow keypad */

  /// The number of keys in the structures
  GUIKEY_COUNT = 0xFF,
};


// Returns the string-name of the given key
const char* GUIKeyboardKeyName(GUIKey key);



// Pre-declare these classes, since they reference each other
class GUIKeyboardSentinel;
class GUIKeyboardInputSource;

// Include the sentinel and input source
#include "guikeyboardinputsource.h"
#include "guikeyboardsentinel.h"



#endif