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
#include "animatedmeshrenderer.h"
#include "animatedmeshutils.h"
#include "shared/evidyond3drenderstatemanager.h"
#include "common/safemacros.h"
#include <assert.h>
#include "animatedgeometryvertex.h"

namespace Evidyon {
namespace AnimatedMesh {


//----[  AnimatedMeshAnimationTrackElement  ]----------------------------------
// An animation track is an array of these elements.  This is an internal type
// to encourage the user of the class to consider a track to be an abstract
// unit manipulated by the renderer.
union AnimatedMeshAnimationTrackElement {

  // Holds the transform data in this element.  This cannot have a constructor
  // because it is a member of a union
  struct FrameTransform {
    float t[3]; // translation vector
    float r[4]; // rotation quaternion
    float s[3]; // scaling vector
    void resetToIdentity() {
      t[0] = 0.0f; t[1] = 0.0f; t[2] = 0.0f;
      D3DXQuaternionIdentity(rotation());
      s[0] = 1.0f; s[1] = 1.0f; s[2] = 1.0f;
    }
    D3DXVECTOR3* translation() { return reinterpret_cast<D3DXVECTOR3*>(t); }
    D3DXQUATERNION* rotation() { return reinterpret_cast<D3DXQUATERNION*>(r); }
    D3DXVECTOR3* scaling() { return reinterpret_cast<D3DXVECTOR3*>(s); }
    const D3DXVECTOR3* translation() const { return reinterpret_cast<const D3DXVECTOR3*>(t); }
    const D3DXQUATERNION* rotation() const { return reinterpret_cast<const D3DXQUATERNION*>(r); }
    const D3DXVECTOR3* scaling() const { return reinterpret_cast<const D3DXVECTOR3*>(s); }
  };

  // When blending animation tracks, this holds the transform state used for
  // each frame in the mesh's hierarchy.
  FrameTransform frame_transform;

  // This matrix is created from the frame transforms based on the animated
  // mesh's hierarchy when the bones are being built.
  D3DX_ALIGN16 float frame_matrix_data[16];

  // Accessors that exposes the D3DXMATRIX interface, since D3DXMATRIX has a
  // default constructor and therefore cannot be a union member
  D3DXMATRIXA16* frameMatrix() {
    return reinterpret_cast<D3DXMATRIXA16*>(frame_matrix_data);
  }
};


  
//----[  AnimatedMeshRenderer  ]-----------------------------------------------
AnimatedMeshRenderer::AnimatedMeshRenderer() {
  zero();
}





//----[  create  ]-------------------------------------------------------------
bool AnimatedMeshRenderer::create(LPDIRECT3DDEVICE9 d3d_device) {
  (d3d_device_ = d3d_device)->AddRef();
  return true;
}





//----[  destroy  ]------------------------------------------------------------
void AnimatedMeshRenderer::destroy() {
  clearAnimatedMeshes();
  clearAnimatedMeshAnimations();
  SAFE_RELEASE(d3d_device_);
  zero();
}





//----[  addRenderableAnimatedMesh  ]------------------------------------------
bool AnimatedMeshRenderer::addRenderableAnimatedMesh(
  AnimatedMeshIndex index,
  const RenderableAnimatedMesh& renderable_animated_mesh) {
  if (animated_meshes_.size() != index) return false;
  animated_meshes_.push_back(renderable_animated_mesh);
  highest_number_of_mesh_frames_
    = max(highest_number_of_mesh_frames_,
          renderable_animated_mesh.number_of_frames);
  return true;
}





//----[  clearAnimatedMeshes  ]------------------------------------------------
void AnimatedMeshRenderer::clearAnimatedMeshes() {
  for (AnimatedMeshArray::iterator i = animated_meshes_.begin();
                                   i != animated_meshes_.end();
                                 ++i) {
    AnimatedMeshUtil_DeallocateRenderableAnimatedMesh(&(*i));
  }
  animated_meshes_.clear();
  highest_number_of_mesh_frames_ = 0;
}





//----[  addAnimatedMeshAnimation  ]-------------------------------------------
bool AnimatedMeshRenderer::addAnimatedMeshAnimation(
    AnimatedMeshAnimationIndex index,
    const AnimatedMeshAnimation& animated_mesh_animation) {
  if (animated_mesh_animations_.size() != index) return false;
  animated_mesh_animations_.push_back(animated_mesh_animation);
  return true;
}




//----[  clearAnimatedMeshAnimations  ]----------------------------------------
void AnimatedMeshRenderer::clearAnimatedMeshAnimations() {
  for (AnimatedMeshAnimationArray::iterator i
          = animated_mesh_animations_.begin();
       i != animated_mesh_animations_.end();
     ++i) {
    AnimatedMeshUtil_DeallocateAnimatedMeshAnimation(&(*i));
  }
  animated_mesh_animations_.clear();
}




//----[  acquireAnimationTrack  ]----------------------------------------------
AnimatedMeshAnimationTrack*
AnimatedMeshRenderer::acquireAnimationTrack() const {
  assert(highest_number_of_mesh_frames_); // if this is 0, no meshes exist
  if (!highest_number_of_mesh_frames_) return NULL;
  AnimatedMeshAnimationTrackElement* track
    = new AnimatedMeshAnimationTrackElement[highest_number_of_mesh_frames_];
  return reinterpret_cast<AnimatedMeshAnimationTrack*>(track);
}




//----[  releaseAnimationTrack  ]----------------------------------------------
void AnimatedMeshRenderer::releaseAnimationTrack(
    AnimatedMeshAnimationTrack* track) const {
  assert(track);
  AnimatedMeshAnimationTrackElement* internal_track =
    reinterpret_cast<AnimatedMeshAnimationTrackElement*>(track);
  delete[] internal_track;
}




//----[  resetTrack  ]---------------------------------------------------------
void AnimatedMeshRenderer::resetTrack(
    AnimatedMeshAnimationTrack* track) const {
  AnimatedMeshAnimationTrackElement* internal_track =
    reinterpret_cast<AnimatedMeshAnimationTrackElement*>(track);
  size_t number_of_frames = highest_number_of_mesh_frames_;
  AnimatedMeshAnimationTrackElement reset_element;
  reset_element.frame_transform.resetToIdentity();
  for (size_t i = 0; i < number_of_frames; ++i) {
    memcpy(internal_track + i, &reset_element, sizeof(reset_element));
  }
}



//----[  setTrackAnimation  ]--------------------------------------------------
void AnimatedMeshRenderer::setTrackAnimation(
    AnimatedMeshAnimationTrack* track,
    AnimatedMeshAnimationIndex animation,
    float position) const {
  assert(position >= 0.0f && position < 1.0f);
  AnimatedMeshAnimationTrackElement* internal_track =
    reinterpret_cast<AnimatedMeshAnimationTrackElement*>(track);
  assert(animation < animated_mesh_animations_.size());
  if (animation >= animated_mesh_animations_.size()) return;
  const AnimatedMeshAnimation& animated_mesh_animation
    = animated_mesh_animations_.at(animation);
  size_t number_of_animated_frames
    = animated_mesh_animation.number_of_animated_frames;

  // Get the data about the keys that bound this animation position
  size_t number_of_keys = animated_mesh_animation.number_of_keys;
  assert(number_of_keys);
  double key_position = position * double(number_of_keys-1);
  double lower_key_flt = floor(key_position);
  size_t lower_key = size_t(lower_key_flt);
  float upper_weight;
  upper_weight = key_position - lower_key_flt;
  size_t upper_key = (number_of_keys == 1) ? 0 : (lower_key + 1);
  assert(upper_key < number_of_keys); // ensure the upper key is valid

  // The upper and lower transform pointers are advanced as the naimation
  // is applied.
  const AnimatedMeshAnimation::AnimatedFrameTransform* lower_transform
    = animated_mesh_animation.animated_frame_transforms
        + (lower_key * number_of_animated_frames);
  const AnimatedMeshAnimation::AnimatedFrameTransform* upper_transform
    = animated_mesh_animation.animated_frame_transforms
        + (upper_key * number_of_animated_frames);

  // Animate all frames
  for (size_t i = 0; i < number_of_animated_frames; ++i) {
    unsigned int frame_index
      = animated_mesh_animation.animated_frame_to_frame_table[i];
    AnimatedMeshAnimationTrackElement* frame
      = internal_track + frame_index;
    D3DXVec3Lerp(frame->frame_transform.scaling(),
                 &lower_transform->scaling,
                 &upper_transform->scaling,
                 upper_weight);
    D3DXVec3Lerp(frame->frame_transform.translation(),
                 &lower_transform->translation,
                 &upper_transform->translation,
                 upper_weight);
    D3DXQuaternionSlerp(frame->frame_transform.rotation(),
                        &lower_transform->rotation,
                        &upper_transform->rotation,
                        upper_weight);
    ++upper_transform;
    ++lower_transform;
  }
}



//----[  addTrackAnimation  ]--------------------------------------------------
void AnimatedMeshRenderer::addTrackAnimation(
    AnimatedMeshAnimationTrack* track,
    float track_weight,
    AnimatedMeshAnimationIndex animation,
    float animation_weight,
    float position) const {

  assert(position >= 0.0f && position < 1.0f);
  AnimatedMeshAnimationTrackElement* internal_track =
    reinterpret_cast<AnimatedMeshAnimationTrackElement*>(track);
  assert(animation < animated_mesh_animations_.size());
  if (animation >= animated_mesh_animations_.size()) return;
  const AnimatedMeshAnimation& animated_mesh_animation
    = animated_mesh_animations_.at(animation);
  size_t number_of_animated_frames
    = animated_mesh_animation.number_of_animated_frames;

  // Get the data about the keys that bound this animation position
  size_t number_of_keys = animated_mesh_animation.number_of_keys;
  assert(number_of_keys);
  double key_position = position * double(number_of_keys-1);
  double lower_key_flt = floor(key_position);
  size_t lower_key = size_t(lower_key_flt);
  float upper_weight;
  upper_weight = key_position - lower_key_flt;
  size_t upper_key = (number_of_keys == 1) ? 0 : (lower_key + 1);
  assert(upper_key < number_of_keys); // ensure the upper key is valid

  // The upper and lower transform pointers are advanced as the animation
  // is applied.
  const AnimatedMeshAnimation::AnimatedFrameTransform* lower_transform
    = animated_mesh_animation.animated_frame_transforms
        + (lower_key * number_of_animated_frames);
  const AnimatedMeshAnimation::AnimatedFrameTransform* upper_transform
    = animated_mesh_animation.animated_frame_transforms
        + (upper_key * number_of_animated_frames);

  // Animate all frames
  for (size_t i = 0; i < number_of_animated_frames; ++i) {
    unsigned int frame_index
      = animated_mesh_animation.animated_frame_to_frame_table[i];
    AnimatedMeshAnimationTrackElement* frame
      = internal_track + frame_index;
    D3DXVECTOR3 scaling;
    D3DXVec3Lerp(&scaling,
                 &lower_transform->scaling,
                 &upper_transform->scaling,
                 upper_weight);
    frame->frame_transform.s[0] *= track_weight;
    frame->frame_transform.s[1] *= track_weight;
    frame->frame_transform.s[2] *= track_weight;
    frame->frame_transform.s[0] += animation_weight * scaling.x;
    frame->frame_transform.s[1] += animation_weight * scaling.y;
    frame->frame_transform.s[2] += animation_weight * scaling.z;

    D3DXVECTOR3 translation;
    D3DXVec3Lerp(&translation,
                 &lower_transform->translation,
                 &upper_transform->translation,
                 upper_weight);
    frame->frame_transform.t[0] *= track_weight;
    frame->frame_transform.t[1] *= track_weight;
    frame->frame_transform.t[2] *= track_weight;
    frame->frame_transform.t[0] += animation_weight * translation.x;
    frame->frame_transform.t[1] += animation_weight * translation.y;
    frame->frame_transform.t[2] += animation_weight * translation.z;

    D3DXQUATERNION rotation;
    D3DXQuaternionSlerp(&rotation,
                        &lower_transform->rotation,
                        &upper_transform->rotation,
                        upper_weight);
    if (animation_weight >= 1.0f || track_weight <= 0.0f) {
      frame->frame_transform.r[0] = rotation.x;
      frame->frame_transform.r[1] = rotation.y;
      frame->frame_transform.r[2] = rotation.z;
      frame->frame_transform.r[3] = rotation.w;
    } else {
      D3DXQuaternionSlerp(frame->frame_transform.rotation(),
                          frame->frame_transform.rotation(),
                          &rotation,
                          animation_weight);
    }
    /*
    frame->frame_transform.r[0] *= track_weight;
    frame->frame_transform.r[1] *= track_weight;
    frame->frame_transform.r[2] *= track_weight;
    frame->frame_transform.r[3] *= track_weight;
    frame->frame_transform.r[0] += animation_weight * rotation.x;
    frame->frame_transform.r[1] += animation_weight * rotation.y;
    frame->frame_transform.r[2] += animation_weight * rotation.z;
    frame->frame_transform.r[3] += animation_weight * rotation.w;*/

    ++upper_transform;
    ++lower_transform;
  }
}



    
//----[  blendTracks  ]--------------------------------------------------------
void AnimatedMeshRenderer::blendTracks(
    const AnimatedMeshAnimationTrack* input_track_0,
    const AnimatedMeshAnimationTrack* input_track_1,
    float track_1_weight,
    AnimatedMeshAnimationTrack* output_track) const {
  //if (input_track_1 == output_track) {
  //  input_track_1 = input_track_0;
  //  input_track_0 = output_track;
  //}

  assert(track_1_weight >= 0.0f && track_1_weight <= 1.0f);
  const AnimatedMeshAnimationTrackElement* internal_track_0 =
    reinterpret_cast<const AnimatedMeshAnimationTrackElement*>(input_track_0);
  const AnimatedMeshAnimationTrackElement* internal_track_1 =
    reinterpret_cast<const AnimatedMeshAnimationTrackElement*>(input_track_1);
  AnimatedMeshAnimationTrackElement* internal_output_track =
    reinterpret_cast<AnimatedMeshAnimationTrackElement*>(output_track);

  size_t number_of_mesh_frames = highest_number_of_mesh_frames_;
  for (size_t frame_index = 0;
              frame_index < number_of_mesh_frames;
            ++frame_index) {
    const AnimatedMeshAnimationTrackElement* frames[2] = {
      internal_track_0 + frame_index,
      internal_track_1 + frame_index,
    };
    AnimatedMeshAnimationTrackElement* output_frame =
      internal_output_track + frame_index;
    D3DXVec3Lerp(output_frame->frame_transform.translation(),
                 frames[0]->frame_transform.translation(),
                 frames[1]->frame_transform.translation(),
                 track_1_weight);
    D3DXVec3Lerp(output_frame->frame_transform.scaling(),
                 frames[0]->frame_transform.scaling(),
                 frames[1]->frame_transform.scaling(),
                 track_1_weight);
    D3DXQuaternionSlerp(output_frame->frame_transform.rotation(),
                        frames[0]->frame_transform.rotation(),
                        frames[1]->frame_transform.rotation(),
                        track_1_weight);
  }
}





//----[  numberOfBones  ]------------------------------------------------------
unsigned int AnimatedMeshRenderer::numberOfBoneMatrices(
    AnimatedMeshIndex animated_mesh) const {
  assert(animated_mesh < animated_meshes_.size());
  return animated_meshes_.at(animated_mesh).number_of_bones;
}




//----[  buildAnimatedBones  ]-------------------------------------------------
void AnimatedMeshRenderer::buildAnimatedBones(
    const D3DXMATRIX* root_transform,
    AnimatedMeshIndex animated_mesh,
    AnimatedMeshAnimationTrack* animation_track,
    D3DXMATRIX* bone_matrices) const {
  assert(animated_mesh < animated_meshes_.size());
  assert(animation_track);
  assert(bone_matrices);

  const RenderableAnimatedMesh& renderable_animated_mesh
    = animated_meshes_.at(animated_mesh);
  AnimatedMeshAnimationTrackElement* internal_animation_track =
    reinterpret_cast<AnimatedMeshAnimationTrackElement*>(animation_track);
  RenderableAnimatedMesh::Frame* frames = renderable_animated_mesh.frames;
  size_t number_of_frames = renderable_animated_mesh.number_of_frames;
  RenderableAnimatedMesh::Bone* bones = renderable_animated_mesh.bones;
  size_t number_of_bones = renderable_animated_mesh.number_of_bones;

  // Build the frames using the data in the animation track
  D3DXMATRIXA16 matrix;
  for (size_t frame_index = 0; frame_index < number_of_frames; ++frame_index) {
    RenderableAnimatedMesh::Frame* frame = &frames[frame_index];
    AnimatedMeshAnimationTrackElement* track_element
      = &internal_animation_track[frame_index];
    AnimatedMeshAnimationTrackElement::FrameTransform* frame_transform
      = &track_element->frame_transform;

    // Build the transform matrix from the scale/rotate/translate settings
    //D3DXMatrixTransformation(&matrix,
    //                         NULL,
    //                         NULL,
    //                         frame_transform->scaling(),
    //                         NULL,
    //                         frame_transform->rotation(),
    //                         frame_transform->translation());

    // This method of constructing the transform matrices is necessary because
    // it allows us to transpose the quaternion's matrix during the build.
    D3DXMATRIXA16 cumulative, builder;
    D3DXMatrixScaling(&cumulative,
                      frame_transform->s[0],
                      frame_transform->s[1],
                      frame_transform->s[2]);

    D3DXQUATERNION quat;
    D3DXQuaternionNormalize(&quat, frame_transform->rotation());
    D3DXMatrixRotationQuaternion(&builder, &quat);
    D3DXMatrixTranspose(&builder, &builder);

    D3DXMatrixMultiply(&cumulative, &cumulative, &builder);
    D3DXMatrixTranslation(&builder,
                          frame_transform->t[0],
                          frame_transform->t[1],
                          frame_transform->t[2]);
    D3DXMatrixMultiply(&matrix, &cumulative, &builder);



    // Use the frame hierarchy to construct this frame's final transformation
    size_t parent_frame_index = frame->parent_frame_index;
    assert(!frame_index || (parent_frame_index < frame_index));
    const D3DXMATRIX* parent_frame_matrix
      = frame_index == 0 ? root_transform
                         : internal_animation_track[parent_frame_index]
                             .frameMatrix();
    D3DXMatrixMultiply(track_element->frameMatrix(),
                       &matrix,
                       parent_frame_matrix);
  }

  // Construct the bone transformations for this framte state
  for (size_t bone_index = 0; bone_index < number_of_bones; ++bone_index) {
    D3DXMATRIX* bone_matrix = bone_matrices + bone_index;
    const RenderableAnimatedMesh::Bone* bone = bones + bone_index;
    const D3DXMATRIXA16* frame_matrix
      = (internal_animation_track + bone->frame_index)->frameMatrix();
    D3DXMatrixMultiply(bone_matrix,
                       &bone->inverse_offset,
                       frame_matrix);
  }
}



//----[  beginScene  ]---------------------------------------------------------
void AnimatedMeshRenderer::beginScene() {
  initialized_subset_ = NULL;
}



//----[  initializeAnimatedMesh  ]---------------------------------------------
bool AnimatedMeshRenderer::initializeAnimatedMesh(
    AnimatedMeshIndex animated_mesh,
    unsigned int texture_group,
    unsigned int texture_group_subset,
    EvidyonD3DRenderStateManager* renderstate_manager) {
  assert(animated_mesh < animated_meshes_.size());
  const RenderableAnimatedMesh& renderable_animated_mesh
    = animated_meshes_.at(animated_mesh);
  assert(texture_group < renderable_animated_mesh.number_of_texture_groups);
  const RenderableAnimatedMesh::TextureGroup* renderable_texture_group
    = &renderable_animated_mesh.texture_groups[texture_group];
  assert(texture_group_subset < renderable_texture_group->number_of_subsets);
  RenderableAnimatedMesh::TextureGroupSubset* renderable_texture_group_subset
    = &renderable_texture_group->subsets[texture_group_subset];
  initialized_subset_ = renderable_texture_group_subset;
  return renderstate_manager->setStream0GeometryBuffers(
    renderable_texture_group_subset->vb,
    D3DFVF_ANIMATEDGEOMETRYVERTEX,
    sizeof(AnimatedGeometryVertex),
    renderable_texture_group_subset->ib);
}






//----[  renderInstance  ]-----------------------------------------------------
bool AnimatedMeshRenderer::renderInstance(const D3DXMATRIX* bone_matrices) {
  assert(initialized_subset_);
  LPDIRECT3DDEVICE9 d3d_device = d3d_device_;

  { // Set all of the bone matrices in this subset
    const unsigned int* matrix_bone_indices
      = initialized_subset_->matrix_bone_indices;
    size_t number_of_matrices = initialized_subset_->number_of_matrices;
    for (size_t i = 0; i < number_of_matrices; ++i) {
      d3d_device->SetTransform(D3DTS_WORLDMATRIX(i),
                               &bone_matrices[matrix_bone_indices[i]]);
    }
  }

  // Render
  HRESULT hr = d3d_device->DrawIndexedPrimitive(
      D3DPT_TRIANGLELIST,
      0,
      0,
      initialized_subset_->number_of_vertices,
      0,
      initialized_subset_->number_of_triangles);
  return SUCCEEDED(hr) ? true : false;
}




//----[  numberOfTextureGroups  ]----------------------------------------------
unsigned int AnimatedMeshRenderer::numberOfTextureGroups(
    AnimatedMeshIndex animated_mesh) {
  assert(animated_mesh < animated_meshes_.size());
  return animated_meshes_.at(animated_mesh).number_of_texture_groups;
}




//----[  numberOfTextureGroupSubsets  ]----------------------------------------
unsigned int AnimatedMeshRenderer::numberOfTextureGroupSubsets(
    AnimatedMeshIndex animated_mesh,
    unsigned int texture_group) {
  assert(animated_mesh < animated_meshes_.size());
  const RenderableAnimatedMesh& renderable_animated_mesh
    = animated_meshes_.at(animated_mesh);
  assert(texture_group < renderable_animated_mesh.number_of_texture_groups);
  return renderable_animated_mesh
          .texture_groups[texture_group].number_of_subsets;
}




//----[  zero  ]---------------------------------------------------------------
void AnimatedMeshRenderer::zero() {
  d3d_device_ = NULL;
  initialized_subset_ = NULL;
  animated_meshes_.clear();
  highest_number_of_mesh_frames_ = 0;
}



}
}