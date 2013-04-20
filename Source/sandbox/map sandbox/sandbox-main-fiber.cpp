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
#include "sandbox-main-fiber.h"

namespace Evidyon {

namespace Evidyon {
  
  
  
}  // Fiber--it's good for you, and it's also good for preventing context switches.


#define WINVER          0x400
#define _WIN32_WINNT    0x400


#include <windows.h>
#include <stdio.h>

struct FiberData
{
    
};

#define FIBER_PRIMARY 0
#define FIBER1 1
#define FIBER2 2
LPVOID g_pFibers[3];

TCHAR buffer[32];
int i;


VOID __stdcall Fiber1( LPVOID lpParameter )
{
    buffer[i++] = '1';
    while( i < sizeof(buffer) - 1 )
    {
        buffer[i++] = 'b';
        SwitchToFiber( g_pFibers[FIBER2] );
    }

    SwitchToFiber( g_pFibers[FIBER_PRIMARY] );
}

VOID __stdcall Fiber2( LPVOID lpParameter )
{
    buffer[i++] = '2';
    while( i < sizeof(buffer) - 1 )
    {
        buffer[i++] = 'a';
        SwitchToFiber( g_pFibers[FIBER1] );
    }

    SwitchToFiber( g_pFibers[FIBER_PRIMARY] );
}

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
  // Reset data
  i = 0;
  memset( buffer, 0, sizeof(buffer) );

  // Convert this thread
  g_pFibers[FIBER_PRIMARY] = ConvertThreadToFiber( NULL );

  // Create the fibers
  g_pFibers[FIBER1] = CreateFiber( 0, Fiber1, NULL );
  g_pFibers[FIBER2] = CreateFiber( 0, Fiber2, NULL );

  // Execute the fibers
  SwitchToFiber( g_pFibers[FIBER1] );

  // Terminate the input
  buffer[i++] = '\0';

  DeleteFiber( g_pFibers[FIBER1] );
  DeleteFiber( g_pFibers[FIBER2] );

  OutputDebugString( buffer );
}


/*

use a MMF for the resource file (CreateFileMapping, MapViewOfFile) and just
pass pointers.  when the loading task exits, destroy the mapping



two kinds of task relationships: hierarchy and sequence.

some tasks contain other tasks,
some tasks must be done in a particular order


for simplicity, all tasks can only have one prereq; however,
a task can contain as many other tasks as it wants.

a task is only complete once its children are complete

ex.
  StartGame
    ReadConfigFile
    AcquireWindow
    AcquireGraphics(AcquireWindow)
    LoadLoginGUIImages(AcquireGraphics)
    LoadImages(AcquireGraphics)
    LoadTextures(LoadImages)
    AcquireInput(AcquireWindow)
    AcquireNetwork
    AcquireAudio
    LoadSounds(AcquireAudio)

   -> EstablishServerConnection(AcquireNetwork)
   -> RenderConnectToServer(AcquireLoginImages)

 EstablishServerConnection
  


*/

#if 0
//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#ifndef __WORLDATLAS_H__
#define __WORLDATLAS_H__


#include <d3d9.h>

namespace Evidyon {
namespace World {

//----[  Navigability  ]-------------------------------------------------------
// Defines how players can interact with map locations.
enum Navigability {
  NAVIGABILITY_NORMAL,            // walk, teleport, fly, etc.
  NAVIGABILITY_NO_TELEPORTING,    // can't teleport on, from or over
  NAVIGABILITY_WADE,              // move character down
  NAVIGABILITY_SWIM,              // water is too deep to wade; must swim
  NAVIGABILITY_PIT,               // fly over, otherwise player falls & dies
  NAVIGABILITY_LOWWALL,           // fly over but can't walk through
  NAVIGABILITY_HIGHWALL,          // must teleport over this location
  NAVIGABILITY_SAFEZONE,          // NORMAL, plus safe-zone behavior
  NAVIGABILITY_IMPASSABLE,        // high wall + no teleporting
};

namespace Tools {



//----[  Location  ]-----------------------------------------------------------
class Location : public dc::dcResource<Location> {
public:
  Location();
private:
  dc::dcInteger x_, y_;
};


//----[  MapMask  ]------------------------------------------------------------
// A map mask is a source image that is used to make it easy to paint features
// onto large areas of the map.
class MapMask : public dc::dcResource<MapMask> {
public:
  static std::string staticTypeName() { return "MapMask"; }
  typedef std::vector<D3DCOLOR> Colors;

private:
  // // these methods were pulled from MapColorMask and adopted here.  In the
  // // future these should be integrated completely and removed, but for now
  // // I just want to get stuff done.
  static bool GetMapMaskInfo(const char* file_name,
                             unsigned int* width,
                             unsigned int* height);
  static bool LoadMapMaskData(LPDIRECT3DDEVICE9 d3d_device,
                              const char* file_name,
                              unsigned int width,
                              unsigned int height,
                              D3DCOLOR* data_buffer);
  static bool AcquireMapMask(LPDIRECT3DDEVICE9 d3d_device,
                             const char* file_name,
                             unsigned int* width_out,
                             unsigned int* height_out,
                             D3DCOLOR** data_buffer);
  static void ReleaseMapMask(D3DCOLOR* data_buffer);

public:
  MapMask();
  ~MapMask();

  // Frees all allocated memory
  void destroy();

  // Updates the map mask's in-memory format.  If force_reload is 'true', the
  // map will scan the file on disk again even if an in-memory copy already
  // exists.  Otherwise, if the map has already been loaded, this method just
  // returns 'true' without doing any work.
  bool load(LPDIRECT3DDEVICE9 d3d_device, bool force_reload);

  // Writes the map mask to disk, committing any changes to it.
  void save();

  // Called when the mask is loaded to update the colors in the mask.  Any
  // color in color_names_ that doesn't exist anymore is removed; any new
  // ones are added with a default name.
  void updateColorNames();

  // If the map mask has been loaded, this returns the internal list of all
  // of the colors in the mask.
  const Colors& getColors();

  // Valid only when the mask has been loaded.  Returns the 
  int getColorIndex(D3DCOLOR color) const;

  // Returns the name of the given color, or "n/a" if the color doesn't exist
  // in the names list.
  std::string colorName(D3DCOLOR color) const;

  // Returns the first set of coordinates at which the mask contains the given
  // color.  After calling this method, iterate through the rest of the
  // locations with getNextLocation.  If this method returns 'false', the map
  // does not contain the given color.
  bool getFirstLocation(D3DCOLOR color,
                        int* x,
                        int* y);

  // Used to iterate through the map locations containing the given color.
  // This method returns 'false' if the color can no longer be found.
  bool getNextLocation(D3DCOLOR color,
                       int last_x,
                       int last_y,
                       int* x,
                       int* y);

private:

  // Determines whether or not the color_names_ mask has the given color.
  bool colorNamesContains(D3DCOLOR color);

private:

  // The file on which this mask is based
  dc::dcFileName source_file_;

  // Allows each color in the mask to be named so that they are easier to
  // manage.
  dc::dcList<dcx::dcxD3DColor> color_names_;


// Active data used when the mask has been loaded.
private:

  // Size of the map mask
  unsigned int width_, height_;

  // All of the colors that are present in the map mask
  Colors colors_;
  
  // Array of width_ x height_ colors representing the mask.
  D3DCOLOR* map_mask_;
};



//----[  MapLayerBlendingType  ]-----------------------------------------------
enum MapLayerBlendingType {

  //  o   i             i
  // iii  io    iii    oi
  //      i      o      i
  NORTH, EAST, SOUTH, WEST,

  //  o            o
  //  io  io  oi  oi
  //      o    o  
  NORTHEAST_OUTER_CORNER,
  SOUTHEAST_OUTER_CORNER,
  SOUTHWEST_OUTER_CORNER,
  NORTHWEST_OUTER_CORNER,

  //  io          oi
  //  ii  ii  ii  ii
  //      io  oi  
  NORTHEAST_INNER_CORNER,
  SOUTHEAST_INNER_CORNER,
  SOUTHWEST_INNER_CORNER,
  NORTHWEST_INNER_CORNER,

  NUMBER_OF_MAP_LAYER_BLENDING_TYPES,
  INVALID_MAP_LAYER_BLENDING_TYPE,
};



//----[  MapLayerBlendingTypeString  ]-----------------------------------------
const char* MapLayerBlendingTypeString(MapLayerBlendingType blending_type);




//----[  MapLocationData  ]----------------------------------------------------
struct MapLocationData {

  // During construction, this_layer is copied to last_layer after every layer
  // completes its modifications.  This prevents layers that base their behavior
  // on previous layers from inadvertently messing themselves up.
  struct {

    // Information about this location
    Evidyon::Map::EvidyonMapTile_Data rendering_data;
    Navigability navigability;
    LifeformAIIndex spawn;
    
    // Stores data about the last layer to change this location.  This is used
    // by blending layers.
    struct {
      size_t mask_index;
      D3DCOLOR mask_color;
    } last_modified_by;

  } last_layer, this_layer;

  // During construction, these are initialized to the north/east/south/west
  // neighbors of this location on the map to facilitate blending operations.
  // These pointers are ALWAYS valid--even on the edges of the map.  Edges will
  // all point to a default location that is entirely filled with invalid
  // attributes.  For the default location, all of these pointers just point
  // back to the structure itself.
  const MapLocationData* neighbors_nesw[4];

  // Returns the blending type that should be applied to this location, if any.
  // This method checks the square of 9 locations centered on this one to see
  // if they match one of the defined blending types.
  MapLayerBlendingType getLayerBlendingType(size_t inner_mask_index,
                                            D3DCOLOR inner_mask_color,
                                            size_t outer_mask_index,
                                            D3DCOLOR outer_mask_color);
};




//----[  MapCompilationContext  ]----------------------------------------------
struct MapCompilationContext {
  unsigned int width, height;
  MapLocationData* locations;

  // Edge locations reference this location
  MapLocationData default_location;

  // Gets the map location data structure for the given coordinates.  If the
  // coordinates are beyond the size of the map, this method returns a
  // reference to the default location.
  MapLocationData* at(int x, int y);
};




//----[  MapLayer  ]-----------------------------------------------------------
class MapLayer : public dc::dcGenericResource {
public:
  static std::string staticTypeName() { return "MapLayer"; }
  typedef void (*MaskCallback)(MapLocationData* location, void* context);

public:
  MapLayer();

  // Writes the data for this layer into the compilation context.  The area to
  // modify is bounded by 'area' (do not pass a NULL pointer for 'area'!)
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context) = 0;

private:

  // Used for sequencing the layers
  dc::dcInteger priority_;
};



//----[  MapMaskColorReference  ]----------------------------------------------
// References a specific color-set in the map layer mask stack.
class MapMaskColorReference : public dc::dcResource<MapMaskColorReference> {
public:
  static std::string staticTypeName() { return "MapMaskColorReference"; }
public:
  MapMaskColorReference();

  // Calls layer->*callback for each of the locations in the referenced mask
  // that lie within 'area'.  This pointer should not ever be NULL.
  void invokeLayerCallbackForMaskLocations(
      const RECT* area,
      MapCompilationContext* compilation_context,
      MapLayer* layer,
      MapLayer::Callback callback,
      void* context);

  // Returns a string containing the defined name of the specified color
  // in the referenced mask.
  std::string toString();

private:
  dc::dcList<MapMask>::Reference mask_;
  dcx::dcxD3DColor color_;
};


//----[  TerrainMapLayer  ]----------------------------------------------------
class TerrainMapLayer : public dc::dcResource<TerrainMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "TerrainMapLayer"; }
public:
  TerrainMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);

private:
  MapMaskColorReference mask_area_;
  dc::dcBoolean randomize_orientation_;
  dc::dcList<dc::dcTable<Texture>::Reference> textures_;
};


//----[  TerrainHeightMapLayer  ]----------------------------------------------
class TerrainHeightMapLayer : public dc::dcResource<TerrainHeightMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "TerrainHeightMapLayer"; }
public:
  TerrainHeightMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
private:
  MapMaskColorReference mask_area_;
  dc::dcFloat height_;
};

//----[  NavigabilityMapLayer  ]-----------------------------------------------
class NavigabilityMapLayer : public dc::dcResource<NavigabilityMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "NavigabilityMapLayer"; }
public:
  NavigabilityMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
private:
  MapMaskColorReference mask_area_;
  dc::dcEnum<MapNavigability> navigability_;
};



//----[  FluidMapLayer  ]------------------------------------------------------
class FluidMapLayer : public dc::dcResource<FluidMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "FluidMapLayer"; }
public:
  FluidMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
private:
  MapMaskColorReference mask_area_;
  dc::dcTable<Texture>::Reference texture_;
};



//----[  WallMapLayer  ]-------------------------------------------------------
class WallMapLayer : public dc::dcResource<WallMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "WallMapLayer"; }
public:
  WallMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
private:
  MapMaskColorReference mask_area_;
  dc::dcTable<Texture>::Reference texture_;
  dc::dcEnum<WallType> wall_type_;
};



//----[  OcclusionMapLayer  ]--------------------------------------------------
// Causes the masked areas to occlude vision.
class OcclusionMapLayer : public dc::dcResource<OcclusionMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "OcclusionMapLayer"; }
public:
  OcclusionMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
private:
  MapMaskColorReference mask_area_;
};



//----[  SceneryMapLayer  ]----------------------------------------------------
class SceneryMapLayer : public dc::dcResource<SceneryMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "SceneryMapLayer"; }
public:
  SceneryMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
private:
  MapMaskColorReference mask_area_;
  dc::dcList<Scenery>::Reference scenery_;
};




//----[  TerrainBlendingMapLayer  ]--------------------------------------------
// Fills seams at the interface between colors in other map layers.  This is
// used to make nice boundaries and blending effects using defined tile types.
// From the raw texture types, this map layer will automatically rotate tiles
// to create variations if the flag specifying this behavior is set.  At the
// very least, it's only necessary to create three types to achieve a full set
// of blendings: north, northeast-outside and northeast-inside.
class TerrainBlendingMapLayer :
    public dc::dcResource<TerrainBlendingMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "TerrainBlendingMapLayer"; }
  typedef dc::dcList<dc::dcTable<Texture>::Reference> TerrainTextureReferenceList;

public:
  TerrainBlendingMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
private:
  MapLayerMaskColorReference inner_, outer_;
  
  // If this flag is set, the layer will automatically create rotations of
  // the defined blending types to create more unique types of blends.
  dc::dcBoolean create_variations_by_rotation_;

  // All types blending from inner to outer.
  TerrainTextureReferenceList blends_[NUMBER_OF_MAP_TERRAIN_BLENDING_TYPES];

  // Passed during compilation of this layer
  struct Context {
    struct Terrain {
      TextureIndex texture;
      int rotation;
    };
  };
};



//----[  LifeformAISwarmSpawnMapLayer  ]---------------------------------------
class LifeformAISwarmSpawnMapLayer :
  public dc::dcResource<LifeformAISwarmSpawnMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "LifeformAISwarmSpawnMapLayer"; }
public:
  LifeformAISwarmSpawnMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
private:
  MapLayerMaskColorReference mask_area_;
  dc::dcList<dc::dcTable<LifeformAI>::Reference> lifeform_ai_;
  dc::dcInteger density_;
};



//----[  LifeformAIInhabitantSpawn  ]------------------------------------------
class LifeformAIInhabitantSpawn
    : public dc::dcResource<LifeformAIInhabitantSpawn> {
public:
  static std::string staticTypeName() { return "LifeformAIInhabitantSpawn"; }
public:
    LifeformAIInhabitantSpawn();
private:

  // Where to spawn this inhabitant
  Location location_;

  // The inhabitant itself
  dc::dcTable<LifeformAI>::Reference lifeform_ai_;

  // If this inhabitant dies, this method specifies the amount of time it
  // takes to be re-created.  This is set to 0 for all NPCs, but is useful for
  // bosses that should only be created once in a while.
  dc::dcInteger minutes_to_respawn_;
};




//----[  LifeformAIInhabitantSpawnMapLayer  ]----------------------------------
class LifeformAIInhabitantSpawnMapLayer
  : public dc::dcResource<LifeformAIInhabitantSpawnMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "LifeformAIInhabitantSpawnMapLayer"; }
public:
  LifeformAIInhabitantSpawnMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
private:
  dc::dcList<LifeformAIInhabitantSpawn> inhabitants_;
};




//----[  Geosid  ]-------------------------------------------------------------
class Geosid : public dc::dcResource<Geosid> {
public:
  static std::string staticTypeName() { return "Geosid"; }
public:
  Geosid();

private:
  Location location_;
  dc::dcList<Scenery>::Reference scenery_;
};



//----[  Map  ]----------------------------------------------------------------
// Holds all the information about a plane of the world:
//  - monsters
//  - npcs
//  - scenery
//  - terrain
//  - fluids (water/lava/etc)
//  - navigability type
//  - named zones
//  - triggers
class Map : public dc::dcResource<Map> {
public:
  static std::string staticTypeName() { return "Map"; }
public:
  Map();

  // Generates a context used to compile the map layers
  bool createCompilationContext(MapCompilationContext** context);
  void destroyCompilationContext(MapCompilationContext* context);

  // Updates the contents of the context for the given area.  If the area is
  // NULL, an area the size of the entire map will be substituted.
  bool buildContextContents(MapCompilationContext* context, const RECT* area);


private:
  
  // All the masks that define this map
  dc::dcList<MapMask> masks_;

  // Layers that reference map masks and define everything about the map.  This
  // table is sorted by layer priority before compiling the map.
  dc::dcTable<MapLayer> layers_;

  // All of the geosids on this map
  dc::dcList<Geosid> geosids_;
};




//----[  MapLocationReference  ]-----------------------------------------------
class MapLocationReference : public dc::dcResource<MapLocationReference> {
public:
  static std::string staticTypeName() { return "MapLocationReference"; }
public:
  MapLocationReference();
private:
  dc::dcList<Map>::Reference map_;
  dc::dcInteger x_, y_;
};




//----[  MapAreaReference  ]---------------------------------------------------
class MapAreaReference : public dc::dcResource<MapAreaReference> {
public:
  static std::string staticTypeName() { return "MapAreaReference"; }
public:
  MapAreaReference();
private:
  dc::dcList<Map>::Reference map_;
  dc::dcInteger top_, left_, bottom_, right_;
};


//----[  WorldAtlas  ]---------------------------------------------------------
// Stores the list of all maps in the world, as well as global configuration
// data for Evidyon.
class WorldAtlas : public dc::dcResource<WorldAtlas> {
public:
  static std::string staticTypeName() { return "WorldAtlas"; }
public:
  WorldAtlas();
private:
  dc::dcList<Map> maps_;
  MapLocationReference good_spawn_point_, evil_spawn_point_;
};


/*
server map methods:
bool canTeleport(int x0, int y0, int x1, int y1) const;
void enterMapLocation(Actor* actor, int x, int y) const;
*/

}
}
}

#endif

#endif