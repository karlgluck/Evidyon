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
#include "image.h"
#include "texture.h"
#include "animatedtexture.h"


/// Resolve this class's implementation linker
dcClass<AnimatedTexture>::Implements<Texture> implementsTexture;



//------------------------------------------------------------------------------------------------
// Name:  AnimatedTexture
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
AnimatedTexture::AnimatedTexture()
{
    member("Horizontal Frame Divisions", &myFramesWide);
    member("Vertical Frame Divisions", &myFramesHigh);
    member("Speed", &myAnimationSpeed);
}



//------------------------------------------------------------------------------------------------
// Name:  configureTextureStageState
// Desc:  Sets up the texture stage indicated on the provided device for rendering
//------------------------------------------------------------------------------------------------
HRESULT AnimatedTexture::configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage)
{
    // Figure out the number of frames
    int framesWide = myFramesWide.getValue();
    int framesHigh = myFramesHigh.getValue();
    int totalFrames = framesWide * framesHigh;

    // Calculate the current frame
    int frame = ((int)floorf((GetTickCount() / 1000.0f) * myAnimationSpeed.getValue())) % totalFrames;

    // Generate the projection matrix to put this frame in the [0,0] [1,1] box.  This requires
    // a scaling and translation.
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);
    world._11 = 1.0f / framesWide;
    world._22 = 1.0f / framesHigh;
    world._31 = -1.0f / framesWide * (frame % framesWide);
    world._32 = -1.0f / framesHigh * ((int)(frame / framesWide));

    // Update the base state
    configureBaseState(pd3dDevice, stage, &world);

    // Unclamp texture addresses for this stuff
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    pd3dDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    // Set the matrix
    pd3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), &world);

    // Success
    return S_OK;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string AnimatedTexture::staticTypeName()
{
    return "Animated Texture";
}
