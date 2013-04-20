#include "guikeyboard.h"



const char* GUIKeyboardKeyName(GUIKey key) {
  switch(key) {
    case GUIKEY_NULL:       return "(none)";
    case GUIKEY_ESCAPE:     return "ESCAPE";
    case GUIKEY_1:          return "1";
    case GUIKEY_2:          return "2";
    case GUIKEY_3:          return "3";
    case GUIKEY_4:          return "4";
    case GUIKEY_5:          return "5";
    case GUIKEY_6:          return "6";
    case GUIKEY_7:          return "7";
    case GUIKEY_8:          return "8";
    case GUIKEY_9:          return "9";
    case GUIKEY_0:          return "0";
    case GUIKEY_MINUS:      return "MINUS";
    case GUIKEY_EQUALS:     return "EQUALS";
    case GUIKEY_BACK:       return "BACK";
    case GUIKEY_TAB:        return "TAB";
    case GUIKEY_Q:          return "Q";
    case GUIKEY_W:          return "W";
    case GUIKEY_E:          return "E";
    case GUIKEY_R:          return "R";
    case GUIKEY_T:          return "T";
    case GUIKEY_Y:          return "Y";
    case GUIKEY_U:          return "U";
    case GUIKEY_I:          return "I";
    case GUIKEY_O:          return "O";
    case GUIKEY_P:          return "P";
    case GUIKEY_LBRACKET:   return "[";
    case GUIKEY_RBRACKET:   return "]";
    case GUIKEY_RETURN:     return "RETURN";
    case GUIKEY_LCONTROL:   return "LEFT CTRL"; //
    case GUIKEY_A:          return "A";
    case GUIKEY_S:          return "S";
    case GUIKEY_D:          return "D";
    case GUIKEY_F:          return "F";
    case GUIKEY_G:          return "G";
    case GUIKEY_H:          return "H";
    case GUIKEY_J:          return "J";
    case GUIKEY_K:          return "K";
    case GUIKEY_L:          return "L";
    case GUIKEY_SEMICOLON:  return "SEMICOLON";
    case GUIKEY_APOSTROPHE: return "APOSTROPHE";
    case GUIKEY_GRAVE:      return "GRAVE";
    case GUIKEY_LSHIFT:     return "LEFT SHIFT";    //
    case GUIKEY_BACKSLASH:  return "BACKSLASH";
    case GUIKEY_Z:          return "Z";
    case GUIKEY_X:          return "X";
    case GUIKEY_C:          return "C";
    case GUIKEY_V:          return "V";
    case GUIKEY_B:          return "B";
    case GUIKEY_N:          return "N";
    case GUIKEY_M:          return "M";
    case GUIKEY_COMMA:      return "COMMA";
    case GUIKEY_PERIOD:     return "PERIOD";
    case GUIKEY_SLASH:      return "FWD SLASH";     //
    case GUIKEY_RSHIFT:     return "RIGHT SHIFT";   //
    case GUIKEY_MULTIPLY:   return "MULTIPLY";
    case GUIKEY_LMENU:      return "LEFT ALT";      //
    case GUIKEY_SPACE:      return "SPACE";
    case GUIKEY_CAPITAL:    return "CAPITAL";
    case GUIKEY_F1:         return "F1";
    case GUIKEY_F2:         return "F2";
    case GUIKEY_F3:         return "F3";
    case GUIKEY_F4:         return "F4";
    case GUIKEY_F5:         return "F5";
    case GUIKEY_F6:         return "F6";
    case GUIKEY_F7:         return "F7";
    case GUIKEY_F8:         return "F8";
    case GUIKEY_F9:         return "F9";
    case GUIKEY_F10:        return "F10";
    case GUIKEY_NUMLOCK:    return "NUMLOCK";
    case GUIKEY_SCROLL:     return "SCROLL";
    case GUIKEY_NUMPAD7:    return "NUMPAD 7";  //
    case GUIKEY_NUMPAD8:    return "NUMPAD 8";  //
    case GUIKEY_NUMPAD9:    return "NUMPAD 9";  //
    case GUIKEY_SUBTRACT:   return "SUBTRACT";
    case GUIKEY_NUMPAD4:    return "NUMPAD 4";  //
    case GUIKEY_NUMPAD5:    return "NUMPAD 5";  //
    case GUIKEY_NUMPAD6:    return "NUMPAD 6";  //
    case GUIKEY_ADD:        return "ADD";
    case GUIKEY_NUMPAD1:    return "NUMPAD 1";  //
    case GUIKEY_NUMPAD2:    return "NUMPAD 2";  //
    case GUIKEY_NUMPAD3:    return "NUMPAD 3";  //
    case GUIKEY_NUMPAD0:    return "NUMPAD 0";  //
    case GUIKEY_DECIMAL:    return "DECIMAL";
    case GUIKEY_OEM_102:    return "OEM_102";
    case GUIKEY_F11:        return "F11";
    case GUIKEY_F12:        return "F12";
    case GUIKEY_F13:        return "F13";
    case GUIKEY_F14:        return "F14";
    case GUIKEY_F15:        return "F15";
    case GUIKEY_KANA:       return "KANA";
    case GUIKEY_ABNT_C1:    return "ABNT_C1";
    case GUIKEY_CONVERT:    return "CONVERT";
    case GUIKEY_NOCONVERT:  return "NOCONVERT";
    case GUIKEY_YEN:        return "YEN";
    case GUIKEY_ABNT_C2:    return "ABNT_C2";
    case GUIKEY_NUMPADEQUALS:return "NUMPAD ="; //
    case GUIKEY_PREVTRACK:  return "PREV. TRACK";   //
    case GUIKEY_AT:         return "AT";
    case GUIKEY_COLON:      return "COLON";
    case GUIKEY_UNDERLINE:  return "UNDERLINE";
    case GUIKEY_KANJI:      return "KANJI";
    case GUIKEY_STOP:       return "STOP";
    case GUIKEY_AX:         return "AX";
    case GUIKEY_UNLABELED:  return "UNLABELED";
    case GUIKEY_NEXTTRACK:  return "NEXTTRACK";
    case GUIKEY_NUMPADENTER:return "NUMPADENTER";
    case GUIKEY_RCONTROL:   return "RCONTROL";
    case GUIKEY_MUTE:       return "MUTE";
    case GUIKEY_CALCULATOR: return "CALCULATOR";
    case GUIKEY_PLAYPAUSE:  return "PLAYPAUSE";
    case GUIKEY_MEDIASTOP:  return "MEDIASTOP";
    case GUIKEY_VOLUMEDOWN: return "VOLUMEDOWN";
    case GUIKEY_VOLUMEUP:   return "VOLUMEUP";
    case GUIKEY_WEBHOME:    return "WEBHOME";
    case GUIKEY_NUMPADCOMMA:return "NUMPADCOMMA";
    case GUIKEY_DIVIDE:     return "DIVIDE";
    case GUIKEY_SYSRQ:      return "SYSRQ";
    case GUIKEY_RMENU:      return "RIGHT ALT"; //
    case GUIKEY_PAUSE:      return "PAUSE";
    case GUIKEY_HOME:       return "HOME";
    case GUIKEY_UP:         return "UP";
    case GUIKEY_PRIOR:      return "PRIOR";
    case GUIKEY_LEFT:       return "LEFT";
    case GUIKEY_RIGHT:      return "RIGHT";
    case GUIKEY_END:        return "END";
    case GUIKEY_DOWN:       return "DOWN";
    case GUIKEY_NEXT:       return "NEXT";
    case GUIKEY_INSERT:     return "INSERT";
    case GUIKEY_DELETE:     return "DELETE";
    case GUIKEY_LWIN:       return "LEFT WIN";  //
    case GUIKEY_RWIN:       return "RIGHT WIN"; //
    case GUIKEY_APPS:       return "APPS";
    case GUIKEY_POWER:      return "POWER";
    case GUIKEY_SLEEP:      return "SLEEP";
    case GUIKEY_WAKE:       return "WAKE";
    case GUIKEY_WEBSEARCH:  return "WEBSEARCH";
    case GUIKEY_WEBFAVORITES:return "WEBFAVORITES";
    case GUIKEY_WEBREFRESH: return "WEBREFRESH";
    case GUIKEY_WEBSTOP:    return "WEBSTOP";
    case GUIKEY_WEBFORWARD: return "WEBFORWARD";
    case GUIKEY_WEBBACK:    return "WEBBACK";
    case GUIKEY_MYCOMPUTER: return "MYCOMPUTER";
    case GUIKEY_MAIL:       return "MAIL";
    case GUIKEY_MEDIASELECT:return "MEDIASELECT";
    default:    return "???";
  }

}