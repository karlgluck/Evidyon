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
#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#pragma once




#include "shared/image/tools/image.h"

#include <dc/table>
#include <dc/enum>
#include "shared/texture/texturedescription.h"
#include <dc/boolean>


namespace Evidyon {
namespace Texture {
namespace Tools {


  
//----[  Texture  ]------------------------------------------------------------
class Texture : public dc::dcGenericResource {
public:
  static std::string staticTypeName() { return "Texture"; }

public:
  Texture();

  // Writes data for this texture into the description
  virtual void compile(TextureDescription* description) = 0;

  // Compiles the interface's texture information
  void compileBase(TextureDescription* description);
  void getSourceImage(Evidyon::Image::Tools::Image** image);
  size_t getSourceImageIndex() const;

  // Sorts the textures by ascending referenced image index, blend type,
  // and subclass type.
  int compareTo(const dc::dcGenericResource* other) const;

  // Changes the image-object to which this resource points
  void setImageReference(dc::dcGenericResource* image);

private:
  Evidyon::Tools::ImageReference image_;
  dc::dcEnum<TextureDescription::BlendType> blending_type_;
  dc::dcBoolean flip_horizontal_, flip_vertical_;
};




}
}
}

//----[  TextureReference  ]---------------------------------------------------
namespace Evidyon {
namespace Tools {
typedef dc::dcTable<Evidyon::Texture::Tools::Texture>::Element TextureElement;
typedef dc::dcTable<Evidyon::Texture::Tools::Texture>::Reference TextureReference;
}
}

#endif