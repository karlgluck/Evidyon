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
#ifndef __RESOURCEDIALOG_H__
#define __RESOURCEDIALOG_H__
#pragma once


// TODO: MOVE TO DIFFERENT FILE
    class ResourceEntry
    {
        dc::dcGenericResource* myResource;

        public:

            ResourceEntry(dcGenericResource* resource)
            {
                myResource = resource;
            }

            const std::string& name() const
            {
                return myResource->getName();
            }

            bool hasChildren() const
            {
                return numberOfChildren() > 0;
            }

            size_t numberOfChildren() const
            {
                return myResource->getMembers().size();
            }

            std::list<ResourceEntry*> createChildren() const
            {
                std::list<ResourceEntry*> list;

                const dc::dcGenericResource::Array& p = myResource->getMembers();
                for (dcGenericResource::Array::const_iterator i = p.begin(); i != p.end(); ++i)
                {
                        list.push_back(new ResourceEntry(*i));
                }

                return list;
            }

            dc::dcAction* getActions() const
            {
                return myResource->getManager()->getActions();
            }

            int performAction(dcActionFunction function, void* userData)
            {
                return (*function)(myResource, userData);
            }

            std::string toString() const { return myResource->toString(); }
    };

    struct EntryContainer
    {
        int indentation;
        bool expanded;
        ResourceEntry* entry;
    };

/**
 * A dialog that edits a given resource
 */
class EditResourceDialog : public dcxWin32DialogController
{
    public:

        /**
         * Initializes the class
         */
        EditResourceDialog();

        /**
         * Loads, activates and displays this dialog using the provided resource as the root
         * of the dialog tree.  The root resource is not itself displayed in the tree.
         */
        bool execute(dcGenericResource* root);

        /**
         * Forces this dialog (and any of its children) to close
         */
        void end(INT_PTR result);

        /**
         * Clears the list-box and re-adds the root members of the box
         */
        void synchronizeDisplay();

    protected:

        /**
         * If a menu item is selected, this method is invoked
         *   @param hDlg The dialog containing the menu
         *   @param hMenu The menu containing the selected item
         *   @param ItemID The ID of the item that was selected
         *   @param ItemIndex The index of the selected item
         */
        INT_PTR dialogMenuNotification(HWND hDlg, HMENU hMenu, WORD ItemID, WORD ItemIndex);

        /**
         * When something on the dialog is activated, this function is invoked
         *   @param hWnd Dialog handle
         *   @param hControl Control that spawned the notification
         *   @param ControlID ID of the control that created the notification
         *   @param Notification The event that occurred
         *   @return Return code
         */
        INT_PTR dialogNotification(HWND hDlg, HWND hControl, UINT_PTR ControlID, UINT Notification);

        /**
         * All messages received by the dialog are passed through here.  The default
         * implementation of this method will call EndDialog with a value of "0"
         * when the window is closed.
         *   @param hWnd Dialog handle
         *   @param uMsg Message
         *   @param wParam Parameter
         *   @param lParam Parameter
         *   @return Return code
         */
        INT_PTR dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


    protected:

        /**
         * Called for WM_INITDIALOG
         */
        INT_PTR dialogInitDialog(HWND hDlg);

        /**
         * Called for WM_CLOSE
         */
        INT_PTR dialogClose();

        /**
         * Called for WM_MEASUREITEM
         */
        INT_PTR dialogMeasureItem(LPMEASUREITEMSTRUCT measureItemData);

        /**
         * Called for WM_DRAWITEM
         */
        INT_PTR dialogDrawItem(LPDRAWITEMSTRUCT drawItemData);

        /**
         * Called for WM_DELETEITEM
         */
        INT_PTR dialogDeleteItem(LPDELETEITEMSTRUCT deleteItemData);

        /**
         * Called for WM_CONTEXTMENU
         */
        INT_PTR dialogContextMenu(HWND hWindow, POINTS point);

        /**
         * Called for WM_SIZE
         */
        INT_PTR dialogSize(WORD width, WORD height);


    protected:

        /**
         * Removes children of an object from the list in this editor
         */
        void RecursiveRemoveChildren(EntryContainer* entry, int BaseIndex);


    protected:

        /// This dialog's window handle
        HWND myWindow;

        /// The root resource that this dialog displays
        dc::dcGenericResource* myRootResource;
};




#endif