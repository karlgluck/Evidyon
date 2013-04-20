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
//#include "compositeimage.h"
//
//const char* MapColorMask<dc::dcFileName>::ELEMENT_DATA_MEMBER_NAME = "Image";
//const char* MapColorMask<dc::dcFileName>::ELEMENT_LIST_NAME = "Color -> Image";
//
//
//
//CompositeImage::CompositeImage() {
//  member("Source Image Files", &images_to_stack_);
//    member("Horizontal Sub-Images", &subimage_columns_);
//    member("Vertical Sub-Images", &subimage_rows_);
//    member("Selected Sub-Image", &subimage_index_);
//}
//
//bool CompositeImage::obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice,
//                                      DWORD options,
//                                      LPDIRECT3DTEXTURE9* ppd3dTexture) const {
//  MaskColorTypes mask_colors;
//  {
//    dc::dcGenericResource::Array::const_iterator end = images_to_stack_.getElementsList().end();
//    dc::dcGenericResource::Array::const_iterator i = images_to_stack_.getElementsList().begin();
//    for (; i != end; ++i) {
//      MapColorMask<dc::dcFileName>::Element* element = 
//        (MapColorMask<dc::dcFileName>::Element*)(*i);
//      mask_colors.push_back(element->getMaskD3DColor());
//    }
//  }
//
//  unsigned int width, height;
//  int* types_map = NULL;
//  if (false == AcquireMapToTypeIndexMask(pd3dDevice,
//                                         images_to_stack_.getMaskImageFileName().c_str(),
//                                         mask_colors,
//                                        &width,
//                                        &height,
//                                        &types_map)) return false;
//
//  // Create the template texture
//  LPDIRECT3DTEXTURE9 output_texture = NULL;
//  HRESULT hr = pd3dDevice->CreateTexture(width,
//                                         height,
//                                         1,
//                                         D3DUSAGE_WRITEONLY,
//                                         D3DFMT_A8R8G8B8,
//                                         D3DPOOL_DEFAULT,
//                                        &output_texture,
//                                         NULL);
//  CONFIRM(SUCCEEDED(hr)) else {
//    ReleaseMapToTypeIndexMask(types_map);
//    return false;
//  }
//
//  // Combine the layers
//  D3DLOCKED_RECT output_tex_locked;
//  CONFIRM(SUCCEEDED(output_texture->LockRect(0, &output_tex_locked, NULL, 0))) else {
//    ReleaseMapToTypeIndexMask(types_map);
//    return false;
//  }
//  unsigned char* output_tex_bits = (unsigned char*)output_tex_locked.pBits;
//  {
//    // reset the output texture with transparency
//    for (unsigned int y = 0; y < height; ++y) {
//      memset(output_tex_bits + y*output_tex_locked.Pitch,
//             0,
//             output_tex_locked.Pitch);
//    }
//
//    dc::dcGenericResource::Array::const_iterator end = images_to_stack_.getElementsList().end();
//    dc::dcGenericResource::Array::const_iterator i = images_to_stack_.getElementsList().begin();
//    for (int mask_index = 0; i != end; ++i, ++mask_index) {
//      MapColorMask<dc::dcFileName>::Element* element = 
//        (MapColorMask<dc::dcFileName>::Element*)(*i);
//
//      std::string image_file_name = element->getData().getValue();
//
//      LPDIRECT3DTEXTURE9 image_texture = NULL;
//      hr = D3DXCreateTextureFromFileEx(pd3dDevice,
//                                       image_file_name.c_str(),
//                                       width,
//                                       height,
//                                       1,
//                                       0,
//                                       D3DFMT_A8R8G8B8,
//                                       D3DPOOL_DEFAULT,
//                                       D3DX_DEFAULT,
//                                       D3DX_DEFAULT,
//                                       0,
//                                       NULL,
//                                       NULL,
//                                      &image_texture);
//      CONFIRM(SUCCEEDED(hr)) else {
//        ReleaseMapToTypeIndexMask(types_map);
//        return false;
//      }
//
//      D3DLOCKED_RECT image_tex_locked;
//      hr = image_texture->LockRect(0, &image_tex_locked, NULL, 0);
//      CONFIRM(SUCCEEDED(hr)) else {
//        ReleaseMapToTypeIndexMask(types_map);
//        return false;
//      }
//
//      unsigned char* src_tex_bits = (unsigned char*)image_tex_locked.pBits;
//      for (unsigned int y = 0; y < height; ++y) {
//        for (unsigned int x = 0; x < width; ++x) {
//          if (types_map[y*width+x] != mask_index) continue;
//          D3DCOLOR* output_color =
//            (D3DCOLOR*)(output_tex_bits + output_tex_locked.Pitch*y + (sizeof(D3DCOLOR)*x));
//         *output_color =
//           *(D3DCOLOR*)(src_tex_bits + image_tex_locked.Pitch*y + (sizeof(D3DCOLOR)*x));
//        }
//      }
//    }
//  }
//  output_texture->UnlockRect(0);
//  ReleaseMapToTypeIndexMask(types_map);
//
//  // Get the subtexture
//  if ((subimage_rows_.getValue() > 1) ||
//      (subimage_columns_.getValue() > 1)) {
//    LPDIRECT3DTEXTURE9 segment = NULL;
//    if (SegmentImage(output_texture,
//                     subimage_rows_.getValue(),
//                     subimage_columns_.getValue(),
//                     subimage_index_.getValue(),
//                    &segment)) {
//      *ppd3dTexture = segment;
//      output_texture->Release();
//    } else {
//      DEBUG_INFO("Failed to segment image; using entire texture");
//      *ppd3dTexture = output_texture;
//    }
//  } else {
//    // Return the complete texture
//    *ppd3dTexture = output_texture;
//  }
//
//  return true;
//}
//
//std::string CompositeImage::staticTypeName() {
//  return "CompositeImage";
//}

