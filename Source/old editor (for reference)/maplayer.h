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
#ifndef __MAPLAYER_H__
#define __MAPLAYER_H__
#pragma once

/*
// The different navigation types that can be associated with a map layer
enum MapNavigability
{
    MAPNAV_WALK,
    MAPNAV_WADE,
    MAPNAV_SWIM,
    MAPNAV_PIT,
    MAPNAV_LOWWALL,
    MAPNAV_HIGHWALL,
    MAPNAV_IMPASSABLE,

    /// How many navigability types there are
    MAPNAV_COUNT,
};*/


/**
 * A ground-height is just a float
 */
class Elevation : public dc::dcResource<Elevation,dcFloat>
{
    public:

        static std::string staticTypeName() { return "Ground Height"; }
};



/**
 * Provides a complete definition of a location on the map.  This structure is
 * used to specify exact changes to the templates provided by the map layers.
 */
class MapLocationInfo : public dc::dcResource<MapLocationInfo>
{
    public:

        /**
         * Contains raw information needed to display a preview of this resource
         */
        struct Preview
        {
            /// The resource that created this preview
            MapLocationInfo* mapLocationInfo;

            /// Whether or not to change the navigability
            bool overwriteNavigability;

            /// The layer's navigability type
            MapNavigability navigability;

            /// Whether or not to write this terrain element
            bool overwriteTerrain1;
            bool overwriteTerrain2;

            /// The terrain type for this layer, if any
            Terrain::Preview* terrainPreview1;
            Terrain::Preview* terrainPreview2;

            /// Whether or not to overwrite the height of the terrain element
            bool overwriteElevation1;
            bool overwriteElevation2;

            /// The different heights
            float elevation1, elevation2;

            /// Whether or not to overwrite scenery
            bool overwriteScenery;

            /// The scenery override for this layer, if any
            Scenery::Preview* sceneryPreview;
        };

    public:

        /**
         * Initializes this class
         */
        MapLocationInfo();

        /**
         * Initializes the preview structure for this resource
         */
        bool loadPreview(Terrain::Preview* terrainPreviewArray, size_t terrainPreviews,
                          Scenery::Preview* sceneryPreviewArray, size_t sceneryPreviews,
                          Preview* preview);

        /**
         * Gets the name of this resource types
         */
        static std::string staticTypeName();

        dc::dcBoolean* getOverwriteNavigability() const { return (dcBoolean*)&myOverwriteNavigability; }
        dc::dcEnum<MapNavigability>* getNavigability() const { return (dcEnum<MapNavigability>*)&myNavigability; }
        dc::dcBoolean* getOverwriteTerrain1() const { return (dcBoolean*)&myOverwriteTerrain1; }
        dc::dcBoolean* getOverwriteTerrain2() const { return (dcBoolean*)&myOverwriteTerrain2; }
        dc::dcList<Terrain>::Reference* getTerrain1Ref() const { return (dcList<Terrain>::Reference*)&myTerrainRef1; }
        dc::dcList<Terrain>::Reference* getTerrain2Ref() const { return (dcList<Terrain>::Reference*)&myTerrainRef2; }
        dc::dcBoolean* getOverwriteElevation1() const { return (dcBoolean*)&myOverwriteElevation1; }
        dc::dcBoolean* getOverwriteElevation2() const { return (dcBoolean*)&myOverwriteElevation2; }
        dc::dcList<Elevation>::Reference* getElevation1() { return &myElevation1; }
        dc::dcList<Elevation>::Reference* getElevation2() { return &myElevation2; }
        dc::dcBoolean* getOverwriteScenery() const { return (dcBoolean*)&myOverwriteScenery; }
        dc::dcList<Scenery>::Reference* getSceneryRef() const { return (dcList<Scenery>::Reference*)&mySceneryRef; }


    protected:

        /// Whether or not to overwrite the navigability
        dc::dcBoolean myOverwriteNavigability;

        /// What kind of navigability to associate with this location
        dc::dcEnum<MapNavigability> myNavigability;

        /// Whether or not to overwrite the first/second terrain layer
        dc::dcBoolean myOverwriteTerrain1, myOverwriteTerrain2;

        /// The type of terrain to place here on each layer, if any
        dc::dcList<Terrain>::Reference myTerrainRef1, myTerrainRef2;

        /// Whether or not to overwrite the scenery
        dc::dcBoolean myOverwriteScenery;

        /// The type of scenery to place here, if any
        dc::dcList<Scenery>::Reference mySceneryRef;

        /// Whether or not to overwrite the height
        dc::dcBoolean myOverwriteElevation1, myOverwriteElevation2;

        /// The height of this map type
        dc::dcList<Elevation>::Reference myElevation1, myElevation2;
};



/**
 * This is a class for a template layer of the map specified using a mask.  Masked layers can
 * be used to specify all kinds of important information, and are used as a simple way to
 * outline large areas of the world map using only a bitmap editor.
 */
class MapLayer : public dc::dcResource<MapLayer>
{
    public:

        /**
         * Contains raw information needed to display a preview of this resource
         */
        struct Preview
        {
            /// The resource that created this preview
            MapLayer* mapLayer;

            /// A bit-mask of the layer
            BYTE* layerMaskArray; // TODO: rename layerMaskArray to maskArray
            int layerMaskWidth;
            int layerMaskHeight;

            /// Layer priority
            int priority;

            /// Layer fill options
            MapLocationInfo::Preview* locationInfoOptionArray;

            /// How many layer fill options there are
            size_t locationInfoOptions;

            /// Terrain rules to apply to this layer
            TerrainRule::Preview** terrainRuleArray;

            /// How many terrain rules there are
            size_t terrainRules;
        };

    public:

        /**
         * Initializes this class
         */
        MapLayer();

        /**
         * Sets up the preview for this class
         */
        bool loadPreview(LPDIRECT3DDEVICE9 d3dDevice,
                          Terrain::Preview* terrainPreviewArray, size_t terrainPreviews,
                          Scenery::Preview* sceneryPreviewArray, size_t sceneryPreviews,
                          TerrainRule::Preview* terrainRulesArray, size_t terrainRules, Preview* preview);

        /**
         * Sorts map layers by their priority
         */
        int compareTo(const dc::dcGenericResource* other) const;

        /**
         * Gets the name of this resource types
         */
        static std::string staticTypeName();

        inline dc::dcInteger* getPriority() const { return (dcInteger*)&myPriority; }
        inline dc::dcFileName* getLayerMaskFile() const { return (dcFileName*)&myLayerMask; }
        inline dc::dcList<MapLocationInfo>* getLayerOptionsList() const { return (dcList<MapLocationInfo>*)&myLayerOptions; }

    private:

        bool loadLayerMask(LPDIRECT3DDEVICE9 d3dDevice, BYTE** maskBits, int* width, int* height);

    public:

        /// The priority of this map layer.  All layers with the same priority are combined in
        /// the order in which they are listed in the map; however, map layers with a lower
        /// priority index are applied first; therefore, ones with a higher priority overwrite
        /// those with a lower priority.
        dc::dcInteger myPriority;

        /// This object specifies the source layer mask image for this layer
        dc::dcFileName myLayerMask;

        /// This is a list of all of the different resource types that can be
        /// picked for insertion on the parts of the layer mask.
        dc::dcList<MapLocationInfo> myLayerOptions;

        /// The rules to apply to this layer
        dc::dcList<dcList<TerrainRule>::Reference> myRules;
};



#endif
