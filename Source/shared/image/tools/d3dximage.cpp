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
#include "d3dximage.h"
#include "shared/image/imageutils.h"
#include "common/safemacros.h"


namespace Evidyon {
namespace Image {
namespace Tools {


dc::dcClass<D3DXImage>::Implements<Image> implementsImage;





//----[  D3DXImage  ]----------------------------------------------------------
D3DXImage::D3DXImage() {
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








//----[  obtainD3DTexture  ]---------------------------------------------------
bool D3DXImage::obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice,
                                 LPDIRECT3DTEXTURE9* ppd3dTexture) const {
  
#define D3DTEX_MANAGED      0x01 /* create in managed pool */
#define D3DTEX_MIPMAP       0x02 /* generate a mipmap chain */
#define D3DTEX_SYSTEMMEM    0x04 /* create in system memory; overrides D3DTEX_MANAGED */

  DWORD options = D3DTEX_MIPMAP|D3DTEX_MANAGED;


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
  //D3DFORMAT targetFormat = imageInfo.Format;

  //// Add at least 1 bit of alpha to whatever format we're in
  //if (myColorKey.isEnabled())
  //{
  //    switch(imageInfo.Format)
  //    {
  //        // These special formats have lower-BPP alpha formats
  //        case D3DFMT_P8:       targetFormat = D3DFMT_A8P8;     break;
  //        case D3DFMT_R5G6B5:
  //        case D3DFMT_X1R5G5B5: targetFormat = D3DFMT_A1R5G5B5; break;
  //        case D3DFMT_X4R4G4B4: targetFormat = D3DFMT_A4R4G4B4; break;

  //        // Convert all other formats into standard 32-bit color
  //        default: targetFormat = D3DFMT_A8R8G8B8; break;
  //    }
  //}

  // Always load in 32-bit with 8 bit alpha
  D3DFORMAT targetFormat = D3DFMT_A8R8G8B8;

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
      SAFE_RELEASE(pd3dTexture);
      pd3dTexture = segment;
    } else {
      DEBUG_INFO("Failed to segment image; using entire texture");
    }
  }

  //// rescale the RGB color channels to be in [0, 128]
  //DWORD number_of_levels = pd3dTexture->GetLevelCount();
  //for (DWORD level = 0; level < number_of_levels; ++level) {
  //  D3DLOCKED_RECT lr;
  //  pd3dTexture->LockRect(level, &lr, NULL, 0);
  //  D3DSURFACE_DESC level_desc;
  //  pd3dTexture->GetLevelDesc(level, &level_desc);
  //  D3DCOLOR* src_bits = (D3DCOLOR*)lr.pBits;
  //  DWORD added_pitch = lr.Pitch / sizeof(D3DCOLOR) - level_desc.Width;
  //  for (int y = 0; y < level_desc.Height; ++y) {
  //    for (int x = 0; x < level_desc.Width; ++x) {
  //      D3DCOLOR c = *src_bits;
  //      c = D3DCOLOR_ARGB((c>>24)&0xFF,
  //                        ((c>>16)&0xFF)>>1,
  //                        ((c>> 8)&0xFF)>>1,
  //                        ((c>> 0)&0xFF)>>1);
  //      *src_bits = c;
  //      ++src_bits;
  //    }
  //    src_bits += added_pitch;
  //  }
  //  pd3dTexture->UnlockRect(level);
  //}

  *ppd3dTexture = pd3dTexture;

  return true;
}




//----[  getFileName  ]--------------------------------------------------------
dc::dcFileName* D3DXImage::getFileName() {
    return &mySourceFile;
}



//----[  getColorKey  ]--------------------------------------------------------
ColorKey* D3DXImage::getColorKey() {
  return &myColorKey;
}




//----[  getSubImageColumns  ]-------------------------------------------------
dc::dcInteger* D3DXImage::getSubImageColumns() {
  return &mySubImageColumns;
}



//----[  getSubImageRows  ]----------------------------------------------------
dc::dcInteger* D3DXImage::getSubImageRows() { 
    return &mySubImageRows;
}



//----[  getSubImage  ]--------------------------------------------------------
dc::dcInteger* D3DXImage::getSubImage() {
    return &mySubImage;
}



//----[  setSubImage  ]--------------------------------------------------------
void D3DXImage::setSubImage(int rows, int columns, int selection) {
  mySubImageColumns.setValue(columns);
  mySubImageRows.setValue(rows);
  mySubImage.setValue(selection);
}



}
}
}