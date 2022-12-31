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
#ifndef __D3DXMESH_H__
#define __D3DXMESH_H__
#pragma once


#include "mesh.h"
#include <dc/filename>


namespace Evidyon {
namespace Mesh {
namespace Tools {


  
//----[  D3DXMesh  ]-----------------------------------------------------------
// Loads the geometry from a .x model file
class D3DXMesh : public dc::dcResource<D3DXMesh,Mesh> {
  static dc::dcClass<D3DXMesh>::Implements<Mesh> implements_mesh_;
public:
  static std::string staticTypeName() { return "D3DXMesh"; }
public:
  D3DXMesh();

  // Compiles the mesh data for this mesh
  virtual bool compile(LPDIRECT3DDEVICE9 d3d_device,
                       Evidyon::Mesh::Mesh* description);

  void setSourceFilePath(const std::string& path);

private:
  dc::dcFileName source_file_;
};


}
}
}


#endif