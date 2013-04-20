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
#ifndef __SHARED_ANIMATEDMESH_ANIMATEDMESHRENDERER_H__
#define __SHARED_ANIMATEDMESH_ANIMATEDMESHRENDERER_H__
#pragma once



#include "renderableanimatedmesh.h"
#include "animatedmeshanimation.h"
#include <vector>
#include "animatedmeshindex.h"
#include "animatedmeshanimationindex.h"

namespace Evidyon {
class EvidyonD3DRenderStateManager;

namespace AnimatedMesh {
struct AnimatedMeshAnimationTrack;

//----[  AnimatedMeshRenderer  ]-----------------------------------------------
class AnimatedMeshRenderer {
  typedef std::vector<RenderableAnimatedMesh> AnimatedMeshArray;
  typedef std::vector<AnimatedMeshAnimation> AnimatedMeshAnimationArray;
public:
  AnimatedMeshRenderer();

  // Initializes the renderer
  bool create(LPDIRECT3DDEVICE9 d3d_device);

  // Frees all allocated objects
  void destroy();

  // Puts a new mesh into the renderer.  This method confirms that the provided
  // index is where the mesh is being added; if it isn't, 'false' is returned.
  // See 'animatedmeshutils.h' for help creating the renderable animated mesh
  // structure.  As of 23 June 2009, the path used is to convert a .X file to
  // an Unseen Skinned Mesh (deprecated), then generate an AnimatedMeshData
  // object which can be converted to a RenderableAnimatedMesh.  The .X to
  // Unseen Skinned Mesh tool is very complicated, so in the future .X
  // to AnimatedMeshData will be a direct possibility; however, the renderable
  // animated mesh structure is created per-device.
  bool addRenderableAnimatedMesh(
    AnimatedMeshIndex index,
    const RenderableAnimatedMesh& renderable_animated_mesh);

  // Removes all animated meshes from the renderer
  void clearAnimatedMeshes();

  // Adds a new animation.
  bool addAnimatedMeshAnimation(
    AnimatedMeshAnimationIndex index,
    const AnimatedMeshAnimation& animated_mesh_animation);

  // Removes all of the animations
  void clearAnimatedMeshAnimations();

  // Generates a new animation track that can be used to blend animations
  // for animated meshes.  The returned structure can be used to animate
  // any mesh.
  // The intended usage is for an application to acquire one or more animation
  // tracks at startup and use them for animating all of its meshes.  The
  // provided methods for manipulating tracks 
  AnimatedMeshAnimationTrack* acquireAnimationTrack() const;

  // Frees the memory acquired for the given animation track.  Don't try to
  // use the track after this method has been called.
  void releaseAnimationTrack(AnimatedMeshAnimationTrack* track) const;

  // Clears the state of the animation track so that all transforms are
  // neutral (unity scaling, no translation, no rotation)
  void resetTrack(AnimatedMeshAnimationTrack* track) const;

  // Sets the given animation into the track, overwriting anything that is
  // currently there.  Note that frames for which the animation has no keys
  // are not affected.
  // 'position' must be in the range [0.0f, 1.0f)
  void setTrackAnimation(AnimatedMeshAnimationTrack* track,
                         AnimatedMeshAnimationIndex animation,
                         float position) const;

  // Blends the selected animation into this track.  Any frames that exist
  // in the animation are incorporated into the current state of the track
  // according to the provided weights.
  // 'position' is a value between 0 and 1 defining where in the animation
  // the desired pose is.  This value is not clamped, so values outside
  // of the correct range will produce artifacts.
  // This method will call setTrackAnimation if existing_weight <= 0.0 or
  // this_weight >= 1.0
  // 'position' must be in the range [0.0f, 1.0f)
  void addTrackAnimation(AnimatedMeshAnimationTrack* track,
                         float track_weight,
                         AnimatedMeshAnimationIndex animation,
                         float animation_weight,
                         float position) const;

  // Blends two tracks together and puts the result in the output track.  It is
  // safe to use one of the input tracks as the output track.
  void blendTracks(const AnimatedMeshAnimationTrack* input_track_0,
                   const AnimatedMeshAnimationTrack* input_track_1,
                   float track_1_weight,
                   AnimatedMeshAnimationTrack* output_track) const;

  // Use this method to find the bone matrices that must be allocated for each
  // instance of the given animated mesh.  These matrices are used to set the
  // pose of the mesh.
  unsigned int numberOfBoneMatrices(AnimatedMeshIndex animated_mesh) const;

  // Constructs the bone matrices from the source data in the animation track.
  // You should assume that this method will overwrite the animation track's
  // contents with garbage, so the method can only be called once before it is
  // necessary to rebuild the track.
  void buildAnimatedBones(
    const D3DXMATRIX* root_transform,
    AnimatedMeshIndex animated_mesh,
    AnimatedMeshAnimationTrack* animation_track,
    D3DXMATRIX* bone_matrices) const;

  // Resets the renderer
  void beginScene();

  // Sets up the device to render the given animated mesh subset group.  It
  // is the responsibility of the utilizing class to set the state of
  // D3DRS_INDEXEDVERTEXBLENDENABLE and D3DRS_VERTEXBLEND by calling the
  // setAnimatedGeometryTexture method of the render-state manager.
  bool initializeAnimatedMesh(
      AnimatedMeshIndex animated_mesh,
      unsigned int texture_group,
      unsigned int texture_group_subset,
      EvidyonD3DRenderStateManager* renderstate_manager);

  // Draws an instance of the previously initialized geometry.  The renderer
  // utilizing this class should sort meshes to render by their assigned
  // texture, then by mesh index & subset, then by texture group subset, and
  // finally by instance.  This allows multiple mesh instances to be rendered
  // efficiently--any given geometry will only be assigned once per texture in
  // which it is used.
  bool renderInstance(const D3DXMATRIX* bone_matrices);

  // These methods are used by the skinned mesh renderer
  unsigned int numberOfTextureGroups(AnimatedMeshIndex animated_mesh);
  unsigned int numberOfTextureGroupSubsets(AnimatedMeshIndex animated_mesh,
                                           unsigned int texture_group);

private:
  void zero();

private:

  // Device used to create the renderer
  LPDIRECT3DDEVICE9 d3d_device_;

  // All animated meshes and animations in the renderer
  AnimatedMeshArray animated_meshes_;
  AnimatedMeshAnimationArray animated_mesh_animations_;

  // Greatest number of total frames used by an animated mesh.  This value is
  // updated when meshes are added to the renderer, and is used when creating
  // an animation track.
  size_t highest_number_of_mesh_frames_;

  // The subset currently initialized in the device
  RenderableAnimatedMesh::TextureGroupSubset* initialized_subset_;
};


}
}

#endif