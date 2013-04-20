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

#ifndef __TERRAINRULE_H__
#define __TERRAINRULE_H__
#pragma once


/**
 *
 */
class TerrainRule : public dc::dcResource<TerrainRule>
{
    public:

        typedef dc::dcList<dcList<Terrain>::Reference> OptionList;

    public:
/*
        enum Direction
        {
            DIRECTION_NORTH,
            DIRECTION_EAST,
            DIRECTION_SOUTH,
            DIRECTION_WEST,

            DIRECTION_COUNT,
        };*/

        /**
         * Contains raw information needed to display a preview of this resource
         */
        struct Preview
        {
            /**
             * A list of terrain options for a given direction
             */
            struct TerrainArray
            {
                Terrain::Preview** terrainArray;
                size_t terrainArraySize;
            };

            /// The resource that created this preview
            TerrainRule* terrainRule;

            /// Own canvas types
            TerrainArray canvasTypes;

            // Arrays of terrain types in each direction, starting from the north-west
            // corner:
            //  012
            //  7 3
            //  654
            TerrainArray neighborOptions[8];

            /// Possible fill types
            TerrainArray fillTypes;
        };


    public:

        /**
         * Initializes this class
         */
        TerrainRule();

        /**
         * Fills in the preview structure for this resource
         */
        bool loadPreview(Terrain::Preview* terrainPreviewArray, size_t terrainPreviews, Preview* preview);

        /**
         * Compares this terrain rule with another rule (sorts by priority)
         */
        int compareTo(const dc::dcGenericResource* other) const;

        /**
         * Returns the name of this class type
         */
        static std::string staticTypeName();

        dc::dcInteger* getPriority() { return &myPriority; }
        OptionList* getCanvasTypes() { return &myCanvasTypes; }
        OptionList* getFillTypes() { return &myFillTypes; }

        OptionList* getNeighborTypes(int index) { return &myNeighborTypes[index]; }


    protected:

        /// The priority of this terrain rule.  Rules are applied in order, so the
        /// most specific rules should come last in the list.
        dc::dcInteger myPriority;

        /// The kind of terrain that has to be at this location for the rule to apply
        OptionList myNeighborTypes[8];

        OptionList myCanvasTypes;

        /// Which types are used to fill this layer
        OptionList myFillTypes;
};





#endif