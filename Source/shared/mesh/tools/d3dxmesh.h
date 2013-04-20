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