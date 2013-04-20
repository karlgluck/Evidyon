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
//-----------------------------------------------------------------------------
// File:  unseenskinnedmesh.h
// Desc:  Defines structures for the .usm (Unseen Skinned Mesh) and .uka (Unseen Keyframe Animation) formats
//-----------------------------------------------------------------------------
#ifndef __UNSEENSKINNEDMESH_H__
#define __UNSEENSKINNEDMESH_H__
#pragma once



#pragma pack(push, 16)

/**
 * Defines information about the mesh
 */
struct UnseenSkinnedMeshInfo
{
  /// How many different texture groups there are in this mesh
  DWORD numberOfTextureGroups;

  /// The number of entries in the mesh frame hierarchy.  This is greater than or equal to the
  /// number of transforming bones.
  DWORD numberOfFrames;

  /// How many bones (picked from the bones in the skeleton) that mesh uses to transform geometry
  DWORD numberOfBones;
};

/**
 * Used to define the skeletal hierarchy of the mesh
 */
struct UnseenSkinnedMeshFrameInfo
{
  /// This frame's offset from its parent in the hierarchy
  D3DXMATRIX offset;

  /// This must be less than this frame's index.  The only exception is bone 0, the
  /// root bone, for which this value must be 0xFFFFFFFF.
  DWORD parentFrameIndex;
};

/**
 * Defines a transforming bone
 */
struct UnseenSkinnedMeshBoneInfo
{
  /// Multiply the source frame's adjusted offset by this matrix before setting into the device
  D3DXMATRIX inverseOffset;

  /// The frame index from which this bone's transformation is derived
  DWORD frameIndex;
};


/**
 * Defines information for each texture group
 */
struct UnseenSkinnedMeshTextureGroupInfo
{
  /// How many vertex influence groups are part of this texture group
  DWORD numberOfVertexInfluenceGroups;
};


/**
 * Each texture group is divided into sections where each vertex only takes the
 * influence of a given number of bones.
 */
struct UnseenSkinnedMeshVertexInfluenceGroupInfo
{
  /// How many matrices affect vertices in this group.  This is used to create buffers in software
  /// rendering mode if the user's hardware doesn't have a high enough number of vertex influences.
  /// This can be at most 4.
  DWORD numberOfVertexInfluences;

  /// The FVF format of a vertex in this group
  DWORD d3dVertexFormat;

  /// How many bytes each vertex is
  DWORD vertexSizeInBytes;

  /// The format of an index used in this group
  DWORD d3dIndexFormat;

  /// How many bytes each index is
  DWORD indexSizeInBytes;

  /// How many vertices are in this geometry
  DWORD numberOfVertices;

  /// How many triangles are in this geometry subset
  DWORD numberOfTriangles;
};

/**
 * This structure is used to define the influences each bone has on the geometry in the mesh texture
 * group.  It is essential that this information is filled out correctly, because it is used by the
 * application that loads the mesh to define how to split the mesh between hardware/software rendering
 * if it is unable to render the requred number of bones in hardware.
 */
struct UnseenSkinnedMeshBoneInfluenceInfo
{
  /// Contains rendering information for drawing bones including and below this bone.  When
  /// rendering with Direct3DDevice9::DrawIndexedPrimitive, the BaseVertexIndex, MinIndex
  /// and StartIndex parameters should all be 0.  If the numberOfTriangles member is 0, the
  /// other values are undefined.
  struct
  {
    /// How many indexed triangle list primitives to draw
    DWORD numberOfTriangles;

    /// How many vertices (starting at the beginning of the vertex buffer) are referenced when drawing
    DWORD numberOfVertices;

  } thisAndLower;

  /// Contains rendering information for bones including & after this bone.  If the numberOfTriangles
  /// member is 0, the rest of the values are undefined.  When rendering with DrawIndexedPrimitive, the
  /// BaseVertexIndex parameter should be 0.
  struct
  {
    /// The location in the index buffer at which to start drawing triangles
    DWORD triangleStartIndex;

    /// How many indexed triangle list primitives to draw
    DWORD numberOfTriangles;

    /// The lowest vertex that is referenced by the draw call
    DWORD lowestReferencedVertexIndex;

    /// The number of vertices that should be loaded to render the triangles
    DWORD numberOfVertices;

  } thisAndHigher; // TODO: make this "higher" only!!!!
};



/**
 * This is the structure at the start of a skeletal animation file.
 */
struct UnseenSkeletalAnimationInfo
{
  /// How many ticks long this animation is
  DWORD tickLength;

  /// Should this animation be looped?
  bool loop;

  /// The number of frames in the hierarchy of the mesh that this animation controls
  DWORD numberOfSkeletalFrames;

  /// The number of frames that this animation controls.  This is always less than
  /// or equal to the number of skeletal bones.
  DWORD numberOfAnimatedFrames;

  /// How many keyframes are in this animation
  DWORD numberOfKeyframes;

  /// Approximately how many ticks elapse per keyframe.  This helps optimize the
  /// search for keyframes during interpolation.
  DWORD averageTicksPerKeyframe;

  /// Multiplying a tick index by this value gives the corresponding index in seconds
  double secondsPerTick;
};



/**
 * This is the non-renderable in-memory structure of a mesh
 */
struct UnseenSkinnedMesh
{
  /// Information about this mesh
  UnseenSkinnedMeshInfo info;

  /// This mesh's frame hierarchy.  This is used to apply mesh transformations (for animation), and to build
  /// the hierarchy of the mesh.  The number of entries in this array is info.numberOfFrames
  UnseenSkinnedMeshFrameInfo* frameHierarchy;

  /// For each index 'b' in this array, transformation matrix 'b' in the rendering device should be set to the
  /// updated transform of entry frameHierarchy[skeleton[b].frameIndex] * skeleton[b].inverseOffset.  The size
  // of this array is info.numberOfBones
  UnseenSkinnedMeshBoneInfo* skeleton;

  /**
   * A texture group contains all of the geometry used by a single texture
   */
  struct TextureGroup
  {
    /// Information about this texture group
    UnseenSkinnedMeshTextureGroupInfo info;

    /**
     * A vertex influence group holds all of the geometry in a texture group that has
     * the same number of per-vertex influences.
     */
    struct VertexInfluenceGroup
    {
      /// Defines information about this influence group
      UnseenSkinnedMeshVertexInfluenceGroupInfo info;

      /// How each bone affects the geometry present in this group
      UnseenSkinnedMeshBoneInfluenceInfo* boneInfluences;

      /// Vertex data.  The format of the contents is defined by info.d3dVertexFormat.  The number
      /// of entries in the defined format is info.numberOfVertices
      VOID* vertices;

      /// Index data.  The format of the contents is defined by info.d3dIndexFormat.  The number
      /// of entries in the defined format is info.numberOfIndices
      VOID* indices;
    };

    /// The vertex influence groups in this texture group
    VertexInfluenceGroup* vertexInfluenceGroups;
  };

  /// The texture groups in this mesh.  The number of entries in this array is info.numberOfTextureGroups
  TextureGroup* textureGroups;
};



/**
 * Contains an animation
 */
struct UnseenSkeletalAnimation
{
  /// Defines information about this animation
  UnseenSkeletalAnimationInfo info;

  /// Maps a frame's index in the skeleton to its index in each keyframe's transformation matrix
  /// array.  If a bone doesn't exist in the animation, it is set to 0xFFFFFFFF in this array.
  /// The length of this array is info.numberOfSkeletalFrames
  DWORD* frameIndexToAnimatedFrameTransformTable;

  /**
   * Details information about a keyframe in the animation
   */
  struct Keyframe
  {
    /// The animation-local tick index at which this keyframe should be applied
    DWORD tick;

    /// The transforms of the animated frames.  The length of this array is info.numberOfAnimatedFrames
    D3DXMATRIX* animatedFrameTransforms;
  };

  /// Defines the keyframes in this animation.  The length of this array is info.numberOfKeyframes
  Keyframe* keyframes;
};

#pragma pack(pop)

void deallocateUnseenSkinnedMesh(UnseenSkinnedMesh* mesh);

void deallocateUnseenSkeletalAnimation(UnseenSkeletalAnimation* animation);


/**
 * Writes a mesh to the output stream
 */
bool saveUnseenSkinnedMesh(dc::dcStreamOut* stream, const UnseenSkinnedMesh* mesh);

/**
 * Loads a mesh from the input stream
 */
bool loadUnseenSkinnedMesh(dc::dcStreamIn* stream,  UnseenSkinnedMesh* outputMesh);

/**
 * Writes an animation to the output stream
 */
bool saveUnseenSkeletalAnimation(dc::dcStreamOut* stream, const UnseenSkeletalAnimation* animation);

/**
 * Loads an animation from the input stream
 */
bool loadUnseenSkeletalAnimation(dc::dcStreamIn* stream, UnseenSkeletalAnimation* outputAnimation);



#endif