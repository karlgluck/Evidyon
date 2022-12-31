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