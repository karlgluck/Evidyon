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
#include "colorkey.h"
#include "d3dximage.h"


// Resolve the static implementation declaration
dcClass<D3DXImage>::Implements<Image> implementsImage;





//------------------------------------------------------------------------------------------------
// Name:  D3DXImage
// Desc:  Initializes this resource with the given name
//------------------------------------------------------------------------------------------------
D3DXImage::D3DXImage()
{
    member("Source File", &mySourceFile);
    member("Color Key", &myColorKey);
    member("Horizontal Sub-Images", &mySubImageColumns);
    member("Vertical Sub-Images", &mySubImageRows);
    member("Selected Sub-Image", &mySubImage);

    // Set the default values for sub-images (single division)
    mySubImageColumns.setValue(1);
    mySubImageRows.setValue(1);

    // Select the first sub-image
    mySubImage.setValue(0);
}




//------------------------------------------------------------------------------------------------
// Name:  obtainD3DTexture
// Desc:  Loads the texture denoted by this image from the file system
//------------------------------------------------------------------------------------------------
bool D3DXImage::obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice, DWORD options, LPDIRECT3DTEXTURE9* ppd3dTexture) const
{
    // Fail without a device or if something is wrong with the output pointer
    if (APP_WARNING(!pd3dDevice || !ppd3dTexture)("Invalid parameter to obtainD3DTexture")) return false;

    // Obtain options flags
    bool managed = (options & D3DTEX_MANAGED) != 0;
    bool mipmap = (options & D3DTEX_MIPMAP) != 0;
    bool systemmem = (options & D3DTEX_SYSTEMMEM) != 0;
    D3DPOOL pool = systemmem ? D3DPOOL_SYSTEMMEM : (managed ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT);

    // Holds return codes
    HRESULT hr;

    // Save a NULL first (just in case something goes wrong)
    *ppd3dTexture = NULL;

    // Determine the color key to use
    D3DCOLOR d3dColorKey = myColorKey.isEnabled() ? myColorKey.getD3DColor() : 0;

    // The destination for the texture interface
    LPDIRECT3DTEXTURE9 pd3dTexture;

    // Load information about the image
    D3DXIMAGE_INFO imageInfo;
    if (APP_WARNING(hr = D3DXGetImageInfoFromFile(mySourceFile.getValue().c_str(), &imageInfo))
        ("Failed to get image info from D3DXImage source file %s", mySourceFile.getValue().c_str()))
        return false;

    // Get the target image format
    D3DFORMAT targetFormat = imageInfo.Format;

    // Add at least 1 bit of alpha to whatever format we're in
    if (myColorKey.isEnabled())
    {
        switch(imageInfo.Format)
        {
            // These special formats have lower-BPP alpha formats
            case D3DFMT_P8:       targetFormat = D3DFMT_A8P8;     break;
            case D3DFMT_R5G6B5:
            case D3DFMT_X1R5G5B5: targetFormat = D3DFMT_A1R5G5B5; break;
            case D3DFMT_X4R4G4B4: targetFormat = D3DFMT_A4R4G4B4; break;

            // Convert all other formats into standard 32-bit color
            default: targetFormat = D3DFMT_A8R8G8B8; break;
        }
    }

    // Load the texture
    hr = D3DXCreateTextureFromFileEx(pd3dDevice,
                                     mySourceFile.getValue().c_str(),
                                     D3DX_DEFAULT,
                                     D3DX_DEFAULT,
                                     mipmap ? D3DX_DEFAULT : 1,
                                     0,
                                     targetFormat,
                                     pool,
                                     D3DX_DEFAULT,
                                     D3DX_FILTER_POINT,
                                     d3dColorKey,
                                     NULL,
                                     NULL,
                                    &pd3dTexture);

    // If there is a problem, there's not much more we can do
    if (APP_WARNING(FAILED(hr))("Couldn't load D3DXImage source file %s", mySourceFile.getValue().c_str()))
        return false;

    // Check to see if there is a sub-image
    if ((mySubImageRows.getValue() > 1) ||
        (mySubImageColumns.getValue() > 1)) {
      LPDIRECT3DTEXTURE9 segment = NULL;
      if (SegmentImage(pd3dTexture,
                       imageInfo.Width / mySubImageColumns.getValue(),
                       imageInfo.Height / mySubImageRows.getValue(),
                       mySubImageRows.getValue(),
                       mySubImageColumns.getValue(),
                       mySubImage.getValue(),
                      &segment)) {
        *ppd3dTexture = segment;
        SAFE_RELEASE(pd3dTexture);
      } else {
        DEBUG_INFO("Failed to segment image; using entire texture");
        *ppd3dTexture = pd3dTexture;
      }
    } else {
      // Return the complete texture
      *ppd3dTexture = pd3dTexture;
    }

    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  getFileName
// Desc:  Gets the source file name for this image
//------------------------------------------------------------------------------------------------
dcFileName* D3DXImage::getFileName()
{
    return &mySourceFile;
}


//------------------------------------------------------------------------------------------------
// Name:  getColorKey
// Desc:  Gets the color key object this image uses
//------------------------------------------------------------------------------------------------
ColorKey* D3DXImage::getColorKey()
{
    return &myColorKey;
}


//------------------------------------------------------------------------------------------------
// Name:  getSubImageColumns
// Desc:  Gets the integer representing the number of sub-image columns
//------------------------------------------------------------------------------------------------
dcInteger* D3DXImage::getSubImageColumns()
{
    return &mySubImageColumns;
}


//------------------------------------------------------------------------------------------------
// Name:  getSubImageRows
// Desc:  Gets the integer representing the number of sub-image rows
//------------------------------------------------------------------------------------------------
dcInteger* D3DXImage::getSubImageRows()
{
    return &mySubImageRows;
}


//------------------------------------------------------------------------------------------------
// Name:  getSubImage
// Desc:  Gets the integer for which sub-image is selected
//------------------------------------------------------------------------------------------------
dcInteger* D3DXImage::getSubImage()
{
    return &mySubImage;
}


//------------------------------------------------------------------------------------------------
// Name:  setSubImage
// Desc:  Sets up the sub-image for this image
//------------------------------------------------------------------------------------------------
void D3DXImage::setSubImage(int rows, int columns, int selection)
{
    mySubImageColumns.setValue(columns);
    mySubImageRows.setValue(rows);
    mySubImage.setValue(selection);
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//------------------------------------------------------------------------------------------------
std::string D3DXImage::staticTypeName()
{
    return "D3DXImage";
}

