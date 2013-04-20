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
#include <windows.h>
#include <string>
#include "../dc/dcstream.h"
#include "../dc/dcdebug.h"
#include <string>
#include <list>
#include "../dcx/dcxwin32.h"
#include <commdlg.h>    // Common Win32 dialog processors
#include <commctrl.h>   // Common controls (like the progress bar)
#include "../dcx/dcxwin32dialogtemplate.h"
#include <windowsx.h>   // For simplifying dialog interaction


using namespace dcx;



//-----------------------------------------------------------------------------
// Name:  dcxInitCommonControls
// Desc:  Automatically initializes the common controls library
//-----------------------------------------------------------------------------
void dcx::dcxInitCommonControls()
{
    // Set up extra common controls
    INITCOMMONCONTROLSEX ex = { sizeof(INITCOMMONCONTROLSEX), ICC_TAB_CLASSES|ICC_BAR_CLASSES|ICC_WIN95_CLASSES };
    if( FALSE == InitCommonControlsEx( &ex ) )
        DEBUG_WARNING( "Unable to initialize Win32 common controls.  Some dialogs may not display." );
}



//-----------------------------------------------------------------------------
// Name:  dcxOpenFileDialog
// Desc:  
//-----------------------------------------------------------------------------
bool dcx::dcxOpenFileDialog( HWND hParent, const char* caption, const char* filter,
                             char* buffer, size_t bufferSize )
{
    // Reset the buffer
    ZeroMemory( buffer, bufferSize );

    // Build the structure
    OPENFILENAME ofn = { sizeof(OPENFILENAME), hParent, NULL, filter, 
                         NULL, 0, 1, buffer, (DWORD)bufferSize, NULL, 0, NULL, caption,
                         OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING,
                         0, 0, 0, 0, 0, 0 };

    // If the user picks a file, reload everything
    return TRUE == GetOpenFileName( &ofn );
}


//-----------------------------------------------------------------------------
// Name:  dcxOpenMultipleFilesDialog
// Desc:  Allows the user to select multiple files for output
//-----------------------------------------------------------------------------
bool dcx::dcxOpenMultipleFilesDialog( HWND hParent, const char* caption, const char* filter,
                                      std::string* path, std::list<std::string>* files )
{
    const int FILE_BUFFER_SIZE = 1024 * 32;

    // Confirm input parameters
    if( APP_ERROR( !caption || !filter || !path || !files )( "Invalid parameter to dcxOpenMultipleFilesDialog" ) )
        return false;

    // Clear the output parameters
    path->clear();
    files->clear();

    // To let the user select a lot of files, we need a big buffer!
    char* fileBuffer = new char[FILE_BUFFER_SIZE];

    // Make sure the buffer could be allocated
    if( APP_FATAL( !fileBuffer )( "Out of memory" ) )
        return false;

    // Reset the path and buffer
    ZeroMemory( fileBuffer, FILE_BUFFER_SIZE * sizeof(char) );

    // Build the structure
    OPENFILENAME ofn = { sizeof(OPENFILENAME), hParent, NULL, filter, 
                         NULL, 0, 1, fileBuffer, FILE_BUFFER_SIZE, NULL, 0, NULL, caption,
                         OFN_ALLOWMULTISELECT|OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING,
                         0, 0, 0, 0, 0, 0 };

    // Prompt the user for files
    BOOL selectedFiles = GetOpenFileName( &ofn );

    // If necessary, get the file information
    if( selectedFiles )
    {
        // Add each of the selected files!
        char directory[MAX_PATH];
        char file[MAX_PATH];

        // Each returned element is delimited by a null-terminator, so it's ok
        // just to do a copy!
        char* element = fileBuffer;
        strcpy_s( directory, MAX_PATH, element );

        // Increment to the next token and process it
        if( *(element += (strlen(element) + 1)) != '\0' )
        {
            // Set the output path
            path->assign( directory );

            // Go through each other element in the file
            do
            {
                // Get this file's name
                strcpy_s( file, MAX_PATH, element );

                // Save in the output buffer
                files->push_back( file );
            } while( *(element += (strlen(element) + 1)) != '\0' );
        }
        else
        {
            // Only a single file was selected, so decompose the path
            char* fileInDirectory = strrchr( directory, '\\' );

            // Make sure the file was found
            if( APP_WARNING( fileInDirectory == NULL )( "Invalid path returned from OpenFileName; aborting" ) )
                return false;

            // The file's name is the last part of the directory
            files->push_back( fileInDirectory + 1 );

            // Terminate the directory before the last backslash & the file name
            *fileInDirectory = '\0';

            // Save the path
            path->assign( directory );
        }
    }

    // Get rid of the allocated memory
    delete[] fileBuffer;

    // Return the result
    return selectedFiles == TRUE;
}



//-----------------------------------------------------------------------------
// Name:  dcxSaveFileDialog
// Desc:  
//-----------------------------------------------------------------------------
bool dcx::dcxSaveFileDialog( HWND hParent, const char* caption, const char* filter,
                             char* buffer, size_t bufferSize )
{
    // Reset the buffer
    ZeroMemory( buffer, bufferSize );

    // Build the structure
    OPENFILENAME ofn = { sizeof(OPENFILENAME), hParent, NULL, filter,
                         NULL, 0, 1, buffer, bufferSize, NULL, 0, NULL, caption,
                         OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_ENABLESIZING, 0, 0, 0, 0, 0, 0 };

    // If the user picks a file, reload everything
    return TRUE == GetSaveFileName( &ofn );
}


//-----------------------------------------------------------------------------
// Name:  dcxChooseColorDialog
// Desc:  
//-----------------------------------------------------------------------------
bool dcx::dcxChooseColorDialog( HWND hParent, COLORREF currentColor, COLORREF* chosenColor )
{
    // This is the color palette
    COLORREF colors16[16];

    // The first palette color will be the current color
    colors16[0] = currentColor;

    // Set up the rest of the palette as shades of gray
    for( int i = 1; i < 16; ++i )
        colors16[i] = RGB( i * 16, i * 16, i * 16 );

    // Build the structure
    CHOOSECOLOR choose = { sizeof(CHOOSECOLOR), hParent, NULL, currentColor, colors16,
                           CC_ANYCOLOR|CC_RGBINIT, NULL, NULL, NULL };

    // Run the dialog
    if( ChooseColor( &choose ) )
    {
        // Store the color key
        *chosenColor = choose.rgbResult;

        // User selected a color
        return true;
    }
    else
        // User cancelled
        return false;
}


//-----------------------------------------------------------------------------
// Name:  dcxYesNoCancelDialog
// Desc:  Brings up a yes/no/cancel dialog and asks the user to do something
//-----------------------------------------------------------------------------
int dcx::dcxYesNoCancelDialog( HWND hParent, const char* caption, const char* prompt )
{
    return MessageBox( hParent, prompt, caption, MB_YESNOCANCEL );
}




//-----------------------------------------------------------------------------
// Name:  dcxWin32PromptTextInput_MessageBoxProc
// Desc:  
//-----------------------------------------------------------------------------
INT_PTR CALLBACK dcxWin32PromptTextInput_MessageBoxProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:

            // Just save the input parameter
            SetWindowLongPtr( hDlg, GWL_USERDATA, (LONG_PTR)lParam );

            // Center this window on the screen
            RECT rcSize;
            GetWindowRect( hDlg, &rcSize );
            SetWindowPos( hDlg, HWND_TOPMOST, (GetSystemMetrics(SM_CXSCREEN) - (rcSize.right-rcSize.left)) / 2,
                                              (GetSystemMetrics(SM_CYSCREEN) - (rcSize.bottom-rcSize.top)) / 2,
                                              0, 0, SWP_NOSIZE );
                                      

            break;


        case WM_COMMAND:

            // Only process a click message
            if( HIWORD(wParam) != BN_CLICKED ) break;

        case WM_CLOSE:

            // Get the output structure
            std::string* outputText = (std::string*)GetWindowLongPtr( hDlg, GWL_USERDATA );

            // The user confirmed the changes only if they hit the 'ok' button
            BOOL confirmed = (uMsg == WM_COMMAND) && (LOWORD(wParam) == IDOK);

            // Save the text if necessary
            if( confirmed )
            {
                // Scan the text into a buffer
                char buffer[512];
                Edit_GetText( GetDlgItem( hDlg, 1001 ), buffer, sizeof(buffer) / sizeof(char) );

                // Save the text to the output
                outputText->assign( buffer );
            }

            // Exit the dialog
            EndDialog( hDlg, (INT_PTR)confirmed );

            break;
    }

    // Exit the message box
    return 0;
}





//-----------------------------------------------------------------------------
// Name:  dcxWin32PromptTextInput
// Desc:  Brings up a dialog box that prompts the user to edit a string
//-----------------------------------------------------------------------------
bool dcx::dcxWin32PromptTextInput( HWND hParent, const char* caption, const char* prompt,
                                   std::string* text,
                                   bool password, bool numeric, bool lowercase, bool uppercase )
{
    dcxWin32DialogTemplate dlg;
    dlg.setTitle( caption );

    // How big the prompt is
    size_t defaultTextLength = text->length();
    size_t promptLength = strlen( prompt );
    size_t widthCheck = (size_t)(1.4 * (max( promptLength, defaultTextLength )));

    // Initialize the dialog
    short maxWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
    short width = min( max( 265, widthCheck + 20 ), maxWidth );
    short height = 75 + 20 * widthCheck / width;
    dlg.setSizeInDialogUnits( width, height );
    dlg.setStyleAsPrompt();

    // Develop the edit box style based on the parameters
    DWORD editStyle = ES_AUTOHSCROLL | WS_BORDER |
                      (uppercase ? ES_UPPERCASE : 0) | (lowercase ? ES_LOWERCASE : 0) |
                      (password ? ES_PASSWORD : 0) | (numeric ? ES_NUMBER : 0);

    // Add controls to this dialog
    const int BUTTON_WIDTH = 65;
    dlg.addEditBoxInDialogUnits( 1001, editStyle, 10, height - 42, width - 20, 12, text->c_str() );
    dlg.addStaticTextInDialogUnits( 0, 0, 10, 10, width - 20, height - 42 - 10, prompt );
    dlg.addButtonInDialogUnits( IDOK,     0, 1 * width / 3 - 10 / 2 - BUTTON_WIDTH / 2, height - 22, BUTTON_WIDTH, 15, "OK" );
    dlg.addButtonInDialogUnits( IDCANCEL, 0, 2 * width / 3 + 10 / 2 - BUTTON_WIDTH / 2, height - 22, BUTTON_WIDTH, 15, "Cancel" );

    // Build the dialog template and execute the dialog
    BYTE dialogTemplate[1024];
    INT_PTR retval = DialogBoxIndirectParam( GetModuleHandle(NULL), dlg.compile( dialogTemplate, sizeof(dialogTemplate) ),
                                             hParent, dcxWin32PromptTextInput_MessageBoxProc, (LPARAM)text );

    // Return the result code
    return retval != 0;
}




//-----------------------------------------------------------------------------
// Name:  dcxWin32PromptNumericInput
// Desc:  Executes a dialog that lets the user edit a number
//-----------------------------------------------------------------------------
bool dcx::dcxWin32PromptNumericInput( HWND hParent, const char* caption, const char* prompt,
                                      int* value, bool showInitialValue )
{
    // Convert the number into a string
    char buffer[16];
    sprintf_s( buffer, sizeof(buffer), "%i", *value );
    std::string valueString = showInitialValue ? buffer : "";

    // Run the input dialog
    if( dcxWin32PromptTextInput( hParent, caption, prompt, &valueString ) )
    {
        // Read the number from the string; this only succeeds if the return value
        // is equal to the number of fields read (1)
        return 1 == sscanf_s( valueString.c_str(), "%i", value );
    }
    else
        return false;
}


//-----------------------------------------------------------------------------
// Name:  dcxWin32PromptNumericInput
// Desc:  Executes a dialog that lets the user edit a number
//-----------------------------------------------------------------------------
bool dcx::dcxWin32PromptNumericInput( HWND hParent, const char* caption, const char* prompt,
                                      size_t* value, bool showInitialValue )
{
    // Convert the number into a string
    char buffer[16];
    sprintf_s( buffer, sizeof(buffer), "%i", *value );
    std::string valueString = showInitialValue ? buffer : "";

    // Run the input dialog
    if( dcxWin32PromptTextInput( hParent, caption, prompt, &valueString, false, true ) )
    {
        // Read the number from the string; this only succeeds if the return value
        // is equal to the number of fields read (1)
        return 1 == sscanf_s( valueString.c_str(), "%i", value );
    }
    else
        return false;
}



//-----------------------------------------------------------------------------
// Name:  dcxWin32PromptNumericInput
// Desc:  Executes a dialog that lets the user edit a number
//-----------------------------------------------------------------------------
bool dcx::dcxWin32PromptNumericInput( HWND hParent, const char* caption, const char* prompt,
                                      float* value, bool showInitialValue, int displayPrecision )
{
    // Convert the number into a string
    char buffer[64];
    char mask[64];
    sprintf_s( mask, sizeof(mask), displayPrecision <= 0 ? "%%f" : "%%.0%if", displayPrecision );
    sprintf_s( buffer, sizeof(buffer), mask, *value );
    std::string valueString = showInitialValue ? buffer : "";

    // Run the input dialog
    if( dcxWin32PromptTextInput( hParent, caption, prompt, &valueString ) )
    {
        // Read the number from the string; this only succeeds if the return value
        // is equal to the number of fields read (1)
        return 1 == sscanf_s( valueString.c_str(), "%f", value );
    }
    else
        return false;
}



struct dcxWin32PromptForSelection_Params
{
    const std::list<std::string>* options;
    int* selectionIndex;
    std::list<int>* selectionIndices;
    bool initializeSelection;
};

//-----------------------------------------------------------------------------
// Name:  dcxWin32PromptTextInput_MessageBoxProc
// Desc:  
//-----------------------------------------------------------------------------
INT_PTR CALLBACK dcxWin32PromptForSelection_MessageBoxProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_INITDIALOG:
        {
            // Save the input parameter
            SetWindowLongPtr( hDlg, GWL_USERDATA, (LONG_PTR)lParam );

            // Get the creation parameters
            dcxWin32PromptForSelection_Params* params = (dcxWin32PromptForSelection_Params*)lParam;

            // Get the list box control
            HWND hList = GetDlgItem( hDlg, 1001 );

            // Write out the entries in the list
            std::list<std::string>::const_iterator optionsEnd = params->options->end();
            size_t index = 0;
            for( std::list<std::string>::const_iterator i = params->options->begin(); i != optionsEnd; ++i, ++index )
            {
                // Add the string to the list box
                int stringIndex = ListBox_AddString( hList, i->c_str() );

                // Associate the index with this string in the list box
                //ListBox_SetItemData( hList, stringIndex, index );
            }

            // Handle the single-selection index
            if( params->selectionIndex != 0 )
            {
                // Initialize the selection, if necessary
                if( params->initializeSelection )
                {
                    ListBox_SetCurSel( hList, *params->selectionIndex );
                }
            }

            // Handle the multi-selection index
            else if( params->selectionIndices != 0 )
            {
                // Initialize the selection, if necessary
                if( params->initializeSelection )
                {
                    std::list<int>::const_iterator selectionEnd = params->selectionIndices->end();
                    for( std::list<int>::const_iterator i = params->selectionIndices->begin();
                         i != selectionEnd; ++i )
                    {
                        ListBox_SetSel( hList, TRUE, *i );
                    }
                }
            }

            // If neither selection parameter is valid, fail
            else
                EndDialog( hDlg, FALSE );

            // Center this window on the screen
            RECT rcSize;
            GetWindowRect( hDlg, &rcSize );
            SetWindowPos( hDlg, HWND_TOPMOST, (GetSystemMetrics(SM_CXSCREEN) - (rcSize.right-rcSize.left)) / 2,
                                              (GetSystemMetrics(SM_CYSCREEN) - (rcSize.bottom-rcSize.top)) / 2,
                                              0, 0, SWP_NOSIZE );
                                      

        } break;


        case WM_COMMAND:

            // Only process a click message
            if( HIWORD(wParam) != BN_CLICKED ) break;

        case WM_CLOSE:

            // Get the output structure
            dcxWin32PromptForSelection_Params* params =
                (dcxWin32PromptForSelection_Params*)GetWindowLongPtr( hDlg, GWL_USERDATA );

            // The user confirmed the changes only if they hit the 'ok' button
            BOOL confirmed = (uMsg == WM_COMMAND) && (LOWORD(wParam) == IDOK);

            // Get the list box control
            HWND hList = GetDlgItem( hDlg, 1001 );

            // Save the text if necessary
            if( confirmed )
            {
                // Save the selection
                if( params->selectionIndex )
                {
                    // Set the selection
                    *params->selectionIndex = ListBox_GetCurSel( hList );
                }
                else if( params->selectionIndices )
                {
                    // Clear the selection
                    params->selectionIndices->clear();

                    // Get the number of selected items
                    int selectedItems = ListBox_GetSelCount( hList );
                    if( selectedItems > 0 )
                    {
                        // Allocate the selection array and get the selected items
                        int* selection = new int[selectedItems];
                        ListBox_GetSelItems( hList, selectedItems, selection );

                        // Add each selected item to the list
                        for( int i = 0; i < selectedItems; ++i )
                            params->selectionIndices->push_back( selection[i] );

                        // Free the selection memory
                        delete[] selection;
                    }
                }
                else
                    EndDialog( hDlg, (INT_PTR)FALSE );
            }

            // Exit the dialog
            EndDialog( hDlg, (INT_PTR)confirmed );

            break;
    }

    // Exit the message box
    return 0;
}



//-----------------------------------------------------------------------------
// Name:  dcxWin32PromptForSelection_Helper
// Desc:  
//-----------------------------------------------------------------------------
bool dcxWin32PromptForSelection_Helper( HWND hParent, const char* caption, const char* prompt,
                                        dcxWin32PromptForSelection_Params* params, DWORD extraListboxStyle )
{
    // Construct the dialog
    dcxWin32DialogTemplate dlg;
    dlg.setTitle( caption );

    // Find the longest entry in the options; TODO: make options->end into a local variable
    size_t maxOptionTextLength = 0;
    for( std::list<std::string>::const_iterator i = params->options->begin(); i != params->options->end(); ++i )
    {
        size_t optionLength = i->size();
        maxOptionTextLength = max( maxOptionTextLength, optionLength );
    }

    // How big the prompt is
    size_t promptLength = prompt ? strlen( prompt ) : 0;
    size_t widthCheck = 20 + (size_t)(1.4 * (max( promptLength, maxOptionTextLength )));

    // Do some calculations to figure out the dialog's size
    short maxWidth = GetSystemMetrics(SM_CXSCREEN) / 4;
    short width = max( 200, widthCheck );
    if( width > maxWidth ) width = maxWidth;
    short promptHeight = prompt ? 16 * widthCheck / width + 15 : 0;
    short height = params->options->size() * 16 + promptHeight;
    if( height < width + 40 ) height = width + 40;  // Make sure the dialog is always higher than it is wide
    short maxHeight = GetSystemMetrics(SM_CYSCREEN) / 4 + promptHeight;
    if( height > maxHeight ) height = maxHeight;    // The dialog shouldn't take up the whole screen

    // Set up the dialog's display style
    dlg.setSizeInDialogUnits( width, height );
    dlg.setStyleAsPrompt();

    // Create some markers
    short controlWidth = width - 20;
    short promptBottom = 10 + promptHeight;
    short buttonTop = height - 22;
    short listBoxTop = promptBottom;
    short listBoxHeight = buttonTop - 10 - listBoxTop;

    // Add controls to this dialog
    const int BUTTON_WIDTH = 65;
    if( prompt != 0 )
        dlg.addStaticTextInDialogUnits( 0, 0, 10, 10, controlWidth, promptHeight, prompt );
    dlg.addButtonInDialogUnits( IDOK,     0, 1 * width / 3 - 10 / 2 - BUTTON_WIDTH / 2, buttonTop, BUTTON_WIDTH, 15, "OK" );
    dlg.addButtonInDialogUnits( IDCANCEL, 0, 2 * width / 3 + 10 / 2 - BUTTON_WIDTH / 2, buttonTop, BUTTON_WIDTH, 15, "Cancel" );
    dlg.addListBoxInDialogUnits( 1001, extraListboxStyle | LBS_NOINTEGRALHEIGHT | LBS_DISABLENOSCROLL |
                                       WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | WS_BORDER,
                                       10, listBoxTop, controlWidth, listBoxHeight );

    // Build the dialog template and execute the dialog
    BYTE dialogTemplate[1024];
    INT_PTR retval = DialogBoxIndirectParam( GetModuleHandle(NULL), dlg.compile( dialogTemplate, sizeof(dialogTemplate) ),
                                             hParent, dcxWin32PromptForSelection_MessageBoxProc, (LPARAM)params );

    // Return the result code
    return retval != 0;
}





//-----------------------------------------------------------------------------
// Name:  dcxWin32PromptForSelection
// Desc:  Asks the user to select an item from a list
//-----------------------------------------------------------------------------
bool dcx::dcxWin32PromptForSelection( HWND hParent, const char* caption, const char* prompt,
                                      const std::list<std::string>* options, int* selectionIndex,
                                      bool initializeSelection )
{
    // Can't select from an empty or nonexistant list
    if( !options || options->empty() || !selectionIndex ) return false;

    // Create the parameters structure
    dcxWin32PromptForSelection_Params params = { options, selectionIndex, 0, initializeSelection };

    // Call the helper method
    return dcxWin32PromptForSelection_Helper( hParent, caption, prompt, &params, 0 );
}



//-----------------------------------------------------------------------------
// Name:  dcxWin32PromptForSelection
// Desc:  Asks the user to select some items from a list
//-----------------------------------------------------------------------------
bool dcx::dcxWin32PromptForSelection( HWND hParent, const char* caption, const char* prompt,
                                      const std::list<std::string>* options, std::list<int>* selectionIndices,
                                      bool initializeSelection )
{
    // Can't select from an empty or nonexistant list
    if( !options || options->empty() || !selectionIndices ) return false;

    // Create the parameters structure
    dcxWin32PromptForSelection_Params params = { options, 0, selectionIndices, initializeSelection };

    // Call the helper method
    return dcxWin32PromptForSelection_Helper( hParent, caption, prompt, &params, LBS_EXTENDEDSEL );
}


//-----------------------------------------------------------------------------
// Name:  dcxWin32ForceWindowRedraw
// Desc:  Causes the designated window to completely repaint itself
//-----------------------------------------------------------------------------
void dcx::dcxWin32ForceWindowRedraw( HWND hWindow )
{
    RedrawWindow( hWindow, NULL, NULL, RDW_INVALIDATE );
}


//-----------------------------------------------------------------------------
// Name:  dcxWin32InitMenuItem
// Desc:  Fills in the menu item information for a selectable entry
//-----------------------------------------------------------------------------
void dcx::dcxWin32InitMenuItem( MENUITEMINFO* menuItemInfo, const char* text, bool defaultSelection, int id )
{
    MENUITEMINFO temp = { sizeof(MENUITEMINFO), MIIM_ID|MIIM_STRING|MIIM_STATE,
                          0, defaultSelection ? MFS_DEFAULT : 0, id, NULL, NULL, NULL, 0,
                          (LPSTR)text, 0, NULL };
    memcpy_s( menuItemInfo, sizeof(MENUITEMINFO), &temp, sizeof(temp) );
}



//-----------------------------------------------------------------------------
// Name:  dcxWin32InitMenuItem
// Desc:  Generates a menu item with a submenu
//-----------------------------------------------------------------------------
void dcx::dcxWin32InitMenuItem( MENUITEMINFO* menuItemInfo, const char* text, HMENU hSubMenu )
{
    MENUITEMINFO temp = { sizeof(MENUITEMINFO), MIIM_STRING|MIIM_SUBMENU,
                          0, 0, -1, hSubMenu, NULL, NULL, 0,
                          (LPSTR)text, 0, NULL };
    memcpy_s( menuItemInfo, sizeof(MENUITEMINFO), &temp, sizeof(temp) );
}



//-----------------------------------------------------------------------------
// Name:  dcxWin32InitMenuItemSeperator
// Desc:  Generates a menu item that displays a seperator
//-----------------------------------------------------------------------------
void dcx::dcxWin32InitMenuItemSeperator( MENUITEMINFO* menuItemInfo )
{
    MENUITEMINFO temp = { sizeof(MENUITEMINFO), MIIM_TYPE,
                          MFT_SEPARATOR, 0, -1, NULL, NULL, NULL, 0,
                          NULL, 0, NULL };
    memcpy_s( menuItemInfo, sizeof(MENUITEMINFO), &temp, sizeof(temp) );
}



//-----------------------------------------------------------------------------
// Name:  dcxWin32OpenTextEditor
// Desc:  Opens up Notepad for the provided file
//-----------------------------------------------------------------------------
void dcx::dcxWin32OpenTextEditor( const char* file )
{
    // Generate the command line for the process
    char commandLine[512];
    sprintf_s( commandLine, sizeof(commandLine), "NOTEPAD.EXE \"%s\"", file );

    // This structure just needs to exist to create the process
    STARTUPINFO startupInfo;
    ZeroMemory( &startupInfo, sizeof(startupInfo) );
    startupInfo.cb = sizeof(STARTUPINFO);

    // Receives information about the process
    PROCESS_INFORMATION processInfo;
    ZeroMemory( &processInfo, sizeof(processInfo) );

    // Create the process
    CreateProcess( NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo );
}


//-----------------------------------------------------------------------------
// Name:  dcxWin32SetWindowClientSize
// Desc:  Sets the size of a window's client area in pixels, without moving the window
//-----------------------------------------------------------------------------
void dcx::dcxWin32SetWindowClientSize( HWND hWindow, int width, int height )
{
    RECT rc = { 0, 0, width, height };
    AdjustWindowRect( &rc, GetWindowLong( hWindow, GWL_STYLE ), GetMenu( hWindow ) != 0 );
    SetWindowPos( hWindow, 0, 0, 0, rc.right - rc.left, rc.bottom-rc.top, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE );
}



//-----------------------------------------------------------------------------
// Name:  dcxWin32GetListSelection
// Desc:  Gets a single-selection list-box's currently selected item
//-----------------------------------------------------------------------------
bool dcx::dcxWin32GetListSelection( HWND hList, int* selectionIndex )
{
    int selectedItem = ListBox_GetCurSel( hList );
    if( selectionIndex )
        *selectionIndex = selectedItem;
    return selectedItem >= 0;
}



//-----------------------------------------------------------------------------
// Name:  dcxWin32GetListMultipleSelection
// Desc:  Obtains a multi-select list-box's currently selected items
//-----------------------------------------------------------------------------
int dcx::dcxWin32GetListMultipleSelection( HWND hList, std::list<int>* selectionIndices )
{
    // Make sure the parameters are valid
    if( APP_ERROR( !hList || !selectionIndices )( "Invalid parameter to dcxWin32GetListMutlipleSelection" ) )
        return 0;

    // Reset the selection indices
    selectionIndices->clear();

    // Get the number of selected items
    int selectedItems = ListBox_GetSelCount( hList );
    if( selectedItems > 0 )
    {
        // Allocate the selection array and get the selected items
        int* selection = new int[selectedItems];
        if( APP_FATAL( !selection )( "Out of memory!" ) ) return 0;

        // Get the selection
        ListBox_GetSelItems( hList, selectedItems, selection );

        // Add each selected item to the list
        for( int i = 0; i < selectedItems; ++i )
            selectionIndices->push_back( selection[i] );

        // Deallocate the selection list memory
        delete[] selection;
    }

    // Return the number of items that were selected
    return selectedItems;
}



//-----------------------------------------------------------------------------
// Name:  dcxWin32StdMessagePump
// Desc:  Passes messages through the Windows message queue until there are none left.  When
//        finished, this method returns whether or not the parameter (hWnd) is still a valid
//        window. 
//-----------------------------------------------------------------------------
bool dcx::dcxWin32StdMessagePump( HWND hWnd )
{
    // Run the message loop until it's empty
    MSG msg; 
    ZeroMemory( &msg, sizeof(msg) );
    while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );

        // Clear out the message structure
        ZeroMemory( &msg, sizeof(msg) );
    }

    return FALSE != IsWindow( hWnd );
}




//-----------------------------------------------------------------------------
// Name:  dcxWin32WindowsVersionIs2000OrXP
// Desc:  Determines whether or not the version of Windows running on the current machine is
//        Windows 2000 or Windows XP.
//-----------------------------------------------------------------------------
bool dcx::dcxWin32WindowsVersionIs2000OrXP()
{
    OSVERSIONINFO osVer = { sizeof(osVer) };
    GetVersionEx( &osVer );
    return (VER_PLATFORM_WIN32_NT == osVer.dwPlatformId && osVer.dwMajorVersion >= 5);
}


//----[  dcxWin32FilnameCompatibleDateTime  ]----------------------------------
void dcx::dcxWin32FilnameCompatibleDateTime(char* output, size_t output_size) {
  char date[MAX_PATH];
  SYSTEMTIME st;
  GetSystemTime(&st);
  GetDateFormatA(LOCALE_USER_DEFAULT,
                 0,
                 &st,
                 "yyyy-MM-dd",
                 date,
                 MAX_PATH);
  sprintf_s(output,
            output_size,
            "%s at %lu'%lu'%lu",
            date,
            st.wHour,
            st.wMinute,
            st.wSecond);
}



//----[  dcxWin32SetFileExtension  ]-------------------------------------------
void dcx::dcxWin32SetFileExtension(char* file_path,
                                   size_t buffer_size,
                                   const char* extension) {
  size_t len = strlen(file_path);
  size_t ext_len = strlen(extension);
  if (buffer_size < ext_len) return;
  if (len < ext_len || _stricmp(&file_path[len - ext_len], extension) != 0) {
    if (len + ext_len + 1 < buffer_size) {
      strcat(file_path, extension);
    } else {
      strcpy(file_path + buffer_size - ext_len - 1, extension);
    }
  }
}