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
#ifndef __MESH_H__
#define __MESH_H__
#pragma once



#include <dc/float>
#include <dcx/dcxvector3.h>
#include <d3dx9.h>


namespace Evidyon {
namespace Mesh {
struct Mesh;

namespace Tools {



//----[  Mesh  ]---------------------------------------------------------------
// todo: virtual int getNumberOfSubsets(); would help clean up code elsewhere
//  (specifically the scenery) since we could get this value without having to
// load the mesh.
// Todo: the getTransform/getSRT values are duplicated here and in Scenery
class Mesh : public dc::dcGenericResource {
public:
  static std::string staticTypeName() { return "Mesh"; }

public:
  Mesh();

  // Compiles the mesh data for this mesh
  virtual bool compile(LPDIRECT3DDEVICE9 d3d_device,
                       Evidyon::Mesh::Mesh* description) = 0;


  // Builds this mesh's transformation matrix
  void getTransform(D3DXMATRIX* matrix);

  // Gets the scaling, rotation and translation values of this mesh object.
  // The rotation values are automatically converted from degrees to radians.
  void getSRT(float* sx, float* sy, float* sz,
              float* rx, float* ry, float* rz,
              float* tx, float* ty, float* tz);

  // Sets the scaling, rotation and translation values of this mesh object.
  // The rotation values are automatically converted from radians to degrees.
  // This method is 'combine' and not 'set' because it is used to incorporate
  // changes to the SRT of a mesh while previewing that mesh itself, and the
  // mesh being previewed was created using the old SRT.  Just setting the
  // values directly doesn't work since they're relative to the old SRT and
  // not the mesh's source geometry.
  void combineSRT(float sx, float sy, float sz,
                  float rx, float ry, float rz,
                  float tx, float ty, float tz);


private:
  // These factors are applied in the order they are declared
  dcx::dcxVector3<dc::dcFloat> scaling_, rotation_, translation_; 
};

}
}
}



#include <dc/table>

namespace Evidyon {
namespace Tools {
typedef dc::dcTable<Evidyon::Mesh::Tools::Mesh> MeshTable;
typedef dc::dcTable<Evidyon::Mesh::Tools::Mesh>::Element MeshElement;
typedef dc::dcTable<Evidyon::Mesh::Tools::Mesh>::Reference MeshReference;

}
}



#endif