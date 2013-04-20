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
#ifndef __SHARED_MESH_MESHGEOMETRY_H__
#define __SHARED_MESH_MESHGEOMETRY_H__
#pragma once



#include <d3d9.h>


namespace Evidyon {
namespace Mesh {


//----[  GeometryVertex  ]-----------------------------------------------------
struct GeometryVertex {
  float x, y, z;
  float nx, ny, nz;
  DWORD diffuse;
  float u, v;
};

// The Direct3D vertex format of a geometry stream
#define D3DFVF_EVIDYON_GEOMETRYVERTEX   (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)


//----[  GeometryIndex  ]------------------------------------------------------
// Use 16-bit indices for geometry.  Geometry indices always specify lists of triangles, so
// if given a number of indices, the number of polygons is that number divided by three.
typedef WORD GeometryIndex;
#define D3DFMT_EVIDYON_GEOMETRYINDEX  (D3DFMT_INDEX16)





}
}

#endif