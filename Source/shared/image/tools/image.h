//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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