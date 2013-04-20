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
// File:    unseenmesh.cpp
// Desc:    Defines structures for the .usm (Unseen Skinned Mesh) and .uka (Unseen Keyframe Animation) formats
//-----------------------------------------------------------------------------
#include <dc/debug>
#include <dc/filestream>
#include "unseenskinnedmesh.h"
#include "common/safemacros.h" // for SAFE_DELETE_ARRAY



//-----------------------------------------------------------------------------
// Name: deallocateUnseenSkinnedMesh
// Desc: 
//-----------------------------------------------------------------------------
void deallocateUnseenSkinnedMesh(UnseenSkinnedMesh* mesh)
{
    SAFE_DELETE_ARRAY(mesh->skeleton);
    SAFE_DELETE_ARRAY(mesh->frameHierarchy);

    for (DWORD tgi = 0; tgi < mesh->info.numberOfTextureGroups; ++tgi)
    {
        UnseenSkinnedMesh::TextureGroup* textureGroup = &mesh->textureGroups[tgi];

        for (DWORD vigi = 0; vigi < textureGroup->info.numberOfVertexInfluenceGroups; ++vigi)
        {
            SAFE_DELETE_ARRAY(textureGroup->vertexInfluenceGroups[vigi].boneInfluences);
            SAFE_DELETE_ARRAY(textureGroup->vertexInfluenceGroups[vigi].vertices);
            SAFE_DELETE_ARRAY(textureGroup->vertexInfluenceGroups[vigi].indices);
        }
    }

    SAFE_DELETE_ARRAY(mesh->textureGroups);

    // Reset the structure
    ZeroMemory(mesh, sizeof(UnseenSkinnedMesh));
}




//-----------------------------------------------------------------------------
// Name: saveUnseenSkinnedMesh
// Desc: Writes a mesh to the output stream
//-----------------------------------------------------------------------------
bool saveUnseenSkinnedMesh(dc::dcStreamOut* stream, const UnseenSkinnedMesh* mesh)
{
    stream->write(&mesh->info, sizeof(UnseenSkinnedMeshInfo));
    stream->write(mesh->skeleton, sizeof(UnseenSkinnedMeshBoneInfo) * mesh->info.numberOfBones);
    stream->write(mesh->frameHierarchy, sizeof(UnseenSkinnedMeshFrameInfo) * mesh->info.numberOfFrames);
    
    for (size_t i = 0; i < mesh->info.numberOfTextureGroups; ++i)
    {
        UnseenSkinnedMesh::TextureGroup* textureGroup = &mesh->textureGroups[i];
        stream->write(&textureGroup->info, sizeof(UnseenSkinnedMeshTextureGroupInfo));

        for (size_t g = 0; g < textureGroup->info.numberOfVertexInfluenceGroups; ++g)
        {
            UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup* influenceGroup = &textureGroup->vertexInfluenceGroups[g];
            stream->write(&influenceGroup->info, sizeof(UnseenSkinnedMeshVertexInfluenceGroupInfo));
            stream->write(influenceGroup->boneInfluences, sizeof(UnseenSkinnedMeshBoneInfluenceInfo) * mesh->info.numberOfBones);
            stream->write(influenceGroup->vertices, influenceGroup->info.vertexSizeInBytes * influenceGroup->info.numberOfVertices);
            stream->write(influenceGroup->indices, influenceGroup->info.indexSizeInBytes * influenceGroup->info.numberOfTriangles * 3);
        }
    }

    // Success
    return true;
}



//-----------------------------------------------------------------------------
// Name: saveUnseenSkeletalAnimation
// Desc: Writes an animation to the output stream
//-----------------------------------------------------------------------------
bool saveUnseenSkeletalAnimation(dc::dcStreamOut* stream, const UnseenSkeletalAnimation* animation)
{
    stream->write(&animation->info, sizeof(UnseenSkeletalAnimationInfo));
    stream->write(animation->frameIndexToAnimatedFrameTransformTable, sizeof(DWORD) * animation->info.numberOfSkeletalFrames);
    for (DWORD keyframeIndex = 0; keyframeIndex < animation->info.numberOfKeyframes; ++keyframeIndex)
    {
        UnseenSkeletalAnimation::Keyframe* keyframe = &animation->keyframes[keyframeIndex];
        stream->write(&keyframe->tick, sizeof(DWORD));
        stream->write(keyframe->animatedFrameTransforms, sizeof(D3DXMATRIX) * animation->info.numberOfAnimatedFrames);
    }

    // Success
    return true;
}



//-----------------------------------------------------------------------------
// Name: loadUnseenSkeletalAnimation
// Desc: Reads an animation from the input stream
//-----------------------------------------------------------------------------
bool loadUnseenSkeletalAnimation(dc::dcStreamIn* stream, UnseenSkeletalAnimation* animation)
{
    CONFIRM(stream->read(&animation->info, sizeof(UnseenSkeletalAnimationInfo))) else return false;
//    DEBUG_INFO("\nnumber of skeletal frames: %lu\nnumber of animated frames: %lu\nnumber of keyframes:%lu", animation->info.numberOfSkeletalFrames, animation->info.numberOfAnimatedFrames, animation->info.numberOfKeyframes);
    animation->frameIndexToAnimatedFrameTransformTable = new DWORD[animation->info.numberOfSkeletalFrames];
    CONFIRM(animation->frameIndexToAnimatedFrameTransformTable) else return false;
    animation->keyframes = new UnseenSkeletalAnimation::Keyframe[animation->info.numberOfKeyframes];
    CONFIRM(animation->keyframes) else
    {
        SAFE_DELETE_ARRAY(animation->frameIndexToAnimatedFrameTransformTable);
        return false;
    }

    CONFIRM(stream->read(animation->frameIndexToAnimatedFrameTransformTable, sizeof(DWORD) * animation->info.numberOfSkeletalFrames))
        else return false;

    for (DWORD keyframeIndex = 0; keyframeIndex < animation->info.numberOfKeyframes; ++keyframeIndex)
    {
        UnseenSkeletalAnimation::Keyframe* keyframe = &animation->keyframes[keyframeIndex];
        keyframe->animatedFrameTransforms = new D3DXMATRIX[animation->info.numberOfAnimatedFrames];

        CONFIRM(stream->read(&keyframe->tick, sizeof(DWORD))) else return false;
        CONFIRM(stream->read(keyframe->animatedFrameTransforms, sizeof(D3DXMATRIX) * animation->info.numberOfAnimatedFrames)) else return false;
    }

    // Success
    return true;
}

void deallocateUnseenSkeletalAnimation(UnseenSkeletalAnimation* animation)
{
    for (DWORD keyframeIndex = 0; keyframeIndex < animation->info.numberOfKeyframes; ++keyframeIndex)
    {
        SAFE_DELETE_ARRAY(animation->keyframes[keyframeIndex].animatedFrameTransforms);
    }
    SAFE_DELETE_ARRAY(animation->frameIndexToAnimatedFrameTransformTable);
}


//-----------------------------------------------------------------------------
// Name: loadUnseenSkinnedMesh
// Desc: Reads a skinned mesh from the input stream
//-----------------------------------------------------------------------------
bool loadUnseenSkinnedMesh(dc::dcStreamIn* stream, UnseenSkinnedMesh* mesh)
{
    stream->read(&mesh->info, sizeof(UnseenSkinnedMeshInfo));
    mesh->skeleton = new UnseenSkinnedMeshBoneInfo[mesh->info.numberOfBones];
    mesh->frameHierarchy = new UnseenSkinnedMeshFrameInfo[mesh->info.numberOfFrames];
    mesh->textureGroups = new UnseenSkinnedMesh::TextureGroup[mesh->info.numberOfTextureGroups];

    stream->read(mesh->skeleton, sizeof(UnseenSkinnedMeshBoneInfo) * mesh->info.numberOfBones);
    stream->read(mesh->frameHierarchy, sizeof(UnseenSkinnedMeshFrameInfo) * mesh->info.numberOfFrames);
    
    for (size_t i = 0; i < mesh->info.numberOfTextureGroups; ++i)
    {
        UnseenSkinnedMesh::TextureGroup* textureGroup = &mesh->textureGroups[i];
        stream->read(&textureGroup->info, sizeof(UnseenSkinnedMeshTextureGroupInfo));
        textureGroup->vertexInfluenceGroups = new UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup[textureGroup->info.numberOfVertexInfluenceGroups];

        for (size_t g = 0; g < textureGroup->info.numberOfVertexInfluenceGroups; ++g)
        {
            UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup* influenceGroup = &textureGroup->vertexInfluenceGroups[g];
            stream->read(&influenceGroup->info, sizeof(UnseenSkinnedMeshVertexInfluenceGroupInfo));

            influenceGroup->boneInfluences = new UnseenSkinnedMeshBoneInfluenceInfo[mesh->info.numberOfBones];
            influenceGroup->vertices = new BYTE[influenceGroup->info.vertexSizeInBytes * influenceGroup->info.numberOfVertices];
            influenceGroup->indices = new BYTE[influenceGroup->info.indexSizeInBytes * influenceGroup->info.numberOfTriangles * 3];

            stream->read(influenceGroup->boneInfluences, sizeof(UnseenSkinnedMeshBoneInfluenceInfo) * mesh->info.numberOfBones);
            stream->read(influenceGroup->vertices, influenceGroup->info.vertexSizeInBytes * influenceGroup->info.numberOfVertices);
            stream->read(influenceGroup->indices, influenceGroup->info.indexSizeInBytes * influenceGroup->info.numberOfTriangles * 3);
        }
    }

    // Success
    return true;
}