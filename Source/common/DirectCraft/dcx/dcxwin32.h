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
#ifndef __DCXWIN32_H__
#define __DCXWIN32_H__

namespace dcx {

    /**
     * Automatically initializes the common controls library for all of the controls that are
     * needed for functions in this header to work.
     */
    void dcxInitCommonControls();

    /**
     * Brings up the dialog that lets the user select a single file
     * @return Whether or not the user clicked "ok" (if false, the user canceled)
     */
    bool dcxOpenFileDialog( HWND hParent, const char* caption, const char* filter,
                            char* buffer, size_t bufferSize );

    /**
     * Brings up the dialog that lets the user select one or more files
     * @param hParent The parent window for the modal dialog
     * @param caption Text to display in the title bar
     * @param filter OpenFileName-formatted filter text:  ex:  "Bitmap\0*.bmp\0JPEG\0*.jpg\0\0"
     * @param path Output string for the path name
     * @param files Output list for the file names
     * @return Whether or not the user clicked "ok" (if false, the user canceled)
     */
    bool dcxOpenMultipleFilesDialog( HWND hParent, const char* caption, const char* filter,
                                     std::string* path, std::list<std::string>* files );

    /**
     * Pops up a dialog that lets the user select where to save a file
     * @return Whether or not the user clicked "ok" (if false, the user canceled)
     */
    bool dcxSaveFileDialog( HWND hParent, const char* caption, const char* filter,
                            char* buffer, size_t bufferSize );

    /**
     * Displays a the standard dialog that lets the user pick a color.  The 16-entry color set
     * may or may not contain useful colors.
     * @param hParent The window that the dialog should be made a child of
     * @param currentColor The color that should currently be selected
     * @param chosenColor The color that the user picked (if the return value is 'true')
     * @return Whether or not the user picked a color
     */
    bool dcxChooseColorDialog( HWND hParent, COLORREF currentColor, COLORREF* chosenColor );

    /**
     * Brings up a yes/no/cancel dialog and asks the user to do something.  The return values
     * are either IDYES, IDNO or IDCANCEL.
     */
    int dcxYesNoCancelDialog( HWND hParent, const char* caption, const char* prompt );

    /**
     * This function brings up a dialog box with an edit control that prompts the user for
     * text input.  The text box will, by default, have whatever text is currently in 'text'.
     * Be careful, because this will be the text even if it doesn't match the given special
     * parameters (numeric input, for example).  This text box is modal, so its parent window
     * chain can't be activated until it closes.
     * @param hParent Optionally specifies the parent of this window.  NULL means this is topmost.
     * @param caption The text to display in the title bar of the window
     * @param prompt The string to show in the label
     * @param text The text to edit (in/out)
     * @param password Whether or not to make the text input show up as stars
     * @param numeric Only allows numbers to be typed
     * @param lowercase Converts all text input to lower-case
     * @param uppercase Converts all text input to upper-case
     * @return 'true' if the user clicked 'OK', and the text in the *text string has been updated;
     *         false on error or if the user cancels the dialog.
     */
    bool dcxWin32PromptTextInput( HWND hParent, const char* caption, const char* prompt,
                                  std::string* text, bool password = false, bool numeric = false,
                                  bool lowercase = false, bool uppercase = false );

    /**
     * Brings up a dialog box that lets the user edit (or enter) an integer value.
     * @param hParent Optionally specifies the parent of this window.  NULL means this is topmost.
     * @param caption The text to display in the title bar of the window
     * @param prompt The string to show in the label
     * @param value Pointer to the numeric value to edit
     * @param showInitialValue Whether to write out the initial numeric value in the edit box
     * @return Whether or not the user edited the number
     */
    bool dcxWin32PromptNumericInput( HWND hParent, const char* caption, const char* prompt,
                                     int* value, bool showInitialValue = true );


    /**
     * Brings up a dialog box that lets the user edit (or enter) a positive integer value.
     * @param hParent Optionally specifies the parent of this window.  NULL means this is topmost.
     * @param caption The text to display in the title bar of the window
     * @param prompt The string to show in the label
     * @param value Pointer to the numeric value to edit
     * @param showInitialValue Whether to write out the initial numeric value in the edit box
     * @return Whether or not the user edited the number
     */
    bool dcxWin32PromptNumericInput( HWND hParent, const char* caption, const char* prompt,
                                     size_t* value, bool showInitialValue = true );
    /**
     * Brings up a dialog box that lets the user edit (or enter) a floating-point value.  The
     * precision parameter adjusts to what decimal place the value is written when it is being
     * shown; this has no bearing on the input precision.
     * @param hParent Optionally specifies the parent of this window.  NULL means this is topmost.
     * @param caption The text to display in the title bar of the window
     * @param prompt The string to show in the label
     * @param value Pointer to the numeric value to edit
     * @param showInitialValue Whether to write out the initial numeric value in the edit box
     * @param displayPrecision The number of output decimal places (<1 means maximum)
     * @return Whether or not the user edited the number
     */
    bool dcxWin32PromptNumericInput( HWND hParent, const char* caption, const char* prompt,
                                     float* value, bool showInitialValue = true, int displayPrecision = -1 );

    /**
     * Asks the user to select some items from a list
     * @param hParent The parent window for this dialog
     * @param caption The title of the window
     * @param prompt The text to display above the list box (if this is NULL, none is shown)
     * @param options The entries to put in the list
     * @param selectionIndex The index of the selected entry
     * @param initializeSelection Whether or not to select the items in the selection list
     * @return Whether or not the selection was confirmed
     */
    bool dcxWin32PromptForSelection( HWND hParent, const char* caption, const char* prompt,
                                     const std::list<std::string>* options, int* selectionIndex,
                                     bool initializeSelection = false );

    /**
     * Asks the user to select some items from a list
     * @param hParent The parent window for this dialog
     * @param caption The title of the window
     * @param prompt The text to display above the list box (if this is NULL, none is shown)
     * @param options The entries to put in the list
     * @param selectionIndices The indices of the selected entries
     * @param initializeSelection Whether or not to select the items in the selection list
     * @return Whether or not the selection was confirmed
     */
    bool dcxWin32PromptForSelection( HWND hParent, const char* caption, const char* prompt,
                                     const std::list<std::string>* options, std::list<int>* selectionIndices,
                                     bool initializeSelection = false );

    /**
     * Causes the designated window to completely repaint itself
     * @param hWindow The window to repaint
     */
    void dcxWin32ForceWindowRedraw( HWND hWindow );

    /**
     * Fills in the menu item information for a selectable entry
     */
    void dcxWin32InitMenuItem( MENUITEMINFO* menuItemInfo, const char* text, bool defaultSelection, int id );

    /**
     * Generates a menu item with a submenu
     */
    void dcxWin32InitMenuItem( MENUITEMINFO* menuItemInfo, const char* text, HMENU hSubMenu );

    /**
     * Generates a menu item that displays a seperator
     */
    void dcxWin32InitMenuItemSeperator( MENUITEMINFO* menuItemInfo );

    /**
     * Opens up Notepad for the provided file
     */
    void dcxWin32OpenTextEditor( const char* file );

    /**
     * Sets the size of a window's client area in pixels, without moving the window
     * @param hWindow The window to adjust
     * @param width How wide to make the window
     * @param height How high to make the window
     */
    void dcxWin32SetWindowClientSize( HWND hWindow, int width, int height );

    /**
     * Gets a single-selection list-box's currently selected item
     * @param hList Handle to the list to query
     * @param selectionIndex Destination for the selected index (-1 if nothing selected)
     * @return Whether or not something was selected
     */
    bool dcxWin32GetListSelection( HWND hList, int* selectionIndex );

    /**
     * Obtains a multi-select list-box's currently selected items
     * @param hList The handle to the list to query
     * @param selectionIndices Destination list for selected indices
     * @return How many items were selected
     */
    int dcxWin32GetListMultipleSelection( HWND hList, std::list<int>* selectionIndices );

    /**
     * Passes messages through the Windows message queue until there are none left.  When
     * finished, this method returns whether or not the parameter (hWnd) is still a valid
     * window.
     */
    bool dcxWin32StdMessagePump( HWND hWnd );

    /**
     * Determines whether or not the version of Windows running on the current machine is
     * Windows 2000 or Windows XP.
     */
    bool dcxWin32WindowsVersionIs2000OrXP();

    /**
     * Generates a unique Win32 file-name compatible string based on the current date
     * and time.  This is useful for generating screenshots, backup logs, and anything
     * else that needs a unique name.
     */
    void dcxWin32FilnameCompatibleDateTime(char* output, size_t output_size);

    // Ensures that the extension is set correctly.  The extension should include the "." so
    // for a bitmap, it would be ".bmp"; a JPEG ".jpg", etc.  This is commonly used after
    // a save file propmt to validate the user's extension choice.
    void dcxWin32SetFileExtension(char* file_path, size_t buffer_size, const char* extension);
}



#endif