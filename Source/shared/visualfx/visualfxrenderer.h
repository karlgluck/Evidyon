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
#ifndef __SHARED_VISUALFX_VISUALFXRENDERER_H__
#define __SHARED_VISUALFX_VISUALFXRENDERER_H__
#pragma once



#include "shared/texture/textureindex.h"
#include "visualfxparticledata.h"
#include "visualfxparticle.h"
#include "visualfx.h"
#include "visualfxindex.h"
#include <dcx/memorypool>
#include <d3d9.h>
#include <vector>
#include "shared/binding/bindingptr.h"


namespace Evidyon {
class EvidyonD3DLightManager;
class EvidyonD3DRenderStateManager;
namespace Mesh {
class MeshRenderer;
struct GeometryVertex;
typedef unsigned short GeometryIndex;
}
namespace Scenery{
class SceneryRenderer;
}
}


namespace Evidyon {
namespace VisualFX {
class VisualFXInstance;




//----[  VisualFXInstanceSubset  ]---------------------------------------------
struct VisualFXInstanceSubset {
  Texture::TextureIndex texture;
  VisualFXInstance* visual_fx_instance;
};


//----[  VisualFXRenderer  ]---------------------------------------------------
class VisualFXRenderer {
  static const int MAX_VISUALFX_PARTICLES = 4096;
  typedef dcx::dcxMemoryPool<VisualFXParticleData,
                             MAX_VISUALFX_PARTICLES> ParticleDataPool;

  // Controls how many effects can be instantiated simultaneously.
  static const int MAX_VISUALFX_INSTANCES = 128;

  // The maximum number of subsets of VisualFX instances that render dynamic
  // geometry. Since a single instance can render more than one texture
  // type, it is possible to run out of subsets before the maximum number
  // of instances is allocated.  In this case, the instance allocation will
  // fail even though it is the subset allocation that caused the problem.
  static const int MAX_VISUALFX_INSTANCE_SUBSETS = MAX_VISUALFX_INSTANCES;

  typedef std::vector<Evidyon::VisualFX::VisualFX> VisualFXArray;

public:
  VisualFXRenderer();
  ~VisualFXRenderer();

  // Initializes internal objects
  bool create(LPDIRECT3DDEVICE9 d3d_device,
              Mesh::MeshRenderer* mesh_renderer,
              Scenery::SceneryRenderer* scenery_renderer,
              Evidyon::EvidyonD3DLightManager* light_manager,
              EvidyonD3DRenderStateManager* renderstate_manager);

  // Cleans up allocated memory
  void destroy();

  // Adds a new visual FX type to the renderer
  bool addVisualFX(size_t index, const VisualFX& visual_fx);

  // Remove all visual FX descriptions, as well as clearing all instances
  void clearVisualFX();

  // Creates a new FX instance and adds it to the renderer.  The FX object will
  // continue to render until the binding to which it is attached expires and
  // it cleans up smoothly (for example, particles fade out instead of just
  // popping out of existance)
  // This method will add a reference to valid bindings if the FX was created.
  // At least one of the bindings must be valid, otherwise this method does
  // nothing.
  // The intensity is a value in the range [0, 1] whose behavior is determined
  // by the implementing effect.  This is usually used for spell effects that
  // change their appearance (more particles, wider beams) based on the
  // power of the caster.  The default value should be 1.0.
  // The return value is either NULL, or a valid FX instance.
  VisualFXInstance* acquireVisualFX(VisualFXIndex visual_fx_index,
                                    float intensity,
                                    Binding::BindingPtr source_binding,
                                    Binding::BindingPtr target_binding);

  // Erases all instances from the manager and cleans up all memory they used.
  void clearVisualFXInstances();

  // Advance the state of the VisualFX instances
  void update(double time, double time_since_last_update);

  // Resets the renderer at the start of a new scene
  void beginScene();

  // Draws all dynamic visual FX elements that use the given texture.
  bool render(Texture::TextureIndex texture);

  // Obtains a particle from the internal memory pool.  If this method reutrns
  // NULL, the maximum number of particles have already been allocated.
  VisualFXParticleData* acquireParticleData();
  void releaseParticleData(VisualFXParticleData* data);

  // Accesses the internal reference to the light manager.  This is used by
  // visual FX that want to control lighting--either by allocating light
  // objects, or by (rarely) controlling ambient lighting.
  Evidyon::EvidyonD3DLightManager* lightManager();

  Scenery::SceneryRenderer* sceneryRenderer();

  // Pushes a particle into the current rendering stack.  This method is
  // called by instances during their render() processing.
  void renderParticle(const VisualFXParticle* particle);

  // Draws dynamic geometry.  This method will reset the world transformation
  // before drawing takes place.  Called by instances during their render()
  // processing.
  void renderDynamicGeometry(const Mesh::GeometryVertex* vertices,
                             unsigned int number_of_vertices,
                             const Mesh::GeometryIndex* indices,
                             unsigned int number_of_indices);

private:

  // Erase all contents
  void zero();

  // Puts the particle subsets in order
  void sortInstanceSubsets();

  // Removes all of the particle subsets belonging to the given instance
  // from the buffer.  Maintains the current order of the subsets.
  void eraseSubsetsOf(VisualFXInstance* instance);

  // Invoked to draw any particles in the buffer using the current texture.
  void flushParticles();

  // Adds a new subset to the internal list
  bool addInstanceSubset(Texture::TextureIndex texture,
                         VisualFXInstance* instance);

private:

  // Stores the types of visual FX
  VisualFXArray visual_fx_;

  // Device used for rendering particles
  LPDIRECT3DDEVICE9 d3d_device_;

  // Renderstate manager used to set particles textures and update
  // the device when rendering.
  EvidyonD3DRenderStateManager* renderstate_manager_;

  // The scenery renderer is used to draw geometry visual FX that are based on
  // transformations of some mesh type.  This can be stuff like a shockwave--
  // just make a doughnut-shaped textured mesh, then write an FX type that
  // causes its transformation matrix to explode!
  // This can also be used for more simple things like an arrow.  An arrow is
  // actually a VisualFX object: it's just a scenery VisualFX type bound to
  // a projectile.
  Scenery::SceneryRenderer* scenery_renderer_;

  // Mesh renderer is used to draw dynamic geometry
  Mesh::MeshRenderer* mesh_renderer_;

  // Allows visual FX to control lighting
  Evidyon::EvidyonD3DLightManager* light_manager_;

  // Lists all FX instances that have been acquired.  Each update(), the
  // same-named method is called on the instance and expired instances are
  // deallocated & removed from this list.
  VisualFXInstance* instances_[MAX_VISUALFX_INSTANCES];
  size_t number_of_instances_;

  // Data source for particles used by instances
  ParticleDataPool particle_data_;

  // Vertex buffer for rendering point-particles
  LPDIRECT3DVERTEXBUFFER9 particles_vb_;
  VisualFXParticle* particles_;
  int particles_consumed_;
  int particles_rendered_;
  int particles_remaining_;

  // Keeps a sorted list of the FX objects that render particles
  VisualFXInstanceSubset instance_subsets_[MAX_VISUALFX_INSTANCE_SUBSETS];
  size_t number_of_instance_subsets_;
  bool instance_subsets_out_of_order_;

  // These state variables are used while rendering
  Texture::TextureIndex next_instance_subset_texture_;
  size_t next_instance_subset_index_;
};

}
}


#endif