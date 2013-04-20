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

#ifndef __UNSEENSKINNEDMESHRESOURCE_H__
#define __UNSEENSKINNEDMESHRESOURCE_H__
#pragma once




/**
 * This class references a .usm file containing a skinned mesh ready for animation
 */
class UnseenSkinnedMeshResource : public dc::dcResource<UnseenSkinnedMeshResource>
{
    public:

        /**
         * Initializes this resource
         */
        UnseenSkinnedMeshResource();


    public:

        /**
         * Loads the referenced mesh from the source file
         */
        bool loadSkinnedMesh(UnseenSkinnedMesh* skinnedMesh) const;

        /**
         * Gets the source file name for this skinned mesh
         * @return Pointer to the internal file name object
         */
        dc::dcFileName* getFileName();


    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The file from which to load the mesh
        dc::dcFileName mySourceFile;

        /// Whether or not to reverse the winding order of this mesh.  Setting
        /// this flag corrects meshes that are inside-out.
        dc::dcBoolean myReverseWindingOrderFlag;
};



#endif