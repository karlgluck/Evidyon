//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#ifndef __PACKMESH_H__
#define __PACKMESH_H__
#pragma once

/**
 * A pack mesh is a mesh that is inside of a resource pack
 */
class PackMesh : public dc::dcResource<PackMesh,Mesh>
{
    public:

        /**
         * Initializes this resource
         */
        PackMesh();


    public:


    public:

        /**
         * Changes the resource index that this packed mesh uses to access its source data.  This
         * method is required for building resource packs, and really shouldn't be used anywhere
         * other than there.
         */
        void setResourceIndex(int index);

        /**
         * Get the resource pack reference for this image
         */
        dc::dcTable<ResourcePack>::Reference* getResourcePack() { return &myResourcePack; }

    public:

        /**
         * Loads the geometry held by this class into the destination set.  Note that the
         * returned set is indexed by subset; also, the data that is returned MUST be editable.
         * This means that by editing whatever comes out, the original data loaded by the
         * class must not change.
         *   @param pd3dDevice The source device (if necessary)
         *   @param subsetGeometry The destination for the mesh's subset geometry
         *   @return Result code
         */
        bool obtainSourceGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const;


    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The resource pack to which the image is attached
        dc::dcTable<ResourcePack>::Reference myResourcePack;

        /// The index that this mesh occupies in the pack.  We alias this type
        /// so that it can't be edited like a normal integer.
        Alias<dcInteger> myResourceIndex;

        /// This class implements the Mesh interface
        static dc::dcClass<PackMesh>::Implements<Mesh> implementsMesh;
};



#endif