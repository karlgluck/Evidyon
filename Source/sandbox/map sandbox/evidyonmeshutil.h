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
#ifndef __EVIDYONMESHUTIL_H__
#define __EVIDYONMESHUTIL_H__

#include "evidyonmeshrenderer.h"
#include <d3dx9.h>

namespace Evidyon {
namespace Meshes {


//----[  EvidyonMeshUtil_CreateMeshFromD3DXMesh  ]-----------------------------
// Converts any D3DXMesh object into the unform Mesh geometry format.
bool EvidyonMeshUtil_CreateMeshFromD3DXMesh(LPD3DXMESH d3dx_mesh,
                                            DWORD subsets,
                                            Mesh* mesh);


}
}

#endif