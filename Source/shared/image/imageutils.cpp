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
#include "imageutils.h"
#include <dc/debug>
#include "common/safemacros.h"
#include <d3dx9.h>


namespace Evidyon {
namespace Image {



  
//----[  SegmentImage  ]-------------------------------------------------------
bool SegmentImage(LPDIRECT3DTEXTURE9 texture,
                  int subimage_width,
                  int subimage_height,
                  int rows,
                  int columns,
                  int selected_index,
                  LPDIRECT3DTEXTURE9* segment) {

  D3DSURFACE_DESC desc;
  HRESULT hr = texture->GetLevelDesc(0, &desc);
  CONFIRM(SUCCEEDED(hr)) else return false;


  // Locate the source rectangle
  UINT sii = selected_index; // The sub-image index (sii)
  UINT sixc = sii % columns; // sub-image x-coordinate
  UINT siyc = sii / columns; // sub-image y-coordinate
  RECT sourceRect = { (sixc+0) * subimage_width, (siyc+0) * subimage_height,
                      (sixc+1) * subimage_width, (siyc+1) * subimage_height };


  // Create a texture for the sub-image
  LPDIRECT3DTEXTURE9 subimageTexture = NULL;
  {
    LPDIRECT3DDEVICE9 d3d_device;
    texture->GetDevice(&d3d_device);
    hr = D3DXCreateTexture(d3d_device,
                           subimage_width,
                           subimage_height,
                          -1,
                           desc.Usage,
                           desc.Format,
                           desc.Pool,
                          &subimageTexture);
    SAFE_RELEASE(d3d_device);

    // If we can't make this texture, something is wrong, but we can't do much about it
    if (APP_WARNING(FAILED(hr))("Unable to create sub-image texture")) {
      return false;
    }
  }

  DWORD format_byte_size = 4;
  switch (desc.Format) {
    default: DEBUG_ERROR("Unknown image format"); return false;
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8: format_byte_size = sizeof(D3DCOLOR); break;
    case D3DFMT_A8P8:
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5: 
    case D3DFMT_X4R4G4B4: format_byte_size = sizeof(WORD); break;
  }

  // Copy the texture
  D3DLOCKED_RECT sourceLR, destLR;
  DWORD number_of_levels = subimageTexture->GetLevelCount();
  for (DWORD level = 0; level < number_of_levels; ++level) {
    RECT level_source_rect = sourceRect;
    level_source_rect.left >>= level;
    level_source_rect.top >>= level;
    level_source_rect.right >>= level;
    level_source_rect.bottom >>= level;
    texture->LockRect(level, &sourceLR, &level_source_rect, D3DLOCK_READONLY);
    subimageTexture->LockRect(level, &destLR, NULL, 0);
    BYTE* sourceBits = (BYTE*)sourceLR.pBits;
    BYTE* destBits = (BYTE*)destLR.pBits;

    // Copy each row
    DWORD subimage_level_height = subimage_height>>level;
    DWORD subimage_level_width_bytes  = (subimage_width>>level) * format_byte_size;
    for (long y = 0; y < subimage_level_height; ++y) {
      memcpy(&destBits[destLR.Pitch*y],
             &sourceBits[sourceLR.Pitch*y],
             subimage_level_width_bytes);
    }

    // Unlock the source and destination
    texture->UnlockRect(level);
    subimageTexture->UnlockRect(level);
  }

  // Generate mipmaps, if necessary
  // TODO: this generates an error...why?!
  //if (mipmap)
  //    subimageTexture->GenerateMipSubLevels();

  // Save the sub-image as the output texture
  *segment = subimageTexture;
  return true;
}

}
}