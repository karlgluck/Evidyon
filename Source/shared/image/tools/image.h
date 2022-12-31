//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#ifndef __IMAGE_H__
#define __IMAGE_H__
#pragma once


#include <d3d9.h>
#include <dc/boolean>

namespace Evidyon {
namespace Image {
namespace Tools {


  
//----[  Image  ]--------------------------------------------------------------
class Image : public dc::dcGenericResource {
public:
  static std::string staticTypeName() { return "Image"; }

public:
  Image();
  bool loadD3DTexture(LPDIRECT3DDEVICE9 d3d_device, bool force_reload);
  void unloadD3DTexture();
  LPDIRECT3DTEXTURE9 getD3DTexture();

  // Sorts the textures by ascending referenced image index, blend type,
  // and subclass type.
  int compareTo(const dc::dcGenericResource* other) const;

private:
  virtual bool obtainD3DTexture(LPDIRECT3DDEVICE9 d3d_device,
                                LPDIRECT3DTEXTURE9* d3d_texture) const = 0;

private:
  LPDIRECT3DTEXTURE9 loaded_d3d_texture_;

  // Grouping terrain textures together will improve rendering speed since
  // there will be fewer vertex-buffer switches.  Terrain textures are sorted
  // to the end of the list.  Wall textures are sorted to the end of the list,
  // but before terrain textures.
  dc::dcBoolean wall_texture_;
  dc::dcBoolean terrain_texture_;
};

}
}
}

#include <dc/table>

namespace Evidyon {
namespace Tools {
typedef dc::dcTable<Evidyon::Image::Tools::Image>::Element ImageElement;
typedef dc::dcTable<Evidyon::Image::Tools::Image>::Reference ImageReference;
}
}


#endif