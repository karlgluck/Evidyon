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
#include "texture.h"



namespace Evidyon {
namespace Texture {
namespace Tools {
  
 

  


  
//----[  Texture  ]------------------------------------------------------------
Texture::Texture() {
  member("Source Image", &image_);
  member("Blending Type", &blending_type_);
  member("Flip Horizontal", &flip_horizontal_);
  member("Flip Vertical", &flip_vertical_);
  blending_type_.setValue(TextureDescription::DISABLE_BLENDING);
}



//----[  compileBase  ]--------------------------------------------------------
void Texture::compileBase(TextureDescription* description) {
  dc::dcTable<Evidyon::Image::Tools::Image>::Element* image_element =
    image_.getReferencedResource();
  if (image_element) {
    Evidyon::Image::Tools::Image* image = image_element->getImplementation();
    LPDIRECT3DTEXTURE9 d3d_texture = image->getD3DTexture();
    description->d3d_texture = d3d_texture;
    if (d3d_texture) d3d_texture->AddRef();
  }
  description->blend_type = blending_type_.getValue();
  description->flip_horizontal = flip_horizontal_.getValue();
  description->flip_vertical = flip_vertical_.getValue();
}




//----[  getSourceImage  ]-----------------------------------------------------
void Texture::getSourceImage(Evidyon::Image::Tools::Image** image) {
  dc::dcTable<Evidyon::Image::Tools::Image>::Element* element =
    image_.getReferencedResource();;
  *image = element ? element->getImplementation() : NULL;    
}


//----[  getSourceImageIndex  ]------------------------------------------------
size_t Texture::getSourceImageIndex() const {
  return image_.getReferencedResourceIndex();
}

//----[  compareTo  ]----------------------------------------------------------
int Texture::compareTo(const dc::dcGenericResource* other) const {
  unsigned long typeID = getTypeID(), otherTypeID = other ? other->getTypeID() : 0;
  unsigned long ownerTypeID = getOwner() ? getOwner()->getTypeID() : 0,
           otherOwnerTypeID = other ? (other->getOwner() ? other->getOwner()->getTypeID() : 0) : 0;
  if (other == 0 || ((typeID != otherTypeID) && ((ownerTypeID != otherOwnerTypeID) || !getOwner()->isInterface())) )
      return dc::dcGenericResource::compareTo(other);
  else {
    Texture* other_texture = (Texture*)other;
    TextureDescription::BlendType my_blend_type = blending_type_.getValue();
    TextureDescription::BlendType other_blend_type = other_texture->blending_type_.getValue();
    if (my_blend_type > other_blend_type) return +1; // sort by blend mode
    if (my_blend_type < other_blend_type) return -1;
    size_t my_image = image_.getReferencedResourceIndex();
    size_t other_image = other_texture->image_.getReferencedResourceIndex();
    if (my_image > other_image) return +1;
    if (my_image < other_image) return -1;
    if (typeID > otherTypeID) return +1; // sort by type
    if (typeID < otherTypeID) return -1;
    return 0; // exactly equal...
  }
}


//----[  setImageReference  ]--------------------------------------------------
void Texture::setImageReference(dc::dcGenericResource* image) {
  image_.setGenericReferencedResource(image);
}


}
}
}

