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
#include "mapcolormask.h"
#include "../common/safemacros.h"


bool GetMapMaskInfo(const char* file_name,
                    unsigned int* width,
                    unsigned int* height) {
  D3DXIMAGE_INFO maskInfo;
  CONFIRM(SUCCEEDED(D3DXGetImageInfoFromFile(file_name, &maskInfo))) else return false;
  *width = maskInfo.Width;
  *height = maskInfo.Height;
  return true;
}

bool LoadMapMaskData(LPDIRECT3DDEVICE9 d3d_device,
                     const char* file_name,
                     unsigned int width,
                     unsigned int height,
                     D3DCOLOR* data_buffer) {
  LPDIRECT3DTEXTURE9 texture = NULL;
  HRESULT hr = D3DXCreateTextureFromFileEx(d3d_device,
                                           file_name,
                                           D3DX_DEFAULT, // width
                                           D3DX_DEFAULT, // height
                                           1, // mip levels
                                           0, // usage
                                           D3DFMT_X8R8G8B8, // full format
                                           D3DPOOL_SCRATCH,
                                           D3DX_FILTER_NONE,
                                           D3DX_FILTER_NONE,
                                           0, // no color key
                                           NULL,
                                           NULL,
                                          &texture);
  CONFIRM(SUCCEEDED(hr)) else return false;
  D3DLOCKED_RECT lr;
  CONFIRM(SUCCEEDED(texture->LockRect(0, &lr, NULL, D3DLOCK_READONLY))) else {
    SAFE_RELEASE(texture);
    return false;
  }

  // Copy data directly into the data buffer
  ptrdiff_t pitch_in_d3dcolors = lr.Pitch / sizeof(D3DCOLOR);
  for (unsigned int y = 0; y < height; ++y) {
    D3DCOLOR* src_bits = ((D3DCOLOR*)lr.pBits) + y * pitch_in_d3dcolors;
    for (unsigned int x = 0; x < width; ++x) {
      D3DCOLOR mask_color = *(src_bits + x);
      *data_buffer = mask_color;
      ++data_buffer;
    }
  }

  texture->UnlockRect(0);
  SAFE_RELEASE(texture);
  return true;
}


void ReleaseMapMask(D3DCOLOR* data_buffer) {
  SAFE_DELETE_ARRAY(data_buffer);
}

bool AcquireMapMask(LPDIRECT3DDEVICE9 d3d_device,
                    const char* file_name,
                    unsigned int* width_out,
                    unsigned int* height_out,
                    D3DCOLOR** data_buffer) {
  unsigned int width, height;
  CONFIRM(GetMapMaskInfo(file_name,
                        &width,
                        &height)) else return false;

  D3DCOLOR* buffer = new D3DCOLOR[width*height];
  ASSERT(buffer != NULL) else return false;

  CONFIRM(LoadMapMaskData(d3d_device,
                          file_name,
                          width,
                          height,
                          buffer)) else {
    ReleaseMapMask(buffer);
    return false;
  }

  *width_out = width;
  *height_out = height;
  *data_buffer = buffer;
  return true;
}





bool AcquireMapToTypeIndexMask(LPDIRECT3DDEVICE9 d3d_device,
                               const char* file_name,
                               const MaskColorTypes& mask_color_types,
                               unsigned int* width_out,
                               unsigned int* height_out,
                               int** map_out) {
  unsigned int width, height;
  D3DCOLOR* buffer = NULL;
  CONFIRM(AcquireMapMask(d3d_device,
                         file_name,
                        &width,
                        &height,
                        &buffer)) else return false;
  int* map = new int[width*height];
  ASSERT(map) else {
    ReleaseMapMask(buffer);
    return false;
  }

  // Reset to all -1
  memset(map, -1, sizeof(int) * width * height);

  // Map each entry
  int type_index = 0;
  for (MaskColorTypes::const_iterator i = mask_color_types.begin();
       i != mask_color_types.end(); ++i, ++type_index) {
    D3DCOLOR color = 0x00FFFFFF & (*i); // pull out the RGB components (ignore alpha)
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        if (buffer[y*width+x] != color) continue;
        CONFIRM(map[y*width+x] == -1); // safety check
        map[y*width+x] = type_index;
      }
    }
  }

  ReleaseMapMask(buffer);

  *map_out = map;
  *width_out = width;
  *height_out = height;

  return true;
}



void ReleaseMapToTypeIndexMask(int* indexed_map) {
  SAFE_DELETE_ARRAY(indexed_map);
}