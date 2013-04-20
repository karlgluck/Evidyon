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
#include "stdafx.h"
#include "editresourcedialog.h"
#include "resource"
#include "allgameresources.h"
#include "projectvolucriseditor.h"


/// The main window ID
#define IDD_MAIN 1001




/**

styles: WS_CAPTION|WS_VISIBLE|WS_CLIPSIBLINGS|WS_SYSMENU|WS_THICKFRAME|WS_OVERLAPPED|
        WS_MINIMIZEBOX|WS_MAXIMIZEBOX|DS_3DLOOK|DS_SETFONT|DS_CONTEXTHELP

exStyle:WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR|WS_EX_WINDOWEDGE|WS_EX_CONTEXTHELP|WS_EX_CONTROLPARENT|WS_EX_APPWINDOW

class style: CS_DBLCLKS|CS_SAVEBITS

*/



//------------------------------------------------------------------------------------------------
// Name:  EditResourceDialog
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
EditResourceDialog::EditResourceDialog()
{
    myWindow = NULL;
    myRootResource = NULL;
}



//------------------------------------------------------------------------------------------------
// Name:  execute
// Desc:  Loads, activates and displays this dialog
//------------------------------------------------------------------------------------------------
bool EditResourceDialog::execute(dcGenericResource* root)
{
    // Save the root resource
    myRootResource = root;

    // Get the parent of this window
    HWND hParent = globalEditor()->getTopmostWindowHandle();

    // Run the dialog
    bool returnValue = (S_OK == beginDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAIN), hParent));

    // Remove this window from the list
    globalEditor()->popWindowHandle(myWindow);

    // Get rid of D3D objects if this is the last object
    globalEditor()->releaseD3DObjects();

    // Reset the window reference
    myWindow = NULL;

    // Return the return code
    return returnValue;
}



//------------------------------------------------------------------------------------------------
// Name:  end
// Desc:  Forces this dialog (and any of its children) to close
//------------------------------------------------------------------------------------------------
void EditResourceDialog::end(INT_PTR result)
{
    if (myWindow)
    {
        // Get rid of the dialog
        EndDialog(myWindow, result);
    }
}



//------------------------------------------------------------------------------------------------
// Name:  synchronizeDisplay
// Desc:  Clears the list-box and re-adds the root members of the box
//------------------------------------------------------------------------------------------------
void EditResourceDialog::synchronizeDisplay()
{
    // Get the resources list
    HWND hList = GetDlgItem(myWindow, ID_RESOURCES);

    // Clear the list box
    ListBox_ResetContent(hList);

    // Add all of the children of this resource to the list
    dc::dcGenericResource::Array::const_iterator end = myRootResource->getMembers().end();
    for (dcGenericResource::Array::const_iterator i = myRootResource->getMembers().begin(); i != end; ++i)
    {
        EntryContainer* entry = new EntryContainer();
        entry->indentation = 0;
        entry->expanded = false;
        entry->entry = new ResourceEntry(*i);
        ListBox_AddItemData(hList, entry);
    }
}


//------------------------------------------------------------------------------------------------
// Name: dialogMenuNotification
// Desc: 
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogMenuNotification(HWND hDlg, HMENU hMenu, WORD ItemID, WORD ItemIndex)
{
    switch(ItemID)
    {
        // Create a new project
        case ID_NEW:
            globalEditor()->newProject();
            dcxWin32ForceWindowRedraw(hDlg);
            break;

        // The user wants to open a new list
        case ID_OPEN:
            globalEditor()->openProject();
            dcxWin32ForceWindowRedraw(hDlg);
            break;

        // The user wants to open a new list
        case ID_CLOSE:
            globalEditor()->closeProject();
            dcxWin32ForceWindowRedraw(hDlg);
            break;

        // The user wants to save this list
        case ID_SAVE:
            globalEditor()->saveProject();
            dcxWin32ForceWindowRedraw(hDlg);
            break;

        // The user wants to save this list
        case ID_SAVEAS:
            globalEditor()->saveProjectAs();
            dcxWin32ForceWindowRedraw(hDlg);
            break;

        // Create a resource pack from the current project
        case ID_CREATERESOURCEPACK:
            globalEditor()->createResourcePack();
            dcxWin32ForceWindowRedraw(hDlg);
            break;

         // User wants to import from a project file
        case ID_IMPORT:
            globalEditor()->importProject();
            dcxWin32ForceWindowRedraw(hDlg);
            break;

        // User wants to export the project to another editor
        case ID_EXPORT:
            globalEditor()->exportProject();
            break;

        // User wants to merge another project with this one
        case ID_MERGE:
            globalEditor()->mergeProject();
            dcxWin32ForceWindowRedraw(hDlg);
            break;

        // Compile the project
        case ID_COMPILE:
            globalEditor()->compileProject();
            break;

        // User wants to exit the editor
        case ID_EXIT:
            globalEditor()->closeProject();
            end(0);
            break;

    }

    return 0;
}


//------------------------------------------------------------------------------------------------
// Name: dialogNotification
// Desc: 
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogNotification(HWND hDlg, HWND hControl, UINT_PTR ControlID, UINT Notification)
{
    if (ControlID == ID_RESOURCES && Notification == LBN_DBLCLK)
    {
        // Get the location where the user clicked
        HWND hList = hControl;

        // Find the currently-selected item
        RECT rc;
        int cs = ListBox_GetCurSel(hList);
        EntryContainer* entry = (EntryContainer*)ListBox_GetItemData(hList, cs);
        ListBox_GetItemRect(hList, cs, &rc);

        // Determine if we clicked in the rect
        POINT ptClick = {0};
        GetCursorPos(&ptClick);
        ScreenToClient(hDlg, &ptClick);
        if (PtInRect(&rc, ptClick))
        {
            // We did, so see if we need to change the collapse/expand status
            if (entry->entry->hasChildren())
            {
                // The state is changing!
                if (entry->expanded)
                {
                    // Delete
                    RecursiveRemoveChildren(entry, cs);
                }
                else
                {
                    // Create
                    std::list<ResourceEntry*> resources = entry->entry->createChildren();
                    for (std::list<ResourceEntry*>::reverse_iterator i = resources.rbegin(); i != resources.rend(); ++i)
                    {
                        EntryContainer* ecNew = new EntryContainer();
                        ecNew->expanded = false;
                        ecNew->indentation = entry->indentation + 1;
                        ecNew->entry = *i;
                        ListBox_InsertItemData(hList, cs+1, ecNew);
                    }
                }

                entry->expanded = !entry->expanded;
            }
        }

        // We processed this message
        return TRUE;
    }

    // Default return value
    return 0;
}


//------------------------------------------------------------------------------------------------
// Name: dialogEvent
// Desc: 
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:     return dialogInitDialog(hDlg);
        case WM_CLOSE:          return dialogClose();
        case WM_DELETEITEM:     return dialogDeleteItem((LPDELETEITEMSTRUCT)lParam);
        case WM_MEASUREITEM:    return dialogMeasureItem((LPMEASUREITEMSTRUCT)lParam);
        case WM_DRAWITEM:       return dialogDrawItem((LPDRAWITEMSTRUCT)lParam);
        case WM_SIZE:           return dialogSize(LOWORD(lParam), HIWORD(lParam));
        case WM_CONTEXTMENU:    return dialogContextMenu((HWND)wParam, MAKEPOINTS(lParam));

        // Prevent window flickering
        case WM_ERASEBKGND:     return 1;
    }

    // Default return code
    return 0;
}



//------------------------------------------------------------------------------------------------
// Name: dialogInitDialog
// Desc: Called for WM_INITDIALOG
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogInitDialog(HWND hDlg)
{
    // Save the dialog handle
    myWindow = hDlg;

    // Add this dialog's window to the global list
    globalEditor()->pushWindowHandle(myWindow);

    // Change the window's title
    std::string title = "Volucris Resource Editor [" + myRootResource->getName() + "]";
    SetWindowText(hDlg, title.c_str());

    SetMenu(hDlg, LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(2002)));
    HMENU hMenu = GetMenu(hDlg);
    if (hMenu)
    {
        MENUINFO menuInfo;
        menuInfo.cbSize = sizeof(MENUINFO);
        menuInfo.fMask = MIM_STYLE;
        GetMenuInfo(hMenu, &menuInfo);
        menuInfo.dwStyle |= MNS_NOTIFYBYPOS;
        SetMenuInfo(hMenu, &menuInfo);
    }


    // Disable the compiling option
    // TODO: Add an easy way to disable the compiling capability
    /*
    {
        MENUITEMINFO mii;
        ZeroMemory(&mii, sizeof(mii));
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_STATE;
        APP_WARNING(0 == GetMenuItemInfo(hMenu, ID_COMPILE, FALSE, &mii))("GetMenuItemInfo failed");
        mii.fState |= MFS_GRAYED;
        APP_WARNING(0 == SetMenuItemInfo(hMenu, ID_COMPILE, FALSE, &mii))("SetMenuItemInfo failed");
        EnableMenuItem(hMenu, ID_COMPILE, FALSE);
    }*/

    // Add more D3D objects
    globalEditor()->initializeD3DObjects();

    // Fill the list-box
    synchronizeDisplay();

    // Default return code
    return 0;
}


//------------------------------------------------------------------------------------------------
// Name: dialogClose
// Desc: Called for WM_CLOSE
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogClose()
{
    // Just exit, for now
    end(0);
    return 0;
}


//------------------------------------------------------------------------------------------------
// Name: dialogMeasureItem
// Desc: Called for WM_MEASUREITEM
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogMeasureItem(LPMEASUREITEMSTRUCT measureItemData)
{
    // Set the item height for entries in the list
    measureItemData->itemHeight = 19;

    // We handled this message
    return TRUE;
}


//------------------------------------------------------------------------------------------------
// Name: dialogDrawItem
// Desc: Called for WM_DRAWITEM
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogDrawItem(LPDRAWITEMSTRUCT drawItemData)
{
    if (drawItemData->itemID == -1) return FALSE;

#define BUFFER MAX_PATH 
    TEXTMETRIC tm; 
    int y; 
    HDC hdcMem;

    // Draw the bitmap and text for the list box item. Draw a 
    // rectangle around the bitmap if it is selected. 
    switch(drawItemData->itemAction)
    { 
        case ODA_SELECT: 
        case ODA_DRAWENTIRE:
        {
            // Display the bitmap associated with the item. 

            EntryContainer* entry =(EntryContainer*)SendMessage(drawItemData->hwndItem, 
                LB_GETITEMDATA, drawItemData->itemID, (LPARAM) 0); 

            hdcMem = CreateCompatibleDC(drawItemData->hDC); 
            HPEN colorPen = CreatePen(PS_SOLID, 1, RGB(236,233,216));
            HPEN whitePen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
            HPEN blackPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
            HBRUSH colorBrush = CreateSolidBrush(RGB(236,233,216));
            HBRUSH whiteBrush = CreateSolidBrush(RGB(255,255,255));
            HBRUSH blackBrush = CreateSolidBrush(RGB(0,0,0));

            bool hasChildren = entry->entry->hasChildren();
            int indentation = entry->indentation;

            // Paint the background for the right
            HPEN oldPen = (HPEN)SelectObject(drawItemData->hDC, hasChildren ? colorPen : whitePen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(drawItemData->hDC, hasChildren ? colorBrush : whiteBrush);
            Rectangle(drawItemData->hDC, drawItemData->rcItem.left + (indentation+1) * 12, drawItemData->rcItem.top, drawItemData->rcItem.right, drawItemData->rcItem.bottom);

            GetTextMetrics(drawItemData->hDC, &tm); 

            y = (drawItemData->rcItem.bottom + drawItemData->rcItem.top -
                tm.tmHeight) / 2;

            bool bold = true;
            bool italic = false;
            bool underlined = false;
            HFONT boldFont = CreateFont(8, 0, 0, 0, bold? FW_BOLD : FW_NORMAL, italic, underlined, false,
                                      DEFAULT_CHARSET,
                                      OUT_DEFAULT_PRECIS,
                                      CLIP_DEFAULT_PRECIS,
                                      DEFAULT_QUALITY,
                                      FF_DONTCARE,
                                      "MS Sans Serif");
            HGDIOBJ oldFont = SelectObject(drawItemData->hDC, boldFont);

            // Draw the text for this item
            SetBkMode(drawItemData->hDC, TRANSPARENT);
            RECT rcText = drawItemData->rcItem;
            rcText.left += entry->indentation * 12 + 12 + 10;
            std::string text = entry->entry->name();

            bool drawDesc = false;
            RECT rcDesc;
            if (!hasChildren)
            {
                rcDesc = drawItemData->rcItem;

                RECT drawRect = rcText;
                DrawText(drawItemData->hDC, text.c_str(), text.length(), &drawRect, DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS|DT_LEFT|DT_CALCRECT);
                rcText.right = drawRect.right;

                //rcText.right = rcText.left + text.length() * tm.tmMaxCharWidth;
                if (rcText.right > drawItemData->rcItem.right)
                    rcText.right = drawItemData->rcItem.right;
                else
                {
                    drawDesc = true;
                    rcDesc.left = rcText.right;
                }
            }

            SetTextColor(drawItemData->hDC, RGB(0, 0, 0));
            DrawText(drawItemData->hDC, text.c_str(), text.length(), &rcText, DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS|DT_LEFT);

            if (drawDesc)
            {
                std::string desc = entry->entry->toString();
                // DT_RTLREADING - causes the elipsis to be put on the left
                // DT_RIGHT - align right
                // DT_WORD_ELLIPSIS - cut off elipsis!
                SetTextColor(drawItemData->hDC, RGB(106, 103, 86));
                HFONT descFont = CreateFont(14, 0, 0, 0, FW_NORMAL, false, false, false,
                                          DEFAULT_CHARSET,
                                          OUT_DEFAULT_PRECIS,
                                          CLIP_DEFAULT_PRECIS,
                                          DEFAULT_QUALITY,
                                          FF_DONTCARE,
                                          "Courier New");
                HGDIOBJ oldFont = SelectObject(drawItemData->hDC, descFont);
                TEXTMETRIC tm2;
                GetTextMetrics(drawItemData->hDC, &tm2);
                rcDesc.right = rcDesc.right - rcDesc.right % tm2.tmAveCharWidth;
                DrawText(drawItemData->hDC, desc.c_str(), desc.length(), &rcDesc, DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS|DT_RIGHT);
                SelectObject(drawItemData->hDC, oldFont);
                DeleteObject(descFont);
            }

            SelectObject(drawItemData->hDC, oldFont);

            SelectObject(drawItemData->hDC, colorPen);
            SelectObject(drawItemData->hDC, colorBrush);

            Rectangle(drawItemData->hDC, drawItemData->rcItem.left, drawItemData->rcItem.top, drawItemData->rcItem.left + indentation * 12 + 12 + 6, drawItemData->rcItem.bottom);
            MoveToEx(drawItemData->hDC, drawItemData->rcItem.left, drawItemData->rcItem.bottom, NULL);
            LineTo(drawItemData->hDC, drawItemData->rcItem.right, drawItemData->rcItem.bottom);

            SelectObject(drawItemData->hDC, blackPen);
            SelectObject(drawItemData->hDC, whiteBrush);

            // Draw the square
            if (hasChildren)
            {
                int left = drawItemData->rcItem.left + indentation * 12 + 5;
                int top = drawItemData->rcItem.top + 4;

                Rectangle(drawItemData->hDC, left, top, left + 9, top + 9);
                MoveToEx(drawItemData->hDC, left + 2, top + 4, NULL);
                LineTo(drawItemData->hDC, left + 7, top + 4);

                // Draw the plus unless the entry is expanded
                if (!entry->expanded)
                {
                    MoveToEx(drawItemData->hDC, left + 4, top + 2, NULL);
                    LineTo(drawItemData->hDC, left + 4, top + 7);
                }
            }

            SelectObject(drawItemData->hDC,oldFont);
            SelectObject(drawItemData->hDC,oldPen);
            SelectObject(drawItemData->hDC,oldBrush);
            DeleteObject(boldFont);
            DeleteObject(colorPen);
            DeleteObject(whitePen);
            DeleteObject(blackPen);
            DeleteObject(colorBrush);
            DeleteObject(whiteBrush);
            DeleteObject(blackBrush);
            DeleteDC(hdcMem); 

            // Is the item selected? 

            if (drawItemData->itemState & ODS_SELECTED) 
            { 
                // Set RECT coordinates to surround only the 
                // bitmap. 

                RECT rcBitmap;

                rcBitmap.left = drawItemData->rcItem.left; 
                rcBitmap.top = drawItemData->rcItem.top; 
                rcBitmap.right = drawItemData->rcItem.right; 
                rcBitmap.bottom = drawItemData->rcItem.bottom; 

                // Draw a rectangle around bitmap to indicate 
                // the selection. 

                DrawFocusRect(drawItemData->hDC, &rcBitmap); 
            } 
        } break; 
        case ODA_FOCUS:
            break;
    }

    // We processed this message
    return TRUE;
}


//------------------------------------------------------------------------------------------------
// Name: dialogDeleteItem
// Desc: Called for WM_DELETEITEM
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogDeleteItem(LPDELETEITEMSTRUCT deleteItemData)
{
    delete ((EntryContainer*)deleteItemData->itemData)->entry;
    delete ((EntryContainer*)deleteItemData->itemData);
    return TRUE;
}


//------------------------------------------------------------------------------------------------
// Name: dialogContextMenu
// Desc: Called for WM_CONTEXTMENU
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogContextMenu(HWND hWindow, POINTS point)
{
    // Get the click locations
    POINT ptClick = { point.x, point.y };
    POINT ptClientClick = ptClick;
    ScreenToClient(myWindow, &ptClientClick);

    // Calculate the selected item
    HWND hList = GetDlgItem(myWindow, ID_RESOURCES);
    int item = ListBox_GetTopIndex(hList) + ptClientClick.y / 16; // why isn't this 19?  we set item height to 19!!
    ListBox_SetCurSel(hList, item);

    // Get the entry from the list
    EntryContainer* entry = (EntryContainer*)ListBox_GetItemData(hList, item);

    // Check to make sure that something is in the list
    if (item < 0 || !entry || entry == (EntryContainer*)0xFFFFFFFF) return TRUE;

    // User clicked in the rect, so we need to collapse this item before
    // we can show the popup menu (that way if the children change, we
    // dont have to go through the pain of updating everything
    if (entry->entry->hasChildren() && entry->expanded)
    {
        // Get rid of all of the children and collapse this entry
        RecursiveRemoveChildren(entry, item);
        entry->expanded = !entry->expanded;

        // Sync the list
        UpdateWindow(hList);
    }


    // Generate the popup menu
    HMENU hMenu = CreatePopupMenu();

    // Get actions from this item
    dc::dcAction* actions = entry->entry->getActions();
    int itemIndex = 0;
#define MENU_ITEM_OFFSET 905

    // Go through all of the items in the null-terminated list and add the actions to the menu
    while(actions[itemIndex].name != NULL)
    {
        MENUITEMINFO entry;

        // Build the entry for this function
        if (actions[itemIndex].function != NULL)
            dcxWin32InitMenuItem(&entry, actions[itemIndex].name, itemIndex == 0, itemIndex + MENU_ITEM_OFFSET);
        else
            dcxWin32InitMenuItemSeperator(&entry);

        // Increment the item index
        itemIndex++;

        // Add this item to the end of the list
        InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &entry);
    }

    // If actions are specified, run the popup menu
    if (itemIndex > 0)
    {
        // Execute the menu
        int selected = (int)TrackPopupMenu(hMenu, TPM_RETURNCMD, ptClick.x, ptClick.y, 0, hWindow, NULL);

        // Check to see whether or not the user picked an item
        if (selected >= MENU_ITEM_OFFSET)
        {
            // Get the function pointer from the array
            dc::dcActionFunction function = actions[selected-MENU_ITEM_OFFSET].function;

            // Perform this function
            if (entry->entry->performAction(function, NULL)) // todo: add some sort of parameter
            {
                // If this action did something, the project changed
                globalEditor()->projectChanged();
            }
        }
    }

    // Erase the menu
    DestroyMenu(hMenu);

    // Redraw our list
    dcxWin32ForceWindowRedraw(hList);

    // Successfully processed the message
    return TRUE;
}


//------------------------------------------------------------------------------------------------
// Name: dialogSize
// Desc: Called for WM_SIZE
//------------------------------------------------------------------------------------------------
INT_PTR EditResourceDialog::dialogSize(WORD width, WORD height)
{
    HWND hResources = GetDlgItem(myWindow, ID_RESOURCES);
    SetWindowPos(hResources, 0, 0, 0, width, height, SWP_SHOWWINDOW|SWP_NOMOVE);
    dcxWin32ForceWindowRedraw(hResources);
    return TRUE;
}



//------------------------------------------------------------------------------------------------
// Name: RecursiveRemoveChildren
// Desc: Obtains the handle of the resource list from the dialog
//------------------------------------------------------------------------------------------------
void EditResourceDialog::RecursiveRemoveChildren(EntryContainer* entry, int BaseIndex)
{
    HWND hList = GetDlgItem(myWindow, ID_RESOURCES);

    if (entry->expanded)
    {
        size_t children = entry->entry->numberOfChildren();
        for (size_t i = 0; i < children; ++i)
        {
            size_t childIndex = BaseIndex + 1; // the '+ i' is implied because we delete items
            EntryContainer* child = (EntryContainer*)ListBox_GetItemData(hList, childIndex);
            RecursiveRemoveChildren(child, childIndex);
            ListBox_DeleteString(hList, childIndex);
        }
    }
}