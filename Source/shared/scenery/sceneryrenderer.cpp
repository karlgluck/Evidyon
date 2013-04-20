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
#include "sceneryrenderer.h"
#include "shared/mesh/meshrenderer.h"
#include "shared/visualfx/visualfxrenderer.h"
#include "shared/evidyond3drenderstatemanager.h"
#include "common/safemacros.h"
#include <assert.h>
#include "shared/scenery/scenery.h"
#include "shared/binding/scenerybinding.h"



namespace Evidyon {
namespace Scenery {





//----[  SceneryRenderer  ]----------------------------------------------------
SceneryRenderer::SceneryRenderer() {
  zero();
}






//----[  create  ]-------------------------------------------------------------
bool SceneryRenderer::create(
    Mesh::MeshRenderer* mesh_renderer,
    VisualFX::VisualFXRenderer* visual_fx_renderer) {
  assert(mesh_renderer);
  assert(visual_fx_renderer);
  if (!instances_.create() || !renderable_subsets_.create()) {
    destroy();
    return false;
  }
  mesh_renderer_ = mesh_renderer;
  visual_fx_renderer_ = visual_fx_renderer;
  return true;
}





//----[  destroy  ]------------------------------------------------------------
void SceneryRenderer::destroy() {
  clearScenery(); // also calls clearSceneryInstances
  instances_.destroy();
  renderable_subsets_.destroy();
  zero();
}





//----[  addScenery  ]---------------------------------------------------------
bool SceneryRenderer::addScenery(SceneryIndex index,
                                        const Scenery& scenery) {
  if (scenery_.size() != index) return false;
  scenery_.push_back(scenery);
  return true;
}


//----[  clearScenery  ]-------------------------------------------------------
void SceneryRenderer::clearScenery() {
  clearSceneryInstances();
  for (SceneryArray::iterator i = scenery_.begin(); i != scenery_.end(); ++i) {
    Scenery& scenery = (*i);
    SAFE_DELETE_ARRAY(scenery.subset_textures);
  }
  scenery_.clear();
}



//----[  replaceScenerySubsetTexture  ]----------------------------------------
bool SceneryRenderer::replaceScenerySubsetTexture(
    SceneryIndex scenery_index,
    Mesh::MeshSubsetIndex subset,
    Texture::TextureIndex new_texture) {
  if (scenery_.size() < scenery_index) return false;
  Scenery& scenery = scenery_.at(scenery_index);
  Mesh::MeshSubsetIndex subsets =
    mesh_renderer_->getNumberOfMeshSubsets(scenery.mesh);
  if (subset >= subsets) return false;
  scenery.subset_textures[subset] = new_texture;

  // Replace all existing scenery elements
  for (size_t i = 0; i < number_of_renderable_subsets_; ++i) {
    SceneryInstanceRenderableSubset* renderable_subset =
      renderable_subset_pointers_[i];
    if (renderable_subset->instance->scenery != &scenery ||
        renderable_subset->mesh != scenery.mesh ||
        renderable_subset->subset != subset) continue;
    renderable_subset->texture = new_texture;
    break;
  }

  // Re-sort the subsets next render cycle
  renderable_subsets_out_of_order_ = true;

  return true;
}


//----[  acquire  ]------------------------------------------------------------
SceneryInstance* SceneryRenderer::acquire(SceneryIndex scenery_index) {
  assert(scenery_index < scenery_.size());

  const Scenery& scenery = scenery_.at(scenery_index);

  // Be sure there is room for this scenery's subsets
  Mesh::MeshSubsetIndex subsets =
    mesh_renderer_->getNumberOfMeshSubsets(scenery.mesh);
  if (subsets + number_of_renderable_subsets_ > MAX_RENDERABLE_SUBSETS) {
    return NULL;
  }

  // Get a new scenery instance.  It is valid behavior for this to fail--the
  // user can sometimes request too many instances of scenery.
  SceneryInstance* instance = instances_.acquire();
  if (!instance) return NULL;

  // Initialize the instance
  instance->scenery = &scenery;
  instance->hidden = false;

  // Generate a binding for the visualFX if it exists
  if (scenery.visual_fx != VisualFX::INVALID_VISUALFX_INDEX) {
    instance->visual_fx_binding
      = Binding::SceneryBinding::New(instance);
    visual_fx_renderer_->acquireVisualFX(scenery.visual_fx,
                                         1.0f,
                                         instance->visual_fx_binding,
                                         instance->visual_fx_binding);
  } else {
    instance->visual_fx_binding.reset();
  }


  // Put all of this scenery's subsets into the internal buffer
  for (Mesh::MeshSubsetIndex i = 0; i < subsets; ++i) {
    SceneryInstanceRenderableSubset* subset = renderable_subsets_.acquire();
    assert(subset);
    renderable_subset_pointers_[number_of_renderable_subsets_++] = subset;
    subset->instance = instance;
    subset->mesh = scenery.mesh;
    subset->subset = i;
    subset->texture = scenery.subset_textures[i];
  }

  // Re-sort the subsets
  renderable_subsets_out_of_order_ = true;

  // Return the newly created scenery instance
  return instance;
}






//----[  release  ]------------------------------------------------------------
void SceneryRenderer::release(SceneryInstance* instance) {
  assert(instance);
  Binding::SceneryBinding* binding = instance->visual_fx_binding.get();
  if (binding) binding->releaseScenery();
  instance->visual_fx_binding.reset();
  eraseRenderableSubsetsOf(instance);
  instances_.release(instance);
}



//----[  clearSceneryInstances  ]----------------------------------------------
void SceneryRenderer::clearSceneryInstances() {
  for (SceneryInstances::Iterator i(&instances_); i.hasNext(); i.advance()) {
    SceneryInstance* instance = i.get();
    Binding::SceneryBinding* binding = instance->visual_fx_binding.get();
    if (binding) binding->releaseScenery();
    instance->visual_fx_binding.reset();
  }
  instances_.releaseAll();
  renderable_subsets_.releaseAll();
  number_of_renderable_subsets_ = 0;
  renderable_subsets_out_of_order_ = false;
  beginScene(); // reset the scene-state variables
}




//----[  beginScene  ]---------------------------------------------------------
void SceneryRenderer::beginScene() {
  if (renderable_subsets_out_of_order_) {
    sortRenderableSubsets();
  }

  next_texture_ = number_of_renderable_subsets_ > 0
    ? renderable_subset_pointers_[0]->texture : Texture::INVALID_TEXTURE_INDEX;
  next_renderable_subset_pointer_index_ = 0;
}



//----[  render  ]-------------------------------------------------------------
bool SceneryRenderer::render(
    Texture::TextureIndex texture,
    EvidyonD3DRenderStateManager* renderstate_manager) {
  if (texture != next_texture_) return true;
  size_t next_renderable_subset_pointer_index
    = next_renderable_subset_pointer_index_;
  SceneryInstanceRenderableSubset** renderable_subset_pointer
    = &renderable_subset_pointers_[next_renderable_subset_pointer_index];
  SceneryInstanceRenderableSubset* renderable_subset;
  Mesh::MeshIndex current_mesh_index
    = Mesh::INVALID_MESH_INDEX;
  Mesh::MeshSubsetIndex current_subset_index
    = Mesh::INVALID_MESH_SUBSET_INDEX;
  while ((next_renderable_subset_pointer_index < number_of_renderable_subsets_) &&
         texture == (renderable_subset = *renderable_subset_pointer)->texture) {
    ++next_renderable_subset_pointer_index;
    ++renderable_subset_pointer;

    // Skip hidden instances
    SceneryInstance* instance = renderable_subset->instance;
    if (instance->hidden) continue;

    // Change to the new mesh, if necessary
    Mesh::MeshIndex this_mesh_index = renderable_subset->mesh;
    Mesh::MeshSubsetIndex this_subset_index = renderable_subset->subset;
    if (current_mesh_index != this_mesh_index ||
        current_subset_index != this_subset_index) {
      renderstate_manager->setGeometryTexture(texture, true);
      mesh_renderer_->initializeMeshGeometry(this_mesh_index, this_subset_index);
      current_mesh_index = this_mesh_index;
      current_subset_index = this_subset_index;
    }

    // Set the instance's transform
    renderstate_manager->setWorldMatrix(&instance->combined_transform, 0);

    // Draw the mesh
    if (!mesh_renderer_->renderInstance(renderstate_manager)) return false;
  }

  // Calculate entries for the next rendering loop
  next_texture_ =
    next_renderable_subset_pointer_index < number_of_renderable_subsets_ ?
    renderable_subset->texture : Texture::INVALID_TEXTURE_INDEX;
  next_renderable_subset_pointer_index_ = next_renderable_subset_pointer_index;

  // Success
  return true;
}





//----[  zero  ]---------------------------------------------------------------
void SceneryRenderer::zero() {
  mesh_renderer_ = NULL;
  scenery_.clear();
  memset(renderable_subset_pointers_, 0, sizeof(renderable_subset_pointers_));
  number_of_renderable_subsets_ = 0;
  renderable_subsets_out_of_order_ = false;
  visual_fx_renderer_ = NULL;
}






//----[  EvidyonSceneryRenderer_CompareRenderableSubsetPtrs  ]-----------------
int EvidyonSceneryRenderer_CompareRenderableSubsetPtrs(const void* p1,
                                                       const void* p2) {
  using namespace Evidyon::Scenery;
  const SceneryInstanceRenderableSubset* a =
    *(const SceneryInstanceRenderableSubset**)p1;
  const SceneryInstanceRenderableSubset* b =
    *(const SceneryInstanceRenderableSubset**)p2;
  Evidyon::Texture::TextureIndex a_texture = a->texture;
  Evidyon::Texture::TextureIndex b_texture = b->texture;
  if (a_texture > b_texture) return +1; // sort by ascending texture
  if (a_texture < b_texture) return -1;
  if (a->mesh > b->mesh) return +1;
  if (a->mesh < b->mesh) return -1;
  if (a->subset > b->subset) return +1;
  if (a->subset < b->subset) return -1;
  return 0;
}




//----[  sortRenderableSubsets  ]----------------------------------------------
void SceneryRenderer::sortRenderableSubsets() {
  if (number_of_renderable_subsets_ > 0) {
    qsort(renderable_subset_pointers_,
          number_of_renderable_subsets_,
          sizeof(SceneryInstanceRenderableSubset*),
          EvidyonSceneryRenderer_CompareRenderableSubsetPtrs);
  }
  renderable_subsets_out_of_order_ = false;
}







//----[  eraseRenderableSubsetsOf  ]-------------------------------------------
void SceneryRenderer::eraseRenderableSubsetsOf(SceneryInstance* instance) {
  // Go through the list and copy down members as necessary to overwrite
  // subsets of the instance that was removed.
  // Variable 'i' is the scanner and 'j' is the placer. 'j' will lag 'i' by one
  // more place each time an instance is erased so that valid pointers get
  // copied down in the list.
  size_t i = 0, j = 0;
  while (i < number_of_renderable_subsets_) {
    SceneryInstanceRenderableSubset* subset =
      renderable_subset_pointers_[i];
    if (subset->instance == instance) { 
      renderable_subsets_.release(subset);
    } else {
      if (i != j) renderable_subset_pointers_[j] = subset;
      ++j;
    }
    ++i;
  }
  number_of_renderable_subsets_ -= i - j;
  assert(number_of_renderable_subsets_ ==
         renderable_subsets_.numberOfAllocatedEntries());
}




}
}
