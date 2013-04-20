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
#ifndef __MESHUTILS_H__
#define __MESHUTILS_H__
#pragma once



#include <d3dx9.h>


namespace Evidyon {
namespace Mesh {

struct Mesh;

//----[  EvidyonMeshUtil_CreateMeshFromD3DXMesh  ]-----------------------------
// Converts any D3DXMesh object into the unform Mesh geometry format.
bool EvidyonMeshUtil_CreateMeshFromD3DXMesh(LPD3DXMESH d3dx_mesh,
                                            DWORD subsets,
                                            Mesh* mesh);



//----[  EvidyonMeshUtil_GetUnityScalingFactor  ]------------------------------
// Computes the factor by which to scale every element of the mesh in order
// to make its largest dimension 1.0
float EvidyonMeshUtil_GetUnityScalingFactor(Mesh* mesh);


//----[  EvidyonMeshUtil_ApplyMeshTransform  ]---------------------------------
// Transforms both vertex normals and mesh coordinates.
void EvidyonMeshUtil_ApplyMeshTransform(const D3DXMATRIX* matrix,
                                        Mesh* mesh);




}
}

#endif