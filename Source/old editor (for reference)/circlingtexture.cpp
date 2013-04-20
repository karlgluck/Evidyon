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
#include "circlingtexture.h"


/// Resolve this class's implementation linker
dcClass<CirclingTexture>::Implements<Texture> implementsTexture;


//------------------------------------------------------------------------------------------------
// Name:  CirclingTexture
// Desc:  Initializes this resource with the given name
//------------------------------------------------------------------------------------------------
CirclingTexture::CirclingTexture()
{
    member("Speed", &myCirclingSpeed);
    member("Magnitude", &myCirclingMagnitude);

    // Set up default values
    myCirclingSpeed.setValue(0.5f);
    myCirclingMagnitude.setValue(0.5f);
}



//------------------------------------------------------------------------------------------------
// Name:  configureTextureStageState
// Desc:  Sets up the texture stage indicated on the provided device for rendering
//------------------------------------------------------------------------------------------------
HRESULT CirclingTexture::configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage)
{
    // Create a nontransforming matrix
    D3DXMATRIXA16 mat;
    D3DXMatrixIdentity(&mat);

    // Make the matrix circle
    mat._31 = myCirclingMagnitude.getValue() * cosf(GetTickCount() / 1000.0f * D3DX_PI * 2 * myCirclingSpeed.getValue());
    mat._32 = myCirclingMagnitude.getValue() * sinf(GetTickCount() / 1000.0f * D3DX_PI * 2 * myCirclingSpeed.getValue());

    // Update the base state
    configureBaseState(pd3dDevice, stage, &mat);

    // Unclamp texture addresses for this stuff
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    pd3dDevice->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    pd3dDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    // Set the matrix
    pd3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), &mat);

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string CirclingTexture::staticTypeName()
{
    return "Circling Texture";
}
