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