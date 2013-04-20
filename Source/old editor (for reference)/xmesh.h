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
#ifndef __XMESH_H__
#define __XMESH_H__
#pragma once


/**
 * Implements the mesh type
 */
class XMesh : public dc::dcResource<XMesh,Mesh>
{
    public:

        /**
         * Constructs the subset geometry for a D3DXMesh
         */
        static bool buildGeometryFromD3DXMesh(LPD3DXMESH d3dxMesh, SubsetGeometry* subsetGeometry, DWORD subsets);

    public:

        /**
         * Initializes this class with the given name
         */
        XMesh();

        /**
         * Gets the file-name resource of this mesh
         */
        inline dc::dcFileName* getSourceFile() { return &mySourceFile; }

    public:

        /**
         * Loads geometry from the source file into the output subset geometry
         *   @param pd3dDevice A Direct3D device (if necessary)
         *   @param subsetGeometry The destination for the mesh's subset geometry
         *   @return Result code
         */
        bool obtainSourceGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const;


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The file from which to load the mesh
        dc::dcFileName mySourceFile;


    protected:

        /// This class implements the Mesh class
        static dc::dcClass<XMesh>::Implements<Mesh> implementsMesh;
};



#endif