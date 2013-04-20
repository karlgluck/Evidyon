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
#include "visualfxrenderer.h"
#include "visualfxinstance.h"
#include "visualfxinstance_beam.h"
#include "visualfxinstance_orbitingparticles.h"
#include "visualfxinstance_swirlingparticles.h"
#include "visualfxinstance_particleshooter.h"
#include "visualfxinstance_scenery.h"
#include "visualfxinstance_grounddecal.h"
#include "visualfxinstance_fireflycloud.h"
#include "visualfxinstance_flickeringlight.h"
#include <assert.h>
#include "common/safemacros.h"
#include "shared/evidyond3drenderstatemanager.h"
#include "shared/mesh/meshrenderer.h"


namespace Evidyon {
namespace VisualFX {




  
//----[  VisualFXRenderer  ]---------------------------------------------------
VisualFXRenderer::VisualFXRenderer() {
  zero();
}





//----[  ~VisualFXRenderer  ]--------------------------------------------------
VisualFXRenderer::~VisualFXRenderer() {
  destroy();
}





//----[  create  ]-------------------------------------------------------------
bool VisualFXRenderer::create(
    LPDIRECT3DDEVICE9 d3d_device,
    Mesh::MeshRenderer* mesh_renderer,
    Scenery::SceneryRenderer* scenery_renderer,
    Evidyon::EvidyonD3DLightManager* light_manager,
    EvidyonD3DRenderStateManager* renderstate_manager) {
  assert(d3d_device);
  assert(mesh_renderer);
  assert(scenery_renderer);
  assert(light_manager);
  assert(renderstate_manager);

  if (!particle_data_.create()) return false;

  HRESULT hr = d3d_device->CreateVertexBuffer(
      sizeof(VisualFXParticle) * MAX_VISUALFX_PARTICLES,
      D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
      D3DFVF_EVIDYON_VISUALFXPARTICLE,
      D3DPOOL_DEFAULT,
      &particles_vb_,
      NULL);

  if (FAILED(hr)) {
    return false;
  }

  (d3d_device_ = d3d_device)->AddRef();
  mesh_renderer_ = mesh_renderer;
  scenery_renderer_ = scenery_renderer;
  light_manager_ = light_manager;
  renderstate_manager_ = renderstate_manager;

  return true;
}





//----[  destroy  ]------------------------------------------------------------
void VisualFXRenderer::destroy() {
  clearVisualFXInstances();
  particle_data_.destroy();
  SAFE_RELEASE(d3d_device_);
  SAFE_RELEASE(particles_vb_);
  zero();
}





//----[  addVisualFX  ]--------------------------------------------------------
bool VisualFXRenderer::addVisualFX(size_t index, const VisualFX& visual_fx) {
  if (visual_fx_.size() != index) return false;
  visual_fx_.push_back(visual_fx);
  return true;
}




//----[  clearVisualFX  ]------------------------------------------------------
void VisualFXRenderer::clearVisualFX() {
  clearVisualFXInstances();
  visual_fx_.clear();
}





//----[  acquireVisualFX  ]----------------------------------------------------
VisualFXInstance* VisualFXRenderer::acquireVisualFX(
    VisualFXIndex visual_fx_index,
    float intensity,
    Binding::BindingPtr source_binding,
    Binding::BindingPtr target_binding) {
  assert(source_binding || target_binding);
  if (visual_fx_index >= visual_fx_.size()) return NULL;
  const VisualFX& visual_fx = visual_fx_.at(visual_fx_index);
  if (number_of_instances_ >= MAX_VISUALFX_INSTANCES) return NULL;

  VisualFXInstance* instance = NULL;
  bool succeeded = true;
  switch (visual_fx.type) {
  default: return NULL; // no instance
  case Evidyon::VisualFX::VisualFX::ORBITING_PARTICLES:
    instance = new VisualFXInstance_OrbitingParticles(
      &visual_fx.orbiting_particles);
    succeeded =
      addInstanceSubset(visual_fx.orbiting_particles.particle_texture,
                        instance);
    break;
  case Evidyon::VisualFX::VisualFX::PARTICLE_SHOOTER:
    instance = new VisualFXInstance_ParticleShooter(
      &visual_fx.particle_shooter);
    succeeded =
      addInstanceSubset(visual_fx.particle_shooter.particle_texture,
                        instance);
    break;
  case Evidyon::VisualFX::VisualFX::SWIRLING_PARTICLES:
    instance = new VisualFXInstance_SwirlingParticles(
      &visual_fx.swirling_particles);
    succeeded =
      addInstanceSubset(visual_fx.swirling_particles.particle_texture,
                        instance);
    break;
  case Evidyon::VisualFX::VisualFX::BEAM:
    instance = new VisualFXInstance_Beam(
      &visual_fx.beam);
    succeeded =
      addInstanceSubset(visual_fx.beam.end_particles_texture,
                        instance)
      && addInstanceSubset(visual_fx.beam.beam_texture,
                           instance);
    break;
  case Evidyon::VisualFX::VisualFX::SCENERY:
    instance = new VisualFXInstance_Scenery(
      &visual_fx.scenery);
    break;
  case Evidyon::VisualFX::VisualFX::GROUND_DECAL:
    instance = new VisualFXInstance_GroundDecal(
      &visual_fx.ground_decal);
    succeeded = addInstanceSubset(visual_fx.ground_decal.texture, instance);
    break;
  case Evidyon::VisualFX::VisualFX::FIREFLY_CLOUD:
    instance = new VisualFXInstance_FireflyCloud(
      &visual_fx.firefly_cloud);
    succeeded = addInstanceSubset(visual_fx.firefly_cloud.particle_texture,
                                  instance);
    break;
  case Evidyon::VisualFX::VisualFX::FLICKERING_LIGHT:
    instance = new VisualFXInstance_FlickeringLight(
      &visual_fx.flickering_light);
    break;
  }

  if (succeeded) {

    // Add this instance to the internal list
    instances_[number_of_instances_++] = instance;

    // Configure the instance's internal info
    instance->renderer_ = this;
    instance->intensity_ = intensity;

    // Add references to the instance's bindings
    instance->source_binding = source_binding;
    instance->target_binding = target_binding;

    // Pass back the newly created instance
    return instance;

  } else { // subsets couldn't be allocated
    instance->destroy();
    eraseSubsetsOf(instance);
    SAFE_DELETE(instance);
    return NULL;
  }
}





//----[  clearVisualFXInstances  ]---------------------------------------------
void VisualFXRenderer::clearVisualFXInstances() {
  for (size_t i = 0; i < number_of_instances_; ++i) {
    VisualFXInstance* instance = instances_[i];
    instance->destroy();
    //instance->source_binding.reset();
    //instance->target_binding.reset();
    SAFE_DELETE(instance);
  }
  memset(instances_, 0, sizeof(instances_));
  number_of_instances_ = 0;
  memset(instance_subsets_, 0, sizeof(instance_subsets_));
  number_of_instance_subsets_ = 0;
  instance_subsets_out_of_order_ = false;
}




//----[  update  ]-------------------------------------------------------------
void VisualFXRenderer::update(double time, double time_since_last_update) {

  // Update instances.  Erase ones that have expired and copy down pointers
  // to keep the list contiguous.
  size_t i = 0, j = 0;
  while (i < number_of_instances_) {
    VisualFXInstance* instance = instances_[i];
    if (!instance->update(time, time_since_last_update)) {
      instance->destroy();
      //instance->source_binding.reset();
      //instance->target_binding.reset();
      eraseSubsetsOf(instance);
      SAFE_DELETE(instance);
    } else {
      if (i != j) {
        instances_[j] = instance;
        instances_[i] = NULL;
      }
      ++j;
    }
    ++i;
  }

  number_of_instances_ -= i - j;
}




//----[  beginScene  ]---------------------------------------------------------
void VisualFXRenderer::beginScene() {
  if (instance_subsets_out_of_order_) {
    sortInstanceSubsets();
  }

  next_instance_subset_texture_
    = number_of_instance_subsets_ > 0 ?
      instance_subsets_[0].texture : Texture::INVALID_TEXTURE_INDEX;
  next_instance_subset_index_ = 0;
}





//----[  render  ]-------------------------------------------------------------
bool VisualFXRenderer::render(Texture::TextureIndex texture) {
  if (next_instance_subset_texture_ != texture) return true;
  size_t instance_subset_index
    = next_instance_subset_index_;

  Texture::TextureIndex next_instance_subset_texture;

  // Render all subsets with this texture
  do {
    VisualFXInstance* visual_fx_instance
      = instance_subsets_[instance_subset_index].visual_fx_instance;
    visual_fx_instance->render(texture);
    ++instance_subset_index;
    next_instance_subset_texture
    = number_of_instance_subsets_ > instance_subset_index
        ? instance_subsets_[instance_subset_index].texture
        : Texture::INVALID_TEXTURE_INDEX;
  } while (texture == next_instance_subset_texture);

  flushParticles(); // be sure all the particles were drawn

  next_instance_subset_index_ = instance_subset_index;
  next_instance_subset_texture_
    = next_instance_subset_texture;

  // Success
  return true;
}






//----[  acquireParticle  ]----------------------------------------------------
VisualFXParticleData* VisualFXRenderer::acquireParticleData() {
  return particle_data_.acquire();
}



//----[  releaseParticleData  ]------------------------------------------------
void VisualFXRenderer::releaseParticleData(VisualFXParticleData* data) {
  particle_data_.release(data);
}





//----[  lightManager  ]-------------------------------------------------------
Evidyon::EvidyonD3DLightManager* VisualFXRenderer::lightManager() {
  return light_manager_;
}





//----[  sceneryRenderer  ]----------------------------------------------------
Scenery::SceneryRenderer* VisualFXRenderer::sceneryRenderer() {
  return scenery_renderer_;
}



//----[  renderParticle  ]-----------------------------------------------------
void VisualFXRenderer::renderParticle(const VisualFXParticle* particle) {
  HRESULT hr;
  if (particles_remaining_ <= 0) {
      flushParticles();
      hr = particles_vb_->Lock(0, 0, (void**)&particles_, D3DLOCK_DISCARD);
      particles_consumed_ = 0;
      particles_rendered_ = 0;
      particles_remaining_ = MAX_VISUALFX_PARTICLES;
      if (FAILED(hr)) return;
  } else {
    if (!particles_) {
      // Re-lock the VB.  This should only happen after the VB has been
      // unlocked--which only happens when the particles are flushed (rendered)
      assert(particles_consumed_ == 0);
      hr = particles_vb_->Lock(sizeof(VisualFXParticle) * particles_rendered_,
                               sizeof(VisualFXParticle) * particles_remaining_,
                               (void**)&particles_,
                               D3DLOCK_NOOVERWRITE);
      if (FAILED(hr)) return;
    }
  }

  memcpy(particles_ + particles_consumed_,
         particle,
         sizeof(VisualFXParticle));
  --particles_remaining_;
  ++particles_consumed_;
}





//----[  renderDynamicGeometry  ]----------------------------------------------
void VisualFXRenderer::renderDynamicGeometry(
    const Mesh::GeometryVertex* vertices,
    unsigned int number_of_vertices,
    const Mesh::GeometryIndex* indices,
    unsigned int number_of_indices) {
  assert(vertices);
  assert(indices);
  if (mesh_renderer_->initializeDynamicGeometry(vertices,
                                                number_of_vertices,
                                                indices,
                                                number_of_indices)) {
    renderstate_manager_->setGeometryTexture(next_instance_subset_texture_,
                                             false);
    renderstate_manager_->setWorldMatrix(NULL, 0);
    mesh_renderer_->renderInstance(renderstate_manager_);
  }
}



//----[  zero  ]---------------------------------------------------------------
void VisualFXRenderer::zero() {
  scenery_renderer_ = NULL;
  d3d_device_ = NULL;
  light_manager_ = NULL;
  memset(instances_, 0, sizeof(instances_));
  number_of_instances_ = 0;
  particles_vb_ = NULL;
  particles_ = NULL;
  particles_consumed_ = 0;
  particles_rendered_ = 0;
  particles_remaining_ = 0;
  memset(instance_subsets_, 0, sizeof(instance_subsets_));
  number_of_instance_subsets_ = 0;
  instance_subsets_out_of_order_ = false;
  next_instance_subset_texture_ = Texture::INVALID_TEXTURE_INDEX;
  next_instance_subset_index_ = 0;
}




//----[  EvidyonVisualFXRenderer_CompareParticleSubsets  ]---------------------
int EvidyonVisualFXRenderer_CompareParticleSubsets(const void* p1,
                                                   const void* p2) {
  using namespace Evidyon::Scenery;
  const VisualFXInstanceSubset* a =
    (const VisualFXInstanceSubset*)p1;
  const VisualFXInstanceSubset* b =
    (const VisualFXInstanceSubset*)p2;
  Evidyon::Texture::TextureIndex a_texture = a->texture;
  Evidyon::Texture::TextureIndex b_texture = b->texture;
  if (a_texture > b_texture) return +1; // sort by ascending texture
  if (a_texture < b_texture) return -1;
  return 0;
}




//----[  sortInstanceSubsets  ]------------------------------------------------
void VisualFXRenderer::sortInstanceSubsets() {
  if (number_of_instance_subsets_ > 0) {
    qsort(instance_subsets_,
          number_of_instance_subsets_,
          sizeof(VisualFXInstanceSubset),
          EvidyonVisualFXRenderer_CompareParticleSubsets);
  }
  instance_subsets_out_of_order_ = false;
}








//----[  eraseSubsetsOf  ]-----------------------------------------------------
void VisualFXRenderer::eraseSubsetsOf(VisualFXInstance* instance) {
  // Go through the list and copy down members as necessary to overwrite
  // subsets of the instance that was removed.
  // Variable 'i' is the scanner and 'j' is the placer. 'j' will lag 'i' by one
  // more place each time an instance is erased so that valid pointers get
  // copied down in the list.
  // This code was taken from the scenery renderer.
  size_t i = 0, j = 0;
  while (i < number_of_instance_subsets_) {
    VisualFXInstanceSubset* subset = &instance_subsets_[i];
    if (subset->visual_fx_instance == instance) { 
      // this subset should be removed
    } else {
      if (i != j) {
        memcpy(instance_subsets_ + j,
               subset,
               sizeof(VisualFXInstanceSubset));
      }
      ++j;
    }
    ++i;
  }
  number_of_instance_subsets_ -= i - j;
}




//----[  flushParticles  ]-----------------------------------------------------
void VisualFXRenderer::flushParticles() {
  if (particles_ == NULL) return; // not locked? no particles.
  particles_vb_->Unlock();
  particles_ = NULL;
  renderstate_manager_->setStream0GeometryBuffers(
    particles_vb_,
    D3DFVF_EVIDYON_VISUALFXPARTICLE,
    sizeof(VisualFXParticle),
    NULL); // TODO: optimize by making method not SetIndices for ib_ == NULL
  renderstate_manager_->setParticleTexture(next_instance_subset_texture_);
  renderstate_manager_->setWorldMatrix(0, NULL);
  d3d_device_->DrawPrimitive(D3DPT_POINTLIST,
                             particles_rendered_,
                             particles_consumed_);
  particles_rendered_ += particles_consumed_;
  particles_consumed_ = 0;
}



//----[  addInstanceSubset  ]--------------------------------------------------
bool VisualFXRenderer::addInstanceSubset(Texture::TextureIndex texture,
                                         VisualFXInstance* instance) {
  if (number_of_instance_subsets_ >= MAX_VISUALFX_INSTANCE_SUBSETS) {
   return false;
  }
  instance_subsets_[number_of_instance_subsets_].texture
    = texture;
  instance_subsets_[number_of_instance_subsets_].visual_fx_instance
    = instance;
  number_of_instance_subsets_++;
  instance_subsets_out_of_order_ = true;
  return true;
}



}
}


