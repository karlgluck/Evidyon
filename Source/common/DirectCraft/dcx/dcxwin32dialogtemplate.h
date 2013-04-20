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
#ifndef __DCXWIN32DIALOGTEMPLATE_H__
#define __DCXWIN32DIALOGTEMPLATE_H__



namespace dcx {

    /**
     * The dialog template is used to compile Win32 dialogs without having to use a resource
     * editor or save resources into a .rc file.  This makes it easier to design custom dialogs
     * that can be reused over multiple projects without having to import preconstructed
     * resources or worry about resource ID conflicts and what-not.  Plus, it lets you create
     * dialogs that are exactly the way that you want them--they can resize based on the elements
     * that are contained, for example, which would have to be done after creation if the
     * dialog was created from a .rc file.
     * All dialogs have the DS_SETFONT style implicitly.
     */
    class dcxWin32DialogTemplate
    {
        /**
         * Holds information about a control in the dialog
         */
        struct DialogItem
        {
            /// The dialog item header holds basic info about the element
            DLGITEMTEMPLATE Header;

            /// The ordinal of a window class
            WORD myClassOrdinal;

            /// The name of the window class
            std::string myClassName;

            /// Caption for this dialog item
            std::string myTitle;
        };


        /// This list type holds all of the elements of the dialog
        typedef std::list<DialogItem> DialogItemList;

        /// Used to iterate through the items in the list
        typedef DialogItemList::iterator DialogItemIterator;


        public:

            /**
             * Initializes the class
             */
            dcxWin32DialogTemplate();

            /**
             * Sets the position of the dialog
             */
            void setLocationDialogUnits( short x, short y );

            /**
             * Sets the size of the dialog
             */
            void setSizeInDialogUnits( short cx, short cy );

            /**
             * Saves flags that determine how the window will be displayed
             */
            void setStyle( DWORD flags );

            /**
             * Saves flags that denote the window's extended display settings
             */
            void setStyleEx( DWORD flags );

            /**
             * Sets the window style to be:
             *  DS_3DLOOK | DS_CONTEXTHELP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
             *  WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
             */
            void setStyleAsResizeableAppWindow();

            /**
             * Sets the window style to:
             * WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_ABSALIGN |
             * DS_3DLOOK | DS_SETFONT | DS_MODALFRAME | DS_NOIDLEMSG | DS_CENTER
             */
            void setStyleAsPrompt();

            /**
             * Sets the window style to:
             * WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | DS_ABSALIGN |
             * DS_3DLOOK | DS_SETFONT | DS_MODALFRAME | DS_NOIDLEMSG
             * And the extended style to:
             * WS_EX_TOOLWINDOW
             */
            void setStyleAsToolbox();

            /**
             * Changes the menu that the dialog will use.  The ordinal value is the number specified in
             * the resource file which is associated with the menu resource.  If ordinal is zero, the
             * menu is disabled (it is zero by default).
             * @param ordinal The menu to attach to the dialog template
             */
            void setMenu( WORD ordinal );

            /**
             * Changes the menu that the dialog will use.  This associates the menu for this template with
             * the name of a resource in the executable.  Passing null (0) will disable the menu.  The menu
             * is disabled by default.
             * @param resourceName The name of the menu resource
             */
            void setMenu( const char* resourceName );

            /**
             * Changes the window class that displays the dialog.  Passing zero will make the dialog use the
             * default system dialog class.
             * @param predefinedSystemClass Ordinal of a predefined class to use
             */
            void setClass( WORD predefinedSystemClass );

            /**
             * Changes the window class that displays the dialog.  Passing null (0) will make the dialog use
             * the default system dialog class.  Otherwise, this should specify the name of a registered
             * Win32 window class.
             * @param className The name of the class
             */
            void setClass( const char* className );

            /**
             * Sets the window caption for the dialog.  This method also controls the font size and typeface.
             * @param title The caption to display
             * @param point The size of the caption font
             * @param font The name of the font to use.  Passing null (0) uses the default: "MS Sans Serif"
             */
            void setTitle( const char* title, WORD point = 8, const char* font = 0 );

            /**
             * Adds a new button to the screen, using coordinates in dialog units
             * @param id The control ID to use for this element
             * @param style Style parameters for the element
             * @param x Left coordinate
             * @param y Top coordinate
             * @param width How wide to make the control
             * @param height How high to make the control
             * @param text The text to place in the button
             */
            void addButtonInDialogUnits( WORD id, DWORD style, short x, short y, short width, short height, const char* text );

            /**
             * Adds a new edit box to the screen, using coordinates in dialog units
             * @param id The control ID to use for this element
             * @param style Style parameters for the element
             * @param x Left coordinate
             * @param y Top coordinate
             * @param width How wide to make the control
             * @param height How high to make the control
             * @param defaultText The text to place on the control by default
             */
            void addEditBoxInDialogUnits( WORD id, DWORD style, short x, short y, short width, short height, const char* defaultText );

            /**
             * Adds a new set of static text to the screen, using coordinates in dialog units
             * @param id The control ID to use for this element
             * @param style Style parameters for the element
             * @param x Left coordinate
             * @param y Top coordinate
             * @param width How wide to make the control
             * @param height How high to make the control
             * @param text The text to place on the label
             */
            void addStaticTextInDialogUnits( WORD id, DWORD style, short x, short y, short width, short height, const char* text );

            /**
             * Adds a new list box to the screen, using coordinates in dialog units
             * @param id The control ID to use for this element
             * @param style Style parameters for the element
             * @param x Left coordinate
             * @param y Top coordinate
             * @param width How wide to make the control
             * @param height How high to make the control
             */
            void addListBoxInDialogUnits( WORD id, DWORD style, short x, short y, short width, short height );

            /**
             * Adds a new combo box to the screen, using coordinates in dialog units
             * @param id The control ID to use for this element
             * @param style Style parameters for the element
             * @param x Left coordinate
             * @param y Top coordinate
             * @param width How wide to make the control
             * @param height How high to make the control
             */
            void addComboBoxInDialogUnits( WORD id, DWORD style, short x, short y, short width, short height );

            /**
             * Adds a new progress bar to the screen, using coordinates in dialog units.  For this method
             * to work, InitCommonControlsEx must be called with ICC_PROGRESS_CLASS included in the
             * parameter's .dwICC field.
             * @param id The control ID to use for this element
             * @param style Style parameters for the element
             * @param x Left coordinate
             * @param y Top coordinate
             * @param width How wide to make the control
             * @param height How high to make the control
             */
            void addProgressBarInDialogUnits( WORD id, DWORD style, short x, short y, short width, short height );

            /**
             * Compiles this dialog template using the provided buffer.  Perform any memory management on the
             * original buffer and not on the returned pointer--the pointer will be aligned so that it falls
             * on a DWORD boundary.
             * This method returns 'null' if the buffer isn't big enough.
             * @param buffer The buffer to use to compile the template
             * @param bufferLength How many bytes the buffer can hold
             * @return Complete dialog template, or 'null' if an error occurred
             */
            LPCDLGTEMPLATE compile( BYTE* buffer, size_t bufferLength );


        protected:

            /**
             * Writes data into the buffer
             */
            bool write( const void* data, size_t bytes );

            /**
             * Writes a resource ordinal value.  This will write 0xFFFF then the WORD value provided.
             */
            bool writeOrdinal( WORD ordinal );

            /**
             * Writes a string into the template buffer (if there is enough space) as a
             * Unicode string.  If the text parameter is 'null' or has no length then only 0x0000 is written
             * A warning will be written to the debug log if the buffer pointer is not WORD-aligned.
             * @param text The source text to convert and print into the buffer
             * @return Whether or not the operation succeeded
             */
            bool writeTemplateString( const char* text );

            /**
             * Writes zeroes into the buffer until the pointer references a location that is a multiple of
             * the given number of bytes.  If this takes more bytes than are available in the buffer, the
             * return value is 'false'
             * @param byteMultiple Pad the template buffer until buffer % byteMultiple == 0
             * @return Whether or not the padding succeeded
             */
            bool padToBoundary( size_t byteMultiple );


        protected:

            /// The dialog header contains basic information about the dialog
            DLGTEMPLATE myDialogHeader;

            /// The menu ordinal
            WORD myMenuOrdinal;

            /// The name of a menu resource
            std::string myMenuResourceName;

            /// The ordinal of a window class
            WORD myClassOrdinal;

            /// The name of the window class
            std::string myClassName;

            /// The window caption
            std::string myTitle;

            /// Window caption font options
            WORD myTitleFontPoint;
            std::string myTitleFontFace;

            /// List of dialog controls
            DialogItemList myItems;

            /// The compilation buffer
            BYTE* myBuffer;
            
            /// The number of bytes remaining in the compilation buffer
            size_t myBufferBytesLeft;
    };

}

#endif