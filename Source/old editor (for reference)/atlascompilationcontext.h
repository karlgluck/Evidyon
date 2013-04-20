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
#ifndef __ATLASCOMPILATIONCONTEXT_H__
#define __ATLASCOMPILATIONCONTEXT_H__
#pragma once


#include <queue>
#include "../shared/compiledatlasformat.h"


class AtlasCompilationContext
{
public:

  /**
   * This description defines a terrain layer's attributes
   */
  struct LocationTerrainDescription
  {
      /// The subset of the terrain mesh to use to render this terrain
      /// If this description is not filled with meaningful data, this
      /// member is set to TMS_INVALID.
      TerrainMeshSubset terrainMeshSubset;

      /// The index of the texture with which to draw the
      /// terrain mesh.
      size_t textureIndex;

      /// If this element is valid, the terrain is a wall
      /// so the wall mesh needs to be rendered.
      size_t wallTextureIndex;

      /// How much to translate the mesh used to render this terrain type along Y 
      float elevation;

      /// How much to scale this element vertically
      float height;
  };

  /**
   * Holds all of the information for a given location
   */
  struct LocationDescription
  {
      MapNavigability navigability;

      /// Which scenery element to put here
      size_t sceneryIndex;

      /// Holds information for each of the terrain layers
      LocationTerrainDescription terrainLayers[2];
  };


public:

  /**
   * Takes fields from a LocationDescription structure and fills in a
   * structure that is saved in the compiled file for the client.  NULL is a
   * valid argument for 'description' that will cause the output description
   * to be filled with detectably invalid data
   */
  void convertLocationDescription(LocationDescription* description,
               CompiledAtlasClientLocationDescription* clientDescription);


protected:

  /**
   * STL-usable comparator to compare location description pointers by the data that is
   * relavent to a client program.
   */
  struct less_LDptr_client : public std::binary_function<LocationDescription*,LocationDescription*,bool> {
      bool operator()(const LocationDescription* left,
                      const LocationDescription* right) const;
  };

  typedef std::pair<size_t,LocationDescription*> OccurranceToDescriptionPair;

  /**
   * Used to sort by the occurrance term in a collection of OccurranceToDescriptionPair entries.
   * This will place the entry with the highest occurrance at the start of the collection.
   */
  struct greater_occurrance
      : public std::binary_function<OccurranceToDescriptionPair, OccurranceToDescriptionPair, bool>
  {
      bool operator()(const OccurranceToDescriptionPair& left,
                      const OccurranceToDescriptionPair& right) const;
  };

  /// This table type is used during compilation to track the number of instances of 
  /// a given description type in the map.  The highest ones are added to the
  /// description lookup table.
  typedef std::map<LocationDescription*,size_t,less_LDptr_client> DescriptionOccurranceTable;

  /// This priority queue sorts by the occurrance value of a description
  typedef std::priority_queue<OccurranceToDescriptionPair,
                  std::vector<OccurranceToDescriptionPair>,greater_occurrance> DescriptionsSortedByOccurranceQueue;

  /// Maps a description to its index in the lookup table
  typedef std::map<LocationDescription*,size_t,less_LDptr_client> DescriptionToLookupTableIndexTable;

public:

  /**
   * Initializes the structure
   */
  AtlasCompilationContext();

  /**
   * Erases everything in the class and deallocates all allocated data
   */
  void destroy();

  /**
   * Initializes the default location's information
   */
  void setDefaultLocation(size_t textureIndex0, size_t textureIndex1, float elevation0, float elevation1, MapNavigability navigability);

  /**
   * Initializes character spawning information
   */
  void setSpawnInfo(int spawnX,
                    int spawnY,
                    float spawnRadius,
                    int evil_spawn_x,
                    int evil_spawn_y);

  /**
   * Makes sure there is enough room in the map to include locations in the given region.  The
   * x2, y2 coordinates are exclusive; x1,y1 are inclusive
   */
  bool reserve(int x1, int y1, int x2, int y2);

  /**
   * Adds all of the terrain descriptions from the given cache into this context
   */
  void importDescriptions(AtlasEditDialog::MapCacheType* mapCache);

  void setBackgroundSoundLoopIndex(unsigned int index);

public:

  /**
   * Gets the offset values that will put all of the map data into the positive-x,
   * positive-y quadrant.
   */
  void getSingleQuadrantMapOffset(int* xOffset, int* yOffset);

  /**
   * Writes out the data to compile this map layer into the given stream
   * for the client application.
   */
  bool compile(LPDIRECT3DDEVICE9 d3dDevice,
               dc::dcStreamOut* clientStream,
               dc::dcStreamOut* serverStream,
               int* width_out,
               int* height_out);


protected:

  /// The default location type.  Only the navigability and texture for
  /// layer 0 can be changed; the type is always TMS_FLAT_TEXN, and the
  /// second layer (and other attributes) are disabled.
  LocationDescription myDefaultLocation;

  /// Default navigability type
  MapNavigability myDefaultLocationNavigability;

  /// Size of the data set specified by this layer
  int myX1, myY1, myX2, myY2;

  /// This array specifies each of the locations on the map
  LocationDescription* myMap;

  /// Spawn location for this atlas
  int mySpawnX, mySpawnY;
  float mySpawnRadius;
  int evil_spawn_x_, evil_spawn_y_;

  unsigned int background_sound_loop_index_;
};

#endif