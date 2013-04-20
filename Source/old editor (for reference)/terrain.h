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

#ifndef __TERRAIN_H__
#define __TERRAIN_H__
#pragma once

#include <dc/boolean>

/// Defines the different directions
#define DIRECTION_NORTH 0x0
#define DIRECTION_EAST  0x1
#define DIRECTION_SOUTH 0x2
#define DIRECTION_WEST  0x3


enum TerrainDirection
{
    TERRAINDIRECTION_NORTH,
    TERRAINDIRECTION_EAST,
    TERRAINDIRECTION_SOUTH,
    TERRAINDIRECTION_WEST,

    // These direction types set up a half-unit (unused)
    //TERRAINDIRECTION_NORTHEAST,
    //TERRAINDIRECTION_SOUTHEAST,
    //TERRAINDIRECTION_SOUTHWEST
    //TERRAINDIRECTION_NORTHWEST,
};


/**
 * Allows the user to specify a part of the terrain
 */
class Terrain : public dc::dcResource<Terrain>
{
    public:

        /**
         * Contains raw information needed to display a preview of this resource
         */
        struct Preview
        {
            /// The resource that created this preview
            Terrain* terrain;

            /// The top texture of the terrain element.  0 is considered valid.
            Texture::Preview* topTexturePreview;

            /// How much to rotate the element.   0 is considered valid.
            TerrainDirection direction;

            /// The side texture of the terrain element
            Texture::Preview* sideTexturePreview;

            /// Whether or not this is a wall element
            bool isWall;
        };


    public:

        /**
         * Initializes this class
         */
        Terrain();

        /**
         * Initializes a preview structure for this terrain type
         * @param texturePreviewArray An array of texture preview structures in the same order as the global texture table
         * @param texturePreviews The number of texture previews in the array
         * @param preview Destination preview structure
         * @return Whether or not the method successfully initialized the structure
         */
        bool loadPreview(Texture::Preview* texturePreviewArray, size_t texturePreviews, Preview* preview);

        /**
         * Gets the name of this resource types
         */
        static std::string staticTypeName();

        dc::dcTable<Texture>::Reference* getTopTextureRef() const { return (dcTable<Texture>::Reference*)&myTopTextureRef; }
        dc::dcTable<Texture>::Reference* getSideTextureRef() const { return (dcTable<Texture>::Reference*)&mySideTextureRef; }
        dc::dcBoolean* getWallFlag() const { return (dcBoolean*)&myWallFlag; }
        dc::dcInteger* getDirection() const { return (dcInteger*)&myDirection; }

    protected:

        /// The texture to apply to the top of the location
        dc::dcTable<Texture>::Reference myTopTextureRef;

        /// The texture to apply to the side of the location (null means no texture is applied)
        dc::dcTable<Texture>::Reference mySideTextureRef;

        /// Whether to this is a wall or not
        dc::dcBoolean myWallFlag;

        /// The direction for the top of this texture (north, east, west, south = 0 1 2 3)
        dc::dcEnum<TerrainDirection> myDirection;

        // This is the only implementation of its own type
        static dc::dcClass<Terrain>::Implements<Terrain> implementsSelf;
};




#endif