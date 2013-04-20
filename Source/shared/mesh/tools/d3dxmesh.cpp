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
#include "d3dxmesh.h"
#include "shared/mesh/meshutils.h"
#include "shared/mesh/meshgeometry.h"
#include "shared/mesh/mesh.h"


namespace Evidyon {
namespace Mesh {
namespace Tools {


  
//----[  Implements<Mesh>  ]---------------------------------------------------
dc::dcClass<D3DXMesh>::Implements<Mesh> D3DXMesh::implements_mesh_;



//----[  D3DXMesh  ]-----------------------------------------------------------
D3DXMesh::D3DXMesh() {
  member("Source File", &source_file_);
}



//----[  compile  ]------------------------------------------------------------
bool D3DXMesh::compile(LPDIRECT3DDEVICE9 d3d_device,
                       Evidyon::Mesh::Mesh* description) {
  memset(description, 0, sizeof(Evidyon::Mesh::Mesh));
  LPD3DXMESH pXMesh = NULL;
  DWORD subsets;
  HRESULT hr = D3DXLoadMeshFromX(source_file_.getValue().c_str(),
                                 D3DXMESH_SYSTEMMEM, 
                                 d3d_device,
                                 NULL,
                                 NULL,
                                 NULL,
                                 &subsets,
                                 &pXMesh);
  if (FAILED(hr)) return false;
  bool return_value
    = Evidyon::Mesh::EvidyonMeshUtil_CreateMeshFromD3DXMesh(pXMesh,
                                                            subsets,
                                                            description);
  pXMesh->Release();
  return return_value;
}




//----[  setSourceFilePath  ]--------------------------------------------------
void D3DXMesh::setSourceFilePath(const std::string& path) {
  source_file_.setValue(path);
}



}
}
}

