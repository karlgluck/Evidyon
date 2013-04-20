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
#include "commctrl.h"
#include <string>
#include <list>
#include "../dcx/dcxwin32dialogtemplate.h"
#include "../dc/debug"


// TODO: add listbox/combobox as template options!
#define WNDCLASS_BUTTON     0x0080
#define WNDCLASS_EDIT       0x0081
#define WNDCLASS_STATIC     0x0082
#define WNDCLASS_LISTBOX    0x0083
#define WNDCLASS_SCROLLBAR  0x0084
#define WNDCLASS_COMBOBOX   0x0085


using namespace dcx;



//-----------------------------------------------------------------------------
// Name:  dcxWin32DialogTemplate
// Desc:  Initializes the class
//-----------------------------------------------------------------------------
dcxWin32DialogTemplate::dcxWin32DialogTemplate()
{
    // Reset the dialog header
    ZeroMemory( &myDialogHeader, sizeof(myDialogHeader) );
    myMenuOrdinal = 0;
    myClassOrdinal = 0;
    myTitleFontPoint = 0;
    myBuffer = 0;
    myBufferBytesLeft = 0;
}




//-----------------------------------------------------------------------------
// Name:  setLocationDialogUnits
// Desc:  Sets the position of the dialog by specifying coordinates in pixels
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setLocationDialogUnits( short x, short y )
{
    // Save the values
    myDialogHeader.x = x;
    myDialogHeader.y = y;
}





//-----------------------------------------------------------------------------
// Name:  setSizeInDialogUnits
// Desc:  Sets the size of the dialog by specifying its size
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setSizeInDialogUnits( short cx, short cy )
{
    // Save the values
    myDialogHeader.cx = cx;
    myDialogHeader.cy = cy;
}





//-----------------------------------------------------------------------------
// Name:  setStyle
// Desc:  Saves flags that determine how the window will be displayed
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setStyle( DWORD flags )
{
    myDialogHeader.style = flags;
}





//-----------------------------------------------------------------------------
// Name:  setStyleEx
// Desc:  Saves flags that denote the window's extended display settings
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setStyleEx( DWORD flags )
{
    myDialogHeader.dwExtendedStyle = flags;
}





//-----------------------------------------------------------------------------
// Name:  setStyleAsResizeableAppWindow
// Desc:  Sets the window style
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setStyleAsResizeableAppWindow()
{
    setStyle( DS_SETFONT | DS_3DLOOK | DS_CONTEXTHELP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
              WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME );
    setStyleEx( 0 );
}





//-----------------------------------------------------------------------------
// Name:  setStyleAsPrompt
// Desc:  Sets the window style
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setStyleAsPrompt()
{
    setStyle( WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU |
              DS_3DLOOK | DS_SETFONT | DS_MODALFRAME | DS_NOIDLEMSG | DS_CENTER );
    setStyleEx( 0 );
}






//-----------------------------------------------------------------------------
// Name:  setStyleAsToolbox
// Desc:  Sets the window style
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setStyleAsToolbox()
{
    setStyle( WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | DS_ABSALIGN |
              DS_3DLOOK | DS_SETFONT | DS_MODALFRAME | DS_NOIDLEMSG );
    setStyleEx( WS_EX_TOOLWINDOW );
}




//-----------------------------------------------------------------------------
// Name:  setMenu
// Desc:  Changes the menu that the dialog will use
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setMenu( WORD ordinal )
{
    myMenuOrdinal = ordinal;
    myMenuResourceName.clear();
}



//-----------------------------------------------------------------------------
// Name:  setMenu
// Desc:  Changes the menu that the dialog will use
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setMenu( const char* resourceName )
{
    myMenuOrdinal = 0;
    if( resourceName )
        myMenuResourceName = resourceName;
    else
        myMenuResourceName.clear();
}




//-----------------------------------------------------------------------------
// Name:  setClass
// Desc:  Changes the window class that displays the dialog
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setClass( WORD predefinedSystemClass )
{
    myClassOrdinal = predefinedSystemClass;
    myClassName.clear();
}




//-----------------------------------------------------------------------------
// Name:  setClass
// Desc:  Changes the window class that displays the dialog
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setClass( const char* className )
{
    myClassOrdinal = 0;
    if( className )
        myClassName = className;
    else
        myClassName.clear();
}



//-----------------------------------------------------------------------------
// Name:  setTitle
// Desc:  Sets the window caption for the dialog
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::setTitle( const char* title, WORD point, const char* font )
{
    if( title )
        myTitle = title;
    else
        myTitle.clear();
    myTitleFontPoint = point;
    if( font )
        myTitleFontFace = font;
    else
        myTitleFontFace.clear();
}


//-----------------------------------------------------------------------------
// Name:  addButtonInDialogUnits
// Desc:  Adds a new button to the screen, using coordinates in dialog units
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::addButtonInDialogUnits( WORD id, DWORD style, short x, short y,
                                                     short width, short height, const char* text )
{
    DialogItem item;

    // Write header information
    item.Header.dwExtendedStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY; // todo: allow extended styles?
    item.Header.id = id;
    item.Header.style = style | WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT;
    item.Header.x = x;
    item.Header.y = y;
    item.Header.cx = width;
    item.Header.cy = height;

    // 

    // Write item-specific information
    item.myClassOrdinal = WNDCLASS_BUTTON;
    item.myTitle = text;

    // Add to the internal list
    myItems.push_back( item );
}





//-----------------------------------------------------------------------------
// Name:  addEditBoxInDialogUnits
// Desc:  Adds a new edit box to the screen, using coordinates in dialog units
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::addEditBoxInDialogUnits( WORD id, DWORD style, short x, short y,
                                                      short width, short height, const char* defaultText )
{
    DialogItem item;

    // Write header information
    item.Header.dwExtendedStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY; // todo: allow extended styles?
    item.Header.id = id;
    item.Header.style = style | WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | WS_TABSTOP;
    item.Header.x = x;
    item.Header.y = y;
    item.Header.cx = width;
    item.Header.cy = height;

    // 

    // Write item-specific information
    item.myClassOrdinal = WNDCLASS_EDIT;
    item.myTitle = defaultText;

    // Add to the internal list
    myItems.push_back( item );
}




//-----------------------------------------------------------------------------
// Name:  addStaticTextInDialogUnits
// Desc:  Adds a new set of static text to the screen, using coordinates in dialog units
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::addStaticTextInDialogUnits( WORD id, DWORD style, short x, short y,
                                                         short width, short height, const char* text )
{
    DialogItem item;

    // Write header information
    item.Header.dwExtendedStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY; // todo: allow extended styles?
    item.Header.id = id;
    item.Header.style = style | WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | WS_TABSTOP;
    item.Header.x = x;
    item.Header.y = y;
    item.Header.cx = width;
    item.Header.cy = height;

    // Write item-specific information
    item.myClassOrdinal = WNDCLASS_STATIC;
    item.myTitle = text;

    // Add to the internal list
    myItems.push_back( item );
}




//-----------------------------------------------------------------------------
// Name:  addListBoxInDialogUnits
// Desc:  Adds a new list box to the screen
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::addListBoxInDialogUnits( WORD id, DWORD style, short x, short y,
                                                      short width, short height )
{
    DialogItem item;

    // Write header information
    item.Header.dwExtendedStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY; // todo: allow extended styles?
    item.Header.id = id;
    item.Header.style = style | WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | WS_TABSTOP;
    item.Header.x = x;
    item.Header.y = y;
    item.Header.cx = width;
    item.Header.cy = height;

    // Write item-specific information
    item.myClassOrdinal = WNDCLASS_LISTBOX;
    item.myTitle.clear();

    // Add to the internal list
    myItems.push_back( item );
}



//-----------------------------------------------------------------------------
// Name:  addComboBoxInDialogUnits
// Desc:  Adds a new list box to the screen
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::addComboBoxInDialogUnits( WORD id, DWORD style, short x, short y,
                                                       short width, short height )
{
    DialogItem item;

    // Write header information
    item.Header.dwExtendedStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY; // todo: allow extended styles?
    item.Header.id = id;
    item.Header.style = style | WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | WS_TABSTOP;
    item.Header.x = x;
    item.Header.y = y;
    item.Header.cx = width;
    item.Header.cy = height;

    // Write item-specific information
    item.myClassOrdinal = 0;
    item.myClassName = "COMBOBOX";
    item.myTitle.clear();

    // Add to the internal list
    myItems.push_back( item );
}


//-----------------------------------------------------------------------------
// Name:  addProgressBarInDialogUnits
// Desc:  Adds a new progress bar to the screen
//-----------------------------------------------------------------------------
void dcxWin32DialogTemplate::addProgressBarInDialogUnits( WORD id, DWORD style, short x, short y,
                                                       short width, short height )
{
    DialogItem item;

    // Write header information
    item.Header.dwExtendedStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY; // todo: allow extended styles?
    item.Header.id = id;
    item.Header.style = style | WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP;
    item.Header.x = x;
    item.Header.y = y;
    item.Header.cx = width;
    item.Header.cy = height;

    // Write item-specific information
    item.myClassOrdinal = 0;
    item.myClassName = PROGRESS_CLASS;
    item.myTitle.clear();

    // Add to the internal list
    myItems.push_back( item );
}






//-----------------------------------------------------------------------------
// Name:  compile
// Desc:  Compiles this dialog template using the provided buffer
//-----------------------------------------------------------------------------
LPCDLGTEMPLATE dcxWin32DialogTemplate::compile( BYTE* buffer, size_t bufferLength )
{
/*
typedef struct {
    WORD dlgVer;
    WORD signature;
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;
    sz_Or_Ord menu;
    sz_Or_Ord windowClass;
    WCHAR title[titleLen];
    WORD pointsize;
    WORD weight;
    BYTE italic;
    BYTE charset;
    WCHAR typeface[stringLen];
} DLGTEMPLATEEX;
*/

    // Save the buffer pointer
    myBuffer = buffer;
    myBufferBytesLeft = bufferLength;

    // Adjust the alignment to the DWORD boundary
    if( !padToBoundary( sizeof(DWORD) ) ) goto OUT_OF_BUFFER_SPACE;

    // Grab the padded location (so that we can return it later)
    buffer = myBuffer;

    // Save the dialog header
    myDialogHeader.cdit = (WORD)myItems.size();
    myDialogHeader.style |= DS_SETFONT;
    memcpy_s( myBuffer, myBufferBytesLeft, &myDialogHeader, sizeof(myDialogHeader) );
    myBuffer += sizeof(myDialogHeader);
    myBufferBytesLeft -= sizeof(myDialogHeader);

    // Write the menu
    if( myMenuOrdinal == 0 )
    {
        if( !writeTemplateString( myMenuResourceName.c_str() ) ) goto OUT_OF_BUFFER_SPACE;
    }
    else
    {
        if( !writeOrdinal( myMenuOrdinal ) ) goto OUT_OF_BUFFER_SPACE;
    }

    // Write the window class
    if( myClassOrdinal == 0 )
    {
        if( !writeTemplateString( myClassName.c_str() ) ) goto OUT_OF_BUFFER_SPACE;
    }
    else
    {
        if( !writeOrdinal( myClassOrdinal ) ) goto OUT_OF_BUFFER_SPACE;
    }

    // Write the window caption
    if( !writeTemplateString( myTitle.c_str() ) ) goto OUT_OF_BUFFER_SPACE;
    if( !myTitle.empty() )
    {
        // Write the font-point
        if( myTitleFontPoint == 0 ) myTitleFontPoint = 8;
        if( !write( &myTitleFontPoint, sizeof(WORD) ) ) goto OUT_OF_BUFFER_SPACE;

        // Set the font face
        if( myTitleFontFace.empty() ) myTitleFontFace = "MS Sans Serif";
        if( !writeTemplateString( myTitleFontFace.c_str() ) ) goto OUT_OF_BUFFER_SPACE;

    }

    // Write out the dialog items
    for( DialogItemIterator i = myItems.begin(); i != myItems.end(); ++i )
    {
        // This item must begin on a DWORD boundary
        if( !padToBoundary( sizeof(DWORD) ) ) goto OUT_OF_BUFFER_SPACE;

        // Save this item's header
        //i->Header.style |= WS_CHILD;
        if( !write( &i->Header, sizeof(i->Header) ) ) goto OUT_OF_BUFFER_SPACE;

        // This item must begin on a WORD boundary
        if( !padToBoundary( sizeof(WORD) ) ) goto OUT_OF_BUFFER_SPACE;

        // Save the class
        if( i->myClassOrdinal == 0 )
        {
            if( !writeTemplateString( i->myClassName.c_str() ) ) goto OUT_OF_BUFFER_SPACE;
        }
        else
        {
            if( !writeOrdinal( i->myClassOrdinal ) ) goto OUT_OF_BUFFER_SPACE;
        }

        // This item must begin on a WORD boundary
        //if( !padToBoundary( sizeof(WORD) ) ) goto OUT_OF_BUFFER_SPACE;

        // Save the title
        if( !writeTemplateString( i->myTitle.c_str() ) ) goto OUT_OF_BUFFER_SPACE;

        // Write a 0 for the creation data...we don't ever need any, as far as I can tell
        WORD zero = 0;
        if( !write( &zero, sizeof(WORD) ) ) goto OUT_OF_BUFFER_SPACE;
    }

    // Return a pointer to the DWORD-aligned dialog template
    return (LPCDLGTEMPLATE)buffer;


// This label is jumped to in many locations in this method, and it would be pointless (and annoying)
// to try to write 'if' statements for all of them.  Sorry, purists.
OUT_OF_BUFFER_SPACE:

    // Print the problem to the debug log
    DEBUG_ERROR( "Ran out of buffer space while compiling template" );

    // Couldn't finish compilation
    return 0;
}




//-----------------------------------------------------------------------------
// Name:  write
// Desc:  Writes data into the buffer
//-----------------------------------------------------------------------------
bool dcxWin32DialogTemplate::write( const void* data, size_t bytes )
{
    // Make sure the buffer can hold this information
    if( myBufferBytesLeft < bytes )
        return false;

    // Copy the data
    memcpy_s( myBuffer, myBufferBytesLeft, data, bytes );
    myBuffer += bytes;
    myBufferBytesLeft -= bytes;

    // Success
    return true;
}





//-----------------------------------------------------------------------------
// Name:  writeOrdinal
// Desc:  Writes a resource ordinal value
//-----------------------------------------------------------------------------
bool dcxWin32DialogTemplate::writeOrdinal( WORD ordinal )
{
    if( myBufferBytesLeft < sizeof(WORD) * 2 )
        return false;

    WORD ffff = 0xFFFF;
    memcpy_s( &myBuffer[0], myBufferBytesLeft - 0,               &ffff, sizeof(WORD) );
    memcpy_s( &myBuffer[2], myBufferBytesLeft - sizeof(WORD), &ordinal, sizeof(WORD) );

    // Increment the buffer parameters
    myBuffer += sizeof(WORD) * 2;
    myBufferBytesLeft -= sizeof(WORD) * 2;

    // Success
    return true;
}




//-----------------------------------------------------------------------------
// Name:  writeTemplateString
// Desc:  Writes a string into the destination buffer (if there is enough space) as a
//-----------------------------------------------------------------------------
bool dcxWin32DialogTemplate::writeTemplateString( const char* text )
{
    // We need at very least a WORD of space for this function to work correctly
    if( myBufferBytesLeft <= sizeof(WORD) ) return false;

    // Check to see if the buffer is on a WORD boundary
    APP_WARNING( (((DWORD)(myBuffer)) % sizeof(WORD)) != 0 )( "Dialog template string '%s' is being written to a location that is not WORD-aligned", text );

    // Convert the text to Unicode and write it to the buffer
    int returnCode = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, text, -1, (LPWSTR)myBuffer, (int)(myBufferBytesLeft / sizeof(WCHAR)) );

    // Check the return code for errors
    if( 0 == returnCode )
    {
        // Zero is an error code only if there was text in the buffer
        if( strlen(text) == 0 )
        {
            // No text, so just write out a 0x0000 character
            memcpy_s( myBuffer, myBufferBytesLeft, 0x0000, sizeof(WORD) );
            myBuffer += sizeof(WORD);
            myBufferBytesLeft -= sizeof(WORD);
        }
        else
        {
            DEBUG_ERROR( "There was a problem converting %s to Unicode format", text );
            return false;
        }
    }
    else
    {
        // Advance the buffer
        size_t bufferIncrement = returnCode * sizeof(WCHAR);
        myBuffer += bufferIncrement;
        myBufferBytesLeft -= bufferIncrement;
    }

    // Success!
    return true;
}




//-----------------------------------------------------------------------------
// Name:  padToBoundary
// Desc:  Writes zeroes into the buffer
//-----------------------------------------------------------------------------
bool dcxWin32DialogTemplate::padToBoundary( size_t byteMultiple )
{
    size_t bufferAlignmentAdjust = ((DWORD)(myBuffer)) % byteMultiple;

    // Can't adjust if the buffer isn't long enough
    if( myBufferBytesLeft < bufferAlignmentAdjust )
        return false;

    // Add 0's until we've padded enough
    memset( myBuffer, 0, bufferAlignmentAdjust );
    myBuffer += bufferAlignmentAdjust;
    myBufferBytesLeft -= bufferAlignmentAdjust;

    // Success
    return true;
}

