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
#include "skinnedmeshform.h"
#include "skinnedmeshanimation.h"
#include "skinnedmeshattachmentpointoffset.h"
#include "skinnedmesh.h"
#include "skinnedmeshrenderer.h"
#include "shared/animated-mesh/animatedmeshrenderer.h"
#include "common/null.h"
#include "common/safemacros.h"
#include "skinnedmeshutils.h"
#include <assert.h>
#include "shared/evidyond3drenderstatemanager.h"


namespace Evidyon {
namespace SkinnedMesh {



  
//----[  SkinnedMeshRenderer  ]------------------------------------------------
SkinnedMeshRenderer::SkinnedMeshRenderer() {
  zero();
}






//----[  create  ]-------------------------------------------------------------
bool SkinnedMeshRenderer::create(
    AnimatedMesh::AnimatedMeshRenderer* animated_mesh_renderer) {
  if (!instances_.create() ||
      !renderable_elements_.create()) {
    destroy();
    return false;
  }
  animated_mesh_renderer_ = animated_mesh_renderer;
  return true;
}





//----[  destroy  ]------------------------------------------------------------
void SkinnedMeshRenderer::destroy() {
  clearSkinnedMeshes(); // also clears instances and animation tracks
  renderable_elements_.destroy();
  zero();
}








//----[  addSkinnedMesh  ]-----------------------------------------------------
bool SkinnedMeshRenderer::addSkinnedMesh(SkinnedMeshIndex index,
                                         const SkinnedMesh& skinned_mesh) {
  if (index != skinned_meshes_.size()) return false;
  if (index == 0 && !allocateAnimationTracks()) return false;
  skinned_meshes_.push_back(skinned_mesh);
  return true;
}






//----[  changeSkinnedMeshFormTransform  ]-------------------------------------
void SkinnedMeshRenderer::changeSkinnedMeshFormTransform(
    SkinnedMeshIndex skinned_mesh,
    SkinnedMeshFormIndex skinned_mesh_form,
    const D3DXMATRIX* new_transform) {
  assert(new_transform);
  assert(animated_mesh_renderer_);
  assert(skinned_mesh < skinned_meshes_.size());
  if (skinned_mesh >= skinned_meshes_.size()) return;
  const SkinnedMesh& skinned_mesh_object = skinned_meshes_.at(skinned_mesh);
  assert(skinned_mesh_form < skinned_mesh_object.number_of_forms);
  if (skinned_mesh_form >= skinned_mesh_object.number_of_forms) return;
  SkinnedMeshForm* skinned_mesh_form_object
    = &skinned_mesh_object.forms[skinned_mesh_form];
  memcpy(&skinned_mesh_form_object->transform,
         new_transform,
         sizeof(D3DXMATRIX));
}




//----[  changeSkinnedMeshAnimationSpeed  ]------------------------------------
void SkinnedMeshRenderer::changeSkinnedMeshAnimationSpeed(
    SkinnedMeshIndex skinned_mesh,
    SkinnedMeshAnimationIndex animation,
    float speed_factor) {
  assert(animated_mesh_renderer_);
  assert(skinned_mesh < skinned_meshes_.size());
  if (skinned_mesh >= skinned_meshes_.size()) return;
  const SkinnedMesh& skinned_mesh_object = skinned_meshes_.at(skinned_mesh);
  assert(animation < skinned_mesh_object.number_of_animations);
  if (animation >= skinned_mesh_object.number_of_animations) return;
  SkinnedMeshAnimation* animation_object
    = &skinned_mesh_object.animations[animation];
  animation_object->speed_factor = speed_factor;
}




//----[  changeSkinnedMeshAttachmentPoint  ]-----------------------------------
void SkinnedMeshRenderer::changeSkinnedMeshAttachmentPoint(
    SkinnedMeshIndex skinned_mesh,
    SkinnedMeshAttachmentPoint point,
    unsigned int source_bone,
    const D3DXMATRIX* new_offset) {
  assert(animated_mesh_renderer_);
  assert(skinned_mesh < skinned_meshes_.size());
  if (skinned_mesh >= skinned_meshes_.size()) return;
  SkinnedMesh& skinned_mesh_object = skinned_meshes_.at(skinned_mesh);
  SkinnedMeshAttachmentPointOffset* attachment_point
    = &skinned_mesh_object.attachment_point_offsets[point];
  memcpy(&attachment_point->offset, new_offset, sizeof(D3DXMATRIX));
  attachment_point->source_bone = source_bone;
}


//
//
////----[  changeSkinnedMeshFormMovementFactor  ]--------------------------------
//void SkinnedMeshRenderer::changeSkinnedMeshFormMovementFactor(
//    SkinnedMeshIndex skinned_mesh,
//    SkinnedMeshFormIndex skinned_mesh_form,
//    float new_movement_animation_factor) {
//  assert(animated_mesh_renderer_);
//  assert(skinned_mesh < skinned_meshes_.size());
//  if (skinned_mesh >= skinned_meshes_.size()) return;
//  const SkinnedMesh& skinned_mesh_object = skinned_meshes_.at(skinned_mesh);
//  assert(skinned_mesh_form < skinned_mesh_object.number_of_forms);
//  if (skinned_mesh_form >= skinned_mesh_object.number_of_forms) return;
//  SkinnedMeshForm* skinned_mesh_form_object
//    = &skinned_mesh_object.forms[skinned_mesh_form];
//}




//----[  clearSkinnedMeshes  ]-------------------------------------------------
void SkinnedMeshRenderer::clearSkinnedMeshes() {
  clearSkinnedMeshInstances();
  releaseAnimationTracks();
  for (SkinnedMeshArray::iterator i = skinned_meshes_.begin();
                                  i != skinned_meshes_.end();
                                ++i) {
    Evidyon::SkinnedMesh::SkinnedMesh* skinned_mesh = &(*i);
    SkinnedMeshUtils_DeallocateSkinnedMesh(skinned_mesh);
  }
  skinned_meshes_.clear();
}







//----[  acquire  ]------------------------------------------------------------
SkinnedMeshInstance* SkinnedMeshRenderer::acquire(
    SkinnedMeshIndex skinned_mesh,
    SkinnedMeshFormIndex skinned_mesh_form) {
  assert(animated_mesh_renderer_);
  assert(skinned_mesh < skinned_meshes_.size());
  if (skinned_mesh >= skinned_meshes_.size()) return NULL;
  const SkinnedMesh& skinned_mesh_object = skinned_meshes_.at(skinned_mesh);
  assert(skinned_mesh_form < skinned_mesh_object.number_of_forms);
  if (skinned_mesh_form >= skinned_mesh_object.number_of_forms) return NULL;
  SkinnedMeshForm* skinned_mesh_form_object
    = &skinned_mesh_object.forms[skinned_mesh_form];

  SkinnedMeshInstance* instance = instances_.acquire();

  // Acquire all of this mesh instance's renderable elements
  SkinnedMeshInstanceRenderableElement template_element;
  template_element.instance = instance;
  template_element.animated_mesh = skinned_mesh_form_object->animated_mesh;
  unsigned int number_of_texture_groups
    = animated_mesh_renderer_->numberOfTextureGroups(
        template_element.animated_mesh);

  for (unsigned int texture_group_index = 0;
                    texture_group_index < number_of_texture_groups;
                  ++texture_group_index) {
    template_element.texture_group = texture_group_index;
    template_element.texture_group_texture
      = skinned_mesh_form_object->texture_group_textures[texture_group_index];
    unsigned int number_of_texture_group_subsets
      = animated_mesh_renderer_->numberOfTextureGroupSubsets(
          template_element.animated_mesh,
          texture_group_index);
    for (unsigned int subset_index = 0;
                      subset_index < number_of_texture_group_subsets;
                    ++subset_index) {
      template_element.texture_group_subset = subset_index;

      // the template element is now fully initialized; acquire an element and
      // add it to the internal list
      SkinnedMeshInstanceRenderableElement* renderable_element
        = renderable_elements_.acquire();
      if (!renderable_element) { // too many meshes allocated; roll back & fail
        eraseRenderableElementsOf(instance);
        instances_.release(instance);
        return NULL;
      }
      memcpy(renderable_element, &template_element, sizeof(template_element));
      assert(number_of_renderable_elements_ < MAX_RENDERABLE_ELEMENTS);
      renderable_element_pointers_[number_of_renderable_elements_++]
        = renderable_element;
    }
  }

  // The list of elements needs to be re-sorted
  renderable_elements_out_of_order_ = true;

  // All of the elements for this mesh have been added to the rendering list,
  // so we just have to initialize this instance.
  instance->hidden = false;
  instance->skinned_mesh = &skinned_meshes_.at(skinned_mesh);
  instance->skinned_mesh_form = &skinned_mesh_object.forms[skinned_mesh_form];
  instance->primary_base_animation.animation = NULL;
  instance->secondary_base_animation.animation = NULL;
  instance->action_animation.animation = NULL;

  // Allocate bone matrices for the instance
  unsigned int number_of_bone_matrices
    = animated_mesh_renderer_->numberOfBoneMatrices(
        template_element.animated_mesh);
  instance->bone_matrices = new D3DXMATRIXA16[number_of_bone_matrices];
  if (!instance->bone_matrices) {
    eraseRenderableElementsOf(instance);
    instances_.release(instance);
    return NULL;
  }


  return instance;
}




//----[  mutate  ]-------------------------------------------------------------
bool SkinnedMeshRenderer::mutate(SkinnedMeshInstance* instance,
                                 SkinnedMeshIndex skinned_mesh,
                                 SkinnedMeshFormIndex skinned_mesh_form) {
  assert(animated_mesh_renderer_);
  assert(skinned_mesh < skinned_meshes_.size());
  if (skinned_mesh >= skinned_meshes_.size()) return false;
  const SkinnedMesh& skinned_mesh_object = skinned_meshes_.at(skinned_mesh);
  if (instance->skinned_mesh != &skinned_mesh_object) return false;
  assert(skinned_mesh_form < skinned_mesh_object.number_of_forms);
  if (skinned_mesh_form >= skinned_mesh_object.number_of_forms) return false;
  SkinnedMeshForm* skinned_mesh_form_object
    = &skinned_mesh_object.forms[skinned_mesh_form];

  eraseRenderableElementsOf(instance);

  // Acquire all of this mesh instance's renderable elements
  SkinnedMeshInstanceRenderableElement template_element;
  template_element.instance = instance;
  template_element.animated_mesh = skinned_mesh_form_object->animated_mesh;
  unsigned int number_of_texture_groups
    = animated_mesh_renderer_->numberOfTextureGroups(
        template_element.animated_mesh);

  for (unsigned int texture_group_index = 0;
                    texture_group_index < number_of_texture_groups;
                  ++texture_group_index) {
    template_element.texture_group = texture_group_index;
    template_element.texture_group_texture
      = skinned_mesh_form_object->texture_group_textures[texture_group_index];
    unsigned int number_of_texture_group_subsets
      = animated_mesh_renderer_->numberOfTextureGroupSubsets(
          template_element.animated_mesh,
          texture_group_index);
    for (unsigned int subset_index = 0;
                      subset_index < number_of_texture_group_subsets;
                    ++subset_index) {
      template_element.texture_group_subset = subset_index;

      // the template element is now fully initialized; acquire an element and
      // add it to the internal list
      SkinnedMeshInstanceRenderableElement* renderable_element
        = renderable_elements_.acquire();
      if (!renderable_element) { // too many meshes allocated; roll back & fail
        eraseRenderableElementsOf(instance);
        instances_.release(instance);
        return NULL;
      }
      memcpy(renderable_element, &template_element, sizeof(template_element));
      assert(number_of_renderable_elements_ < MAX_RENDERABLE_ELEMENTS);
      renderable_element_pointers_[number_of_renderable_elements_++]
        = renderable_element;
    }
  }

  // The list of elements needs to be re-sorted
  renderable_elements_out_of_order_ = true;
  instance->skinned_mesh_form = &skinned_mesh_object.forms[skinned_mesh_form];

  // Successful mutation
  return true;
}



//----[  release  ]------------------------------------------------------------
void SkinnedMeshRenderer::release(SkinnedMeshInstance* instance) {
  assert(animated_mesh_renderer_);
  assert(instance);
  SAFE_DELETE_ARRAY(instance->bone_matrices);
  eraseRenderableElementsOf(instance);
  instances_.release(instance);
}






//----[  clearSkinnedMeshInstances  ]------------------------------------------
void SkinnedMeshRenderer::clearSkinnedMeshInstances() {
  instances_.releaseAll();
  number_of_renderable_elements_ = 0; // remove all renderable elements
  renderable_elements_.releaseAll();
  renderable_elements_out_of_order_ = false;
  next_texture_ = Texture::INVALID_TEXTURE_INDEX;
  next_renderable_element_pointer_index_ = 0;
}






//----[  changeBaseAnimation  ]------------------------------------------------
void SkinnedMeshRenderer::changeBaseAnimation(
    SkinnedMeshAnimationIndex animation,
    double current_time,
    float speed,
    SkinnedMeshInstance* instance) {
  assert(animated_mesh_renderer_);
  assert(instance->skinned_mesh);
  assert(instance->skinned_mesh_form);
  const SkinnedMesh* skinned_mesh = instance->skinned_mesh;
  assert(animation < skinned_mesh->number_of_animations);
  const SkinnedMeshAnimation* skinned_mesh_animation
    = &skinned_mesh->animations[animation];

  float animation_speed = 
    skinned_mesh_animation->speed_factor * speed
      + skinned_mesh_animation->speed_offset;

  if (instance->primary_base_animation.animation == NULL) {
    assert(instance->secondary_base_animation.animation == NULL);
    instance->primary_base_animation.animation = skinned_mesh_animation;
    instance->primary_base_animation.speed = animation_speed;
    instance->primary_base_animation.start_time = current_time;
    instance->primary_base_animation.weight = 1.0f;
  } else if (instance->secondary_base_animation.animation == NULL) {
    instance->secondary_base_animation.animation = skinned_mesh_animation;
    instance->secondary_base_animation.speed = animation_speed;
    instance->secondary_base_animation.start_time = current_time;
    instance->secondary_base_animation.weight = 0.0f;
  } else {
    // both are currently valid, so overwrite whichever is less weighted
    if (instance->primary_base_animation.weight <
        instance->secondary_base_animation.weight) {
      memcpy(&instance->primary_base_animation,
             &instance->secondary_base_animation,
             sizeof(instance->primary_base_animation));
      instance->secondary_base_animation.animation = skinned_mesh_animation;
      instance->secondary_base_animation.speed = animation_speed;
      instance->secondary_base_animation.start_time = current_time;
      instance->secondary_base_animation.weight =
        1.0f - instance->primary_base_animation.weight;
    } else {
      instance->secondary_base_animation.animation = skinned_mesh_animation;
      instance->secondary_base_animation.speed = animation_speed;
      instance->secondary_base_animation.start_time = current_time;
      instance->secondary_base_animation.weight = 
          1.0f - instance->primary_base_animation.weight;
    }
  }
}








//----[  changeBaseAnimationSpeed  ]-------------------------------------------
void SkinnedMeshRenderer::changeBaseAnimationSpeed(
    double current_time,
    float new_speed,
    SkinnedMeshInstance* instance) {
  const SkinnedMeshAnimation* primary_animation
    = instance->primary_base_animation.animation;
  const SkinnedMeshAnimation* secondary_animation
    = instance->secondary_base_animation.animation;

  if (primary_animation) {
    float new_animation_speed
      = new_speed * primary_animation->speed_factor
          + primary_animation->speed_offset;
    // mod((current_time - start_time) * speed, length )
    //  = new_animation_speed * (current_time - new_start_time);
    // new_start_time
    //  = current_time -
    //      mod((current_time - start_time) * speed, length )
    //        / new_animation_speed
    instance->primary_base_animation.start_time =
      current_time -
      fmod((current_time - instance->primary_base_animation.start_time) *
          instance->primary_base_animation.speed,
         primary_animation->length_in_seconds) / new_animation_speed;
    instance->primary_base_animation.speed = new_animation_speed;
  } else {
    assert(!secondary_animation);
  }

  if (secondary_animation) {
    float new_animation_speed
      = new_speed * secondary_animation->speed_factor
          + secondary_animation->speed_offset;
    instance->secondary_base_animation.start_time =
      current_time -
      fmod((current_time - instance->secondary_base_animation.start_time) *
          instance->secondary_base_animation.speed,
         secondary_animation->length_in_seconds) / new_animation_speed;
    instance->secondary_base_animation.speed = new_animation_speed;
  }
}



//----[  changeBasePose  ]-----------------------------------------------------
void SkinnedMeshRenderer::changeBasePose(SkinnedMeshAnimationIndex animation,
                                         float pose_percent,
                                         SkinnedMeshInstance* instance) {
  assert(animated_mesh_renderer_);
  assert(instance->skinned_mesh);
  assert(instance->skinned_mesh_form);
  const SkinnedMesh* skinned_mesh = instance->skinned_mesh;
  assert(animation < skinned_mesh->number_of_animations);
  const SkinnedMeshAnimation* skinned_mesh_animation
    = &skinned_mesh->animations[animation];

  instance->primary_base_animation.animation = skinned_mesh_animation;
  instance->primary_base_animation.speed = 0.0f;
  instance->primary_base_animation.start_time = pose_percent;
  instance->primary_base_animation.weight = 0.0f;
}



//----[  changeBaseRunOnceAndStop  ]-------------------------------------------
void SkinnedMeshRenderer::changeBaseRunOnceAndStop(
    SkinnedMeshAnimationIndex animation,
    double current_time,
    float speed,
    SkinnedMeshInstance* instance) {
  assert(instance);
  assert(instance->skinned_mesh);
  assert(instance->skinned_mesh_form);
  const SkinnedMesh* skinned_mesh = instance->skinned_mesh;
  assert(animation < skinned_mesh->number_of_animations);
  const SkinnedMeshAnimation* skinned_mesh_animation
    = &skinned_mesh->animations[animation];

  instance->primary_base_animation.animation = skinned_mesh_animation;
  instance->primary_base_animation.speed = speed;
  instance->primary_base_animation.start_time = current_time;
  instance->primary_base_animation.weight = 0.0f;
  instance->secondary_base_animation.animation = skinned_mesh_animation;
  instance->secondary_base_animation.speed = 0.0f;
  instance->secondary_base_animation.start_time = 0.9999f;
  instance->secondary_base_animation.weight = 1.0f;
  instance->action_animation.animation = skinned_mesh_animation;
  instance->action_animation.speed = speed;
  instance->action_animation.start_time = current_time;
  instance->action_animation.weight = 1.0f;
}





//----[  runActionAnimation  ]-------------------------------------------------
void SkinnedMeshRenderer::runActionAnimation(
    SkinnedMeshAnimationIndex animation,
    double current_time,
    float speed,
    SkinnedMeshInstance* instance) {
  assert(animated_mesh_renderer_);
  assert(instance->skinned_mesh);
  assert(instance->skinned_mesh_form);
  const SkinnedMesh* skinned_mesh = instance->skinned_mesh;
  assert(animation < skinned_mesh->number_of_animations);
  const SkinnedMeshAnimation* skinned_mesh_animation
    = &skinned_mesh->animations[animation];

  float animation_speed = 
    skinned_mesh_animation->speed_factor * speed
      + skinned_mesh_animation->speed_offset;

  instance->action_animation.animation = skinned_mesh_animation;
  instance->action_animation.speed = animation_speed;
  instance->action_animation.start_time = current_time;
  instance->action_animation.weight = 0.0f;
}









//----[  calculateActionAnimationSpeedToHaveCompletedNow  ]--------------------
bool SkinnedMeshRenderer::calculateActionAnimationSpeedToHaveCompletedNow(
    double time,
    const SkinnedMeshInstance* instance,
    float* source_speed) {
  const SkinnedMeshAnimation*
    animation = instance->action_animation.animation;
  if (!animation) return false;
  // dt * (source_speed * speed_factor + speed_offset) = length_in_seconds
  float required_speed
    = animation->length_in_seconds
        / (time - instance->action_animation.start_time);
  required_speed -= animation->speed_offset;
  required_speed /= animation->speed_factor;
  *source_speed = required_speed;
  return true;
}





//----[  changeActionAnimationSpeed  ]-----------------------------------------
void SkinnedMeshRenderer::changeActionAnimationSpeed(
    double current_time,
    float new_speed,
    SkinnedMeshInstance* instance) {
  const SkinnedMeshAnimation* action_animation
    = instance->action_animation.animation;

  if (action_animation) {
    float new_animation_speed
      = new_speed * action_animation->speed_factor
          + action_animation->speed_offset;
    // mod((current_time - start_time) * speed, length )
    //  = new_animation_speed * (current_time - new_start_time);
    // new_start_time
    //  = current_time -
    //      mod((current_time - start_time) * speed, length )
    //        / new_animation_speed
    instance->action_animation.start_time =
      current_time -
      fmod((current_time - instance->action_animation.start_time) *
          instance->action_animation.speed,
         action_animation->length_in_seconds) / new_animation_speed;
    instance->action_animation.speed = new_animation_speed;
  }
}



//----[  updateAnimation  ]----------------------------------------------------
void SkinnedMeshRenderer::updateAnimation(double time,
                                          const D3DXMATRIX* root_transform,
                                          SkinnedMeshInstance* instance) {
  assert(animated_mesh_renderer_);
  assert(instance);
  if (instance->hidden) return;
  assert(instance->skinned_mesh);
  assert(instance->skinned_mesh_form);
  const SkinnedMesh* skinned_mesh = instance->skinned_mesh;
  const SkinnedMeshForm* skinned_mesh_form = instance->skinned_mesh_form;
  AnimatedMesh::AnimatedMeshIndex animated_mesh
    = skinned_mesh_form->animated_mesh;

  // Blend animations to generate the current instance pose
  animated_mesh_renderer_->resetTrack(primary_track_);
  const SkinnedMeshAnimation* primary_animation
    = instance->primary_base_animation.animation;
  if (primary_animation) {
    const SkinnedMeshAnimation* secondary_animation
      = instance->secondary_base_animation.animation;
    animated_mesh_renderer_->resetTrack(secondary_track_);

    if (secondary_animation) {
      float primary_weight = instance->primary_base_animation.weight;
      animated_mesh_renderer_->setTrackAnimation(
        primary_track_,
        primary_animation->animation,
        fmod((time-instance->primary_base_animation.start_time)
                *instance->primary_base_animation.speed,1.0));
      animated_mesh_renderer_->setTrackAnimation(
        secondary_track_,
        secondary_animation->animation,
        fmod((time-instance->secondary_base_animation.start_time)
                *instance->secondary_base_animation.speed,1.0));
      animated_mesh_renderer_->blendTracks(secondary_track_,
                                           primary_track_,
                                           primary_weight,
                                           primary_track_);

      // Blend in the secondary animation until it takes over and
      // becomes the new primary
      float new_primary_weight = primary_weight * 0.5;
      if (primary_weight - new_primary_weight < 0.001f) {
        memcpy(&instance->primary_base_animation,
               &instance->secondary_base_animation,
               sizeof(instance->primary_base_animation));
        instance->primary_base_animation.weight = 1.0;
        instance->secondary_base_animation.animation = NULL;
      } else {
        instance->primary_base_animation.weight = new_primary_weight;
        instance->secondary_base_animation.weight = 1.0f - new_primary_weight;
      }
    } else {
      // special behavior: for a zero speed, set the position to be
      // exactly equal to the start time parameter
      float speed = instance->primary_base_animation.speed;
      float position = fmod(speed == 0.0f
        ? float(instance->primary_base_animation.start_time)
        : (time-instance->primary_base_animation.start_time)
                *instance->primary_base_animation.speed,1.0);
      animated_mesh_renderer_->setTrackAnimation(
        primary_track_,
        primary_animation->animation,
        position);
    }
  }

  // Set the action animation
  const SkinnedMeshAnimation* action_animation
    = instance->action_animation.animation;
  if (action_animation != NULL) {
    float weight = instance->action_animation.weight;
    float position = (time-instance->action_animation.start_time)
                          *instance->action_animation.speed;
    if (position < 1.0f) {
      if (weight < 1.0f) {
        animated_mesh_renderer_->addTrackAnimation(
          primary_track_,
          1.0f - weight,
          action_animation->animation,
          weight,
          position);
        float new_weight = 1.0f - ((1.0f - weight) * 0.5f);
        if (new_weight - weight < 0.01f) {
          new_weight = 1.0f;
        }
        instance->action_animation.weight = new_weight;
      } else {
        animated_mesh_renderer_->setTrackAnimation(
          primary_track_,
          action_animation->animation,
          position);
      }
    } else {
      instance->action_animation.animation = NULL;
    }
  }

  // Create the bone matrices based on the current animation state
  D3DXMATRIXA16 form_root_transform;
  D3DXMatrixMultiply(&form_root_transform,
                     &skinned_mesh_form->transform,
                     root_transform);
  animated_mesh_renderer_->buildAnimatedBones(
    &form_root_transform,
    animated_mesh,
    primary_track_,
    instance->bone_matrices);

  // Create the attachment points
  for (size_t i = 0; i < NUMBER_OF_ATTACHMENT_POINTS; ++i) {
    const SkinnedMeshAttachmentPointOffset* offset
      = skinned_mesh->attachment_point_offsets + i;
    assert(offset->source_bone <
           animated_mesh_renderer_->numberOfBoneMatrices(animated_mesh));
    D3DXMatrixMultiply(&instance->attachment_point_matrices[i],
                       &offset->offset,
                       &instance->bone_matrices[offset->source_bone]);
  }
}





//----[  beginScene  ]---------------------------------------------------------
void SkinnedMeshRenderer::beginScene() {
  assert(animated_mesh_renderer_);
  if (renderable_elements_out_of_order_)  sortRenderableElements();
  next_texture_ = number_of_renderable_elements_ > 0
      ? renderable_element_pointers_[0]->texture_group_texture
      : Texture::INVALID_TEXTURE_INDEX;
  next_renderable_element_pointer_index_ = 0;
}






//----[  render  ]-------------------------------------------------------------
bool SkinnedMeshRenderer::render(Texture::TextureIndex texture,
                                 EvidyonD3DRenderStateManager* renderstate_manager) {
  if (next_texture_ != texture) return true;
  assert(animated_mesh_renderer_);

  // State data
  size_t next_renderable_element_pointer_index
    = next_renderable_element_pointer_index_;
  SkinnedMeshInstanceRenderableElement** renderable_element_pointer
    = &renderable_element_pointers_[next_renderable_element_pointer_index];
  SkinnedMeshInstanceRenderableElement* renderable_element;
  AnimatedMesh::AnimatedMeshIndex current_animated_mesh_index
    = AnimatedMesh::INVALID_ANIMATED_MESH_INDEX;
  unsigned int current_texture_group = -1;
  unsigned int current_texture_group_subset = -1;
  while ((next_renderable_element_pointer_index < number_of_renderable_elements_) &&
         texture == (renderable_element
                      = *renderable_element_pointer)->texture_group_texture) {
    ++next_renderable_element_pointer_index;
    ++renderable_element_pointer;

    // Skip hidden instances
    SkinnedMeshInstance* instance = renderable_element->instance;
    if (instance->hidden) continue;

    // Switch to this subset, if necessary
    AnimatedMesh::AnimatedMeshIndex animated_mesh
      = renderable_element->animated_mesh;
    unsigned int texture_group
      = renderable_element->texture_group;
    unsigned int texture_group_subset
      = renderable_element->texture_group_subset;
    if (animated_mesh != current_animated_mesh_index ||
        texture_group != current_texture_group ||
        texture_group_subset != current_texture_group_subset) {
      renderstate_manager->setAnimatedGeometryTexture(texture);
      animated_mesh_renderer_->initializeAnimatedMesh(
        animated_mesh,
        texture_group,
        texture_group_subset,
        renderstate_manager);
      current_animated_mesh_index = animated_mesh;
      current_texture_group = texture_group;
      current_texture_group_subset = texture_group_subset;
    }

    // Draw the mesh
    if (!animated_mesh_renderer_->renderInstance(instance->bone_matrices)) {
      return false;
    }
  }

  // Calculate entries for the next rendering loop
  next_texture_ =
    next_renderable_element_pointer_index < number_of_renderable_elements_
      ? renderable_element->texture_group_texture
      : Texture::INVALID_TEXTURE_INDEX;
  next_renderable_element_pointer_index_
    = next_renderable_element_pointer_index;

  // Success
  return true;
}




//----[  zero  ]---------------------------------------------------------------
void SkinnedMeshRenderer::zero() {
  animated_mesh_renderer_ = NULL;
  skinned_meshes_.clear();
  renderable_elements_.destroy();
  memset(renderable_element_pointers_, 0, sizeof(renderable_element_pointers_));
  number_of_renderable_elements_ = 0;
  renderable_elements_out_of_order_ = false;
  next_texture_ = Texture::INVALID_TEXTURE_INDEX;
  next_renderable_element_pointer_index_ = 0;
  primary_track_ = NULL;
  secondary_track_ = NULL;
}



//----[  EvidyonSceneryRenderer_CompareRenderableSubsetPtrs  ]-----------------
int CompareSkinnedMeshInstanceRenderableElementPointers(const void* p1,
                                                        const void* p2) {
  using namespace Evidyon::SkinnedMesh;
  const SkinnedMeshInstanceRenderableElement* a =
    *(const SkinnedMeshInstanceRenderableElement**)p1;
  const SkinnedMeshInstanceRenderableElement* b =
    *(const SkinnedMeshInstanceRenderableElement**)p2;
  { // sort by ascending texture index
    Evidyon::Texture::TextureIndex a_texture = a->texture_group_texture;
    Evidyon::Texture::TextureIndex b_texture = b->texture_group_texture;
    if (a_texture > b_texture) return +1;
    if (a_texture < b_texture) return -1;
  }
  { // sort by ascending mesh index
    Evidyon::AnimatedMesh::AnimatedMeshIndex a_animatedmesh = a->animated_mesh;
    Evidyon::AnimatedMesh::AnimatedMeshIndex b_animatedmesh = b->animated_mesh;
    if (a_animatedmesh > b_animatedmesh) return +1;
    if (a_animatedmesh < b_animatedmesh) return -1;
  }
  { // sort by ascending texture group
    unsigned int a_texture_group = a->texture_group;
    unsigned int b_texture_group = b->texture_group;
    if (a_texture_group > b_texture_group) return +1;
    if (a_texture_group < b_texture_group) return -1;
  }
  { // sort by ascending texture group subset
    unsigned int a_texture_group_subset = a->texture_group_subset;
    unsigned int b_texture_group_subset = b->texture_group_subset;
    if (a_texture_group_subset > b_texture_group_subset) return +1;
    if (a_texture_group_subset < b_texture_group_subset) return -1;
  }

  // be sure this instance is not multiply referenced
  assert(a->instance != b->instance);

  // pointers are equal
  return 0;
}



//----[  sortRenderableElements  ]---------------------------------------------
void SkinnedMeshRenderer::sortRenderableElements() {
  qsort(renderable_element_pointers_,
        number_of_renderable_elements_,
        sizeof(SkinnedMeshInstanceRenderableElement*),
        CompareSkinnedMeshInstanceRenderableElementPointers);
  renderable_elements_out_of_order_ = false;
}






//----[  eraseRenderableElementsOf  ]------------------------------------------
void SkinnedMeshRenderer::eraseRenderableElementsOf(
    SkinnedMeshInstance* instance) {
  // Go through the list and copy down members as necessary to overwrite
  // subsets of the instance that was removed.
  // Variable 'i' is the scanner and 'j' is the placer. 'j' will lag 'i' by one
  // more place each time an instance is erased so that valid pointers get
  // copied down in the list.
  // [This code is taken from sceneryrenderer.cpp]
  size_t i = 0, j = 0;
  while (i < number_of_renderable_elements_) {
    SkinnedMeshInstanceRenderableElement* element =
      renderable_element_pointers_[i];
    if (element->instance == instance) { 
      renderable_elements_.release(element);
    } else {
      if (i != j) renderable_element_pointers_[j] = element;
      ++j;
    }
    ++i;
  }
  number_of_renderable_elements_ -= i - j;
  assert(number_of_renderable_elements_ ==
         renderable_elements_.numberOfAllocatedEntries());
}






//----[  allocateAnimationTracks  ]--------------------------------------------
bool SkinnedMeshRenderer::allocateAnimationTracks() {
  assert(animated_mesh_renderer_);
  if (!animated_mesh_renderer_) return false;
  assert(!primary_track_);
  primary_track_ = animated_mesh_renderer_->acquireAnimationTrack();
  secondary_track_ = animated_mesh_renderer_->acquireAnimationTrack();
  return (primary_track_ != NULL) && (secondary_track_ != NULL);
}






//----[  releaseAnimationTracks  ]---------------------------------------------
void SkinnedMeshRenderer::releaseAnimationTracks() {
  if (primary_track_) {
    assert(animated_mesh_renderer_);
    animated_mesh_renderer_->releaseAnimationTrack(primary_track_);
    primary_track_ = NULL;
  }
  if (secondary_track_) {
    assert(animated_mesh_renderer_);
    animated_mesh_renderer_->releaseAnimationTrack(secondary_track_);
    secondary_track_ = NULL;
  }
}




}
}