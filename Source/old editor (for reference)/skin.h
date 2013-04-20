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

#ifndef __SKIN_H__
#define __SKIN_H__
#pragma once


/**
 * A skin maps mesh subsets (for animated meshes or regular meshes) to a 
 */
class Skin : public dc::dcResource<Skin>
{
    public:

        /**
         * Contains raw information needed to display a preview of this resource
         */
        struct Preview
        {
            /// The kind of map that the skin uses
            typedef std::map<int,Texture::Preview*> BasicMapType;

            /// The resource that created this preview
            Skin* skin;

            /// The texture for the image
            BasicMapType map;
        };


    public:

        // The map type used by a skin
        typedef dc::dcMap<dcInteger,dcTable<Texture>::Reference> Map;

    public:

        /**
         * Initializes this resource
         */
        Skin();

        /**
         * Loads the preview structure for this skin.  If the texture can't be found for any of the
         * entries in this skin, a warning is logged but the method still succeeds (the entry is
         * NULL, so be careful!)
         * @param texturePreviewArray A straight array of textures that corresponds to the order of the
         *                            textures in the root texture table
         * @param texturePreviews How many entries are in the texture preview array
         * @param preview The destination preview structure.
         * @return Whether or not the load method succeeds
         */
        bool loadPreview(Texture::Preview* texturePreviewArray, size_t texturePreviews, Preview* preview);

        /**
         * Gets the map for this skin
         */
        Map* getMap() const;


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The internal map of subset indices to texture references
        Map myMap;

        // This is the only implementation of its own type
        static dc::dcClass<Skin>::Implements<Skin> implementsSelf;
};


#endif