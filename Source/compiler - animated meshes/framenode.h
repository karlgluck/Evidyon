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
#ifndef __FRAMENODE_H__
#define __FRAMENODE_H__
#pragma once


// added august 2009
#include "shared/animated-mesh/animatedmeshanimation.h"

typedef std::list<std::pair<std::string,Evidyon::AnimatedMesh::AnimatedMeshAnimation*>> EvidyonAnimatedMeshSet;

/**
 * Used to hold data from a frame loaded from a .X file
 */
class FrameNode
{
    public:

        /**
         * This is used to store a mesh object
         */
        struct Mesh
        {
            DWORD numberOfMaterials;
            ID3DXBuffer* adjacencyBuffer;
            ID3DXBuffer* materialsBuffer;
            ID3DXBuffer* effectInstancesBuffer;
            ID3DXSkinInfo* skinInfo;
            ID3DXMesh* mesh;
        };

        struct ScaleAnimationKey
        {
            DWORD tick;
            D3DXVECTOR3 vector;
        };

        struct TranslateAnimationKey
        {
            DWORD tick;
            D3DXVECTOR3 vector;
        };

        struct RotateAnimationKey
        {
            DWORD tick;
            D3DXQUATERNION quaternion;
        };

        struct TransformAnimationKey
        {
            DWORD tick;
            D3DXMATRIX matrix;
        };

        struct AnimationOptions
        {
            /// Set to true if this animation options structure has been initialized
            bool valid;

            /// If true, this animation loops.  If false, it is "closed" and should
            /// only be played once.
            bool loops;

            /// If set to true, the animation should be spline-interpolated.  If
            /// false, it should be linearly interpolated.
            bool useSplineInterpolation;
        };

        struct FrameAnimation
        {
            /// The name of the frame that this animation applies to.  There can be multiple
            /// names, but only one of them should reference a valid frame.
            std::list<std::string> frameNames;

            ScaleAnimationKey* scalingKeys;
            size_t numberOfScalingKeys;

            TranslateAnimationKey* translationKeys;
            size_t numberOfTranslationKeys;

            RotateAnimationKey* rotationKeys;
            size_t numberOfRotationKeys;

            TransformAnimationKey* transformationKeys;
            size_t numberOfTransformationKeys;

            /// Animation options specified for this bone
            AnimationOptions animationOptions;
        };

        struct Animation
        {
            /// What this animation is called
            std::string name;

            /// Transformation sequences for each of the frames involved in this animation
            std::vector<FrameAnimation*> frameAnimations;

            /// Animation options specified for this animation
            AnimationOptions animationOptions;
        };

    public:

        typedef std::list<std::pair<std::string,UnseenSkeletalAnimation*>> UnseenAnimationSet;

    public:

        /**
         * Initializes this class
         */
        FrameNode();

        /**
         * Backup method to clean up leftover memory
         */
        ~FrameNode();

        /**
         * Loads data into this node
         */
        bool load( ID3DXFileData* dxFileData, IDirect3DDevice9* d3dDevice );

        /**
         * Frees data from this node and sub-nodes
         */
        void destroy();

        /**
         * Gets this frame node's path in the hierarchy
         */
        const std::string& path() const;

        /**
         * "Bakes" this mesh into the output mesh/animation structures
         */

        // new animation type added august 2009
        bool bake( LPDIRECT3DDEVICE9 d3dDevice,
          UnseenSkinnedMesh* mesh,
          UnseenAnimationSet* animationSet,
          EvidyonAnimatedMeshSet* animations,
          float minimumBoneInfluenceWeight = 0.01f);

        /**
         * Frees the memory in an animation set that was allocated by the bake method
         */
        void deallocate( UnseenAnimationSet* animationSet );

    public:

        /**
         * Gets the parent of this node
         */
        FrameNode* getParentNode() { return myParentNode; }

        /**
         * Gets the names for this node
         */
        const std::list<std::string>& getNames() { return myNames; }

        /**
         * Gets this frame's hierarchial offset
         */
        const D3DXMATRIX* getTransform() { return &myTransform; }


    protected:

        /**
         * Loads a mesh encountered while parsing this frame node
         */
        bool loadMesh( const std::string& name, ID3DXFileData* dxFileData, IDirect3DDevice9* d3dDevice );

        /**
         * Loads this frame's transformation matrix
         */
        bool loadFrameTransformMatrix( const std::string& name, ID3DXFileData* dxFileData );

        /**
         * Loads a child frame for this mesh
         */
        bool loadChildFrame( const std::string& name, ID3DXFileData* dxFileData, IDirect3DDevice9* d3dDevice );

        /**
         * Loads an animation encountered while parsing this frame node
         */
        bool loadAnimationSet( const std::string& name, ID3DXFileData* dxFileData );

        /**
         * Loads an animation sub-object encountered while parsing this frame node.  This method is
         * called exclusively by loadAnimationSet.
         */
        bool loadAnimation( const std::string& name, ID3DXFileData* dxFileData, Animation* animation );

        /**
         * Loads animation options into the given object
         */
        bool loadAnimationOptions( const std::string& name, ID3DXFileData* dxFileData, AnimationOptions* animationOptions );

        /**
         * Loads a key that is used to animate the given bone
         */
        bool loadAnimationKey( const std::string& name, ID3DXFileData* dxFileData, FrameAnimation* frameAnimation );

        /**
         * Loads keys from the source data pointer into the 
         */
        bool loadRotationKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation );

        /**
         * Loads keys from the source data pointer into the 
         */
        bool loadScalingKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation );

        /**
         * Loads keys from the source data pointer into the 
         */
        bool loadTranslationKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation );

        /**
         * Loads keys from the source data pointer into the 
         */
        bool loadTransformationKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation );

    protected:

        /**
         * Turns scale/rotate/translate keys into transform keys
         */
        void bakeIntoTransformKeys( const D3DXMATRIX* frameTransform, FrameAnimation* animation );

        /**
         * Recursively searches the frame node hierarchy and builds a table of all of the meshes
         */
        void findAllMeshes( std::multimap<FrameNode*,Mesh*>* meshes );

        /**
         * Recursively searches the frame node hierarchy for all of the frames
         */
        void findAllFrames( std::vector<FrameNode*>* frameNodes );


    protected:

        /**
         * Gets the name of a file data element
         */
        static void getFileDataName( ID3DXFileData* dxFileData, std::string* name );


    protected:

        /// The owner frame node
        FrameNode* myParentNode;

        /// This frame's transformation matrix
        D3DXMATRIX myTransform;

        /// The list of frame children in the hierarchy
        std::vector<FrameNode*> myChildren;

        /// Meshes encountered in this frame node
        std::vector<Mesh*> myMeshes;

        /// Animations encountered in this frame node
        std::vector<Animation*> myAnimations;

        /// The path to this frame node in the hierarchy; returned by path()
        std::string myPath;

        /// This frame's name(s)
        std::list<std::string> myNames;

        /// The number of animation ticks-per-second
        DWORD myAnimationTicksPerSecond;
};



#endif