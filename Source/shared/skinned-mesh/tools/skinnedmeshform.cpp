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
#include "skinnedmeshform.h"
#include <assert.h>
#include "shared/skinned-mesh/skinnedmeshform.h"


namespace Evidyon {
namespace SkinnedMesh {
namespace Tools {



//----[  SkinnedMeshForm  ]--------------------------------------------------------
SkinnedMeshForm::SkinnedMeshForm() {
  member("Animated Mesh", &animated_mesh_);
  member("Textures", &texture_group_textures_);
  member("Transform", &transform_);
}




//----[  compile  ]------------------------------------------------------------
bool SkinnedMeshForm::compile(
    Evidyon::SkinnedMesh::SkinnedMeshForm* description) {
  description->animated_mesh = animated_mesh_.getReferencedResourceIndex();
  transform_.getTransform(&description->transform);

  const dc::dcGenericResource::Array& textures
    = texture_group_textures_.getMembers();
  description->texture_group_textures
    = new Texture::TextureIndex[textures.size()];
  size_t index = 0;
  for (dc::dcGenericResource::Array::const_iterator i = textures.begin();
       i != textures.end(); ++i, ++index) {
    const TextureList::Element* element
        = (const TextureList::Element*)(*i);
    description->texture_group_textures[index]
      = element->getReferencedResourceIndex();
  }
  return true;
}


//----[  setAnimatedMeshReference  ]-------------------------------------------
void SkinnedMeshForm::setAnimatedMeshReference(
    Evidyon::Tools::AnimatedMeshElement* element) {
  assert(element);
  size_t texture_groups = element->numberOfTextureGroups();
  if (texture_group_textures_.getMembers().size() != texture_groups) {
    texture_group_textures_.clear();
    while (texture_group_textures_.getMembers().size() < texture_groups) {
      char name[32];
      _itoa_s(texture_group_textures_.getMembers().size(), name, 32, 10);
      texture_group_textures_.generateElement(name);
    }
  }
  animated_mesh_.setReferencedResource(element);
}


//----[  getNumberOfTextureGroups  ]------------------------------------------
size_t SkinnedMeshForm::getNumberOfTextureGroups() {
  setAnimatedMeshReference(animated_mesh_.getReferencedResource());
  return texture_group_textures_.size();
}

//----[  getGroupTextures  ]--------------------------------------------------
void SkinnedMeshForm::getGroupTextures(Texture::TextureIndex* subset_textures) {
  size_t index = 0;
  const dc::dcGenericResource::Array& members = texture_group_textures_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = members.begin();
                                                    i != members.end(); ++i) {
    TextureList::Element* element = (TextureList::Element*)(*i);
    subset_textures[index++] = element->getReferencedResourceIndex();
  }
}


}
}
}