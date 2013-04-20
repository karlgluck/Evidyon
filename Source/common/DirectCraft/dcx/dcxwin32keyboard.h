//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
//                                                                           //
//  Permission is hereby granted, free of charge, to any person obtaining a  //
//  copy of this software and associated documentation files (the            //
//  "Software"), to deal in the Software without restriction, including      //
//  without limitation the rights to use, copy, modify, merge, publish,      //
//  distribute, sublicense, and/or sell copies of the Software, and to       //
//  permit persons to whom the Software is furnished to do so, subject to    //
//  the following conditions:                                                //
//                                                                           //
//  The above copyright notice and this permission notice shall be included  //
//  in all copies or substantial portions of the Software.                   //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  //
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               //
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     //
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     //
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __DCXWIN32KEYBOARD_H__
#define __DCXWIN32KEYBOARD_H__


#define MAXCH_INPUTTEXTPERFRAME     8
#define INPUTTEXTPERFRAME_LENGTH    7
#define TEXT_PASSWORDCHARACTER      '*'
#define TEXT_CURSORCHARACTER        '_'


// To make things easy, these key codes are synonomous with the DirectInput key codes.  They
// have already been mapped to a [0,255] range, and if/when DI support is added these won't
// require mapping in order to work correctly (although it may be prudent to swap these
// definitions for DI_* macros in case the actual values change).
#define KEY_ESCAPE          0x01
#define KEY_1               0x02
#define KEY_2               0x03
#define KEY_3               0x04
#define KEY_4               0x05
#define KEY_5               0x06
#define KEY_6               0x07
#define KEY_7               0x08
#define KEY_8               0x09
#define KEY_9               0x0A
#define KEY_0               0x0B
#define KEY_MINUS           0x0C    /* - on main keyboard */
#define KEY_EQUALS          0x0D
#define KEY_BACK            0x0E    /* backspace */
#define KEY_TAB             0x0F
#define KEY_Q               0x10
#define KEY_W               0x11
#define KEY_E               0x12
#define KEY_R               0x13
#define KEY_T               0x14
#define KEY_Y               0x15
#define KEY_U               0x16
#define KEY_I               0x17
#define KEY_O               0x18
#define KEY_P               0x19
#define KEY_LBRACKET        0x1A
#define KEY_RBRACKET        0x1B
#define KEY_RETURN          0x1C    /* Enter on main keyboard */
#define KEY_LCONTROL        0x1D
#define KEY_A               0x1E
#define KEY_S               0x1F
#define KEY_D               0x20
#define KEY_F               0x21
#define KEY_G               0x22
#define KEY_H               0x23
#define KEY_J               0x24
#define KEY_K               0x25
#define KEY_L               0x26
#define KEY_SEMICOLON       0x27
#define KEY_APOSTROPHE      0x28
#define KEY_GRAVE           0x29    /* accent grave */
#define KEY_LSHIFT          0x2A
#define KEY_BACKSLASH       0x2B
#define KEY_Z               0x2C
#define KEY_X               0x2D
#define KEY_C               0x2E
#define KEY_V               0x2F
#define KEY_B               0x30
#define KEY_N               0x31
#define KEY_M               0x32
#define KEY_COMMA           0x33
#define KEY_PERIOD          0x34    /* . on main keyboard */
#define KEY_SLASH           0x35    /* / on main keyboard */
#define KEY_RSHIFT          0x36
#define KEY_MULTIPLY        0x37    /* * on numeric keypad */
#define KEY_LMENU           0x38    /* left Alt */
#define KEY_SPACE           0x39
#define KEY_CAPITAL         0x3A
#define KEY_F1              0x3B
#define KEY_F2              0x3C
#define KEY_F3              0x3D
#define KEY_F4              0x3E
#define KEY_F5              0x3F
#define KEY_F6              0x40
#define KEY_F7              0x41
#define KEY_F8              0x42
#define KEY_F9              0x43
#define KEY_F10             0x44
#define KEY_NUMLOCK         0x45
#define KEY_SCROLL          0x46    /* Scroll Lock */
#define KEY_NUMPAD7         0x47
#define KEY_NUMPAD8         0x48
#define KEY_NUMPAD9         0x49
#define KEY_SUBTRACT        0x4A    /* - on numeric keypad */
#define KEY_NUMPAD4         0x4B
#define KEY_NUMPAD5         0x4C
#define KEY_NUMPAD6         0x4D
#define KEY_ADD             0x4E    /* + on numeric keypad */
#define KEY_NUMPAD1         0x4F
#define KEY_NUMPAD2         0x50
#define KEY_NUMPAD3         0x51
#define KEY_NUMPAD0         0x52
#define KEY_DECIMAL         0x53    /* . on numeric keypad */
#define KEY_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define KEY_F11             0x57
#define KEY_F12             0x58
#define KEY_F13             0x64    /*                     (NEC PC98) */
#define KEY_F14             0x65    /*                     (NEC PC98) */
#define KEY_F15             0x66    /*                     (NEC PC98) */
#define KEY_KANA            0x70    /* (Japanese keyboard)            */
#define KEY_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define KEY_CONVERT         0x79    /* (Japanese keyboard)            */
#define KEY_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define KEY_YEN             0x7D    /* (Japanese keyboard)            */
#define KEY_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define KEY_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define KEY_PREVTRACK       0x90    /* Previous Track (KEY_CIRCUMFLEX on Japanese keyboard) */
#define KEY_AT              0x91    /*                     (NEC PC98) */
#define KEY_COLON           0x92    /*                     (NEC PC98) */
#define KEY_UNDERLINE       0x93    /*                     (NEC PC98) */
#define KEY_KANJI           0x94    /* (Japanese keyboard)            */
#define KEY_STOP            0x95    /*                     (NEC PC98) */
#define KEY_AX              0x96    /*                     (Japan AX) */
#define KEY_UNLABELED       0x97    /*                        (J3100) */
#define KEY_NEXTTRACK       0x99    /* Next Track */
#define KEY_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define KEY_RCONTROL        0x9D
#define KEY_MUTE            0xA0    /* Mute */
#define KEY_CALCULATOR      0xA1    /* Calculator */
#define KEY_PLAYPAUSE       0xA2    /* Play / Pause */
#define KEY_MEDIASTOP       0xA4    /* Media Stop */
#define KEY_VOLUMEDOWN      0xAE    /* Volume - */
#define KEY_VOLUMEUP        0xB0    /* Volume + */
#define KEY_WEBHOME         0xB2    /* Web home */
#define KEY_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define KEY_DIVIDE          0xB5    /* / on numeric keypad */
#define KEY_SYSRQ           0xB7
#define KEY_RMENU           0xB8    /* right Alt */
#define KEY_PAUSE           0xC5    /* Pause */
#define KEY_HOME            0xC7    /* Home on arrow keypad */
#define KEY_UP              0xC8    /* UpArrow on arrow keypad */
#define KEY_PRIOR           0xC9    /* PgUp on arrow keypad */
#define KEY_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define KEY_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define KEY_END             0xCF    /* End on arrow keypad */
#define KEY_DOWN            0xD0    /* DownArrow on arrow keypad */
#define KEY_NEXT            0xD1    /* PgDn on arrow keypad */
#define KEY_INSERT          0xD2    /* Insert on arrow keypad */
#define KEY_DELETE          0xD3    /* Delete on arrow keypad */
#define KEY_LWIN            0xDB    /* Left Windows key */
#define KEY_RWIN            0xDC    /* Right Windows key */
#define KEY_APPS            0xDD    /* AppMenu key */
#define KEY_POWER           0xDE    /* System Power */
#define KEY_SLEEP           0xDF    /* System Sleep */
#define KEY_WAKE            0xE3    /* System Wake */
#define KEY_WEBSEARCH       0xE5    /* Web Search */
#define KEY_WEBFAVORITES    0xE6    /* Web Favorites */
#define KEY_WEBREFRESH      0xE7    /* Web Refresh */
#define KEY_WEBSTOP         0xE8    /* Web Stop */
#define KEY_WEBFORWARD      0xE9    /* Web Forward */
#define KEY_WEBBACK         0xEA    /* Web Back */
#define KEY_MYCOMPUTER      0xEB    /* My Computer */
#define KEY_MAIL            0xEC    /* Mail */
#define KEY_MEDIASELECT     0xED    /* Media Select */

/*
 *  Alternate names for keys, to facilitate transition from DOS.
 */
#define KEY_BACKSPACE       KEY_BACK            /* backspace */
#define KEY_NUMPADSTAR      KEY_MULTIPLY        /* * on numeric keypad */
#define KEY_LALT            KEY_LMENU           /* left Alt */
#define KEY_CAPSLOCK        KEY_CAPITAL         /* CapsLock */
#define KEY_NUMPADMINUS     KEY_SUBTRACT        /* - on numeric keypad */
#define KEY_NUMPADPLUS      KEY_ADD             /* + on numeric keypad */
#define KEY_NUMPADPERIOD    KEY_DECIMAL         /* . on numeric keypad */
#define KEY_NUMPADSLASH     KEY_DIVIDE          /* / on numeric keypad */
#define KEY_RALT            KEY_RMENU           /* right Alt */
#define KEY_UPARROW         KEY_UP              /* UpArrow on arrow keypad */
#define KEY_PGUP            KEY_PRIOR           /* PgUp on arrow keypad */
#define KEY_LEFTARROW       KEY_LEFT            /* LeftArrow on arrow keypad */
#define KEY_RIGHTARROW      KEY_RIGHT           /* RightArrow on arrow keypad */
#define KEY_DOWNARROW       KEY_DOWN            /* DownArrow on arrow keypad */
#define KEY_PGDN            KEY_NEXT            /* PgDn on arrow keypad */


/// These flags are used in the Flags parameter to the BeginTextInputMode method to specify
/// special attributes of the text input process.
#define TEXTINPUT_KEEPTEXT      0x1     /* Prevents the incoming buffer from being cleared */
#define TEXTINPUT_NUMBERSONLY   0x2     /* Only allows numeric input */

/// These text flags are used only internally
#define TEXTINPUT_TEXTCHANGED   0x4     /* Lets the user know that the text has changed */

namespace dcx {


    /**
     * This class is intended to be a standalone global manager for all of the keyboard functionality
     * required by a program.  Only one instance should be created, and this instance should be
     * manually linked into a window's message handling procedure via handleWindowsMessage.
     */
    class dcxWin32Keyboard
    {
        public:

            /**
             * Initialize internal values
             */
            dcxWin32Keyboard();

            /**
             * Updates the internal keycode statuses and writes text into input buffer, if it is
             * enabled.  This ensures that the Win32 message processing thread and the user's
             * buffer are separated.
             *   @param inputTextChanged Notifies the calling procedure if the input text buffer
             *                           is updated during the call; this parameter is optional
             *                           (just set it to NULL)
             *   @return Result code
             */
            void query( bool* inputTextChanged );

            /**
             * Erases all of the text input information without attempting to finalize or modify
             * the buffers, and clears all of the keyboard state information.
             */
            void reset();

            /**
             * Processes Windows messages
             */
            LRESULT handleWindowsMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* returnResult );

            /**
             * Detects whether or not a key is currently pressed
             *   @param KeyCode The key to check
             *   @return Key down state
             */
            bool keyDown( DWORD KeyCode );

            /**
             * Detects whether or not a key was pressed this frame (its state has changed from being
             * not pressed to pressed over the last call to Query)
             *   @param KeyCode The key to check
             *   @return Key pressed state
             */
            bool keyPressed( DWORD KeyCode );

            /**
             * Detects whether or not a key is currently released
             *   @param KeyCode The key to check
             *   @return Key up state
             */
            bool keyUp( DWORD KeyCode );

            /**
             * Determines whether or not a key was released this frame (its state has changed from
             * pressed to released over the last call to Query)
             *   @param KeyCode The key to check
             *   @return Key released state
             */
            bool keyReleased( DWORD KeyCode );

            /**
             * Sets up the keyboard class to read text input from the Windows message queue into the
             * provided buffer.  The buffer will only be updated when Query is called, so that
             * the destination buffer is guaranteed to be written only when it is not being used
             * by the calling thread.
             *   @param destBuffer The destination character buffer.  It IS safe to modify this
             *                      buffer's contents between calls to Query; however, doing so
             *                      can produce unintuitive results for the end user.
             *   @param passwordBuffer Destination buffer filled with the standard password character
             *                          when the user types a text character.  This is assumed to be
             *                          the same size as the destination buffer.  Set to NULL if
             *                          this isn't necessary.
             *   @param DestBufferSize How large the destination buffer is.  This must always be at
             *                         least 2 characters larger than the maximum input length, to
             *                         allow for the underscore and terminating character ("_\0")
             *   @param MaxInputCharacters How many characters can be typed
             *   @param Flags Specifies special text input options, such as whether or not to keep
             *                text in the destination buffer (TEXTINPUT_KEEPTEXT) or prevent any
             *                input other than numbers (TEXTINPUT_NUMBERSONLY).  This parameter is
             *                optional.
             *   @return Result code
             */
            int beginTextInputMode( char* destBuffer, char* passwordBuffer,
                                    int destBufferSize, int maxInputCharacters, DWORD Flags = 0 );

            /**
             * When the destination text input buffer is modified outside of this class, calling
             * this function causes Query() to return a status indicating that the input buffer
             * changed.
             */
            void modifiedTextInputBuffer();

            /**
             * Stops reading text from the keyboard, and removes the cursor from the destination
             * text buffer.
             */
            void endTextInputMode();


        public:

            /**
             * Swaps a Windows virtual key code into the standard PV key code format
             *   @param vk The Windows virtual key-code to convert
             *   @return Equivalent PV key code
             */
            DWORD convertVKToKeyCode( int vk );

            /**
             * Resets the class
             */
            void zero();

            /**
             * If the password input buffer is enabled, this method updates the string
             */
            void updatePasswordInputBuffer();


        protected:

            /// The working keyboard state (for use because of Windows asynchronous input)
            BYTE myWorkingKeyboardState[256];

            /// The state of the keyboard this frame
            BYTE myCurrentKeyboardState[256];

            /// The state of the keyboard last frame.  The "current" keyboard state gets moved here
            /// during a call to Query.  Comparisons between this buffer and the "current" buffer
            /// allow KeyPressed and KeyReleased to work correctly.
            BYTE myLastKeyboardState[256];

            /// Whether or not the user requires text input
            bool myEnableTextInput;

            /// The buffer into which text is being read.
            char* myDestTextInputBuffer;

            /// Password buffer (if provided)
            char* myDestPasswordInputBuffer;

            /// How large the destination buffer is
            int myDestTextInputBufferSize;

            /// The maximum number of characters that can be written into the buffer
            int myMaxInputCharacters;

            /// Temporary storage space for Windows input during text input mode
            char myInputTextBuffer[MAXCH_INPUTTEXTPERFRAME];

            /// Internal flags, such as allowing only numeric input or notifying of
            /// text changes.
            DWORD myTextInputFlags;
    };

}

#endif