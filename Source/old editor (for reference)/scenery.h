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

#ifndef __SCENERY_H__
#define __SCENERY_H__
#pragma once

#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "skin.h"
#include "mesh.h"

/**
 * Generates the scenery
 */
class Scenery : public dc::dcResource<Scenery>
{
    public:

        /**
         * Contains raw information needed to display a preview of this resource
         */
        struct Preview
        {
            /// The resource that created this preview
            Scenery* scenery;

            /// The mesh that this scenery references.  0 is valid.
            Mesh::Preview* meshPreview;

            /// The skin that this scenery references.  0 is valid.
            Skin::Preview* skinPreview;

            /// The transformation to apply to the scenery
            D3DXMATRIXA16 transform;
        };


    public:

        /**
         * Initializes this scenery with a name
         */
        Scenery();

        /**
         * Initializes a preview structure for this scenery element
         * @param meshPreviewArray Array of mesh previews in the same order as the global mesh table
         * @param meshPreviews The number of entries in the mesh preview array
         * @param skinPreviewArray Array of skin previews in the same order as the global skin table
         * @param skinPreviews The number of entries in the skin preview array
         * @param preview Destination preview structure
         * @return Whether or not the preview was initialized correctly
         */
        bool loadPreview(Mesh::Preview* meshPreviewArray, size_t meshPreviews,
                          Skin::Preview* skinPreviewArray, size_t skinPreviews, Preview* preview);

        dc::dcTable<Mesh>::Reference* getMeshRef() const { return (dcTable<Mesh>::Reference*)&myMesh; }

        dc::dcList<Skin>::Reference* getSkinRef() const { return (dcList<Skin>::Reference*)&mySkin; }

        dcxVector3<dcFloat>* getRotation() const { return (dcxVector3<dcFloat>*)&myRotation; }
        dcxVector3<dcFloat>* getScaling() const { return (dcxVector3<dcFloat>*)&myScaling; }
        dcxVector3<dcFloat>* getTranslation() const { return (dcxVector3<dcFloat>*)&myTranslation; }

    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The source mesh to use for this scenery
        dc::dcTable<Mesh>::Reference myMesh;

        /// The skin to apply to the mesh
        dc::dcList<Skin>::Reference mySkin;

        /// Transforms the mesh
        dcxVector3<dcFloat> myRotation;
        dcxVector3<dcFloat> myScaling;
        dcxVector3<dcFloat> myTranslation;


    protected:

        // This is the only implementation of its own type
        static dc::dcClass<Scenery>::Implements<Scenery> implementsSelf;
};



#endif