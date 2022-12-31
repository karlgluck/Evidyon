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

