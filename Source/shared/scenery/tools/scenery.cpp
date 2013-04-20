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
#include "scenery.h"
#include "shared/mesh/meshrenderer.h"
#include "shared/scenery/sceneryrenderer.h"
#include <assert.h>


namespace Evidyon {
namespace Scenery {
namespace Tools {



  
//----[  Scenery  ]------------------------------------------------------------
Scenery::Scenery() {
  member("Mesh", &mesh_);
  member("Subsets", &subsets_);
  member("Scaling", &scaling_);
  member("Rotation", &rotation_);
  member("Translation", &translation_); 
  member("VisualFX", &visual_fx_);
  member("VisualFX Offset", &visual_fx_offset_);
  scaling_.getX()->setValue(1.0f);
  scaling_.getY()->setValue(1.0f);
  scaling_.getZ()->setValue(1.0f);
}





//----[  setReferencedMesh  ]--------------------------------------------------
void Scenery::setReferencedMesh(dc::dcGenericResource* mesh,
                                size_t number_of_subsets) {
  assert(mesh->getTypeID() == // ensure the type is correct
         dc::dcClass<dc::dcTable<Evidyon::Mesh::Tools::Mesh>::Element>::staticResourceManager()->uniqueID());
  mesh_.setGenericReferencedResource(mesh);
  subsets_.clear();
  for (size_t i = 0; i < number_of_subsets; ++i) {
    char name[16];
    sprintf_s(name, 16, "%lu", i);
    subsets_.generateElement(name);
  }
}



//----[  copy  ]---------------------------------------------------------------
void Scenery::copy(Scenery* other) {

  // Duplicate the mesh reference
  mesh_.setGenericReferencedResource(other->mesh_.getGenericReferencedResource());

  // Copy all subsets
  subsets_.clear();
  const dc::dcGenericResource::Array& members = other->subsets_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;
  for (i = members.begin(); i != members.end(); ++i) {
    ScenerySubset* other_subset =(ScenerySubset*)(*i);
    ScenerySubset* my_subset
      = (ScenerySubset*)subsets_.generateElement(other_subset->getName());
    my_subset->setReferencedTexture(other_subset->getReferencedTexture());
  }

  // Duplicate the transforms
  scaling_.getX()->setValue(other->scaling_.getX()->getValue());
  scaling_.getY()->setValue(other->scaling_.getY()->getValue());
  scaling_.getZ()->setValue(other->scaling_.getZ()->getValue());
  rotation_.getX()->setValue(other->rotation_.getX()->getValue());
  rotation_.getY()->setValue(other->rotation_.getY()->getValue());
  rotation_.getZ()->setValue(other->rotation_.getZ()->getValue());
  translation_.getX()->setValue(other->translation_.getX()->getValue());
  translation_.getY()->setValue(other->translation_.getY()->getValue());
  translation_.getZ()->setValue(other->translation_.getZ()->getValue());

  // Duplicate the special FX reference
  visual_fx_.setGenericReferencedResource(
    other->visual_fx_.getGenericReferencedResource());
  visual_fx_offset_.getX()->setValue(other->visual_fx_offset_.getX()->getValue());
  visual_fx_offset_.getY()->setValue(other->visual_fx_offset_.getY()->getValue());
  visual_fx_offset_.getZ()->setValue(other->visual_fx_offset_.getZ()->getValue());
}



//----[  getReferencedMeshElement  ]-------------------------------------------
dc::dcTable<Mesh::Tools::Mesh>::Element*
    Scenery::getReferencedMeshTableElement() {
  return mesh_.getReferencedResource();
}





//----[  compile  ]------------------------------------------------------------
void Scenery::compile(Mesh::MeshRenderer* mesh_renderer,
                      Evidyon::Scenery::Scenery* description,
                      size_t* number_of_subsets) {
  Evidyon::Mesh::MeshIndex mesh_index = mesh_.getReferencedResourceIndex();
  description->mesh = mesh_index;
  Mesh::MeshSubsetIndex number_of_mesh_subsets =
    mesh_renderer ? mesh_renderer->getNumberOfMeshSubsets(mesh_index)
                  : subsets_.getMembers().size();
  if (number_of_subsets) *number_of_subsets = number_of_mesh_subsets;
  description->subset_textures = new Texture::TextureIndex[number_of_mesh_subsets];

  subsets_.sortByName();
  const dc::dcGenericResource::Array& members = subsets_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;

  size_t index = 0;
  Texture::TextureIndex last_texture_index = Texture::INVALID_TEXTURE_INDEX;
  for (i = members.begin();
       i != members.end() && index < number_of_mesh_subsets; ++i, ++index) {
    ScenerySubset* subset = (ScenerySubset*)(*i);
    last_texture_index = subset->getTextureIndex();
    description->subset_textures[index] = last_texture_index;
  }
  while (index < number_of_mesh_subsets) {
    description->subset_textures[index++] = last_texture_index;
  }

  getTransform(&description->local_transform);

  description->visual_fx = visual_fx_.getReferencedResourceIndex();
  description->visual_fx_offset.x = visual_fx_offset_.getX()->getValue();
  description->visual_fx_offset.y = visual_fx_offset_.getY()->getValue();
  description->visual_fx_offset.z = visual_fx_offset_.getZ()->getValue();
}



//----[  getTransform  ]-------------------------------------------------------
void Scenery::getTransform(D3DXMATRIX* matrix) {
  D3DXMATRIX s, r, t;
  D3DXMatrixScaling(&s,
                    scaling_.getX()->getValue(),
                    scaling_.getY()->getValue(),
                    scaling_.getZ()->getValue());
  D3DXMatrixRotationYawPitchRoll(&r,
                                 D3DXToRadian(rotation_.getY()->getValue()),
                                 D3DXToRadian(rotation_.getX()->getValue()),
                                 D3DXToRadian(rotation_.getZ()->getValue()));
  D3DXMatrixTranslation(&t,
                        translation_.getX()->getValue(),
                        translation_.getY()->getValue(),
                        translation_.getZ()->getValue());
  D3DXMatrixMultiply(matrix, &s, &r);
  D3DXMatrixMultiply(matrix, matrix, &t);
}




//----[  getSRT  ]-------------------------------------------------------------
void Scenery::getSRT(float* sx, float* sy, float* sz,
                     float* rx, float* ry, float* rz,
                     float* tx, float* ty, float* tz) {
  *sx = scaling_.getX()->getValue();
  *sy = scaling_.getY()->getValue();
  *sz = scaling_.getZ()->getValue();
  *rx = D3DXToRadian(rotation_.getX()->getValue());
  *ry = D3DXToRadian(rotation_.getY()->getValue());
  *rz = D3DXToRadian(rotation_.getZ()->getValue());
  *tx = translation_.getX()->getValue();
  *ty = translation_.getY()->getValue();
  *tz = translation_.getZ()->getValue();
}


//----[  setSRT  ]-------------------------------------------------------------
void Scenery::setSRT(float sx, float sy, float sz,
                     float rx, float ry, float rz,
                     float tx, float ty, float tz) {
  scaling_.getX()->setValue(sx);
  scaling_.getY()->setValue(sy);
  scaling_.getZ()->setValue(sz);
  rotation_.getX()->setValue(D3DXToDegree(rx));
  rotation_.getY()->setValue(D3DXToDegree(ry));
  rotation_.getZ()->setValue(D3DXToDegree(rz));
  translation_.getX()->setValue(tx);
  translation_.getY()->setValue(ty);
  translation_.getZ()->setValue(tz);
}


//----[  getSubset  ]----------------------------------------------------------
ScenerySubset* Scenery::getSubset(size_t index) {
  const dc::dcGenericResource::Array& subsets = subsets_.getMembers();
  return index < subsets.size() ? (ScenerySubset*)subsets.at(index) : NULL;
}



//----[  getVisualFXOffset  ]--------------------------------------------------
void Scenery::getVisualFXOffset(float* x, float* y, float* z) {
  *x = visual_fx_offset_.getX()->getValue();
  *y = visual_fx_offset_.getY()->getValue();
  *z = visual_fx_offset_.getZ()->getValue();
}


//----[  setVisualFXOffset  ]--------------------------------------------------
void Scenery::setVisualFXOffset(float x, float y, float z) {
  visual_fx_offset_.getX()->setValue(x);
  visual_fx_offset_.getY()->setValue(y);
  visual_fx_offset_.getZ()->setValue(z);
}



//----[  compareTo  ]----------------------------------------------------------
int Scenery::compareTo(const dc::dcGenericResource* other) const {
if (!other || getTypeID() != other->getTypeID()) {
     return dc::dcGenericResource::compareTo(other);
  } else {
    const Scenery* other_scenery = ((const Scenery*)other);
    int mesh = mesh_.compareTo(&other_scenery->mesh_);
    if (mesh != 0) return mesh;
    int subsets = subsets_.compareTo(&other_scenery->subsets_);
    if (subsets != 0) return subsets;
    return dc::dcGenericResource::compareTo(other);
  }
}



}
}
}

