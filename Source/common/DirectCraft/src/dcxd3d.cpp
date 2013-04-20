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
//-----------------------------------------------------------------------------
// File:    dcxd3d.cpp
// Desc:    Utility functions to ease interaction with Direct3D on the Windows platform
//-----------------------------------------------------------------------------
#include <d3dx9.h>
#include "../dcx/d3d"
#include "../dc/debug"


//-----------------------------------------------------------------------------
// Name:  dcxD3DDuplicateDeviceForWindow
// Desc:  Creates a copy of a current Direct3D device on a new window
//-----------------------------------------------------------------------------
bool dcx::dcxD3DDuplicateDeviceForWindow( LPDIRECT3DDEVICE9 d3dDevice, HWND targetWindow,
                                          LPDIRECT3DSWAPCHAIN9* newSwapChain,
                                          D3DPRESENT_PARAMETERS* d3dppOut )
{
    // Check the incoming parameters
    if( APP_ERROR( !d3dDevice || !targetWindow || !newSwapChain )
            ( "Invalid parameter to dcxD3DDuplicateDeviceForWindow" ) )
        return false;

    // Temporary swap chain structure
    IDirect3DSwapChain9* d3dSwapChain;

    // Create parameters
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.hDeviceWindow = targetWindow;
    d3dpp.Windowed = TRUE;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    HRESULT hr = d3dDevice->CreateAdditionalSwapChain( &d3dpp, &d3dSwapChain );

    // Make sure the swap chain could be generated
    if( APP_ERROR( FAILED( hr ) )( "Unable to create new Direct3D device swap chain" ) )
        return false;

    // Save the output variables
    if( d3dppOut )
        memcpy( d3dppOut, &d3dpp, sizeof(D3DPRESENT_PARAMETERS) );
    *newSwapChain = d3dSwapChain;

    // Success
    return true;
}


//-----------------------------------------------------------------------------
// Name:  dcxD3DSetTargetSwapChain
// Desc:  Sets up the Direct3D device associated with the provided swap chain
//-----------------------------------------------------------------------------
bool dcx::dcxD3DSetTargetSwapChain( LPDIRECT3DSWAPCHAIN9 swapChain,
                                    float zNear, float zFar, float fov )
{
    // Check the parameters
    if( APP_ERROR( !swapChain )( "Invalid parameter to dcxD3DSetTargetSwapChain" ) )
        return false;

    // Get associated Direct3D objects
    IDirect3DDevice9* d3dDevice;
    IDirect3DSurface9* d3dBuffer;
    swapChain->GetDevice( &d3dDevice );
    swapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &d3dBuffer );

    // Make sure we have the required objects
    if( APP_ERROR( !d3dDevice || !d3dBuffer )( "Swap chain was unable to provide D3D references for activation" ) )
        return false;

    // Get the suface description of the back-buffer; this is our rendering window
    D3DSURFACE_DESC desc;
    d3dBuffer->GetDesc( &desc );

    // Set the device to render into this window
    d3dDevice->SetRenderTarget( 0, d3dBuffer );
    d3dBuffer->Release();

    // Calculate the aspect ratio for the perspective matrix
    float aspect = desc.Width / (float)desc.Height;

    // Generate the perspective matrix
    D3DXMATRIXA16 proj;
    D3DXMatrixPerspectiveFovLH( &proj, fov, aspect, zNear, zFar );

    // Set the perspective matrix
    d3dDevice->SetTransform( D3DTS_PROJECTION, &proj );

    // Set the viewport
    D3DVIEWPORT9 vpt = { 0, 0, desc.Width, desc.Height, 0.0f, 1.0f };
    APP_WARNING( FAILED( d3dDevice->SetViewport( &vpt ) ) )( "dcxD3DSetTargetSwapChain couldn't set a new viewport!" );

    // Set the scissor rect (just in case)
    RECT scissor = { 0, 0, desc.Width, desc.Height };
    d3dDevice->SetScissorRect( &scissor );

    // Check the current stencil surface
    LPDIRECT3DSURFACE9 stencilSurface;
    bool recreateStencil = true;
    if( !APP_WARNING( d3dDevice->GetDepthStencilSurface( &stencilSurface ) )( "dcxD3DSetTargetSwapChain couldn't get stencil surface" ) )
    {
        // Obtain the stencil surface description
        D3DSURFACE_DESC stencilDesc;
        stencilSurface->GetDesc( &stencilDesc );

        // If the stencil is already the right size, we don't have to make a new one
        if( stencilDesc.Width == desc.Width && stencilDesc.Height == desc.Height )
            recreateStencil = false;

        // Get rid of the stencil
        stencilSurface->Release();
        stencilSurface = NULL;
    }

    // Create a new depth stencil surface, if necesary (to make sure that the entire region is rendered)
    if( recreateStencil )
    {
        if( !APP_WARNING( FAILED( d3dDevice->CreateDepthStencilSurface(
                desc.Width, desc.Height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &stencilSurface, NULL ) ) )
            ( "dcxD3DSetTargetSwapChain was unable to create a new depth stencil surface; the swap chain may not display properly" ) )
        {
            // The surface was created, so set it
            d3dDevice->SetDepthStencilSurface( stencilSurface );

            // Get rid of our reference to the depth stencil
            stencilSurface->Release();
            stencilSurface = NULL;
        }
    }

    // Release our device reference
    d3dDevice->Release();

    // Success
    return true;
}
