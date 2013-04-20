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
#ifndef __DCXD3D_H__
#define __DCXD3D_H__

namespace dcx {

    /**
     * This function is intended for use when one wants to use an existing Direct3D device on a
     * new window.  This function generates the parameters automatically (using default values)
     * and creates a new swap chain on the device.  After this method, the swap chain must be
     * activated as the render target by dcxD3DSetTargetSwapChain.
     * @note The source device must have a rendering area at least as large as that of this
     *       device for the rendering to show up correctly; otherwise, rendering will occur
     *       only in a section of the window.
     * @param d3dDevice The source device
     * @param targetWindow The window the swap chain should render into
     * @param newSwapChain Destination for the new rendering interface pointer
     * @param d3dppOut Direct3D device parameter output; optional
     * @param setAsTarget Whether to immediately set the device to render into the created chain
     */
    bool dcxD3DDuplicateDeviceForWindow( LPDIRECT3DDEVICE9 d3dDevice, HWND targetWindow,
                                         LPDIRECT3DSWAPCHAIN9* newSwapChain,
                                         D3DPRESENT_PARAMETERS* d3dppOut = 0 );

    /**
     * Sets up the Direct3D device associated with the provided swap chain to render into
     * that chain's window.  This must be done outside of BeginScene/EndScene pairs.  This
     * method automatically creates and sets a new perspective matrix that is valid for the
     * swap chain.
     * @param swapChain The target swap chain for rendering
     * @param zNear The view frustum's near z-value
     * @param zFar The view frustum's far z-value
     * @param fov The y-axis (horizontal) field of view for the perspective matrix
     * @return Whether or not the method succeeded
     */
    bool dcxD3DSetTargetSwapChain( LPDIRECT3DSWAPCHAIN9 swapChain,
                                   float zNear, float zFar,
                                   float fov = D3DX_PI / 4.0f );


}


#endif