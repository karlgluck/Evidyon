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
#ifndef __D3DXIMAGE_H__
#define __D3DXIMAGE_H__
#pragma once


#include "image.h"
#include <dc/filename>
#include "colorkey.h"
#include <dc/integer>


namespace Evidyon {
namespace Image {
namespace Tools {


//----[  D3DXImage  ]----------------------------------------------------------
class D3DXImage : public dc::dcResource<D3DXImage,Image> {
public:
  static std::string staticTypeName() { return "D3DXImage"; }
  static dc::dcClass<D3DXImage>::Implements<Image> implementsImage;

public:
  D3DXImage();
  bool obtainD3DTexture(LPDIRECT3DDEVICE9 d3d_device,
                        LPDIRECT3DTEXTURE9* d3d_texture) const;

  dc::dcFileName* getFileName();
  ColorKey* getColorKey();
  dc::dcInteger* getSubImageColumns();
  dc::dcInteger* getSubImageRows();
  dc::dcInteger* getSubImage();
  void setSubImage(int rows, int columns, int selection);

private:
  dc::dcFileName mySourceFile;
  ColorKey myColorKey;
  dc::dcInteger mySubImageColumns;
  dc::dcInteger mySubImageRows;
  dc::dcInteger mySubImage;
};
  
}
}
}




#endif