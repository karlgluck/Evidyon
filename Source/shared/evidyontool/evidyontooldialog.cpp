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
#include "evidyontool.h"
#include "evidyontooldialog.h"
#include <windowsx.h>
#include "evidyontooldialoglistelement.h"
#include <dcx/win32>



namespace Evidyon {
namespace Tools {

static const WORD EVIDYONTOOLDIALOG_LISTBOXID = 1002;

//----[  EvidyonToolDialog  ]--------------------------------------------------
EvidyonToolDialog::EvidyonToolDialog(EvidyonTool* owner) {
  owner_ = owner;
  window_ = NULL;
  root_resource_ = NULL;
}


//----[  execute  ]------------------------------------------------------------
bool EvidyonToolDialog::execute(dc::dcGenericResource* root,
                                HWND parent_window,
                                WORD menu_resource_id) {
  if (!owner_) return false;
  root_resource_ = root;

  BYTE dialog_buffer[1024];
  LPCDLGTEMPLATE compiled_dlg = compileDialogTemplate(menu_resource_id,
                                                      dialog_buffer,
                                                      sizeof(dialog_buffer));

  // Get the parent of this window
  bool returnValue = (S_OK == beginDialog(GetModuleHandle(NULL),
                                          compiled_dlg,
                                          parent_window));
  // Reset the window reference
  window_ = NULL;

  // Return the return code
  return returnValue;
}




//----[  begin  ]--------------------------------------------------------------
bool EvidyonToolDialog::begin(dc::dcGenericResource* root,
                              HWND parent_window,
                              WORD menu_resource_id) {
  if (!owner_) return false;
  root_resource_ = root;

  BYTE dialog_buffer[1024];
  LPCDLGTEMPLATE compiled_dlg = compileDialogTemplate(menu_resource_id,
                                                      dialog_buffer,
                                                      sizeof(dialog_buffer));
  window_ = createDialog(GetModuleHandle(NULL), compiled_dlg, parent_window);
  return window_ != NULL;
}


//----[  end  ]----------------------------------------------------------------
void EvidyonToolDialog::end(INT_PTR result) {
  if (owner_) owner_->dialogClosing(this);
  if (window_) {
    EndDialog(window_, result); // Get rid of the dialog
  }
}



//----[  synchronizeDisplay  ]-------------------------------------------------
void EvidyonToolDialog::synchronizeDisplay() {
  // Get the resources list
  HWND hList = GetDlgItem(window_, EVIDYONTOOLDIALOG_LISTBOXID);

  // Clear the list box
  ListBox_ResetContent(hList);

  // Add all of the children of this resource to the list
  const dc::dcGenericResource::Array& members = root_resource_->getMembers();
  dc::dcGenericResource::Array::const_iterator i, end = members.end();
  for (i = members.begin(); i != end; ++i) {
      EvidyonToolDialogListElement* element = new EvidyonToolDialogListElement(*i, 0);
      ListBox_AddItemData(hList, element);
  }
}






//----[  getWindowHandle  ]----------------------------------------------------
HWND EvidyonToolDialog::getWindowHandle() const {
  return window_;
}


//----[  getTool  ]------------------------------------------------------------
EvidyonTool* EvidyonToolDialog::getTool() const {
  return owner_;
}


//----[  dialogMenuNotification  ]---------------------------------------------
INT_PTR EvidyonToolDialog::dialogMenuNotification(HWND hDlg,
                                                  HMENU hMenu,
                                                  WORD ItemID,
                                                  WORD item_index) {
  if (owner_) owner_->dialogMenuNotification(this, ItemID, item_index);
  dcx::dcxWin32ForceWindowRedraw(hDlg);
  return 0;
}




//----[  dialogNotification  ]-------------------------------------------------
INT_PTR EvidyonToolDialog::dialogNotification(HWND hDlg,
                                              HWND hControl,
                                              UINT_PTR ControlID,
                                              UINT Notification) {
  if (ControlID == EVIDYONTOOLDIALOG_LISTBOXID && Notification == LBN_DBLCLK) {
    // Get the location where the user clicked
    HWND hList = hControl;

    // Find the currently-selected item
    RECT rc;
    int cs = ListBox_GetCurSel(hList);
    EvidyonToolDialogListElement* element =
      (EvidyonToolDialogListElement*)ListBox_GetItemData(hList, cs);
    ListBox_GetItemRect(hList, cs, &rc);

    // Determine if we clicked in the rect
    POINT ptClick = {0, 0};
    GetCursorPos(&ptClick);
    ScreenToClient(hDlg, &ptClick);
    if (PtInRect(&rc, ptClick)) {
      // We did, so see if we need to change the collapse/expand status
      if (element->hasChildren()) {
        // The state is changing!
        if (element->expanded()) {
            recursiveRemoveChildren(element, cs);
        } else {
          std::list<EvidyonToolDialogListElement*> resources =
            element->createChildren();
          for (std::list<EvidyonToolDialogListElement*>::reverse_iterator i =
               resources.rbegin(); i != resources.rend(); ++i) {
            ListBox_InsertItemData(hList, cs+1, *i);
          }
        }

        element->toggleExpanded();
      }
    }

    // We processed this message
    return TRUE;
  }

  // Default return value
  return 0;
}


//----[  dialogEvent  ]--------------------------------------------------------
INT_PTR EvidyonToolDialog::dialogEvent(HWND hDlg,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam) {
  switch(uMsg) {
    case WM_INITDIALOG: return dialogInitDialog(hDlg);
    case WM_CLOSE:      return dialogClose();
    case WM_DELETEITEM: return dialogDeleteItem((LPDELETEITEMSTRUCT)lParam);
    case WM_MEASUREITEM:return dialogMeasureItem((LPMEASUREITEMSTRUCT)lParam);
    case WM_DRAWITEM:   return dialogDrawItem((LPDRAWITEMSTRUCT)lParam);
    case WM_SIZE:       return dialogSize(LOWORD(lParam), HIWORD(lParam));
    case WM_CONTEXTMENU:return dialogContextMenu((HWND)wParam, MAKEPOINTS(lParam));

    // Prevent window flickering
    case WM_ERASEBKGND: return 1;
  }

  // Default return code
  return 0;
}



//----[  dialogInitDialog  ]---------------------------------------------------
INT_PTR EvidyonToolDialog::dialogInitDialog(HWND hDlg) {
  window_ = hDlg;

  if (owner_) owner_->dialogOpening(this);

  // Change the window's title
  std::string title = "Evidyon Tool - [" + root_resource_->getName() + "]";
  SetWindowText(hDlg, title.c_str());

  //HMENU hMenu = GetMenu(hDlg);
  //if (hMenu) {
  //    MENUINFO menuInfo;
  //    menuInfo.cbSize = sizeof(MENUINFO);
  //    menuInfo.fMask = MIM_STYLE;
  //    GetMenuInfo(hMenu, &menuInfo);
  //    menuInfo.dwStyle |= MNS_NOTIFYBYPOS;
  //    SetMenuInfo(hMenu, &menuInfo);
  //}

  // Fill the list-box
  synchronizeDisplay();

  // Default return code
  return 0;
}



//----[  dialogClose  ]--------------------------------------------------------
INT_PTR EvidyonToolDialog::dialogClose() {
  end(0);
  return 0;
}



//----[  dialogMeasureItem  ]--------------------------------------------------
INT_PTR EvidyonToolDialog::dialogMeasureItem(LPMEASUREITEMSTRUCT measureItemData) {
  // Set the item height for entries in the list
  measureItemData->itemHeight = 19;

  // We handled this message
  return TRUE;
}


//----[  dialogDrawItem  ]-----------------------------------------------------
INT_PTR EvidyonToolDialog::dialogDrawItem(LPDRAWITEMSTRUCT drawItemData) {
  if (drawItemData->itemID == -1) return FALSE;

#define BUFFER MAX_PATH 
  TEXTMETRIC tm; 
  int y; 
  HDC hdcMem;

  // Draw the bitmap and text for the list box item. Draw a 
  // rectangle around the bitmap if it is selected. 
  switch(drawItemData->itemAction) {
    case ODA_SELECT: 
    case ODA_DRAWENTIRE: {
      // Display the bitmap associated with the item. 

      EvidyonToolDialogListElement* element =(EvidyonToolDialogListElement*)SendMessage(drawItemData->hwndItem, 
          LB_GETITEMDATA, drawItemData->itemID, (LPARAM) 0); 

      hdcMem = CreateCompatibleDC(drawItemData->hDC); 
      HPEN colorPen = CreatePen(PS_SOLID, 1, RGB(236,233,216));
      HPEN whitePen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
      HPEN blackPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
      HBRUSH colorBrush = CreateSolidBrush(RGB(236,233,216));
      HBRUSH whiteBrush = CreateSolidBrush(RGB(255,255,255));
      HBRUSH blackBrush = CreateSolidBrush(RGB(0,0,0));

      bool hasChildren = element->hasChildren();
      int indentation = element->indentation();

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
      HFONT boldFont = CreateFont(8,
                                  0,
                                  0,
                                  0,
                                  bold? FW_BOLD : FW_NORMAL,
                                  italic,
                                  underlined,
                                  false,
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
      rcText.left += element->indentation() * 12 + 12 + 10;
      std::string text = element->name();

      bool drawDesc = false;
      RECT rcDesc;
      if (!hasChildren) {
        rcDesc = drawItemData->rcItem;

        RECT drawRect = rcText;
        DrawText(drawItemData->hDC,
                 text.c_str(),
                 text.length(),
                 &drawRect,
                 DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS|DT_LEFT|DT_CALCRECT);
        rcText.right = drawRect.right;

        //rcText.right = rcText.left + text.length() * tm.tmMaxCharWidth;
        if (rcText.right > drawItemData->rcItem.right) {
            rcText.right = drawItemData->rcItem.right;
        } else {
          drawDesc = true;
          rcDesc.left = rcText.right;
        }
      }

      SetTextColor(drawItemData->hDC, RGB(0, 0, 0));
      DrawText(drawItemData->hDC, text.c_str(), text.length(), &rcText, DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS|DT_LEFT);

      if (drawDesc) {
        std::string desc = element->toString();
        // DT_RTLREADING - causes the elipsis to be put on the left
        // DT_RIGHT - align right
        // DT_WORD_ELLIPSIS - cut off elipsis!
        SetTextColor(drawItemData->hDC, RGB(106, 103, 86));
        HFONT descFont = CreateFont(14,
                                    0,
                                    0,
                                    0,
                                    FW_NORMAL,
                                    false,
                                    false,
                                    false,
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
      if (hasChildren) {
        int left = drawItemData->rcItem.left + indentation * 12 + 5;
        int top = drawItemData->rcItem.top + 4;

        Rectangle(drawItemData->hDC, left, top, left + 9, top + 9);
        MoveToEx(drawItemData->hDC, left + 2, top + 4, NULL);
        LineTo(drawItemData->hDC, left + 7, top + 4);

        // Draw the plus unless the element is expanded
        if (!element->expanded()) {
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

      if (drawItemData->itemState & ODS_SELECTED)  {
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



//----[  dialogDeleteItem  ]---------------------------------------------------
INT_PTR EvidyonToolDialog::dialogDeleteItem(LPDELETEITEMSTRUCT deleteItemData) {
  delete ((EvidyonToolDialogListElement*)deleteItemData->itemData);
  return TRUE;
}



//----[  dialogContextMenu  ]--------------------------------------------------
INT_PTR EvidyonToolDialog::dialogContextMenu(HWND hWindow, POINTS point) {
  // Get the click locations
  POINT ptClick = { point.x, point.y };
  POINT ptClientClick = ptClick;
  ScreenToClient(window_, &ptClientClick);

  // Calculate the selected item
  HWND hList = GetDlgItem(window_, EVIDYONTOOLDIALOG_LISTBOXID);
  int item = ListBox_GetTopIndex(hList) + ptClientClick.y / 16; // why isn't this 19?  we set item height to 19!!
  ListBox_SetCurSel(hList, item);

  // Get the element from the list
  EvidyonToolDialogListElement* element = (EvidyonToolDialogListElement*)ListBox_GetItemData(hList, item);

  // Check to make sure that something is in the list
  if (item < 0 || !element || element == (EvidyonToolDialogListElement*)0xFFFFFFFF) return TRUE;

  // User clicked in the rect, so we need to collapse this item before
  // we can show the popup menu (that way if the children change, we
  // dont have to go through the pain of updating everything
  if (element->hasChildren() && element->expanded()) {
    // Get rid of all of the children and collapse this element
    recursiveRemoveChildren(element, item);
    element->toggleExpanded();

    // Sync the list
    UpdateWindow(hList);
  }


  // Generate the popup menu
  HMENU hMenu = CreatePopupMenu();

  // Get actions from this item
  dc::dcAction* actions = element->getActions();
  int item_index = 0;
#define MENU_ITEM_OFFSET 905

  // Go through all of the items in the null-terminated list and add the actions to the menu
  while (actions[item_index].name != NULL) {
    MENUITEMINFO element;

    // Build the element for this function
    if (actions[item_index].function != NULL) {
      dcx::dcxWin32InitMenuItem(&element,
                                actions[item_index].name,
                                item_index == 0,
                                item_index + MENU_ITEM_OFFSET);
    } else {
      dcx::dcxWin32InitMenuItemSeperator(&element);
    }

    // Increment the item index
    item_index++;

    // Add this item to the end of the list
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &element);
  }

  // Add generic actions: save or load this element (and all children) to a
  // file.  This allows sections of a project to be easily imported and
  // exported.
  static const int MENU_ITEM_EXPORT = 900;
  static const int MENU_ITEM_IMPORT = 901;
  static const int MENU_ITEM_FIND_INVALID_REFERENCES = 902;
  static const int MENU_ITEM_REPLACE_IN_REFERENCE_PATHS = 903;
  {
    MENUITEMINFO element;
    if (item_index != 0) {
      dcx::dcxWin32InitMenuItemSeperator(&element);
      InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &element);
    }
    dcx::dcxWin32InitMenuItem(&element,
                              "Export...",
                              false,
                              MENU_ITEM_EXPORT);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &element);
    dcx::dcxWin32InitMenuItem(&element,
                              "Import...",
                              false,
                              MENU_ITEM_IMPORT);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &element);
    dcx::dcxWin32InitMenuItem(&element,
                              "Find Invalid References",
                              false,
                              MENU_ITEM_FIND_INVALID_REFERENCES);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &element);
    dcx::dcxWin32InitMenuItem(&element,
                              "Replace in Reference Paths",
                              false,
                              MENU_ITEM_REPLACE_IN_REFERENCE_PATHS);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &element);
  }

  // Execute the menu
  int selected = (int)TrackPopupMenu(hMenu,
                                     TPM_RETURNCMD,
                                     ptClick.x,
                                     ptClick.y,
                                     0,
                                     hWindow,
                                     NULL);

  // Check to see whether or not the user picked an item
  if (selected >= MENU_ITEM_OFFSET) {
    // Get the function pointer from the array
    dc::dcActionFunction function = actions[selected-MENU_ITEM_OFFSET].function;

    // Perform this function
    if (element->performAction(function,
                               static_cast<void*>(this))) {
      // If this action did something, the project changed
      if (owner_) owner_->projectChanged();
    }
  } else {
    switch (selected) {
      case MENU_ITEM_EXPORT:
        owner_->exportResourceToFile(element->getResource());
        break;
      case MENU_ITEM_IMPORT:
        owner_->importResourceFromFile(element->getResource());
        break;
      case MENU_ITEM_FIND_INVALID_REFERENCES:
        owner_->findInvalidReferences(element->getResource());
        break;
      case MENU_ITEM_REPLACE_IN_REFERENCE_PATHS:
        owner_->replaceInReferencePaths(element->getResource());
        break;
    }
  }

  // Erase the menu
  DestroyMenu(hMenu);

  // Redraw our list
  dcx::dcxWin32ForceWindowRedraw(hList);

  // Successfully processed the message
  return TRUE;
}



//----[  dialogSize  ]---------------------------------------------------------
INT_PTR EvidyonToolDialog::dialogSize(WORD width, WORD height) {
  HWND hResources = GetDlgItem(window_, EVIDYONTOOLDIALOG_LISTBOXID);
  SetWindowPos(hResources, 0, 0, 0, width, height, SWP_SHOWWINDOW|SWP_NOMOVE);
  dcx::dcxWin32ForceWindowRedraw(hResources);
  return TRUE;
}





//----[  recursiveRemoveChildren  ]--------------------------------------------
void EvidyonToolDialog::recursiveRemoveChildren(EvidyonToolDialogListElement* element,
                                                int BaseIndex) {
  HWND hList = GetDlgItem(window_, EVIDYONTOOLDIALOG_LISTBOXID);
  if (element->expanded()) {
    size_t children = element->numberOfChildren();
    for (size_t i = 0; i < children; ++i) {
      size_t childIndex = BaseIndex + 1; // the '+ i' is implied because we delete items
      EvidyonToolDialogListElement* child =
        (EvidyonToolDialogListElement*)ListBox_GetItemData(hList, childIndex);
      if (child->isChildOf(element) == false) break; // out of children
      recursiveRemoveChildren(child, childIndex);
      ListBox_DeleteString(hList, childIndex);
    }
  }
}




//----[  compileDialogTemplate  ]----------------------------------------------
LPCDLGTEMPLATE EvidyonToolDialog::compileDialogTemplate(WORD menu_resource_id,
                                                        BYTE* buffer,
                                                        size_t buffer_size) {

  // Use the wonderful DCX library to create a dialog from scratch using code.
  // This dialog simply contains a list and a 
  dcx::dcxWin32DialogTemplate dialog_template;
  dialog_template.setSizeInDialogUnits( 469, 469 );
  dialog_template.setTitle("Evidyon Tool");
  dialog_template.setClass((WORD)0);
  dialog_template.setStyleAsResizeableAppWindow();
  dialog_template.addListBoxInDialogUnits(EVIDYONTOOLDIALOG_LISTBOXID,
                                          LBS_OWNERDRAWFIXED  // custom draw
                                            | LBS_NOINTEGRALHEIGHT
                                            | LBS_DISABLENOSCROLL
                                            | WS_VSCROLL
                                            | WS_TABSTOP
                                            | LBS_NOTIFY,
                                          0, 0,                // x, y
                                          100, 100);           // width, height
  dialog_template.setMenu(menu_resource_id);

  // Compile the dialog
  return dialog_template.compile(buffer, buffer_size);
}



}
}