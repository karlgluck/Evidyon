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
#ifndef __SCENERY_H__
#define __SCENERY_H__
#pragma once



#include <dc/table>
#include <dc/list>
#include "shared/mesh/tools/mesh.h"
#include "scenerysubset.h"
#include "shared/visualfx/tools/visualfx.h"


namespace Evidyon {
namespace Mesh {
class MeshRenderer;
}

namespace Scenery {
struct Scenery;

namespace Tools {


  
//----[  Scenery  ]------------------------------------------------------------
// Todo: the getTransform/getSRT values are duplicated here and in Mesh
class Scenery : public dc::dcResource<Scenery> {
public:
  static std::string staticTypeName() { return "Scenery"; }

public:
  Scenery();

  // Writes the data from this scenery object into the description.  If the
  // mesh renderer is not provided, this object assumes that it has the
  // correct number of subsets stored internally.
  // The output parameter is optional.
  void compile(Mesh::MeshRenderer* mesh_renderer,
               Evidyon::Scenery::Scenery* description,
               size_t* number_of_subsets);

  // Changes the mesh to which this scenery points, and generates new subsets
  // for it.
  void setReferencedMesh(dc::dcGenericResource* mesh,
                         size_t number_of_subsets);

  // Duplicates the source mesh and subset texture assignments of another
  // scenery object
  void copy(Scenery* other);

  // Returns a pointer to the table element referenced by this scenery.  This
  // means the returned value is not the mesh itself, but the interface
  // abstracting it in the collection.
  dc::dcTable<Mesh::Tools::Mesh>::Element* getReferencedMeshTableElement();

  // Builds this mesh's transformation matrix
  void getTransform(D3DXMATRIX* matrix);

  // Gets the scaling, rotation and translation values of this scenery object.
  // The rotation values are automatically converted from degrees to radians.
  void getSRT(float* sx, float* sy, float* sz,
              float* rx, float* ry, float* rz,
              float* tx, float* ty, float* tz);

  // Sets the scaling, rotation and translation values of this scenery object.
  // The rotation values are automatically converted from radians to degrees.
  void setSRT(float sx, float sy, float sz,
              float rx, float ry, float rz,
              float tx, float ty, float tz);

  // Obtains the subset for the given texture group.  Returns NULL if the index
  // is invalid.
  ScenerySubset* getSubset(size_t index);

  void getVisualFXOffset(float* x, float* y, float* z);
  void setVisualFXOffset(float x, float y, float z);

  // Sorts by referenced mesh, then by subsets, then by the remaining factors.
  int compareTo(const dc::dcGenericResource* other) const;

private:
  dc::dcTable<Mesh::Tools::Mesh>::Reference mesh_;
  dc::dcList<ScenerySubset> subsets_;

  // These factors are applied in the order they are declared
  dcx::dcxVector3<dc::dcFloat> scaling_, rotation_, translation_; 

  // The scenery has this special effect bound to it
  Evidyon::Tools::VisualFXReference visual_fx_;
  dcx::dcxVector3<dc::dcFloat> visual_fx_offset_;
};



}
}
}

namespace Evidyon {
namespace Tools {
typedef dc::dcList<Evidyon::Scenery::Tools::Scenery> SceneryList;
typedef dc::dcList<Evidyon::Scenery::Tools::Scenery>::Element SceneryElement;
typedef dc::dcList<Evidyon::Scenery::Tools::Scenery>::Reference SceneryReference;
}
}

#endif