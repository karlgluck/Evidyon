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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../dcx/dcxwin32keyboard.h"
#include "../dc/debug"


using namespace dcx;


//-----------------------------------------------------------------------------
// Name: dcxWin32Keyboard
// Desc: Initialize internal values
//-----------------------------------------------------------------------------
dcxWin32Keyboard::dcxWin32Keyboard()
{
    // Just reset the class
    zero();
}



//-----------------------------------------------------------------------------
// Name: query
// Desc: Updates the internal keycode statuses and writes text into input buffer
//-----------------------------------------------------------------------------
void dcxWin32Keyboard::query( bool* inputTextChanged )
{
    // Make the current buffer the old buffer
    memcpy_s( myLastKeyboardState, sizeof(myLastKeyboardState),
              myCurrentKeyboardState, sizeof(myCurrentKeyboardState) );

    // Make the working buffer into the current buffer
    memcpy_s( myCurrentKeyboardState, sizeof(myCurrentKeyboardState),
              myWorkingKeyboardState, sizeof(myWorkingKeyboardState) );

    // Clear the working buffer
    // TODO: added 11/25/07: don't clear this.  why was this being cleared in the first place?
    //ZeroMemory( myWorkingKeyboardState, sizeof(myWorkingKeyboardState) );

    // If we're editing text, copy into the target buffer
    if( myEnableTextInput && (myInputTextBuffer[0] != '\0') )
    {
        // If the user wants to know about input text, update them
        if( inputTextChanged )
            *inputTextChanged = true;

        // Find out how much text we've written so far
        int textLength = (int)strlen( myDestTextInputBuffer );

        // Decrement the text length to get rid of the cursor
        textLength--;

        // These pointers handle backspacing and such
        char* copySrcPointer = myInputTextBuffer;

        // Advance the source copy pointer and decrement the text length while the source
        // copy pointer references a '\b' character and the textLength is greater than 0.
        while( *copySrcPointer == '\b' && textLength > 0 )
        {
            copySrcPointer++;
            textLength--;
        }

        // Go through the input buffer, starting at the copy source pointer, and handle
        // backspace characters there.
        char* inputTextWrite = copySrcPointer;
        char* inputTextRead  = copySrcPointer;
        while( *inputTextRead != '\0' )
        {
            if( *inputTextRead == '\b' )
            {
                // Move backward, if there's room
                if( inputTextWrite > copySrcPointer )
                    inputTextWrite--;
            }
            else
            {
                // In debug mode, make sure we're never writing ahead of the read location
#if defined ENABLE_DEBUGGING
                if( inputTextWrite > inputTextRead )
                    return APP_ERROR( "Writing occurs ahead of reading while parsing input text", E_FAIL );
#endif

                // Add this character into the buffer (unless the locations are the same)
                if( inputTextWrite != inputTextRead )
                    *inputTextWrite = *inputTextRead;

                // Advance the writing location
                inputTextWrite++;
            }

            // Always advance the read location
            inputTextRead++;
        }

        // Set the last character to a null-terminator
        *inputTextWrite = '\0';

        // Get the length of the input text.  The input should have no special characters in
        // it at this point.
        int inputTextLength = (int)strlen( copySrcPointer );

        // Make sure we don't exceed the number of characters that we can copy
        {
            int maxCopyCharacters = myMaxInputCharacters - textLength;
            if( inputTextLength > maxCopyCharacters )
                inputTextLength = maxCopyCharacters;
        }

        // Append the cursor to the input text
        copySrcPointer[inputTextLength] = TEXT_CURSORCHARACTER;
        copySrcPointer[inputTextLength + 1] = '\0';

        // Copy into the target buffer
        strncpy_s( myDestTextInputBuffer + textLength, myDestTextInputBufferSize - textLength,
                   copySrcPointer, inputTextLength + 1 );

        // Make sure the password buffer is updated
        updatePasswordInputBuffer();

        // Reset the input buffer
        ZeroMemory( myInputTextBuffer, sizeof(myInputTextBuffer) );
    }
    else
    {
        // Either nothing was typed or we're not editing, so this parameter is not set
        if( inputTextChanged )
        {
            if( (myTextInputFlags & TEXTINPUT_TEXTCHANGED) != 0 )
            {
                // The input text has changed
                *inputTextChanged = TRUE;
                
                // Remove the flag without modifying other flags
                myTextInputFlags &= !TEXTINPUT_TEXTCHANGED;

                // Make sure the password input buffer matches the current text
                updatePasswordInputBuffer();
            }
            else
                *inputTextChanged = FALSE;

        }
    }

    // If we're debugging, make sure that the password buffer is the same length as
    // whatever text is being typed
#if defined ENABLE_DEBUGGING
    if( myDestPasswordInputBuffer )
    {
        int passwordLength = strlen( myDestPasswordInputBuffer ),
            textLength = strlen( myDestTextInputBuffer );
        if( passwordLength != textLength )
            return APP_ERROR( "Password and string lengths don't match!", E_FAIL );
    }
#endif
}



//-----------------------------------------------------------------------------
// Name: reset
// Desc: Erases all of the text input information
//-----------------------------------------------------------------------------
VOID dcxWin32Keyboard::reset()
{
    ZeroMemory( myWorkingKeyboardState, sizeof(myWorkingKeyboardState) );
    ZeroMemory( myCurrentKeyboardState, sizeof(myCurrentKeyboardState) );
    ZeroMemory( myLastKeyboardState, sizeof(myLastKeyboardState) );
    myEnableTextInput = false;
    myDestTextInputBuffer = NULL;
    myDestPasswordInputBuffer = NULL;
    myDestTextInputBufferSize = 0;
    myMaxInputCharacters = 0;
    ZeroMemory( myInputTextBuffer, sizeof(myInputTextBuffer) );
    myTextInputFlags = 0;
}



//-----------------------------------------------------------------------------
// Name: handleWindowsMessage
// Desc: Processes Windows messages
//-----------------------------------------------------------------------------
LRESULT dcxWin32Keyboard::handleWindowsMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* returnResult )
{
    // By default, we don't have to return any results
    // TODO: figure out if this is even a necessary parameter (or what happens if we do return
    // from keyboard processing without calling DefWndProc?)
    *returnResult = false;

    // Handle the message
    switch( uMsg )
    {
        // Handle a character message
        case WM_CHAR:
        {
            // If we're not receiving input, we don't have to do anything
            if( !myEnableTextInput ) break;

            // Convert the input parameter
            char inputChar = (char)wParam;
            bool bAppendInput = FALSE;

            // Check to see if we should only input numbers
            if( (myTextInputFlags & TEXTINPUT_NUMBERSONLY) != 0 )
            {
                // Append only if we're in the range
                bAppendInput = (inputChar == '\b') || (inputChar >= '0' && inputChar <= '9' );
            }
            else
            {
                // Make sure this character is valid before appending
                if( inputChar == '\b' || inputChar >= ' ' && inputChar <= '}' )
                {
                    // Check for CapsLock state--this enables special characters
                    if( inputChar >= 'A' && inputChar <= 'Z' && !(GetAsyncKeyState(VK_SHIFT)) )
                        inputChar += 64;

                    // Append this character
                    bAppendInput = TRUE;
                }
            }

            // Make sure there is space in the buffer
            if( bAppendInput && (strlen(myInputTextBuffer) >= INPUTTEXTPERFRAME_LENGTH) )
                bAppendInput = FALSE;

            // Add to the input buffer if the flag says so
            if( bAppendInput )
            {
                char input[] = { inputChar, '\0' };
                strcat_s( myInputTextBuffer, MAXCH_INPUTTEXTPERFRAME, input );
            }

        } break;

        // Process key presses
        case WM_KEYDOWN:
        {
            // Get the key-code out of the parameters
            int vkCode = (int)wParam;

            // Translate the key code and set the correct state
            myWorkingKeyboardState[convertVKToKeyCode( vkCode )] = 1;

        } break;

        // Process key releases
        case WM_KEYUP:
        {
            // Get the key-code out of the parameters
            int vkCode = (int)wParam;

            // Translate the key code and set the correct state
            myWorkingKeyboardState[convertVKToKeyCode( vkCode )] = 0;

        } break;
    }

    // Success
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: keyDown
// Desc: Detects whether or not a key is currently pressed
//-----------------------------------------------------------------------------
bool dcxWin32Keyboard::keyDown( DWORD KeyCode )
{
    // TODO: might have to change for DI
    return myCurrentKeyboardState[KeyCode] != 0;
}



//-----------------------------------------------------------------------------
// Name: keyPressed
// Desc: Detects whether or not a key was pressed this frame (its state has changed from being
//-----------------------------------------------------------------------------
bool dcxWin32Keyboard::keyPressed( DWORD KeyCode )
{
    return keyDown( KeyCode ) && (myLastKeyboardState[KeyCode] != myCurrentKeyboardState[KeyCode]);
}



//-----------------------------------------------------------------------------
// Name: keyUp
// Desc: Detects whether or not a key is currently released
//-----------------------------------------------------------------------------
bool dcxWin32Keyboard::keyUp( DWORD KeyCode )
{
    // TODO: might have to change for DI
    return myCurrentKeyboardState[KeyCode] == 0;
}



//-----------------------------------------------------------------------------
// Name: keyReleased
// Desc: Determines whether or not a key was released this frame
//-----------------------------------------------------------------------------
bool dcxWin32Keyboard::keyReleased( DWORD KeyCode )
{
    return keyUp( KeyCode ) && (myLastKeyboardState[KeyCode] != myCurrentKeyboardState[KeyCode]);
}


//-----------------------------------------------------------------------------
// Name: BeginTextInputMode
// Desc: Sets up the keyboard class to read text input from the Windows message queue
//-----------------------------------------------------------------------------
int dcxWin32Keyboard::beginTextInputMode( char* destBuffer, char* passwordBuffer,
                                      int DestBufferSize, int MaxInputCharacters, DWORD Flags )
{
    // Make sure the parameters are valid
    if( !destBuffer || DestBufferSize <= 0 || MaxInputCharacters <= 0 ||
        (MaxInputCharacters + 2 >= DestBufferSize) )
    {
        DEBUG_ERROR( "Invalid parameter(s) to beginTextInputMode" );
        return -1;
    }

    // If text input is currently enabled, disable it and display a warning
    if( myEnableTextInput )
    {
        // If the destination buffer and the current buffer are the same,
        // then we don't need to do anything
        if( destBuffer == myDestTextInputBuffer &&
            passwordBuffer == myDestPasswordInputBuffer )
            return S_OK;

        // Display a warning message
        DEBUG_WARNING( "BeginTextInputMode is forcing another text input to cease; is this the intent?" );

        // Get rid of the last input
        endTextInputMode();
    }

    // If necessary, clear the destination buffer; otherwise, make sure the text doesn't
    // extend past the end of the buffer.
    if( (Flags & TEXTINPUT_KEEPTEXT) == 0 )
        ZeroMemory( destBuffer, DestBufferSize * sizeof(char) );
    else
        destBuffer[MaxInputCharacters] = '\0';

    // Append the cursor and terminator
    char cursor[] = { TEXT_CURSORCHARACTER, '\0' };
    strcat_s( destBuffer, DestBufferSize, cursor );

    // Set up the numeric-input flag
    DWORD userFlags = ((Flags & TEXTINPUT_NUMBERSONLY) != 0) ? TEXTINPUT_NUMBERSONLY : 0;
    myTextInputFlags = userFlags | TEXTINPUT_TEXTCHANGED;

    // Store incoming parameters
    myDestTextInputBuffer = destBuffer;
    myDestPasswordInputBuffer = passwordBuffer;
    myDestTextInputBufferSize = DestBufferSize;
    myMaxInputCharacters = MaxInputCharacters;

    // Reset the internal input buffer
    ZeroMemory( myInputTextBuffer, sizeof(myInputTextBuffer) );

    // Enable input
    myEnableTextInput = true;

    // Success
    return 0;
}


//-----------------------------------------------------------------------------
// Name: modifiedTextInputBuffer
// Desc: Causes query() to return a status indicating that the input buffer changed
//-----------------------------------------------------------------------------
void dcxWin32Keyboard::modifiedTextInputBuffer()
{
    myTextInputFlags |= TEXTINPUT_TEXTCHANGED;
}


//-----------------------------------------------------------------------------
// Name: endTextInputMode
// Desc: Stops reading text from the keyboard
//-----------------------------------------------------------------------------
void dcxWin32Keyboard::endTextInputMode()
{
    // If text input isn't enabled, just exit
    if( !myEnableTextInput ) return;

    // Disable input
    myEnableTextInput = false;

    // Get rid of the cursor (if it exists)
    int inputLength = (int)strlen( myDestTextInputBuffer );
    if( inputLength > 0 && myDestTextInputBuffer[inputLength - 1] == TEXT_CURSORCHARACTER )
    {
        myDestTextInputBuffer[inputLength - 1] = '\0';

        // The password buffer should be identical, so it's safe to set it too
        if( myDestPasswordInputBuffer )
            myDestPasswordInputBuffer[inputLength - 1] = '\0';
    }

    // Clear all of the buffers and pointers
    ZeroMemory( myInputTextBuffer, sizeof(myInputTextBuffer) );
    myMaxInputCharacters = 0;
    myDestTextInputBufferSize = 0;
    myDestPasswordInputBuffer = NULL;
    myDestTextInputBuffer = NULL;

    // Let the user know that the text changed (this is returned, then cleared, via Query)
    myTextInputFlags = TEXTINPUT_TEXTCHANGED;
}


//-----------------------------------------------------------------------------
// Name: convertVKToKeyCode
// Desc: Swaps a Windows virtual key code into the standard PV key code format
//-----------------------------------------------------------------------------
DWORD dcxWin32Keyboard::convertVKToKeyCode( int vk )
{
    // Make sure the key isn't out of range
    if( vk < 0 || vk > 255 ) { return 0; }

    // This table does a single-lookup conversion
    const BYTE VK_TO_KEYCODE[256] =
    {
        0, 0, 0, 0, 0, 0, 0, 0,    // 0x00 - 0x07
        KEY_BACK, // VK_BACK = 0x08
        KEY_TAB, // VK_TAB = 0x09
        0, 0, // 0x0A-0x0B
        0, // VK_CLEAR = 0x0C
        KEY_RETURN, // VK_RETURN = 0x0D
        0, 0, // 0x0E - 0x0F
        0, // VK_SHIFT (unspecified) = 0x10
        0, // VK_CONTROL (unspecified) = 0x11
        0, // VK_MENU (alt, unspecified) = 0x12
        KEY_PAUSE, // VK_PAUSE = 0x13
        KEY_CAPITAL, // VK_CAPITAL = 0x14
        0, 0, 0, 0, 0, // VK_KANA = 0x15, 0x16, VK_JUNJA, VK_FINAL, VK_HANJA = 0x19
        0, // 0x1A
        KEY_ESCAPE, // VK_ESCAPE = 0x1B
        KEY_CONVERT, // VK_CONVERT = 0x1C
        KEY_NOCONVERT, // VK_NONVONVERT = 0x1D
        0, // VK_ACCEPT = 0x1E
        0, // VK_MODECHANGE = 0x1F
        KEY_SPACE, // VK_SPACE = 0x20
        KEY_PRIOR, // VK_PRIOR = 0x21
        KEY_NEXT, // VK_NEXT = 0x22
        KEY_END, // VK_END = 0x23
        KEY_HOME, // VK_HOME = 0x24
        KEY_LEFT, // VK_LEFT = 0x25
        KEY_UP, // VK_UP = 0x26,
        KEY_RIGHT, // VK_RIGHT = 0x27
        KEY_DOWN, // VK_DOWN = 0x28
        0, // VK_SELECT
        0, // VK_PRINT
        0, // VK_EXECUTE
        0, // VK_SNAPSHOT
        KEY_INSERT, // VK_INSERT
        KEY_DELETE, // VK_DELETE
        0, // KEY_HELP = 0x2F
        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9, // asme as ASCII  '0' - '9' (0x30 - 0x39)
        0, 0, 0, 0, 0, 0, // 0x3A-0x3F
        0, // 0x40
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z, // same as ASCII 'A' - 'Z' (0x41 - 0x5A)
        KEY_LWIN, // VK_LWIN = 0x5B
        KEY_RWIN, // VK_RWIN = 0x5C
        KEY_APPS, // VK_APPS = 0x5D
        0, // 0x5E
        KEY_SLEEP, // VK_SLEEP = 0x5F
        KEY_NUMPAD0,
        KEY_NUMPAD1,
        KEY_NUMPAD2,
        KEY_NUMPAD3,
        KEY_NUMPAD4,
        KEY_NUMPAD5,
        KEY_NUMPAD6,
        KEY_NUMPAD7,
        KEY_NUMPAD8,
        KEY_NUMPAD9, // 0x60 - 0x69
        KEY_MULTIPLY, // VK_MULTIPLY = 0x6A
        KEY_ADD, // VK_ADD = 0x6B
        0, // VK_SEPARATOR = 0x6C
        KEY_SUBTRACT, // VK_SUBTRACT = 0x6D
        KEY_DECIMAL, // VK_DECIMAL = 0x6E
        KEY_DIVIDE, // VK_DIVIDE = 0x6F
        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,
        KEY_F13,
        KEY_F14,
        KEY_F15, // 0x70-0x7E
        0, 0, 0, 0, 0,
        0, 0, 0, 0, // I don't know who has this many function keys (-0x87)
        0, 0, 0, 0, 0, 0, 0, 0, // 0x88 - 0x8F
        KEY_NUMLOCK, // VK_NUMLOCK = 0x90
        KEY_SCROLL, // VK_SCROLL = 0x91
        KEY_NUMPADEQUALS, // VK_OEM_NEC_EQUAL = 0x92
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x93 - 0x9F
        KEY_LSHIFT, // VK_LSHIFT = 0xA0
        KEY_RSHIFT, // VK_RSHIFT = 0xA1
        KEY_LCONTROL, // VK_RCONTROL = 0xA2
        KEY_RCONTROL, // VK_RCONTROL = 0xA3
        KEY_LMENU, // VK_LMENU = 0xA4
        KEY_RMENU, // VK_RMENU = 0xA5
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, // 0xA6-0xB7: only for higher windows versions
        KEY_SEMICOLON, // VK_OEM_1 = 0xBA (;: for US kbd)
        KEY_EQUALS, // VK_OEM_PLUS = 0xBB (+=)
        KEY_COMMA, // VK_OEM_COMMA = 0xBC
        KEY_MINUS, // VK_OEM_MINUS = 0xBD
        KEY_PERIOD, // VK_OEM_PERIOD = 0xBE
        KEY_SLASH, // VK_OEM_2 = 0xBF (/? on kbd)
        KEY_GRAVE, // VK_OEM_3 = 0xC0 ('~ on kbd)
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xC1-0xDA
        KEY_LBRACKET, // VK_OEM_4 = 0xDB ([{ on kbd)
        KEY_BACKSLASH, // VK_OEM_5 = 0xDC (\| on kbd)
        KEY_RBRACKET, // VK_OEM_6 = 0xDD (]} on kbd)
        KEY_APOSTROPHE, // VK_OEM_7 = 0xDE ('" on kbd)
        0, // VK_OEM_8 = 0xDF
        0, // 0xE0
        KEY_AX, // VK_OEM_AX = 0xE1
        KEY_OEM_102, // VK_OEM_102 = 0xE2
        0, // VK_ICO_HELP = 0xE3
        0, // VK_ICO_00 = 0xE4
        0, // VK_PROCESSKEY = 0xE5
        0, // VK_ICO_CLEAR = 0xE6
        0, // VK_PACKET = 0xE7
        0, // 0xE8

        // 0xE9-0xFE:  Nokia/Ericsson definitions (unused)
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        // reserved: 0xFF
        0,
    };

    // Look up the code in the table and return it
    return VK_TO_KEYCODE[vk];
}


//-----------------------------------------------------------------------------
// Name: zero
// Desc: Resets the class
//-----------------------------------------------------------------------------
void dcxWin32Keyboard::zero()
{
    // Erase all of the text input information
    reset();

    // todo: Perform other zeroing here (such as DI information)
}


//-----------------------------------------------------------------------------
// Name: updatePasswordInputBuffer
// Desc: If the password input buffer is enabled, this method updates the string
//-----------------------------------------------------------------------------
void dcxWin32Keyboard::updatePasswordInputBuffer()
{
    // Copy into the password buffer if it exists
    if( myDestPasswordInputBuffer )
    {
        // The number of characters to copy
        int passwordCharsToCopy = (int)strlen( myDestTextInputBuffer ) - 1;

        // Create a buffer of password characters
        for( int ch = 0; (ch < myDestTextInputBufferSize - 1) &&
                         (ch < passwordCharsToCopy); ++ch )
            myDestPasswordInputBuffer[ch] = TEXT_PASSWORDCHARACTER;

        // Set the cursor
        myDestPasswordInputBuffer[passwordCharsToCopy] = TEXT_CURSORCHARACTER;
        myDestPasswordInputBuffer[passwordCharsToCopy + 1] = '\0';
    }
}