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
#ifndef __ATLAS_H__
#define __ATLAS_H__
#pragma once

#include <dc/integer>
#include "location.h"
class Atlas;
typedef dc::dcInteger WorldCoordinateType;
typedef Location WorldLocationType;

#include "stdafx.h"

#include "magic.h"
#include "detailedmagic.h"


#include "colorkey.h"

#include "image.h"
#include "d3dximage.h"

#include "texture.h"
#include "animatedtexture.h"
#include "circlingtexture.h"
#include "slidingtexture.h"
#include "statictexture.h"

#include "skin.h"

#include "mesh.h"
#include "xmesh.h"
#include "mdjmesh.h"


#include "scenery.h"

#include "location.h"

#include "terrain.h"
#include <dc/table>

#include "terrainrule.h"

#include "../shared/compiledatlasformat.h"

#include "maplayer.h"

#include "atlasterrainmesh.h"
#include <dcx/win32>
#include <dcx/d3d>
#include "../common/isometriccamera.h"  // The common camera for viewing the world
#include "atlas.h"
#include "atlaseditdialog.h"
#include "atlascompilationcontext.h"


#include "contributor.h"

#include "resourcepack.h"
#include "packimage.h"
#include "packmesh.h"


#include "wavsound.h"

#include "visualfximage.h"
#include "visualfx.h"
#include "magicfx.h"
#include "visualfx_orbitingparticles.h"
#include "visualfx_particletrail.h"

#include "avatarmodifier.h"

#include "item.h"

#include "unseenskinnedmeshresource.h"
#include "unseenkeyframeanimation.h"
#include "skinnedmeshattachmentpoint.h"
#include "actortype.h"

#include "aicharactertype.h"

#include "playercharacterrace.h"
#include "playercharacterclasslevelspells.h"
#include "playercharacterclass.h"

#include "leveldefinitions.h"

#include "projectile.h"

#include "enchantment.h"
#include "enchantmentclass.h"
#include "enchantmentclassimpl.h"
#include "enchantmentclasslist.h"

#include "lifeformai.h"
#include <dcx/d3d>

#include <d3dx9.h>
#include <dc/enum>
#include <dc/list>
#include "maplocationspecialfunction.h"
#include "terrain.h"
#include "location.h"
#include "swarmspawner.h"
#include "rarityscale.h"

#include "wavsound.h"

class AtlasCompilationContext;

/**
 * Allows the user to link up a bunch of map layers into an actual world atlas
 */
class Atlas : public dc::dcResource<Atlas> {
public:

  /**
   * Initializes this class
   */
  Atlas();

  /**
   * Gets the name of this resource types
   */
  static std::string staticTypeName();

  /**
   * Loads map components and generates a preview window which lets the user visually edit
   * the world contents.
   */
  void edit();

  inline dc::dcList<Terrain>::Reference* getDefaultTerrain(size_t index0Or1) { return &myDefaultTerrainRef[index0Or1]; }
  inline float getDefaultTerrainElevationValue(size_t index0Or1) { return myDefaultTerrainElevations[index0Or1].getValue(); }
  inline dc::dcEnum<MapNavigability>* getDefaultNavigability() { return &myDefaultNavigability; }
  inline dc::dcList<MapLayer>* getMapLayers() { return &myMapLayers; }
  inline dc::dcMap<WorldLocationType,MapLocationInfo>* getDetailedLocations() { return &myDetailedLocations; }
  inline Location::BasicType getSpawnLocationValue() const { return mySpawnLocation.getValue(); }
  inline float getSpawnRadiusValue() const { return mySpawnRadius.getValue(); }
  inline Location::BasicType getEvilSpawnLocationValue() const { return myEvilSpawnLocation.getValue(); }
  inline unsigned int getBackgroundSoundLoopIndex() const { return background_sound_loop_.getReferencedResourceIndex(); }

  /**
   * This method fills the output context class with data.  If anything is currently in
   * the context, it is removed.
   */
  bool compile(LPDIRECT3DDEVICE9 d3dDevice, AtlasCompilationContext* context);

  // Compiles information for the server that is written after the compilation context
  // has been used.  This method writes out, for example, special functions of the map,
  // monster spawning information, and more.
  // The monster level rarity boost is created by the treasure generator.  It specifies
  // how much of an increase in treasure-level is given to a region's swarm spawner
  // depending on its average monster level.
  bool compileServerData(LPDIRECT3DDEVICE9 d3d_device,
                         dc::dcStreamOut* serverStream,
                         int map_width,
                         int map_height,
                         int monster_level_rarity_boost[RARITY_SCALE_SIZE]);

protected:

  /// The terrain type that is applied if no other type can be found.  Each terrain layer
  /// has its own default terrain.
  dc::dcList<Terrain>::Reference myDefaultTerrainRef[2];

  /// The default terrain heights for each of the layers
  dc::dcFloat myDefaultTerrainElevations[2];

  /// The default navigability type of areas outside of the map
  dc::dcEnum<MapNavigability> myDefaultNavigability;

  /// Unique things that map locations do; for example, transport between maps
  dc::dcTable<MapLocationSpecialFunction> mySpecialFunctions;

  /// Stores map template information
  dc::dcList<MapLayer> myMapLayers;

  /// A list of specific, hand-edited modifications to the world (these always override
  /// whatever is in the template).
  dc::dcMap<WorldLocationType,MapLocationInfo> myDetailedLocations;

  /// This is the range in which characters are spawned in the world
  Location mySpawnLocation;
  dc::dcFloat mySpawnRadius;
  Location myEvilSpawnLocation;

  /// Spawn points on this map
  SwarmSpawner swarm_spawner_;

  // The sound loop to play in the background
  dc::dcList<WAVSound>::Reference background_sound_loop_;

  // An inhabitant is spawned only at a certain location.  There is only ever a single
  // instance of the controller active at any given time.
  typedef dc::dcMap<WorldLocationType, dc::dcTable<LifeformAI>::Reference> Inhabitants;
  Inhabitants inhabitants_;
};




#endif

