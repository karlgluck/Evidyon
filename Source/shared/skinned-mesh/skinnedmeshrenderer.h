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
#ifndef __SHARED_SKINNEDMESH_SKINNEDMESHRENDERER_H__
#define __SHARED_SKINNEDMESH_SKINNEDMESHRENDERER_H__
#pragma once



#include "skinnedmesh.h"
#include "skinnedmeshinstance.h"
#include <dcx/memorypool>
#include <vector>
#include "skinnedmeshindex.h"
#include "skinnedmeshformindex.h"
#include "skinnedmeshanimationindex.h"
#include "shared/animated-mesh/animatedmeshindex.h"
#include "shared/texture/textureindex.h"

struct D3DXMATRIX;

namespace Evidyon {
class EvidyonD3DRenderStateManager;
namespace AnimatedMesh {
class AnimatedMeshRenderer;
struct AnimatedMeshAnimationTrack;
}
}

namespace Evidyon {
namespace SkinnedMesh {


//----[  SkinnedMeshInstanceRenderableElement  ]-------------------------------
// The texture group and subset indices were made into 2 bytes each so that the
// structure's overall size would be 16 bytes, which allows optimal alignment.
struct SkinnedMeshInstanceRenderableElement {
  Texture::TextureIndex texture_group_texture;    // 1st sort priority
  AnimatedMesh::AnimatedMeshIndex animated_mesh;  // 2nd
  unsigned short texture_group;                   // 3rd
  unsigned short texture_group_subset;            // 4th
  SkinnedMeshInstance* instance;
};




//----[  SkinnedMeshRenderer  ]------------------------------------------------
class SkinnedMeshRenderer {
  typedef std::vector<SkinnedMesh> SkinnedMeshArray;

  // This value bounds the number of skinned mesh instances that can be
  // outstanding at any given time.
  static const int MAX_SKINNED_MESH_INSTANCES = 256;
  typedef dcx::dcxMemoryPool<SkinnedMeshInstance,
                             MAX_SKINNED_MESH_INSTANCES> SkinnedMeshInstances;

  // This specifies the maximum number of skinned mesh subsets that can be
  // present in the renderer simultaneously.  Each mesh instance takes up
  // one element for each combination of texture group and texture group
  // subset.
  static const int MAX_RENDERABLE_ELEMENTS
    = MAX_SKINNED_MESH_INSTANCES * 2 * 4;
  typedef dcx::dcxMemoryPool<SkinnedMeshInstanceRenderableElement,
                             MAX_RENDERABLE_ELEMENTS> RenderableElements;

public:
  SkinnedMeshRenderer();

  // Initializes the renderer.  It is not necessary for the animated mesh
  // renderer to be filled at this point.
  bool create(AnimatedMesh::AnimatedMeshRenderer* animated_mesh_renderer);

  // Frees all data used by this class
  void destroy();

  // Adds a new type of skinned mesh to this renderer.  When the first mesh is
  // added, the renderer assumes that the animated mesh renderer it was given
  // in the create() method is now full of valid data, and it allocates an
  // animation track.
  bool addSkinnedMesh(SkinnedMeshIndex index,
                      const SkinnedMesh& skinned_mesh);

  // Used while editing meshes.  Changes the base root transform of a form.
  void changeSkinnedMeshFormTransform(SkinnedMeshIndex skinned_mesh,
                                      SkinnedMeshFormIndex skinned_mesh_form,
                                      const D3DXMATRIX* new_transform);

  // Used while editing meshes.
  void changeSkinnedMeshAnimationSpeed(SkinnedMeshIndex skinned_mesh,
                                       SkinnedMeshAnimationIndex animation,
                                       float speed_factor);
  // Used while editing meshes.
  void changeSkinnedMeshAttachmentPoint(SkinnedMeshIndex skinned_mesh,
                                        SkinnedMeshAttachmentPoint point,
                                        unsigned int source_bone,
                                        const D3DXMATRIX* new_offset);

  // Removes all skinned mesh types and all skinned mesh instances.  This will
  // also erase the internal animation track.
  void clearSkinnedMeshes();

  // Generates a new skinned mesh.  It is the responsibility of the class that
  // acquires the mesh to regularly call update() to advance the animation
  // state.
  SkinnedMeshInstance* acquire(SkinnedMeshIndex skinned_mesh,
                               SkinnedMeshFormIndex skinned_mesh_form);

  // Changes the skinned mesh to the requested type.  If the mesh couldn't be
  // mutated, the method returns 'false'.
  // This method is preferable to releasing and reacquiring a mesh because it
  // allows the renderer to skip the re-allocation of all of the bone matrices
  // and instance structure data when changing forms in a single mesh type.
  //
  // Implementation Issue:  currently skinned_mesh must be the same skinned
  //                        mesh that the instance is using!
  //
  bool mutate(SkinnedMeshInstance* instance,
              SkinnedMeshIndex skinned_mesh,
              SkinnedMeshFormIndex skinned_mesh_form);

  // Completely erases a skinned mesh instance.  After calling this method, do
  // not try to dereference the provided pointer.
  void release(SkinnedMeshInstance* instance);

  // Erases all skinned mesh instances.  Any outstanding instance pointers
  // will be invalidated.
  void clearSkinnedMeshInstances();

  // Sets a new base animation for the mesh.  The old animation is blended out
  // smoothly as this one is introduced.
  void changeBaseAnimation(SkinnedMeshAnimationIndex animation,
                           double current_time,
                           float speed,
                           SkinnedMeshInstance* instance);

  // Changes the speed of both primary and secondary base animations, 
  // ensuring that there are no skips in either one's position.
  void changeBaseAnimationSpeed(double current_time,
                                float new_speed,
                                SkinnedMeshInstance* instance);

  // Sets a static base animation as the new animation.  This is primarily
  // intended for death animations: a death animation action animation
  // with a change to the last frame (pose_percent = 1.0) in the death
  // animation so that the character remains in the final pose.
  void changeBasePose(SkinnedMeshAnimationIndex animation,
                      float pose_percent,
                      SkinnedMeshInstance* instance);

  // Executes the given animation, freezing on the final frame
  void changeBaseRunOnceAndStop(SkinnedMeshAnimationIndex animation,
                                double current_time,
                                float speed,
                                SkinnedMeshInstance* instance);


  // Executes a single-shot animation that overrides the base animation on
  // whichever bones it is applied to.
  void runActionAnimation(SkinnedMeshAnimationIndex animation,
                          double current_time,
                          float speed,
                          SkinnedMeshInstance* instance);

  // Determines the speed at which an action animation would have had to be
  // played in order to have finished at exactly this point in time.
  // This is used by the actor renderer to recalculate action speeds based
  // on how fast it is receiving the actions from the server.
  // Returns true if there was an action animation and source_speed is set
  bool calculateActionAnimationSpeedToHaveCompletedNow(
      double time,
      const SkinnedMeshInstance* instance,
      float* source_speed);

  // Changes the speed of the action animation animations, 
  // ensuring that there are no skips in its position by modifying the
  // animation's start time.
  void changeActionAnimationSpeed(double current_time,
                                  float new_speed,
                                  SkinnedMeshInstance* instance);

  // Advances the animation state to the current time index and builds the 
  // bone and attachment-point matrices.  If the instance is hidden, this
  // method is a no-op.
  // The scaling factors for this instance's current mesh form are applied.
  void updateAnimation(double time,
                       const D3DXMATRIX* root_transform,
                       SkinnedMeshInstance* instance);

  // Resets the renderer at the start of a new scene
  void beginScene();

  // Draws all skinned mesh elements that use the given texture.  Only
  // instances that are not hidden will be drawn.
  bool render(Texture::TextureIndex texture,
              EvidyonD3DRenderStateManager* renderstate_manager);


private:

  // Erase all contents
  void zero();

  // Puts the renderable elements in order and clears the out-of-order flag
  void sortRenderableElements();

  // Removes all of the renderable elements belonging to the given instance
  // from the buffer.  The implementation preserves the order of the elements.
  void eraseRenderableElementsOf(SkinnedMeshInstance* instance);

  // Allocates memory for the animation tracks.  The class must have been
  // successfully created for this to work.
  bool allocateAnimationTracks();

  // Frees memory for the animation tracks.  Requires that the variable
  // animated_mesh_renderer_ is valid.
  void releaseAnimationTracks();


private:

  // Object used to allocate, draw and manage the animated meshes that skinned
  // meshes reference.
  AnimatedMesh::AnimatedMeshRenderer* animated_mesh_renderer_;

  // All the skinned meshes held in this renderer
  SkinnedMeshArray skinned_meshes_;

  // Memory pool for mesh instances
  SkinnedMeshInstances instances_;

  // Pool of memory from which new renderable elements are drawn
  RenderableElements renderable_elements_;

  // Buffer of elements created to render instantiated skinned meshes
  SkinnedMeshInstanceRenderableElement*
    renderable_element_pointers_[MAX_RENDERABLE_ELEMENTS];
  size_t number_of_renderable_elements_;

  // Whether or not the renderable elements buffer has rearranged since the
  // last render call.  The buffer is re-sorted on beginScene if this flag
  // is set.
  bool renderable_elements_out_of_order_;

  // These state variables are used while rendering
  Texture::TextureIndex next_texture_;
  size_t next_renderable_element_pointer_index_;

  // These are the animation tracks used to update the mesh instance animations
  // Currently, all animations can be applied using just a single track.
  AnimatedMesh::AnimatedMeshAnimationTrack* primary_track_;
  AnimatedMesh::AnimatedMeshAnimationTrack* secondary_track_;
};

}
}

#endif