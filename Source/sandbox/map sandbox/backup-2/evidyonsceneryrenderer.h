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
// Scenery consists of a mesh-geometry element with a texture specified for
// each subset, a local transformation for that mesh and a special effect to
// attach when the scenery is instantiated.  A scenery instance holds the
// special effect reference and contains a combined transformation
// (word + local) matrix that is used to render each scenery subset.
//-----------------------------------------------------------------------------
#ifndef __EVIDYONSCENERYRENDERER_H__
#define __EVIDYONSCENERYRENDERER_H__

#include <d3dx9.h>
#include <vector>

// The scenery instances are stored in a memory pool
#include <dcx/memorypool>

#include "../shared/evidyontextureindextype.h"
#include "../shared/evidyonmeshindextype.h"


namespace Evidyon {
class EvidyonD3DRenderStateManager;

namespace Meshes {
class EvidyonMeshRenderer;
}

namespace Scenery {

typedef unsigned int SceneryIndex;
static const SceneryIndex INVALID_SCENERY_INDEX = SceneryIndex(-1);

  
//----[  Scenery  ]------------------------------------------------------------
struct Scenery {

  // Geometry to render for this scenery
  Meshes::MeshIndex mesh;

  // The number of entries in this array is determined by the mesh type
  TextureIndex* subset_textures;

  // Local reconfiguration of the geometry.  Multiplied into the combined
  // transform when a scenery instance's location is updated.
  D3DXMATRIX/*A16*/ local_transform;

  // What kind of special effect to attach to this scenery.  This is to allow
  // for cool effects like sparkly swords, burning torches, etc.
  //SpecialFXIndex special_fx;
};



//----[  SceneryInstance  ]----------------------------------------------------
struct SceneryInstance {

  // Source data for this instance to draw
  const Scenery* scenery;

  // Set this flag to prevent ths instance from being rendered.  This will
  // also hide the special FX bound to this instance, if any.
  bool hidden;

  // Transform used to render the scenery.  This is the combination of the
  // word and local matrices, so it's updated every time the scenery moves.
  D3DXMATRIXA16 combined_transform;

  // Initializes combined_transform based on the provided word matrix and the
  // scenery-specified local transform.
  void setTransform(const D3DXMATRIX* world_matrix);

  // Special effect attached to the scenery.  When the scenery is destroyed,
  // this effect is unbound automatically.
  //SpecialFXBinding* special_fx_binding;
};



//----[  SceneryInstanceRenderableSubset  ]------------------------------------
// An entry of this type is stored in an array for each subset of each mesh
// of every scenery instance.  Sorting this array optimizes draw calls by
// putting like textures and meshes together for the renderer.
// Note: hidden instances are just skipped during rendering, not sorted to
// the back, since objects can potentially flash in and out of view rapidly
// and such frequent sorts would be determental to performance.
struct SceneryInstanceRenderableSubset {
  TextureIndex texture;
  Meshes::MeshIndex mesh;
  Meshes::MeshSubsetIndex subset;
  SceneryInstance* instance;
};



//----[  EvidyonSceneryRenderer  ]---------------------------------------------
class EvidyonSceneryRenderer {
  typedef std::vector<Scenery> SceneryArray;

  // This value bounds the number of scenery instances that can be outstanding
  // at any given time.  The memory pool for scenery is limited to this size,
  // and trying to allocate any more instance than this will aways fail.
  static const int MAX_SCENERY_INSTANCES = 512;
  typedef dcx::dcxMemoryPool<SceneryInstance,
                             MAX_SCENERY_INSTANCES> SceneryInstances;

  // This specifies the maximum number of scenery subsets that can be present
  // in the buffer simultaneously.  Hidden scenery DOES take up space in this
  // buffer, and scenery with N subsets takes up N spots.  Since N>=1 always,
  // this value shoud be greater than MAX_SCENERY_INSTANCES.
  static const int MAX_RENDERABLE_SUBSETS = 1024;
  typedef dcx::dcxMemoryPool<SceneryInstanceRenderableSubset,
                             MAX_RENDERABLE_SUBSETS> RenderableSubsets;



public:
  EvidyonSceneryRenderer();

  // Allocates the memory pool for this structure and assigns the renderer
  // it will use for meshes.
  bool create(Meshes::EvidyonMeshRenderer* mesh_renderer);

  // Frees all allocated objects, including all scenery objects.  
  void destroy();

  // Puts a new type of scenery into this renderer
  bool addScenery(SceneryIndex index, const Scenery& scenery);

  // Generates a new scenery instance and adds its subsets to the renderer. 
  // If this method returns 'NULL', it couldn't get enough space in the
  // renderabe subsets buffer for the scenery.  Try increasing the value
  // of MAX_RENDERABLE_SUBSETS.
  SceneryInstance* acquire(SceneryIndex scenery_index);

  // Completely erases a scenery instance.  After calling this method, do
  // not try to dereference the provided pointer.
  void release(SceneryInstance* instance);

  // Erases all scenery instances.  Any outstanding scenery
  // instance pointers will be invalidated.
  void clear();

  // Resets the renderer at the start of a new scene
  void beginScene();

  // Draws all scenery elements that use the given texture.
  bool render(TextureIndex texture,
              EvidyonD3DRenderStateManager* renderstate_manager);


public:

  // Erase all contents
  void zero();

  // Puts the renderable subsets in order
  void sortRenderableSubsets();

  // Removes all of the renderable subsets belonging to the given instance
  // from the buffer.  The implementation does *not* cause the subsets to be
  // re-ordered.
  void eraseRenderableSubsetsOf(SceneryInstance* instance);


private:

  // This is the data source used to render the meshes to the screen
  Meshes::EvidyonMeshRenderer* mesh_renderer_;

  // All the scenery held in this renderer
  SceneryArray scenery_;

  // Pool of memory from which new scenery instances are allocated.
  SceneryInstances instances_;

  // Pool of memory from which new subsets are drawn
  RenderableSubsets renderable_subsets_;

  // Buffer of instantiated scenery subsets
  SceneryInstanceRenderableSubset*
    renderable_subset_pointers_[MAX_RENDERABLE_SUBSETS];
  size_t number_of_renderable_subsets_;

  // Whether or not the renderable subsets buffer has rearranged
  // since the last render call.  When this flag is set, the
  // subset buffer must be re-sorted before rendering.
  bool renderable_subsets_out_of_order_;

  // These state variables are used while rendering
  TextureIndex next_texture_;
  size_t next_renderable_subset_pointer_index_;
};

}
}

#endif