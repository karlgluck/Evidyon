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
#ifndef __SHARED_SCENERY_SCENERYRENDERER_H__
#define __SHARED_SCENERY_SCENERYRENDERER_H__
#pragma once



#include <d3dx9.h>
#include <vector>

// The scenery instances are stored in a memory pool
#include <dcx/memorypool>

#include "shared/texture/textureindex.h"
#include "shared/mesh/meshindex.h"
#include "sceneryindex.h"
#include "shared/visualfx/visualfxindex.h"
#include "scenery.h"
#include "sceneryinstance.h"


namespace Evidyon {
class EvidyonD3DRenderStateManager;

namespace Mesh {
class MeshRenderer;
}

namespace VisualFX {
class VisualFXRenderer;
}
}


namespace Evidyon {
namespace Scenery {


//----[  SceneryInstanceRenderableSubset  ]------------------------------------
// An entry of this type is stored in an array for each subset of each mesh
// of every scenery instance.  Sorting this array optimizes draw calls by
// putting like textures and meshes together for the renderer.
// Note: hidden instances are just skipped during rendering, not sorted to
// the back, since objects can potentially flash in and out of view rapidly
// and such frequent sorts would be determental to performance.
struct SceneryInstanceRenderableSubset {
  Texture::TextureIndex texture;
  Mesh::MeshIndex mesh;
  Mesh::MeshSubsetIndex subset;
  SceneryInstance* instance;
};




//----[  SceneryRenderer  ]----------------------------------------------------
class SceneryRenderer {
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
  SceneryRenderer();

  // Allocates the memory pool for this structure and assigns the renderer
  // it will use for meshes.
  bool create(Mesh::MeshRenderer* mesh_renderer,
              VisualFX::VisualFXRenderer* visual_fx_renderer);

  // Frees all allocated objects, including all scenery objects.  
  void destroy();

  // Puts a new type of scenery into this renderer
  bool addScenery(SceneryIndex index, const Scenery& scenery);

  // Erases all scenery type definitions and instances.
  void clearScenery();

  // Replaces the texture used by a scenery element with another one.  This
  // method is safe to call at any time--all renderable elements are
  // automatically updated.
  // This was created for use by the scenery-editing dialog.
  bool replaceScenerySubsetTexture(SceneryIndex scenery_index,
                                   Mesh::MeshSubsetIndex subset,
                                   Texture::TextureIndex new_texture);

  // Generates a new scenery instance and adds its subsets to the renderer. 
  // If this method returns 'NULL', it couldn't get enough space in the
  // renderabe subsets buffer for the scenery.  Try increasing the value
  // of MAX_RENDERABLE_SUBSETS.
  // This method does not initialize the scenery's local transform.
  SceneryInstance* acquire(SceneryIndex scenery_index);

  // Completely erases a scenery instance.  After calling this method, do
  // not try to dereference the provided pointer.
  void release(SceneryInstance* instance);

  // Erases all scenery instances.  Any outstanding scenery
  // instance pointers will be invalidated.
  void clearSceneryInstances();

  // Resets the renderer at the start of a new scene
  void beginScene();

  // Draws all scenery elements that use the given texture.
  bool render(Texture::TextureIndex texture,
              EvidyonD3DRenderStateManager* renderstate_manager);


private:

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
  Mesh::MeshRenderer* mesh_renderer_;

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
  Texture::TextureIndex next_texture_;
  size_t next_renderable_subset_pointer_index_;

  // Draws the visual FX that are attached to scenery
  VisualFX::VisualFXRenderer* visual_fx_renderer_;
};

}
}

#endif