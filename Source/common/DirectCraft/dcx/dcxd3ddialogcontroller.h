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
#ifndef __DCXD3DDIALOGCONTROLLER_H__
#define __DCXD3DDIALOGCONTROLLER_H__


namespace dcx {

    /**
     * A class that derives from this one is passed information from a standard dialog-
     * processing routine.  As an extension to dcxWin32DialogController, there are additional
     * built-in methods to help manage a Direct3D application window.
     */
    class dcxD3DDialogController : public dcxWin32DialogController
    {
        public:

            /**
             * Initializes the class
             */
            dcxD3DDialogController();


        protected:

            /**
             * Creates a new dialog that uses a new swap-chain of a preexisting Direct3D device
             */
            bool createD3DDialog( HINSTANCE hInstance, HWND hParent, WORD menuResourceID, const char* caption,
                                  int width, int height, LPDIRECT3DDEVICE9 sourceD3DDevice );

            /**
             * Erases allocations made by this class
             */
            void releaseObjects();

            /**
             * Runs the Windows message pump.  This class returns 'true' as long as the dialog
             * exists; it returns 'false' when the dialog closes.
             */
            bool messagePump();

            /**
             * Puts whatever rendering has occurred onto the main screen
             */
            bool present();


        protected:

            /// The window that the dialog uses
            HWND myMainWindow;

            /// The device that is associated with rendering the window
            LPDIRECT3DDEVICE9 myD3DDevice;

            /// The swap chain created specifically for this window
            LPDIRECT3DSWAPCHAIN9 myD3DSwapChain;
    };

}



#endif