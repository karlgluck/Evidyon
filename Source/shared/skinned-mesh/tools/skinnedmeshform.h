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
#ifndef __TOOLS_SKINNEDMESHFORM_H__
#define __TOOLS_SKINNEDMESHFORM_H__
#pragma once

// todo: change to AnimatedMesh.h
#include "shared/animated-mesh/tools/unseenskinnedmeshresource.h"
#include "shared/texture/textureindex.h"
#include "skinnedmeshanimation.h"
#include "shared/skinned-mesh/skinnedmeshattachmentpoint.h"
#include "shared/texture/tools/texture.h"
#include <dc/list>
#include <dcx/d3dxmatrix>


namespace Evidyon {
namespace SkinnedMesh {
struct SkinnedMeshForm;

namespace Tools {


//----[  SkinnedMeshForm  ]--------------------------------------------------------
class SkinnedMeshForm : public dc::dcResource<SkinnedMeshForm> {
  typedef dc::dcList<Evidyon::Tools::TextureReference> TextureList;
public:
  static std::string staticTypeName() { return "SkinnedMeshForm"; }
public:
  SkinnedMeshForm();
  bool compile(Evidyon::SkinnedMesh::SkinnedMeshForm* description);

  // Changes the referenced mesh.  This will update the subset textures to
  // reflect the number of subsets in the target.  If the target is invalid,
  // the textures will be cleared.
  void setAnimatedMeshReference(
    Evidyon::Tools::AnimatedMeshElement* element);

  // Checks the target mesh to make sure its number of textures matches
  // the internal elements of subset_textures_.  If not, subset_textures_ is
  // cleared and the reference is reassigned.
  size_t getNumberOfTextureGroups();

  // Obtains the texture indices for this mesh and fills out entries in the
  // provided array.  getNumberOfTextureGroups must ALWAYS be called before
  // invoking this method, so the length of the buffer array is defined
  // without having to pass or return a parameter.
  void getGroupTextures(Texture::TextureIndex* subset_textures);

public:
  dcx::dcxD3DXMatrix* getTransform() { return &transform_; }

private:

  // The source mesh
  Evidyon::Tools::AnimatedMeshReference animated_mesh_;

  // Textures to apply to the texture groups of the animated mesh
  TextureList texture_group_textures_;

  // Rescales, rotates and centers the source mesh
  dcx::dcxD3DXMatrix transform_;
};

  



}
}
}

#include <dc/list>

namespace Evidyon {
namespace Tools {


  
//----[  SkinnedMeshFormReference  ]-------------------------------------------
typedef dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMeshForm>::Reference
  SkinnedMeshFormReference;

  

//----[  SkinnedMeshFormElement  ]---------------------------------------------
typedef dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMeshForm>::Element
SkinnedMeshFormElement;

}
}


#endif