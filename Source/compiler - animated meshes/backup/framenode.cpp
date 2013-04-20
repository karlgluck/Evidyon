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
#include "stdafx.h"
#include "rmxfguid.h"   // Resolves X file template GUIDs



//------------------------------------------------------------------------------------------------
// Name: FrameNode
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
FrameNode::FrameNode()
{
    myParentNode = 0;
    D3DXMatrixIdentity( &myTransform );
    myAnimationTicksPerSecond = 0;
}



//------------------------------------------------------------------------------------------------
// Name: ~FrameNode
// Desc: Backup method to clean up leftover memory
//------------------------------------------------------------------------------------------------
FrameNode::~FrameNode()
{
    destroy();
}



//------------------------------------------------------------------------------------------------
// Name: load
// Desc: Loads data into this node
//------------------------------------------------------------------------------------------------
bool FrameNode::load( ID3DXFileData* dxFileData, IDirect3DDevice9* d3dDevice )
{
    // Get this data's type
    GUID dataType;
    if( APP_ERROR( FAILED( dxFileData->GetType( &dataType ) ) )( "Couldn't get data object's type" ) )
        return false;

    // Get the template name (if any).  We have to get the size first, otherwise we could
    // unknowingly get back a null name.
    std::string templateName = path() + " / ";
    SIZE_T nameSize;
    if( APP_ERROR( FAILED( dxFileData->GetName( NULL, &nameSize ) ) )( "Couldn't get data object's name size" ) )
        return false;
    if( nameSize )
    {
        templateName.reserve( nameSize + 1 );
        char name[256];
        DWORD nameSize = sizeof(name)/sizeof(char);
        dxFileData->GetName( name, &nameSize );
        templateName.append( name );
    }
    else
        templateName.append( "(nameless)" );

    // Depending on the data type, call the appropriate handler
    if( dataType == TID_D3DRMFrame )
    {
        DEBUG_INFO( "Frame template found:  %s", templateName.c_str() );
        if( !(APP_INFO( dxFileData->IsReference() )( "Frame template was a reference; not adding to hierarchy" ) ) )
            return loadChildFrame( templateName + " <frame>", dxFileData, d3dDevice );
    } else
    if( dataType == TID_D3DRMMesh )
    {
        DEBUG_INFO( "Mesh template found:  %s", templateName.c_str() );
        return loadMesh( templateName + " <mesh>", dxFileData, d3dDevice );
    } else
    if( dataType == TID_D3DRMMaterial )
    {
        DEBUG_INFO( "IGNORED:  Material template found:  %s", templateName.c_str() );
    } else
    if( dataType == TID_D3DRMFrameTransformMatrix )
    {
        DEBUG_INFO( "Frame transformation matrix template found:  %s", templateName.c_str() );
        return loadFrameTransformMatrix( templateName + " <matrix template>", dxFileData );
    } else
    if( dataType == TID_D3DRMAnimationSet )
    {
        DEBUG_INFO( "Animation Set found:  %s", templateName.c_str() );
        return loadAnimationSet( templateName + " <animation set>", dxFileData );
    } else
    if( dataType == TID_D3DRMAnimation )
    {
        DEBUG_INFO( "IGNORED:  TID_D3DRMAnimation (%s) outside of animation set", templateName.c_str() );
        //return loadAnimation( templateName + " <animation>", dxFileData );
    } else
    if( dataType == DXFILEOBJ_XSkinMeshHeader)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_XSkinMeshHeader" );
    } else
    if( dataType == DXFILEOBJ_VertexDuplicationIndices)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_VertexDuplicationIndices" );
    } else
    if( dataType == DXFILEOBJ_FaceAdjacency)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_FaceAdjacency" );
    } else
    if( dataType == DXFILEOBJ_SkinWeights)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_SkinWeights" );
    } else
    if( dataType == DXFILEOBJ_Patch)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_Patch" );
    } else
    if( dataType == DXFILEOBJ_PatchMesh)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_PatchMesh" );
    } else
    if( dataType == DXFILEOBJ_PatchMesh9)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_PatchMesh9" );
    } else
    if( dataType == DXFILEOBJ_PMInfo)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_PMInfo" );
    } else
    if( dataType == DXFILEOBJ_PMAttributeRange)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_PMAttributeRange" );
    } else
    if( dataType == DXFILEOBJ_PMVSplitRecord)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_PMVSplitRecord" );
    } else
    if( dataType == DXFILEOBJ_FVFData)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_FVFData" );
    } else
    if( dataType == DXFILEOBJ_VertexElement)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_VertexElement" );
    } else
    if( dataType == DXFILEOBJ_DeclData)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_DeclData" );
    } else
    if( dataType == DXFILEOBJ_EffectFloats)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_EffectFloats" );
    } else
    if( dataType == DXFILEOBJ_EffectString)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_EffectString" );
    } else
    if( dataType == DXFILEOBJ_EffectDWord)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_EffectDWord" );
    } else
    if( dataType == DXFILEOBJ_EffectParamFloats)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_EffectParamFloats" );
    } else
    if( dataType == DXFILEOBJ_EffectParamString )
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_EffectParamString" );
    } else
    if( dataType == DXFILEOBJ_EffectParamDWord)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_EffectParamDWord" );
    } else
    if( dataType == DXFILEOBJ_EffectInstance)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_EffectInstance" );
    } else
    if( dataType == DXFILEOBJ_AnimTicksPerSecond )
    {
        SIZE_T dataSize = 0;
        DWORD* dataPointer = 0;
        if( APP_ERROR( FAILED( dxFileData->Lock( &dataSize, (LPCVOID*)&dataPointer ) ) )( "Couldn't lock file data to load DXFILEOBJ_AnimTicksPerSecond" ) )
            return false;

        // Warn about weird data sizes
        APP_WARNING( dataSize != sizeof(DWORD) )( "DXFILEOBJ_AnimTicksPerSecond has %lu bytes; expecting %lu", dataSize, sizeof(DWORD) );

        // Load the number of ticks per second
        DWORD animationTicksPerSecond = *dataPointer++;
        APP_WARNING( myAnimationTicksPerSecond )( "Animation ticks-per-second already set; replacing %lu with %lu", myAnimationTicksPerSecond, animationTicksPerSecond );
        myAnimationTicksPerSecond = animationTicksPerSecond;

        // Unlock this entry
        dxFileData->Unlock();

    } else
    if( dataType == DXFILEOBJ_CompressedAnimationSet)
    {
        DEBUG_INFO( "IGNORED: DXFILEOBJ_CompressedAnimationSet" );
    } else
    {
        // Unknown template type
        DEBUG_INFO( "Unknown data object encountered:  %s (ignored)", templateName.c_str() );
    }

    return true;
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Frees data from this node and sub-nodes
//------------------------------------------------------------------------------------------------
void FrameNode::destroy()
{
    // Free memory in the list of children
    for( std::vector<FrameNode*>::iterator i = myChildren.begin(); i != myChildren.end(); ++i )
    {
        FrameNode* child = *i;
        if( child )
        {
            // Make sure the child's children and other data is freed
            child->destroy();

            // Get rid of this child
            delete child;
        }
    }

    // Free skinned mesh entries
    for( std::vector<Mesh*>::iterator i = myMeshes.begin(); i != myMeshes.end(); ++i )
    {
        Mesh* mesh = *i;
        if( mesh )
        {
            SAFE_RELEASE( mesh->adjacencyBuffer );
            SAFE_RELEASE( mesh->materialsBuffer );
            SAFE_RELEASE( mesh->effectInstancesBuffer );
            SAFE_RELEASE( mesh->skinInfo );
            SAFE_RELEASE( mesh->mesh );
            delete mesh;
        }
    }

    // Get rid of animations
    for( std::vector<Animation*>::iterator i = myAnimations.begin(); i != myAnimations.end(); ++i )
    {
        Animation* animation = *i;

        // Free each of the frame animations
        std::vector<FrameAnimation*>::iterator end = animation->frameAnimations.end();
        for( std::vector<FrameAnimation*>::iterator j = animation->frameAnimations.begin(); j != end; ++j )
        {
            FrameAnimation* frameAnimation = *j;
            SAFE_DELETE_ARRAY( frameAnimation->scalingKeys );
            SAFE_DELETE_ARRAY( frameAnimation->translationKeys );
            SAFE_DELETE_ARRAY( frameAnimation->rotationKeys );
            SAFE_DELETE_ARRAY( frameAnimation->transformationKeys );
            delete frameAnimation;
        }

        delete animation;
    }
    myAnimations.clear();

    // Clear entries
    myChildren.clear();
    myMeshes.clear();
}



//------------------------------------------------------------------------------------------------
// Name: path
// Desc: Gets this frame node's path in the hierarchy
//------------------------------------------------------------------------------------------------
const std::string& FrameNode::path() const
{
    return myPath;
}


/**
 * Used inside of FrameNode::bake
 */
struct BoneVertexInfluence
{
    DWORD boneIndex;
    float weight;
};

struct MeshTriangle
{
    DWORD index[3];
};

// Build the sets of triangles for this mesh
typedef std::list<MeshTriangle> MeshTriangles;

typedef std::map<std::string,DWORD> BoneNameToIndexTable;
typedef std::multimap<DWORD,BoneVertexInfluence> VertexIndexToBoneInfluenceTable;
typedef std::set<DWORD> VertexIndexSet;


typedef std::map<DWORD,DWORD> VertexIndexToHighestInfluencingBoneIndexList;
typedef std::multimap<DWORD,DWORD> HighestInfluencingBoneIndexToVertexIndexList;
typedef std::multimap<DWORD,MeshTriangle> HighestInfluencingBoneIndexToTriangleList;
typedef std::map<DWORD,DWORD> VertexRemapTable;
typedef std::map<DWORD,DWORD> BoneRemapTable;

// 
struct InfluenceGroup
{
    VertexIndexToBoneInfluenceTable vertexIndexToBoneInfluenceTable;
    MeshTriangles triangles;
    VertexIndexSet vertexIndexSet;

    /// This list stores the highest bone index that influences a given vertex index
    VertexIndexToHighestInfluencingBoneIndexList vertexIndexToHighestInfluencingBoneIndexList;

    /// This list defines the highest bone index that influences a vertex referenced by the given triangle
    HighestInfluencingBoneIndexToTriangleList highestInfluencingBoneIndexToTriangleList;

    /// This remap serves a dual purpose:  it sorts vertex indices in this influence group so that
    /// those with lower maximum bone index influences are listed first, and ensures that the vertex
    /// buffer fills starting with index 0.
    VertexRemapTable vertexRemapTable;
};

typedef std::map<DWORD,DWORD> VertexIndexToNumberOfBoneInfluencesTable;


struct TextureSubset
{
    // Group 0: no weights, no bone indices.  All use the root transform (bone 0) and have 1.0f influence
    // Group 1: no weights, indexed bone transform.  Each vertex has a soure bone with a 1.0f influence
    // Groups 2-4 are indexed transform weighted blending with 2-4 indices
    InfluenceGroup influenceGroups[5];
    VertexIndexToNumberOfBoneInfluencesTable vertexInfluences;
};

#pragma pack(push,1)
struct SourceMeshVertex
{
    D3DXVECTOR3 pos;
    D3DXVECTOR3 normal;
    D3DXVECTOR2 tex;
};
static const DWORD SOURCE_MESH_VERTEX_FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
#pragma pack(pop)






//------------------------------------------------------------------------------------------------
// Name:  sanitizeVertexInfluences
// Desc:  Because the program that generated the bone influences can be sloppy, some vertices will
//        have more than 4 influences, will have 0 (or very tiny) bone weights, or will have other
//        problems.  The following method cleans up the influences so that the skeleton becomes
//        valid and renderable.
//------------------------------------------------------------------------------------------------
void sanitizeVertexInfluences( DWORD numberOfVerticesInMesh, VertexIndexToBoneInfluenceTable* vertexIndexToBoneInfluenceTable, float minimumBoneInfluenceWeight )
{
    for( DWORD originalVertexIndex = 0; originalVertexIndex < numberOfVerticesInMesh; ++originalVertexIndex )
    {
        VertexIndexToBoneInfluenceTable::iterator lower = vertexIndexToBoneInfluenceTable->lower_bound( originalVertexIndex );
        VertexIndexToBoneInfluenceTable::iterator upper = vertexIndexToBoneInfluenceTable->upper_bound( originalVertexIndex );

        // Warn if there are no influences, and give the vertex an influence.  
        if( APP_WARNING( lower == upper )( "Vertex %lu has no influences; assigning default", originalVertexIndex ) )
        {
            BoneVertexInfluence influence = { 0, 1.0f };
            vertexIndexToBoneInfluenceTable->insert( VertexIndexToBoneInfluenceTable::value_type( originalVertexIndex, influence ) );
            continue;
        }

        // This entry contains the list of influences sorted by their magnitude
        typedef std::multimap<float,DWORD> VertexBoneInfluences;
        VertexBoneInfluences vertexBoneInfluences;

        // Iterate through the influences for this vertex and add them to the sorted list
        for( VertexIndexToBoneInfluenceTable::iterator i = lower; i != upper; ++i )
            vertexBoneInfluences.insert( VertexBoneInfluences::value_type( i->second.weight, i->second.boneIndex ) );

        VertexBoneInfluences::iterator eraseEnd;

        // Remove any influences that are less than the threashold value, but leave at least 1 entry.  For each of the
        // influences that is removed, add to the influence of the remaining vertex.
        float sumOfRemovedInfluenceWeights = 0.0f;
        for( eraseEnd = vertexBoneInfluences.begin(); eraseEnd != vertexBoneInfluences.end() && eraseEnd->first < minimumBoneInfluenceWeight; ++eraseEnd );
        if( eraseEnd != vertexBoneInfluences.begin() )
        {
            VertexBoneInfluences::iterator eraseStart = vertexBoneInfluences.begin();
            if( APP_WARNING( eraseEnd == vertexBoneInfluences.end() )( "All of vertex %lu's influences were below the minimum threashold", originalVertexIndex ) )
                ++eraseStart;

            // Sum up the removed influences
            int numberOfRemovedInfluences = 0;
            for( VertexBoneInfluences::iterator r = eraseStart; r != eraseEnd; ++r, ++numberOfRemovedInfluences ) sumOfRemovedInfluenceWeights += r->first;

            // Tell the output about this
            DEBUG_INFO( "Erasing %lu influences (%f weight) from vertex %lu because they don't meet minimum weight",
                        numberOfRemovedInfluences, sumOfRemovedInfluenceWeights, originalVertexIndex );

            // Remove the lower influences
            vertexBoneInfluences.erase( eraseStart, eraseEnd );
        }

        // Remove entries from the start until there are less than 4 entries.  We erase from the
        // start so that those with the least noticeable influence are removed first.
        DWORD entries = vertexBoneInfluences.size();
        for( eraseEnd = vertexBoneInfluences.begin(); eraseEnd != vertexBoneInfluences.end() && entries > 4; --entries, ++eraseEnd );
        if( eraseEnd != vertexBoneInfluences.begin() )
        {
            // Count the number of entries
            int numberOfRemovedInfluences = 0;
            for( VertexBoneInfluences::iterator r = vertexBoneInfluences.begin(); r != eraseEnd; ++r, ++numberOfRemovedInfluences ) sumOfRemovedInfluenceWeights += r->first;

            // Tell the output about this
            DEBUG_INFO( "Removing %lu influences (%f weight) from vertex %lu because there are more than 4 influences on this vertex",
                        numberOfRemovedInfluences, sumOfRemovedInfluenceWeights, originalVertexIndex );

            // Remove the influences
            vertexBoneInfluences.erase( vertexBoneInfluences.begin(), eraseEnd );
        }

        // Calculate the per-remaining-vertex removed influence weight
        float weightToAddToEachRemainingVertex = sumOfRemovedInfluenceWeights / vertexBoneInfluences.size();

        // Now that we have a valid number of influences for this vertex, the influence values
        // need to be rescaled until they sum to 1.0
        float sumOfInfluences = 0.0f;
        for( VertexBoneInfluences::iterator i = vertexBoneInfluences.begin(); i != vertexBoneInfluences.end(); ++i )
            sumOfInfluences += i->first;
        float weightToAddToEachRemainingVertexToNormalize = (1.0f - sumOfInfluences) / vertexBoneInfluences.size();

        // If the normalization weight and removed weights are very different, warn because
        // that means that the weights were not normalized in the first place.  This usually
        // means that something is wrong with the model.
        APP_WARNING( fabs(weightToAddToEachRemainingVertexToNormalize - weightToAddToEachRemainingVertex) > 0.00005f )
            ( "Bone influences for vertex %lu may not have been normalized; make sure source mesh was exported correctly", originalVertexIndex );

        // Erase the original entries from the table and add the adjusted influences
        vertexIndexToBoneInfluenceTable->erase( lower, upper );
        for( VertexBoneInfluences::iterator i = vertexBoneInfluences.begin(); i != vertexBoneInfluences.end(); ++i )
        {
            BoneVertexInfluence influence = { i->second, i->first + weightToAddToEachRemainingVertexToNormalize };
            vertexIndexToBoneInfluenceTable->insert( VertexIndexToBoneInfluenceTable::value_type( originalVertexIndex, influence ) );
        }
    }
}



//------------------------------------------------------------------------------------------------
// Name: findRangeGeometry
// Desc: Gets geometry from the given attribute range
//------------------------------------------------------------------------------------------------
bool findRangeGeometry( const D3DXATTRIBUTERANGE* range, const DWORD* meshIndices, VertexIndexSet* vertexIndicesInRange, MeshTriangles* trianglesInRange )
{
    // Fail if a parameter is invalid
    if( APP_ERROR( !range || !meshIndices || !vertexIndicesInRange || !trianglesInRange )( "Invalid parameter to findRangeGeometry" ) )
        return false;

    DWORD numberOfFaces = range->FaceCount;
    DWORD faceStartIndex = range->FaceStart;

    // Add each of the faces in this range to the texture subset's triangle buffer
    for( DWORD faceIndex = 0; faceIndex < numberOfFaces; ++faceIndex )
    {
        // Get the first index in the triangle
        DWORD firstIndexInTriangle = 3 * (faceIndex + faceStartIndex);

        // This structure holds the triangle's indices
        MeshTriangle triangle;

        // Add each of the indices to this triangle
        for( int i = 0; i < 3; ++i )
        {
            // Get the index of this triangle vertex
            DWORD vertexIndex = meshIndices[firstIndexInTriangle + i];

            // Add this vertex to the output set
            vertexIndicesInRange->insert( vertexIndex );

            // Add this vertex to the triangle
            triangle.index[i] = vertexIndex;
        }

        // Add this triangle into the output set
        trianglesInRange->push_back( triangle );
    }

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  calculateNumberOfVertexBoneInfluences
// Desc:  Fills the output table with the index of each vertex in the input set mapped to the
//        number of bones that influence that vertex
//------------------------------------------------------------------------------------------------
bool calculateNumberOfVertexBoneInfluences(
    const VertexIndexSet* vertexIndices,
    VertexIndexToBoneInfluenceTable* vertexBoneInfluences,
    VertexIndexToNumberOfBoneInfluencesTable* vertexToNumberOfBoneInfluences )
{
    // Validate parameters
    if( APP_ERROR( !vertexIndices || !vertexBoneInfluences || !vertexToNumberOfBoneInfluences )( "Invalid parameter to calculateNumberOfVertexBoneInfluences" ) )
        return false;

    for( VertexIndexSet::const_iterator vertexIndexIterator = vertexIndices->begin();
                                        vertexIndexIterator != vertexIndices->end(); ++vertexIndexIterator )
    {
        DWORD vertexIndex = *vertexIndexIterator;
        VertexIndexToBoneInfluenceTable::iterator lower = vertexBoneInfluences->lower_bound( vertexIndex );
        VertexIndexToBoneInfluenceTable::iterator upper = vertexBoneInfluences->upper_bound( vertexIndex );

        // Count the number of influences
        DWORD numberOfInfluences = 0;
        for( VertexIndexToBoneInfluenceTable::iterator infl = lower; infl != upper; ++infl, ++numberOfInfluences );

        // Add the number of influences to the storage table
        vertexToNumberOfBoneInfluences->insert( VertexIndexToNumberOfBoneInfluencesTable::value_type( vertexIndex, numberOfInfluences ) );

        // If the number of bone influences is illogical for the weight, fix the weight
        if( APP_WARNING( numberOfInfluences == 1 && lower->second.weight < 1.0f )
            ( "Vertex %lu has only 1 influence (%f) but its weight is not 1.0; correcting", vertexIndex, lower->second.weight ) )
        {
            lower->second.weight = 1.0f;
        }
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: getTriangleInfluenceGroup
// Desc: Find the highest influence group among the vertices in this triangle.  This method
//       contains two special cases:  if all of a triangle's vertices have only 1 influence
//       vertices from bone 0 only, the triangle is a "root" triangle and can be rendered
//       without indexed vertex blending at all.  If all three vertices have only 1 influence
//       but do not fall into the previous category, they can be rendered using 0 influence
//       weights (but still with indexed vertex blending). All other cases are mixed weight/index
//       values and must be pushed into one of the 4 remaining categories, 2-6.  These correspond
//       to 2 blend matrices with 1 weight, 3 blend matrices with 2 weights or 4 blend matrices with 3 weights.
//------------------------------------------------------------------------------------------------
DWORD getTriangleInfluenceGroup(
    const MeshTriangle* triangle,
    const VertexIndexToBoneInfluenceTable* boneInfluences,
    const VertexIndexToNumberOfBoneInfluencesTable* numberOfBoneInfluences
    )
{
    DWORD influenceGroup = 0;
    bool allAreBone0 = true, allAreSingleInfluence = true;
    for( int i = 0; i < 3; ++i )
    {
        // Get the index of the vertex in the triangle that we're examining
        DWORD vertexIndex = triangle->index[i];

        // Get the number of influences on this triangle
        VertexIndexToNumberOfBoneInfluencesTable::const_iterator influenceIterator = numberOfBoneInfluences->find( vertexIndex );
        if( APP_ERROR( influenceIterator == numberOfBoneInfluences->end() )( "Vertex %lu not in number of bone influences table!", vertexIndex ) )
            return 0xFFFFFFFF;
        DWORD influences = influenceIterator->second;

        // Handle special cases
        if( influences == 1 )
        {
            if( allAreBone0 )
            {
                // Since there is only 1 influence, we can safely assume that find(...) brings us the
                // only influence of this vertex.
                VertexIndexToBoneInfluenceTable::const_iterator vertexInfluence = boneInfluences->find( vertexIndex );

                // Check to see whether or not this is bone 0
                allAreBone0 = vertexInfluence->second.boneIndex == 0;
            }
        }
        else
        {
            allAreBone0 = false;
            allAreSingleInfluence = false;
        }

        // Save the highest influence group as the number of influences
        if( influences > influenceGroup ) influenceGroup = influences;
    }

    // If all of the vertices are sourced from only bone 0, this triangle has no influences
    //
    //
    //  AUGUST 25, 2008:
    //
    //      group 1 is now erased.  no vertices will be assigned to group 1 because D3DVBF_0WEIGHTS is
    //      incompatible with indexed vertex blending.
    //
    //      TODO: change group 1 to be a non-index-blended state
    //
    //
    if( allAreSingleInfluence )
        return allAreBone0 ? 0 : 2; // group 0 = no weight, root transform only; group 1 = 1.0f weight, indexed transform
    else
        return influenceGroup; // groups 2-4 are 2-4 indexed blendweights
}


typedef std::vector<FrameNode*> FrameNodeArray;
typedef std::map<std::string,FrameNode*> NameToFrameNodeTable;
typedef std::map<FrameNode*,DWORD> FrameNodeToHierarchyIndexTable;
typedef std::map<std::string,DWORD> FrameNameToHierarchyIndexTable;


//------------------------------------------------------------------------------------------------
// Name: buildFrameHierarchy
// Desc: Constructs the frame hierarchy as a sorted, flat array.  The frameInfo parameter must
//       have 'frameNodes->size()' entries.  At the end of this method, the output map
//       'frameNameToHierarchyIndexTable' can be used to map bone names to their corresponding
//       frame indices in the table.
//------------------------------------------------------------------------------------------------
bool buildFrameHierarchy( const FrameNodeArray& frameNodes, FrameNameToHierarchyIndexTable* frameNameToHierarchyIndexTable,
                          UnseenSkinnedMeshFrameInfo* frameHierarchy )
{
    // Check the parameters
    if( APP_ERROR( frameNodes.empty() || !frameNameToHierarchyIndexTable || !frameHierarchy )( "Invalid parameter" ) )
        return false;

    // Get the number of frames
    size_t numberOfFrames = frameNodes.size();

    // Build a virtual frame hierarchy so that we can assign indices elements.  We have to have indices because
    // a hierarchy can't be effectively sorted using only recursive parent pointers.
    FrameNodeToHierarchyIndexTable frameNodeToVirtualHierarchyIndexTable;

    // Add all of the frame nodes to the virtual hierarchy
    DWORD frameIndex = 0;
    for( FrameNodeArray::const_iterator i = frameNodes.begin(); i != frameNodes.end(); ++i, ++frameIndex )
        frameNodeToVirtualHierarchyIndexTable.insert( FrameNodeToHierarchyIndexTable::value_type( *i, frameIndex ) );

    /// This structure is used to resort the frame hierarchy so that its frames are in optimal ascending order
    struct FrameSortingElement
    {
        size_t virtualFrameIndex;
        size_t sortedParentIndex;
    };

    // Fill in the frame sorting hierarchy element values
    FrameSortingElement* frameSortingHierarchy = new FrameSortingElement[frameNodes.size()];
    if( APP_FATAL( !frameSortingHierarchy )( "Unable to allocate frame sorting hierarchy (%lu frames)" ) )
        return false;
    for( FrameNodeArray::const_iterator i = frameNodes.begin(); i != frameNodes.end(); ++i )
    {
        // Get this node and its parent
        FrameNode* node = *i;
        FrameNode* parentNode = node->getParentNode();

        // Get the frame/parent indices
        DWORD virtualFrameIndex = frameNodeToVirtualHierarchyIndexTable.find( node )->second;
        DWORD parentVirtualFrameIndex = parentNode ? frameNodeToVirtualHierarchyIndexTable.find( parentNode )->second : 0xFFFFFFFF;

        // Set the values in the temporary hierarchy
        frameSortingHierarchy[virtualFrameIndex].virtualFrameIndex = virtualFrameIndex;
        frameSortingHierarchy[virtualFrameIndex].sortedParentIndex = parentVirtualFrameIndex;
    }

    // Re-order the frames so that they obey sorting rules:  no frame with index "f" can reference a parent
    // with index >= f, except for b==f in which case the parent must be 0xFFFFFFFF
    size_t currentFrameIndex = 0;
    while( currentFrameIndex < numberOfFrames )
    {
        if( frameSortingHierarchy[currentFrameIndex].sortedParentIndex > currentFrameIndex &&
          !(frameSortingHierarchy[currentFrameIndex].sortedParentIndex == 0xFFFFFFFF && currentFrameIndex == 0) )
        {
            size_t parentIndex = frameSortingHierarchy[currentFrameIndex].sortedParentIndex == 0xFFFFFFFF ? 0 :
                                 frameSortingHierarchy[currentFrameIndex].sortedParentIndex;

            // Replace parent index references
            for( size_t f = 0; f < numberOfFrames; ++f )
            {
                if( frameSortingHierarchy[f].sortedParentIndex == currentFrameIndex ) frameSortingHierarchy[f].sortedParentIndex = parentIndex;
                else if( frameSortingHierarchy[f].sortedParentIndex == parentIndex )  frameSortingHierarchy[f].sortedParentIndex = currentFrameIndex;
            }

            // Swap
            FrameSortingElement temp;
            memcpy( &temp, &frameSortingHierarchy[currentFrameIndex], sizeof(FrameSortingElement) );
            memcpy( &frameSortingHierarchy[currentFrameIndex], &frameSortingHierarchy[parentIndex], sizeof(FrameSortingElement) );
            memcpy( &frameSortingHierarchy[parentIndex], &temp, sizeof(FrameSortingElement) );
            currentFrameIndex = 0;
        }
        else
            ++currentFrameIndex;
    }

    // Now, re-sort them so that the bones are in as close to ascending matrix order as possible.  We always
    // start this sort at frame #2 because the first entry (frame 0) is guaranteed to be first, and we don't want
    // to swap it.
    // Note that this is a bubble sort.  Although this is algorithmically unfortunate, an elegant solution
    // to this problem is not easy to find: the conditions for sorting (parent index) actually change throughout
    // the skeleton array WHILE sorting goes on!  So more advanced sorts could be perpetually confused--whenever
    // an element is moved, all of its references must be updated.  This algorithm doesn't need to be particularly
    // fast but its important that it works and is easy to debug.  Hence, bubble sort.
    currentFrameIndex = 2;
    while( currentFrameIndex < numberOfFrames )
    {
        // Get the previous bone's index
        size_t previousFrameIndex = currentFrameIndex - 1;

        // Does this bone reference a bone lower than the predecessor but greater than the
        // predecessor's index?
        if( frameSortingHierarchy[previousFrameIndex].sortedParentIndex > frameSortingHierarchy[currentFrameIndex].sortedParentIndex )
        {
            // Swap these entries' references throughout the skeleton
            for( size_t b = 0; b < numberOfFrames; ++b )
            {
                if( frameSortingHierarchy[b].sortedParentIndex == currentFrameIndex )       frameSortingHierarchy[b].sortedParentIndex = previousFrameIndex;
                else if( frameSortingHierarchy[b].sortedParentIndex == previousFrameIndex ) frameSortingHierarchy[b].sortedParentIndex = currentFrameIndex;
            }

            // Swap the entries
            FrameSortingElement temp;
            memcpy( &temp, &frameSortingHierarchy[previousFrameIndex], sizeof(FrameSortingElement) );
            memcpy( &frameSortingHierarchy[previousFrameIndex], &frameSortingHierarchy[currentFrameIndex], sizeof(FrameSortingElement) );
            memcpy( &frameSortingHierarchy[currentFrameIndex], &temp, sizeof(FrameSortingElement) );

            // Restart
            currentFrameIndex = 2;
        }
        else
            ++currentFrameIndex;
    }

    // Build a remap of virtual frame hierarchy indices to actual hierarchy indices
    typedef std::map<DWORD,DWORD> FrameRemapTable;
    FrameRemapTable frameRemapTable;
    for( size_t f = 0; f < numberOfFrames; ++f )
        frameRemapTable.insert( FrameRemapTable::value_type( frameSortingHierarchy[f].virtualFrameIndex, f ) );

    // Remap each of the FrameNode -> hierarchy index entries
    for( FrameNodeArray::const_iterator i = frameNodes.begin(); i != frameNodes.end(); ++i )
    {
        // Get the node
        FrameNode* node = *i;
        DWORD virtualIndex = frameNodeToVirtualHierarchyIndexTable.find( node )->second;

        // Get the node's parent index
        FrameNode* parentNode = node->getParentNode();
        DWORD parentVirtualIndex = parentNode ? frameNodeToVirtualHierarchyIndexTable.find( parentNode )->second : 0xFFFFFFFF;

        // Find the node and its parent's remapped index
        DWORD remappedIndex = frameRemapTable.find( virtualIndex )->second;
        DWORD remappedParentIndex = parentVirtualIndex < numberOfFrames ? frameRemapTable.find( parentVirtualIndex )->second : 0xFFFFFFFF;

        // Add to the external hierarchy
        const std::list<std::string>& frameNames = node->getNames();
        for( std::list<std::string>::const_iterator nameIterator = frameNames.begin(); nameIterator != frameNames.end(); ++nameIterator )
        {
            const std::string& name = *nameIterator;
            if( !APP_WARNING( name.empty() )( "Not adding empty name to hierarchy" ) )
                frameNameToHierarchyIndexTable->insert( FrameNameToHierarchyIndexTable::value_type( *nameIterator, remappedIndex ) );
        }

        // Save the frame's information
        memcpy( &frameHierarchy[remappedIndex].offset, node->getTransform(), sizeof(D3DXMATRIX) );
        frameHierarchy[remappedIndex].parentFrameIndex = remappedParentIndex;
    }

    // Free the temporary sorting structure
    SAFE_DELETE_ARRAY( frameSortingHierarchy );

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: bake
// Desc: "Bakes" this mesh into the output mesh/animation structures
//------------------------------------------------------------------------------------------------
bool FrameNode::bake( LPDIRECT3DDEVICE9 d3dDevice, UnseenSkinnedMesh* mesh, UnseenAnimationSet* animationSet, float minimumBoneInfluenceWeight )
{
    // Get all of the meshes in this hierarchy
    std::multimap<FrameNode*,Mesh*> meshes;
    findAllMeshes( &meshes );

    // Output the number of meshes
    DEBUG_INFO( "%lu meshes found in hierarchy", meshes.size() );


    Mesh* xMesh = meshes.begin()->second;
    ID3DXSkinInfo* skin = xMesh->skinInfo;
    ID3DXMesh* d3dxMesh = NULL;

    // Clone the original mesh into the new format.  
    {
        DWORD oldFormat = xMesh->mesh->GetFVF();
        DWORD newFormat = SOURCE_MESH_VERTEX_FVF;
        if( oldFormat != newFormat ) DEBUG_INFO( "Mesh format changing from %lu to %lu", oldFormat, newFormat );
        if( APP_ERROR( FAILED( xMesh->mesh->CloneMeshFVF( D3DXMESH_32BIT|D3DPOOL_SYSTEMMEM, newFormat, d3dDevice, &d3dxMesh ) ) )
                ( "Couldn't clone source mesh into standard format" ) )
            return false;

        if (oldFormat&D3DFVF_NORMAL != D3DFVF_NORMAL) {
          if( APP_ERROR( FAILED( D3DXComputeNormals(d3dxMesh, (DWORD*)xMesh->adjacencyBuffer->GetBufferPointer()) ) )( "Couldn't regenerate normals" ) ) {
            SAFE_RELEASE(d3dxMesh);
            return false;
          }
        }
    }

    // Build the frame hierarchy
    FrameNameToHierarchyIndexTable frameNameToHierarchyIndexTable;
    {
        // Get all of the frames in this skeleton
        FrameNodeArray frameNodes;
        frameNodes.push_back( this );
        findAllFrames( &frameNodes );

        // Allocate the frame hierarchy array
        DWORD numberOfFrames = frameNodes.size();
        mesh->info.numberOfFrames = numberOfFrames;
        mesh->frameHierarchy = new UnseenSkinnedMeshFrameInfo[numberOfFrames]; // TODO: confirm allocation

        // Fill the hierarchy
        buildFrameHierarchy( frameNodes, &frameNameToHierarchyIndexTable, mesh->frameHierarchy );

        // Make sure the hierarchy is valid
        for( FrameNodeArray::iterator i = frameNodes.begin(); i != frameNodes.end(); ++i )
        {
            // Find name
            bool found = false;
            DWORD frameIndex = 0;
            for( std::list<std::string>::const_iterator name = (*i)->getNames().begin(); name != (*i)->getNames().end(); ++name )
            {
                FrameNameToHierarchyIndexTable::iterator index = frameNameToHierarchyIndexTable.find( *name );
                if( index != frameNameToHierarchyIndexTable.end() )
                {
                    APP_WARNING( found )( "Multiple frame name bindings in hierarchy" );
                    frameIndex = index->second;
                    found = true;
                }
            }

            if( !found ) continue;

            // Make sure the frame has the same transform
            // jan09 : i have no idea what this is for...
            CONFIRM( 0 == memcmp( &mesh->frameHierarchy[frameIndex].offset, (*i)->myTransform, sizeof(D3DXMATRIX) ) );

            // Find parent
            FrameNode* parent = (*i)->myParentNode;
            if( parent )
            {
                found = false;
                for( std::list<std::string>::const_iterator name = parent->getNames().begin(); name != parent->getNames().end(); ++name )
                {
                    FrameNameToHierarchyIndexTable::iterator index = frameNameToHierarchyIndexTable.find( *name );
                    if( index != frameNameToHierarchyIndexTable.end() )
                    {
                        APP_WARNING( found )( "Multiple frame name bindings in hierarchy" );
                        frameIndex = index->second;
                        found = true;
                    }
                }

                if( !found ) continue;

                // Make sure the frame has the same transform
                CONFIRM(0 == memcmp( &mesh->frameHierarchy[frameIndex].offset, parent->myTransform, sizeof(D3DXMATRIX)));
            }
        }
    }

    // Build the bone structure
    {
        // Allocate output buffers for the bone structure
        DWORD numberOfBones = skin->GetNumBones();
        mesh->info.numberOfBones = numberOfBones;
        mesh->skeleton = new UnseenSkinnedMeshBoneInfo[numberOfBones];

        // Save the bone information and 
        mesh->info.numberOfBones = numberOfBones;
        mesh->skeleton = new UnseenSkinnedMeshBoneInfo[numberOfBones]; // TODO: confirm allocation

        for( DWORD boneIndex = 0; boneIndex < numberOfBones; ++boneIndex )
        {
            // Get the bone's name
            LPCSTR boneName = skin->GetBoneName( boneIndex );

            // Look up this bone in the frame hierarchy
            FrameNameToHierarchyIndexTable::iterator frameIndexIterator = frameNameToHierarchyIndexTable.find( skin->GetBoneName( boneIndex ) );
            if( APP_ERROR( frameIndexIterator == frameNameToHierarchyIndexTable.end() )( "Bone %lu (%s) has no frame association!", boneIndex, boneName ) )
                continue;

            // Assign this bone's source frame and copy the offset matrix
            mesh->skeleton[boneIndex].frameIndex = frameIndexIterator->second;
/*
            // Multiply up the hierarchy
            D3DXMATRIX combined;
            D3DXMatrixIdentity( &combined );
            DWORD frameIndex = frameIndexIterator->second;
            std::list<DWORD> frameIndices;
            while( frameIndex != 0xFFFFFFFF )
            {
                frameIndices.push_front( frameIndex );
                frameIndex = mesh->frameHierarchy[frameIndex].parentFrameIndex;
            }
            for( std::list<DWORD>::iterator i = frameIndices.begin(); i != frameIndices.end(); ++i )
            {
                DWORD index = *i;
                D3DXMatrixMultiply( &combined, &mesh->frameHierarchy[index].offset, &combined );
            }

            LPD3DXMATRIX offsetMatrix = skin->GetBoneOffsetMatrix( boneIndex );
            D3DXMatrixInverse( &mesh->skeleton[boneIndex].inverseOffset, NULL, &mesh->frameHierarchy[frameIndexIterator->second].offset );
*/
            memcpy( &mesh->skeleton[boneIndex].inverseOffset, skin->GetBoneOffsetMatrix( boneIndex ), sizeof(D3DXMATRIX) );
        }
    }


    // This table is used to index vertices in the source mesh so that we can determine how many
    // influences various vertices have.  This uses remapped bone indices, as per the table developed above.
    VertexIndexToBoneInfluenceTable vertexIndexToBoneInfluenceTable;

    // This is the number of vertices in the mesh controlled by this skinning.  It's used to
    // validate and assign vertex influences
    DWORD numberOfVerticesInMesh = xMesh->mesh->GetNumVertices();
    DWORD numberOfBones = xMesh->skinInfo->GetNumBones();

    // Scan through the bones and build up the vertex maps
    for( DWORD boneIndex = 0; boneIndex < numberOfBones; ++boneIndex )
    {
        // Get the list of bone influences
        DWORD numberOfAffectedVertices = skin->GetNumBoneInfluences( boneIndex );
        if( APP_WARNING( !numberOfAffectedVertices )( "Bone %lu affects no vertices", boneIndex ) ) continue;

        // These temporary arrays store influence data for this bone
        FLOAT* weights = new FLOAT[numberOfAffectedVertices];
        DWORD* vertexIndices = new DWORD[numberOfAffectedVertices];

        // Get the influences and add them
        if( !APP_ERROR( FAILED( skin->GetBoneInfluence( boneIndex, vertexIndices, weights ) ) )( "Couldn't get bone %lu influence set", boneIndex ) )
        {
            // Add each of the vertex influences for this bone
            for( DWORD v = 0; v < numberOfAffectedVertices; ++v )
            {
                DWORD vertexIndex = vertexIndices[v];
                BoneVertexInfluence influence = { boneIndex, weights[v] };

                // Check the vertex data because the input information isn't necessairily sanitized.  We don't
                // check the minimum vertex influence here because it's possible we would eliminiate all 
                // influences for a certain vertex; however, if the weight is less than or equal to 0 it makes
                // sense that the entry isn't valid.
                if( APP_WARNING( vertexIndex >= numberOfVerticesInMesh || influence.weight < 0.0f )( "Invalid vertex encountered in bone %lu:  { %lu, %f }",
                    boneIndex, vertexIndex, influence.weight ) ) continue;
                vertexIndexToBoneInfluenceTable.insert( VertexIndexToBoneInfluenceTable::value_type( vertexIndex, influence ) );
            }
        }

        // Free the vertex data
        delete[] weights;
        delete[] vertexIndices;
    }


    // Because the program that generated the bone influences can be sloppy, some vertices will have more than 4 influences,
    // will have 0 (or very tiny) bone weights, or will have other problems.  The following method cleans up the influences
    // so that the skeleton becomes valid and renderable.
    sanitizeVertexInfluences( numberOfVerticesInMesh, &vertexIndexToBoneInfluenceTable, minimumBoneInfluenceWeight );







    typedef std::map<DWORD,TextureSubset*> AttributeIDToTextureSubsetTable;
    AttributeIDToTextureSubsetTable attributeIDToTextureSubsetTable;

    // Load the attribute table
    DWORD attributeTableSize = 0;
    d3dxMesh->GetAttributeTable( NULL, &attributeTableSize );
    D3DXATTRIBUTERANGE* ranges = new D3DXATTRIBUTERANGE[attributeTableSize];
    d3dxMesh->GetAttributeTable( ranges, &attributeTableSize );

    // Add triangles to the list
    DWORD* meshIndices = 0;
    d3dxMesh->LockIndexBuffer( D3DLOCK_READONLY, (LPVOID*)&meshIndices );

    // Create a subset entry for each of the units
    for( DWORD i = 0; i < attributeTableSize; ++i )
    {
        // Generate the texture subset and add it to the table
        TextureSubset* textureSubset = new TextureSubset;
        attributeIDToTextureSubsetTable.insert( AttributeIDToTextureSubsetTable::value_type( ranges[i].AttribId, textureSubset ) );

        // Distribute geometry into the influence groups
        {
            // This is the set of vertices referenced by triangles in this texture subset
            VertexIndexSet vertexIndicesInTextureSubset;

            // This is the set of triangles in this range
            MeshTriangles trianglesInTextureSubset;

            // Get the geometry in this range
            findRangeGeometry( &ranges[i], meshIndices, &vertexIndicesInTextureSubset, &trianglesInTextureSubset );

            // Import vertex influences for each of the ranges
            // Bring in each of the vertex influences for vertices in this texture subset.  While doing so, 
            // get the total number of vertex influences for each of the vertices in the texture subset so that
            // we split up triangles into groups by the number of influences that they have.
            calculateNumberOfVertexBoneInfluences( &vertexIndicesInTextureSubset, &vertexIndexToBoneInfluenceTable, &textureSubset->vertexInfluences );

            // Propogate triangles into the subset's influence groups.  Each of the triangles
            // must be placed in the highest influence group among its vertices.
            for( MeshTriangles::iterator triangleIterator = trianglesInTextureSubset.begin();
                                        triangleIterator != trianglesInTextureSubset.end(); ++triangleIterator )
            {
                // Get the triangle being analyzed
                MeshTriangle* triangle = &(*triangleIterator);

                // Figure out which influence group this triangle should be placed in
                DWORD influenceGroup = getTriangleInfluenceGroup( triangle, &vertexIndexToBoneInfluenceTable, &textureSubset->vertexInfluences );
                CONFIRM( influenceGroup != 1 ); // Added August 25th; group 1 is based on an invalid assumption

                // Add this triangle to the influence group
                if( !APP_ERROR( influenceGroup > 4 )( "Too many influences on triangle!  Discarding." ) )
                    textureSubset->influenceGroups[influenceGroup].triangles.push_back( *triangle );
            }
        }

        // For each of the triangles in the influence groups, add the referenced vertices to the influence group's
        // set, then map all of the vertex influences into the influence group
        for( DWORD influenceGroupIndex = 0; influenceGroupIndex < 5; ++influenceGroupIndex )
        {
            // Get the destination influence group and links to its members
            InfluenceGroup* influenceGroup = &textureSubset->influenceGroups[influenceGroupIndex];
            MeshTriangles& influenceGroupTriangles = influenceGroup->triangles;
            VertexIndexSet& influenceGroupVertexIndexSet = influenceGroup->vertexIndexSet;

            // Add each of the triangles' vertex indices to the influence group
            for( MeshTriangles::iterator triangleIterator = influenceGroupTriangles.begin();
                                        triangleIterator != influenceGroupTriangles.end(); ++triangleIterator )
                for( int tv = 0; tv < 3; ++tv ) influenceGroupVertexIndexSet.insert( triangleIterator->index[tv] );

            // Add all of the vertex bone/weight associations to this influence group
            for( VertexIndexSet::iterator vertexIndexIterator = influenceGroupVertexIndexSet.begin();
                                          vertexIndexIterator != influenceGroupVertexIndexSet.end(); ++vertexIndexIterator )
            {
                // Get the current vertex indexs
                DWORD vertexIndex = *vertexIndexIterator;

                // Copy bone influence definitions for this vertex into this influence group
                VertexIndexToBoneInfluenceTable::iterator lower = vertexIndexToBoneInfluenceTable.lower_bound( vertexIndex );
                VertexIndexToBoneInfluenceTable::iterator upper = vertexIndexToBoneInfluenceTable.upper_bound( vertexIndex );
                influenceGroup->vertexIndexToBoneInfluenceTable.insert( lower, upper );

                // Search this vertex's influences to find the influencing bone that has the highest bone index
                DWORD highestInfluencingBoneIndex = 0;
                for( VertexIndexToBoneInfluenceTable::iterator vi = lower; vi != upper; ++vi )
                    if( vi->second.boneIndex > highestInfluencingBoneIndex ) highestInfluencingBoneIndex = vi->second.boneIndex;

                // Add this vertex's highest influencing bone index to the list
                influenceGroup->vertexIndexToHighestInfluencingBoneIndexList.insert(
                    VertexIndexToHighestInfluencingBoneIndexList::value_type( vertexIndex, highestInfluencingBoneIndex ) );
            }

            // Create a list that maps bone indices to triangles.  Iterating through this list when creating triangles
            // will create an optimally-ordered set of polygons.
            for( MeshTriangles::iterator triangleIterator = influenceGroup->triangles.begin();
                                        triangleIterator != influenceGroup->triangles.end(); ++triangleIterator )
            {
                // Find the highest influencing bone index among this triangle's vertices
                DWORD highestTriangleBoneIndex = 0;
                for( int v = 0; v < 3; ++v )
                {
                    DWORD vertexIndex = triangleIterator->index[v];
                    DWORD highestVertexBoneIndex = influenceGroup->vertexIndexToHighestInfluencingBoneIndexList.find( vertexIndex )->second;
                    if( highestVertexBoneIndex > highestTriangleBoneIndex ) highestTriangleBoneIndex = highestVertexBoneIndex;
                }

                // Copy this triangle into the output list
                influenceGroup->highestInfluencingBoneIndexToTriangleList.insert(
                    HighestInfluencingBoneIndexToTriangleList::value_type( highestTriangleBoneIndex, *triangleIterator ) );
            }

            // Clear the other set of triangles so we don't get it confused
            influenceGroup->triangles.clear();

            // Create a remap so that the vertices are sorted by their highest influencing bone index.  The first step
            // is to swap the order of the vertex index->highest influencing bone list so that it is ordered by bone
            // index.  This will put the vertices and triangles in the same order as far as bones are concerned.
            HighestInfluencingBoneIndexToVertexIndexList highestInfluencingBoneIndexToVertexIndexList;
            for( VertexIndexToHighestInfluencingBoneIndexList::iterator vithibil = influenceGroup->vertexIndexToHighestInfluencingBoneIndexList.begin();
                 vithibil != influenceGroup->vertexIndexToHighestInfluencingBoneIndexList.end(); ++vithibil )
            {
                highestInfluencingBoneIndexToVertexIndexList.insert( HighestInfluencingBoneIndexToVertexIndexList::value_type( vithibil->second, vithibil->first ) );
            }

            // Build the vertex remap table for this influence group.  This remap is on a per-influence-group basis
            // since each group will need its own geometry buffers (because each is in a different format); thus, it
            // serves a dual purpose: (1) map vertices from the complete texture subset into this influence group,
            // and (2) reorder the vertices so that their influences are bone-ascending.
            DWORD currentVertexIndex = 0;
            for( HighestInfluencingBoneIndexToVertexIndexList::iterator i =
                    highestInfluencingBoneIndexToVertexIndexList.begin();
                    i != highestInfluencingBoneIndexToVertexIndexList.end(); ++i, ++currentVertexIndex )
            {
                influenceGroup->vertexRemapTable.insert( VertexRemapTable::value_type( i->second, currentVertexIndex ) );
            }

            // Clear other temporary data so it's not used later
            // TODO: make these local variables and take them out of the influence group
            influenceGroup->vertexIndexSet.clear();
            influenceGroup->vertexIndexToHighestInfluencingBoneIndexList.clear();

            // Remap each of the triangle indices in this influence group so that they reference valid vertices
            for( HighestInfluencingBoneIndexToTriangleList::iterator hibittl =
                influenceGroup->highestInfluencingBoneIndexToTriangleList.begin();
                hibittl != influenceGroup->highestInfluencingBoneIndexToTriangleList.end(); ++hibittl )
            {
                // Remap each of the vertices
                MeshTriangle* triangle = &hibittl->second;
                for( int v = 0; v < 3; ++v )
                    triangle->index[v] = influenceGroup->vertexRemapTable.find( triangle->index[v] )->second;
            }
        }
    }

    d3dxMesh->UnlockIndexBuffer();

    // Get rid of the ranges
    delete[] ranges;

    // Allocate the output texture groups
    DWORD numberOfTextureGroups = attributeIDToTextureSubsetTable.size();
    mesh->info.numberOfTextureGroups = numberOfTextureGroups;
    mesh->textureGroups = new UnseenSkinnedMesh::TextureGroup[numberOfTextureGroups];

    // Fill in each of the texture groups
    size_t textureSubsetIndex = 0;
    for( AttributeIDToTextureSubsetTable::iterator textureSubsetIterator = attributeIDToTextureSubsetTable.begin();
                                                   textureSubsetIterator != attributeIDToTextureSubsetTable.end();
                                                 ++textureSubsetIterator, ++textureSubsetIndex )
    {
        // This is the texture subset that was constructed using the mesh/skin info
        TextureSubset* sourceTextureSubset = textureSubsetIterator->second;
        UnseenSkinnedMesh::TextureGroup* destTextureGroup = &mesh->textureGroups[textureSubsetIndex];

        // Count the number of influence groups that are valid
        DWORD numberOfInfluenceGroups = 0;
        for( size_t i = 0; i < 5; ++i )
            if( !sourceTextureSubset->influenceGroups[i].highestInfluencingBoneIndexToTriangleList.empty() ) ++numberOfInfluenceGroups;

        // Save the number of influence groups and allocate the output array
        destTextureGroup->info.numberOfVertexInfluenceGroups = numberOfInfluenceGroups;
        destTextureGroup->vertexInfluenceGroups = new UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup[numberOfInfluenceGroups];
        // TODO: check allocation!!

        // Fill each of the valid influence groups
        size_t currentDestInfluenceGroupIndex = 0;
        for( size_t sourceInfluenceGroupIndex = 0; sourceInfluenceGroupIndex < 5; ++sourceInfluenceGroupIndex )
        {
            // If this influence group is invalid, skip it
            InfluenceGroup* sourceInfluenceGroup = &sourceTextureSubset->influenceGroups[sourceInfluenceGroupIndex];
            if( sourceInfluenceGroup->highestInfluencingBoneIndexToTriangleList.empty() ) continue;

            // Get the current output group
            UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup* destVertexInfluenceGroup =
                &destTextureGroup->vertexInfluenceGroups[currentDestInfluenceGroupIndex++];

            // Reset this group
            ZeroMemory( destVertexInfluenceGroup, sizeof(UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup) );

            // Added August 25th; group 1 is based on an invalid assumption
            CONFIRM( sourceInfluenceGroupIndex != 1 ) else continue;

            // Get the vertex count, format, size and matrix influence count for this influence group
            DWORD numberOfVertices = sourceInfluenceGroup->vertexRemapTable.size();
            DWORD vertexFVF, vertexSizeInBytes = 0;
            DWORD numberOfVertexInfluences = 0;
            switch( sourceInfluenceGroupIndex )
            {
                // Influence group entirely dependent on world matrix 0 (the primary world matrix)
                case 0: vertexFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1; numberOfVertexInfluences = 0; break;
                
                // Influence group with a single 100%-influence source matrix
                case 1: vertexFVF = D3DFVF_XYZB1|D3DFVF_NORMAL|D3DFVF_LASTBETA_UBYTE4|D3DFVF_TEX1; numberOfVertexInfluences = 1; break;

                // Influence groups with blend weights and source matrix indices
                case 2: vertexFVF = D3DFVF_XYZB2|D3DFVF_NORMAL|D3DFVF_LASTBETA_UBYTE4|D3DFVF_TEX1; numberOfVertexInfluences = 2; break;
                case 3: vertexFVF = D3DFVF_XYZB3|D3DFVF_NORMAL|D3DFVF_LASTBETA_UBYTE4|D3DFVF_TEX1; numberOfVertexInfluences = 3; break;
                case 4: vertexFVF = D3DFVF_XYZB4|D3DFVF_NORMAL|D3DFVF_LASTBETA_UBYTE4|D3DFVF_TEX1; numberOfVertexInfluences = 4; break;
            }

            // Save data to the output structure
            destVertexInfluenceGroup->info.numberOfVertexInfluences = numberOfVertexInfluences;
            destVertexInfluenceGroup->info.numberOfVertices = numberOfVertices;
            destVertexInfluenceGroup->info.d3dVertexFormat = vertexFVF;
            vertexSizeInBytes = D3DXGetFVFVertexSize( vertexFVF );
            destVertexInfluenceGroup->info.vertexSizeInBytes = vertexSizeInBytes;

            // Since we're expecting low-poly meshes, the indices should always be WORDs.  Produce an error if this won't work.
            DWORD numberOfIndices = 0, indexSizeInBytes = 0;
            if( APP_ERROR( numberOfVertices >= 0xFFFF )( "There are too many indices in the source mesh; output mesh will be invalid" ) )
                continue;
            size_t numberOfTriangles = sourceInfluenceGroup->highestInfluencingBoneIndexToTriangleList.size();
            numberOfIndices = numberOfTriangles * 3;
            indexSizeInBytes = sizeof(WORD);
            destVertexInfluenceGroup->info.numberOfTriangles = numberOfTriangles;
            destVertexInfluenceGroup->info.d3dIndexFormat = D3DFMT_INDEX16;
            destVertexInfluenceGroup->info.indexSizeInBytes = indexSizeInBytes;


            // Allocate arrays for this influence group
            destVertexInfluenceGroup->vertices = new BYTE[vertexSizeInBytes * numberOfVertices];
            destVertexInfluenceGroup->indices = new BYTE[indexSizeInBytes * numberOfIndices];
            destVertexInfluenceGroup->boneInfluences = new UnseenSkinnedMeshBoneInfluenceInfo[numberOfBones];
            // TODO: check allocations

            // Fill in the vertices for this group
            {
                BYTE* destVertices = (BYTE*)destVertexInfluenceGroup->vertices;
                SourceMeshVertex* sourceVertices = 0;

                // Lock the source buffer
                if( APP_ERROR( FAILED( d3dxMesh->LockVertexBuffer( D3DLOCK_READONLY, (LPVOID*)&sourceVertices ) ) )( "Couldn't lock source vertex set" ) )
                    return false; // TODO: graceful error handling

                // Move the vertices over using the remap
                for( VertexRemapTable::iterator vertexRemap = sourceInfluenceGroup->vertexRemapTable.begin();
                                                vertexRemap != sourceInfluenceGroup->vertexRemapTable.end(); ++vertexRemap )
                {
                    DWORD sourceVertexIndex = vertexRemap->first;
                    DWORD destVertexIndex = vertexRemap->second;

                    BYTE* destVertexMemory = destVertices + (destVertexIndex * vertexSizeInBytes);
                    SourceMeshVertex* sourceVertex = sourceVertices + sourceVertexIndex;

                    // Build the vertex
                    size_t nextAvailableVertexFloat = 0;
                    float vertexBuilder[3+5+3+2]; // make sure to change this if vtx format changes
                    DWORD indexedInfluencePacker = 0;
                    vertexBuilder[nextAvailableVertexFloat++] = sourceVertex->pos.x;
                    vertexBuilder[nextAvailableVertexFloat++] = sourceVertex->pos.y;
                    vertexBuilder[nextAvailableVertexFloat++] = sourceVertex->pos.z;

                    // If this is the 0th influence group, we don't have to build influences
                    if( numberOfVertexInfluences )
                    {
                        // Build each of the vertex influences
                        DWORD currentInfluence = 0;
                        DWORD lastBoneIndex = 0;
                        VertexIndexToBoneInfluenceTable::iterator lower = vertexIndexToBoneInfluenceTable.lower_bound( sourceVertexIndex );
                        VertexIndexToBoneInfluenceTable::iterator upper = vertexIndexToBoneInfluenceTable.upper_bound( sourceVertexIndex );
                        for( VertexIndexToBoneInfluenceTable::iterator i = lower; i != upper; ++i, ++currentInfluence, ++nextAvailableVertexFloat )
                        {
                            vertexBuilder[nextAvailableVertexFloat] = i->second.weight;
                            lastBoneIndex = 0xFF&i->second.boneIndex;
                            indexedInfluencePacker |= lastBoneIndex << (8*currentInfluence);
                        }

                        // If we have more than the required number of weights, something went wrong with sorting
                        if( APP_ERROR( currentInfluence > numberOfVertexInfluences )( "Too many blend weights on vertex!  Reducing, but this will lead to mesh artifacts" ) )
                        {
                            while( currentInfluence > numberOfVertexInfluences )
                            {
                                nextAvailableVertexFloat--;
                                currentInfluence--;
                            }
                        }

                        // Fill in any indices that are not yet valid with 0-influence duplicates of the prior matrices.  This is done
                        // to solve cases like the following example:
                        // vertex 0 has 1 influence
                        // vertex 1 has 2 influences
                        // vertex 2 has 2 influences
                        // triangle 0 uses vertices { 0, 1, 2 } so it's placed in influence group 2
                        //
                        // This segment "promotes" vertex 0 to 2 influences by padding with a 0-influence.  This is
                        // double-checked by confirming the vertex's size before copying into the output array.
                        for( DWORD i = currentInfluence; i < numberOfVertexInfluences; ++i, ++currentInfluence, ++nextAvailableVertexFloat )
                        {
                            vertexBuilder[nextAvailableVertexFloat] = 0.0f;
                            DWORD boneIndex = lastBoneIndex;
                            indexedInfluencePacker |= boneIndex << (8*currentInfluence);
                        }

                        // Move backward, because the last float is used as 4 packed BYTE matrix indices
                        --nextAvailableVertexFloat;

                        // Add the packed coordinates by bitcasting the matrix indices
                        vertexBuilder[nextAvailableVertexFloat++] = *(FLOAT*)&(indexedInfluencePacker);
                    }

                    // Add the normals
                    vertexBuilder[nextAvailableVertexFloat++] = sourceVertex->normal.x;
                    vertexBuilder[nextAvailableVertexFloat++] = sourceVertex->normal.y;
                    vertexBuilder[nextAvailableVertexFloat++] = sourceVertex->normal.z;

                    // Add the texture coordinates
                    vertexBuilder[nextAvailableVertexFloat++] = sourceVertex->tex.x;
                    vertexBuilder[nextAvailableVertexFloat++] = sourceVertex->tex.y;

                    // Confirm this vertex's size
                    APP_ERROR( nextAvailableVertexFloat * sizeof(float) != vertexSizeInBytes )( "Invalid vertex size assumption!  Matrix vertices will messed up" );

                    // Copy output
                    memcpy( destVertexMemory, vertexBuilder, vertexSizeInBytes );
                }

                // Unlock source buffer
                d3dxMesh->UnlockVertexBuffer();
            }


            // Fill in the indices for this group from the triangles in the influence group
            {
                WORD* destIndices = (WORD*)destVertexInfluenceGroup->indices;

                VertexRemapTable& vertexRemapTable = sourceInfluenceGroup->vertexRemapTable;

                size_t triangleIndex = 0;
                for( HighestInfluencingBoneIndexToTriangleList::iterator hibittl = sourceInfluenceGroup->highestInfluencingBoneIndexToTriangleList.begin();
                                                                        hibittl != sourceInfluenceGroup->highestInfluencingBoneIndexToTriangleList.end(); ++hibittl, ++triangleIndex )
                {
                    // Get each of the triangle's indices
                    WORD triangle[3] = { hibittl->second.index[0], hibittl->second.index[1], hibittl->second.index[2] };

                    // Copy this triangle into the output index buffer
                    memcpy( &destIndices[triangleIndex*3], triangle, sizeof(WORD) * 3 );
                }
            }



            // Fill in the bone influences
            for( int boneIndex = 0; boneIndex < numberOfBones; ++boneIndex )
            {
                HighestInfluencingBoneIndexToTriangleList::iterator begin = sourceInfluenceGroup->highestInfluencingBoneIndexToTriangleList.begin();
                HighestInfluencingBoneIndexToTriangleList::iterator boneLowerBound = sourceInfluenceGroup->highestInfluencingBoneIndexToTriangleList.lower_bound( boneIndex );
                HighestInfluencingBoneIndexToTriangleList::iterator boneUpperBound = sourceInfluenceGroup->highestInfluencingBoneIndexToTriangleList.upper_bound( boneIndex );
                HighestInfluencingBoneIndexToTriangleList::iterator end = sourceInfluenceGroup->highestInfluencingBoneIndexToTriangleList.end();

                // Find the number of bones below this one
                DWORD numberOfTrianglesBelowThisBone = 0;
                for( HighestInfluencingBoneIndexToTriangleList::iterator i = begin; i != boneLowerBound; ++i, ++numberOfTrianglesBelowThisBone );

                // Find the number of triangles before/including this bone index and the highest vertex referenced therein
                DWORD numberOfTrianglesUsingThisAndLowerBones = 0;
                DWORD highestVertexIndexUsedByThisAndLowerBones = 0;
                for( HighestInfluencingBoneIndexToTriangleList::iterator i = begin; i != boneUpperBound; ++i, ++numberOfTrianglesUsingThisAndLowerBones )
                {
                    for( size_t vi = 0; vi < 3; ++vi )
                    {
                        //DWORD remappedIndex = sourceInfluenceGroup->vertexRemapTable.find(i->second.index[vi])->second;
                        DWORD remappedIndex = i->second.index[vi];

                        if( remappedIndex > highestVertexIndexUsedByThisAndLowerBones )
                            highestVertexIndexUsedByThisAndLowerBones = remappedIndex;
                    }
                }

                // Find the number of triangles after/including this bone index and the lowest vertex referenced therein
                DWORD lowestVertexIndexReferencedByThisAndHigherBones = 0xFFFFFFFF;
                DWORD numberOfTrianglesUsingThisAndHigherBones = 0;
                for( HighestInfluencingBoneIndexToTriangleList::iterator i = boneLowerBound; i != end; ++i, ++numberOfTrianglesUsingThisAndHigherBones )
                {
                    for( size_t vi = 0; vi < 3; ++vi )
                    {
                        // Remap this vertex index
                        //DWORD remappedIndex = sourceInfluenceGroup->vertexRemapTable.find(i->second.index[vi])->second;
                        DWORD remappedIndex = i->second.index[vi];

                        if( remappedIndex < lowestVertexIndexReferencedByThisAndHigherBones )
                            lowestVertexIndexReferencedByThisAndHigherBones = remappedIndex;
                    }
                }

                // Save the output data
                UnseenSkinnedMeshBoneInfluenceInfo* destBoneInfluence = &destVertexInfluenceGroup->boneInfluences[boneIndex];
                destBoneInfluence->thisAndLower.numberOfTriangles = numberOfTrianglesUsingThisAndLowerBones;
                destBoneInfluence->thisAndLower.numberOfVertices = highestVertexIndexUsedByThisAndLowerBones + 1;
                destBoneInfluence->thisAndHigher.lowestReferencedVertexIndex = lowestVertexIndexReferencedByThisAndHigherBones;
                destBoneInfluence->thisAndHigher.numberOfTriangles = numberOfTrianglesUsingThisAndHigherBones;
                destBoneInfluence->thisAndHigher.triangleStartIndex = numberOfTrianglesBelowThisBone * 3;
                destBoneInfluence->thisAndHigher.numberOfVertices = numberOfVertices - lowestVertexIndexReferencedByThisAndHigherBones;
            }
        }
    }

    // Get rid of the mesh object
    SAFE_RELEASE( d3dxMesh );


    // Get the tick-scaling rate
    double tickToSecondScaling = 1.0 / (APP_WARNING( !myAnimationTicksPerSecond )( "Animation ticks per second not found in source file; using default of 100" ) ? 100.0 : (double)myAnimationTicksPerSecond);

    // Add each of the animations to the output set
    for( std::vector<Animation*>::iterator animationIterator = myAnimations.begin(); animationIterator != myAnimations.end(); ++ animationIterator )
    {
        // Get the animation currently being processed
        Animation* animation = *animationIterator;
/*
        // Rebuild each of the animation keys
        for( std::vector<FrameAnimation*>::iterator frameAnimationIterator = animation->frameAnimations.begin(); frameAnimationIterator != animation->frameAnimations.end(); ++frameAnimationIterator )
        {
            FrameAnimation* frameAnimation = *frameAnimationIterator;
            bakeIntoTransformKeys(  frameAnimation );
        }
*/
        // Build a pairing for each of the frame animations
        typedef std::map<DWORD,FrameAnimation*> FrameIndexToFrameAnimationTable;
        FrameIndexToFrameAnimationTable frameIndexToFrameAnimationTable;
        for( std::vector<FrameAnimation*>::iterator frameAnimationIterator = animation->frameAnimations.begin(); frameAnimationIterator != animation->frameAnimations.end(); ++frameAnimationIterator )
        {
            FrameAnimation* frameAnimation = *frameAnimationIterator;

            // Find this frame in the frame name->index map
            FrameNameToHierarchyIndexTable::iterator hierarchyIndexIterator = frameNameToHierarchyIndexTable.end();
            std::string validFrameName = "(invalid)";
            for( std::list<std::string>::iterator name = frameAnimation->frameNames.begin(); name != frameAnimation->frameNames.end(); ++name )
            {
                if( hierarchyIndexIterator == frameNameToHierarchyIndexTable.end() )
                {
                    // Search for the bone's name
                    hierarchyIndexIterator = frameNameToHierarchyIndexTable.find( *name );
                    if( hierarchyIndexIterator != frameNameToHierarchyIndexTable.end() ) validFrameName = *name;
                }
                else
                    if( APP_WARNING( frameNameToHierarchyIndexTable.end() != frameNameToHierarchyIndexTable.find( *name ) )( "Animation %s has multiple associated bones!  This could result in invalid hierarchy for children and bad animation controls", validFrameName.c_str() ) )
                        continue;
            }

            if( APP_ERROR( hierarchyIndexIterator == frameNameToHierarchyIndexTable.end() )( "Frame animator %s couldn't find a frame to animate!", validFrameName.c_str() ) )
                continue;

            // Make sure the index doesn't already exist
            DWORD frameIndex = hierarchyIndexIterator->second;
            APP_WARNING( frameIndexToFrameAnimationTable.find( frameIndex ) != frameIndexToFrameAnimationTable.end() )( "Frame %lu (%s) controlled by multiple frame animations!", frameIndex, validFrameName.c_str() );

            // Add to the output animations
            frameIndexToFrameAnimationTable.insert( FrameIndexToFrameAnimationTable::value_type( frameIndex, frameAnimation ) );
            bakeIntoTransformKeys( &mesh->frameHierarchy[frameIndex].offset, frameAnimation );
        }

        // Build a set of all of the keyframe times
        std::set<DWORD> keyframeTicks;
        for( FrameIndexToFrameAnimationTable::iterator fatfit = frameIndexToFrameAnimationTable.begin(); fatfit != frameIndexToFrameAnimationTable.end(); ++fatfit )
        {
            FrameAnimation* frameAnimation = fatfit->second;

            // only deal with translation keys for now
            DWORD numberOfKeys = frameAnimation->numberOfTransformationKeys;
            for( DWORD i = 0; i < numberOfKeys; ++i )
                keyframeTicks.insert( frameAnimation->transformationKeys[i].tick );
        }

        // Calculate the average time per keyframe
        DWORD averageTicksPerKeyframe;
        DWORD highestTickValue = 0;
        {
            DWORD sumOfKeyframeDeltas = 0;
            DWORD totalNumberOfKeyframeDeltas = 0;
            std::set<DWORD>::iterator i = keyframeTicks.begin();
             DWORD lastTickValue = *i;
            ++i; // start with the second element
            for( ; i != keyframeTicks.end(); ++i )
            {
                DWORD thisTickValue = *i;
                sumOfKeyframeDeltas += thisTickValue - lastTickValue;
                ++totalNumberOfKeyframeDeltas;
                lastTickValue = thisTickValue;

                if( highestTickValue < thisTickValue )
                    highestTickValue = thisTickValue;
            }

            // Calculate the average tick advance per keyframe
            if( totalNumberOfKeyframeDeltas > 0 )
                averageTicksPerKeyframe = sumOfKeyframeDeltas / totalNumberOfKeyframeDeltas;
            else
                averageTicksPerKeyframe = lastTickValue;

            // If the average ticks per keyframe is 0, just set it to 1
            if( !averageTicksPerKeyframe ) averageTicksPerKeyframe = 1;
        }


        DWORD numberOfKeyframes = keyframeTicks.size();
        DWORD numberOfSkeletalFrames = mesh->info.numberOfFrames;
        DWORD numberOfAnimatedFrames = frameIndexToFrameAnimationTable.size();

        // Allocate the animation data
        UnseenSkeletalAnimation* outputAnimation = new UnseenSkeletalAnimation;
        outputAnimation->info.numberOfSkeletalFrames = numberOfSkeletalFrames;
        outputAnimation->info.numberOfAnimatedFrames = numberOfAnimatedFrames;
        outputAnimation->info.numberOfKeyframes = numberOfKeyframes;
        outputAnimation->keyframes = new UnseenSkeletalAnimation::Keyframe[numberOfKeyframes];
        outputAnimation->frameIndexToAnimatedFrameTransformTable = new DWORD[numberOfSkeletalFrames];
        outputAnimation->info.averageTicksPerKeyframe = averageTicksPerKeyframe;
        outputAnimation->info.tickLength = highestTickValue;
        outputAnimation->info.secondsPerTick = tickToSecondScaling;
        outputAnimation->info.loop = animation->animationOptions.loops;

        // Complete the frame map
        //std::map<DWORD,DWORD> animatedFrameIndexToAnimatedFrameTransformIndex; // removed...doesn't seem to do anything
        {
            // Fill with defaults
            for( size_t i = 0; i < numberOfSkeletalFrames; ++i )
                outputAnimation->frameIndexToAnimatedFrameTransformTable[i] = 0xFFFFFFFF;

            // Fill in the animated entries
            DWORD animatedFrameTransformIndex = 0;
            for( FrameIndexToFrameAnimationTable::iterator frameIterator = frameIndexToFrameAnimationTable.begin();
                                                          frameIterator != frameIndexToFrameAnimationTable.end(); ++frameIterator, ++animatedFrameTransformIndex )
            {
                FrameAnimation* frameAnimation = frameIterator->second;

                // Find this animation in the bone name-> bone index map
                //BoneNameToIndexTable::iterator boneNameIterator = boneNameToIndexTable.end();
                //std::string validBoneName = "(invalid)";
                //for( std::list<std::string>::iterator name = frameAnimation->frameNames.begin(); name != frameAnimation->frameNames.end(); ++name )
                //{
                //    if( boneNameIterator == boneNameToIndexTable.end() )
                //    {
                //        // Search for the bone's name
                //        boneNameIterator = boneNameToIndexTable.find( *name );
                //        if( boneNameIterator != boneNameToIndexTable.end() ) validBoneName = *name;
                //    }
                //    else
                //        if( APP_WARNING( boneNameToIndexTable.end() != boneNameToIndexTable.find( *name ) )( "Animation %s has multiple associated bones!  This could result in invalid hierarchy for children and bad animation controls", validBoneName.c_str() ) )
                //            continue;
                //}
                // Map this bone to an index
                //DEBUG_INFO( "Using bone index %lu instead of %lu!!!", boneNameIterator->second, animatedFrameTransformIndex );
                //animatedFrameIndexToAnimatedFrameTransformIndex.insert( std::map<DWORD,DWORD>::value_type( frameIterator->first, boneNameIterator->second ) );

                outputAnimation->frameIndexToAnimatedFrameTransformTable[frameIterator->first] = animatedFrameTransformIndex;
            }
        }

size_t numberOfAddedMatrices = 0;

        // Create each of the keyframes
        DWORD keyframeIndex = 0;
        for( std::set<DWORD>::const_iterator keyframeTickIterator = keyframeTicks.begin(); keyframeTickIterator != keyframeTicks.end(); ++keyframeTickIterator, ++keyframeIndex )
        {
            // Get the keyframe being updated
            UnseenSkeletalAnimation::Keyframe* keyframe = &outputAnimation->keyframes[keyframeIndex];

            // Get the current tick
            DWORD keyframeTick = *keyframeTickIterator;
            double keyframeTime = keyframeTick * tickToSecondScaling;

            // Allocate the frame transforms and set the time of the keyframe
            keyframe->animatedFrameTransforms = new D3DXMATRIX[numberOfAnimatedFrames];
            keyframe->tick = keyframeTick;

            // This is the approximate index of this keyframe in the sources
            DWORD approximateKeyframeIndex = (keyframeTick) / averageTicksPerKeyframe;

            // Iterate through each of the animated frames and set their matrices in the output animation
            DWORD animatedFrameTransformIndex = 0;
            for( FrameIndexToFrameAnimationTable::iterator frameIterator = frameIndexToFrameAnimationTable.begin();
                                                          frameIterator != frameIndexToFrameAnimationTable.end(); ++frameIterator, ++animatedFrameTransformIndex )
            {
                //animatedFrameTransformIndex = animatedFrameIndexToAnimatedFrameTransformIndex.find( frameIterator->first )->second;

                FrameAnimation* frameAnimation = frameIterator->second;
                DWORD keysInAnimation = frameAnimation->numberOfTransformationKeys;
                TransformAnimationKey* transformationKeys = frameAnimation->transformationKeys;

                // Find the keyframes around this tick location
                DWORD lower = approximateKeyframeIndex ? approximateKeyframeIndex - 1 : 0;
                DWORD highestKeyIndex = keysInAnimation - 1;
                DWORD upper = approximateKeyframeIndex;
                { // bounds-check the indices
                  lower = min( highestKeyIndex, lower);
                  upper = min( highestKeyIndex, upper);
                }
                while( transformationKeys[lower].tick < keyframeTick && lower < upper ) ++lower;
                while( transformationKeys[upper].tick > keyframeTick && upper > lower ) --upper;
                while( transformationKeys[lower].tick > keyframeTick && lower > 0 ) --lower;
                while( transformationKeys[upper].tick < keyframeTick && upper < highestKeyIndex ) ++upper;

                // Interpolate the transformation matrices
                DWORD lowerTick = transformationKeys[lower].tick;
                DWORD upperTick = transformationKeys[upper].tick;

                // Calculate interpolated matrix
                D3DXMATRIXA16 interpolated;
                if( (keyframeTick == lowerTick) ||  // just lower
                    (lowerTick == upperTick) ) {    // or there is only 1 keyframe
                  interpolated = transformationKeys[lower].matrix;
                }
                else if( keyframeTick == upperTick ) interpolated = transformationKeys[upper].matrix;
                else
                {
                    // Get the blending factors
                    double lowerTime = lowerTick * tickToSecondScaling;
                    double upperTime = upperTick * tickToSecondScaling;
                    double timeDelta = upperTime - lowerTime;
                    double blendFactor = (keyframeTime - lowerTime) / timeDelta;

                    // Interpolate
                    interpolated = transformationKeys[upper].matrix - transformationKeys[lower].matrix;
                    interpolated *= blendFactor;
                    interpolated += transformationKeys[lower].matrix;

                    // Add
                    ++numberOfAddedMatrices;
                }

                // Save the transformation matrix
                keyframe->animatedFrameTransforms[animatedFrameTransformIndex] = interpolated;
            }
        }

        // Add this animation to the output
        animationSet->push_back( UnseenAnimationSet::value_type( animation->name, outputAnimation ) );

        DEBUG_INFO( "Interpolated %lu extra matrices", numberOfAddedMatrices );
    }




    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: deallocate
// Desc: Frees the memory in an animation set that was allocated by the bake method
//------------------------------------------------------------------------------------------------
void FrameNode::deallocate( UnseenAnimationSet* animationSet )
{
    for( UnseenAnimationSet::iterator i = animationSet->begin(); i != animationSet->end(); ++i )
    {
        deallocateUnseenSkeletalAnimation( i->second );
        delete i->second;
    }
    animationSet->clear();
}



//------------------------------------------------------------------------------------------------
// Name: loadMesh
// Desc: Loads a mesh encountered while parsing this frame node
//------------------------------------------------------------------------------------------------
bool FrameNode::loadMesh( const std::string& name, ID3DXFileData* dxFileData, IDirect3DDevice9* d3dDevice )
{
    // Allocate mesh memory
    Mesh* mesh = new Mesh;
    if( APP_FATAL( !mesh )( "Couldn't allocate memory for mesh %s", name.c_str() ) ) return false;

    // Load data into system memory for later processing
    HRESULT hr;
    if( APP_ERROR( FAILED( hr = D3DXLoadSkinMeshFromXof( dxFileData, D3DXMESH_SYSTEMMEM, d3dDevice,
        &mesh->adjacencyBuffer, &mesh->materialsBuffer, &mesh->effectInstancesBuffer,
        &mesh->numberOfMaterials, &mesh->skinInfo, &mesh->mesh ) ) )
        ( "Unable to load the skinned mesh at node location %s", name.c_str() ) )
    {
        // Free the container memory
        delete mesh;

        // Failed
        return false;
    }

    // If there are no bones in this mesh, release the skin info buffer because this
    // is not a skinned mesh.  This makes it easy for processing to detect the lack of bones.
    if( !mesh->skinInfo->GetNumBones() )
        SAFE_RELEASE( mesh->skinInfo );

    // Add to the mesh buffer
    myMeshes.push_back( mesh );

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: loadFrameTransformMatrix
// Desc: Loads this frame's transformation matrix
//------------------------------------------------------------------------------------------------
bool FrameNode::loadFrameTransformMatrix( const std::string& name, ID3DXFileData* dxFileData )
{
    // Get a pointer to the transformation matrix
    const D3DXMATRIX* matrixPointer;
    DWORD size;
    if( APP_ERROR( FAILED( dxFileData->Lock( &size, (LPCVOID*)&matrixPointer ) ) )( "Unable to get frame transform %s", name.c_str() ) )
        return false;

    // Copy the matrix into this frame's transform
    if( !APP_ERROR( size != sizeof(D3DXMATRIX) )( "Invalid transform matrix data size" ) )
        memcpy( &myTransform, matrixPointer, sizeof(D3DXMATRIX) );

    // Unlock the data
    dxFileData->Unlock();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: loadChildFrame
// Desc: Loads a child frame for this mesh
//------------------------------------------------------------------------------------------------
bool FrameNode::loadChildFrame( const std::string& name, ID3DXFileData* dxFileData, IDirect3DDevice9* d3dDevice )
{
    // Create a new child node
    FrameNode* child = new FrameNode();
    if( APP_FATAL( !child )( "Not enough memory to allocate child node!" ) ) return false;
    child->myPath.assign( name );
    child->myParentNode = this;

    // Set the child's frame name
    {
        std::string frameName;
        getFileDataName( dxFileData, &frameName );
        child->myNames.push_back( frameName );
    }

    // Load all of the defined members of this child node
    bool continueLoading = true;
    SIZE_T numberOfChildren;
    dxFileData->GetChildren( &numberOfChildren );
    for( SIZE_T i = 0; continueLoading && i < numberOfChildren; ++i )
    {
        ID3DXFileData* dxFileDataObject = NULL;
        continueLoading = !APP_ERROR( FAILED( dxFileData->GetChild( i, &dxFileDataObject ) ) || !child->load( dxFileDataObject, d3dDevice ) )( "Couldn't load child object" );
        SAFE_RELEASE( dxFileDataObject );
    }

    // Add to the internal buffer if the child was correctly loaded
    if( continueLoading )
    {
        // Add to the internal list
        myChildren.push_back( child );
    }
    else
    {
        // Print error
        DEBUG_ERROR( "Failed to completely process frame %s", name.c_str() );

        // Free memory
        child->destroy();
        delete child;
    }


    // Return the error code
    return continueLoading;
}



//------------------------------------------------------------------------------------------------
// Name: loadAnimationSet
// Desc: Loads an animation encountered while parsing this frame node
//------------------------------------------------------------------------------------------------
bool FrameNode::loadAnimationSet( const std::string& name, ID3DXFileData* dxFileData )
{
    // Create a new animation object to contain this set
    Animation* animation = new Animation;
    if( APP_FATAL( !animation )( "Out of memory" ) ) return false;

    // Load the actual name (instead of the path name) of this file data element into the animation
    getFileDataName( dxFileData, &animation->name );

    // Load all of the defined members of this child node
    bool continueLoading = true;
    SIZE_T numberOfChildren;
    dxFileData->GetChildren( &numberOfChildren );
    DEBUG_INFO( "Animation set %s has %lu children", name.c_str(), numberOfChildren );
    for( SIZE_T i = 0; continueLoading && i < numberOfChildren; ++i )
    {
        ID3DXFileData* dxFileDataObject = NULL;

        // Load the child data object
        continueLoading = !APP_ERROR( FAILED( dxFileData->GetChild( i, &dxFileDataObject ) ) )( "Unable to get %s child %lu", name.c_str(), i );

        // Load this child, it if was valid
        if( continueLoading )
        {
            // Get the child element's type
            GUID type;
            dxFileDataObject->GetType( &type );

            // Get the name of the element
            std::string animationName;
            getFileDataName( dxFileDataObject, &animationName );
            animationName = name + " / " + animationName + " <animation>";

            // Load this if it is not a null name
            if( type == TID_D3DRMAnimation )
            {
                continueLoading = loadAnimation( animationName, dxFileDataObject, animation );
            }
            else
                DEBUG_INFO( "Animation set %s contained non-animation element '%s'; ignoring", name.c_str(), animationName );
        }

        SAFE_RELEASE( dxFileDataObject );
    }

    // Add to the internal buffer if the child was correctly loaded
    if( continueLoading )
    {
        // Add to the internal list
        myAnimations.push_back( animation );
    }
    else
    {
        // Print error
        DEBUG_ERROR( "Failed to completely process animation %s", name.c_str() );

        // Free memory
        std::vector<FrameAnimation*>::iterator end = animation->frameAnimations.end();
        for( std::vector<FrameAnimation*>::iterator j = animation->frameAnimations.begin(); j != end; ++j )
        {
            FrameAnimation* frameAnimation = *j;
            SAFE_DELETE_ARRAY( frameAnimation->scalingKeys );
            SAFE_DELETE_ARRAY( frameAnimation->translationKeys );
            SAFE_DELETE_ARRAY( frameAnimation->rotationKeys );
            SAFE_DELETE_ARRAY( frameAnimation->transformationKeys );
            delete frameAnimation;
        }

        delete animation;
    }

    // Return the flag
    return continueLoading;
}



//------------------------------------------------------------------------------------------------
// Name: loadAnimation
// Desc: Loads an animation sub-object encountered while parsing this frame node
//------------------------------------------------------------------------------------------------
bool FrameNode::loadAnimation( const std::string& name, ID3DXFileData* dxFileData, Animation* animation )
{
    // Create a frame animation for this element
    FrameAnimation* frameAnimation = new FrameAnimation();
    if( APP_FATAL( !frameAnimation )( "Out of memory" ) ) return false;

    // Load the name of the frame that this element animates
    std::string frameName;
    getFileDataName( dxFileData, &frameName );
    frameAnimation->frameNames.push_back( frameName );

    // Reset the keys
    frameAnimation->scalingKeys = NULL;
    frameAnimation->numberOfScalingKeys = 0;
    frameAnimation->translationKeys = NULL;
    frameAnimation->numberOfTranslationKeys = 0;
    frameAnimation->rotationKeys = NULL;
    frameAnimation->numberOfRotationKeys = 0;
    frameAnimation->transformationKeys = NULL;
    frameAnimation->numberOfTransformationKeys = 0;

    // Reset animation options
    frameAnimation->animationOptions.valid = false;
    frameAnimation->animationOptions.loops = false;
    frameAnimation->animationOptions.useSplineInterpolation = false;

    // Load all of the defined members of this node
    bool continueLoading = true;
    SIZE_T numberOfChildren;
    dxFileData->GetChildren( &numberOfChildren );
    for( SIZE_T i = 0; continueLoading && i < numberOfChildren; ++i )
    {
        ID3DXFileData* dxFileDataObject = NULL;

        // Load the child data object
        continueLoading = !APP_ERROR( FAILED( dxFileData->GetChild( i, &dxFileDataObject ) ) )( "Unable to get %s child %lu", name.c_str(), i );

        // Load this child, it if was valid
        if( continueLoading )
        {
            // Get the child element's type
            GUID type;
            dxFileDataObject->GetType( &type );

            // Get the name of the element
            std::string childName;
            getFileDataName( dxFileDataObject, &childName );

            // Load this element
            if( type == TID_D3DRMFrame )
            {
                // Only resolve references; don't allow frame declarations inside of animations
                if( dxFileDataObject->IsReference() )
                {
                    // Set the referenced frame
                    //APP_WARNING( !boneAnimation->boneName.empty() )( "Bone %s references multiple frames:  %s replacing %s", name.c_str(), childName.c_str(), boneAnimation->boneName.c_str() );
                    frameAnimation->frameNames.push_back( childName );
                }
                else
                    DEBUG_WARNING( "Bone animation %s contains actual frame; only frame references are allowed", name.c_str() );
            }
            else if( type == TID_D3DRMAnimationKey )
            {
                // Create a new key for this bone animation
                continueLoading = !APP_ERROR( !loadAnimationKey( name + " / " + childName + " <animation key>", dxFileDataObject, frameAnimation ) )( "Failed to load animation key" );
            }
            else if( type == TID_D3DRMAnimationOptions )
            {
                // Load animation options here!
                continueLoading = !APP_ERROR( !loadAnimationOptions( name + " / " + childName + " <animation options>", dxFileDataObject, &frameAnimation->animationOptions ) )( "Failed to load animation options" );
            }
            else
                DEBUG_INFO( "Bone animation %s contained unknown element '%s'; ignoring", name.c_str(), childName.c_str() );
        }

        // Release the object
        SAFE_RELEASE( dxFileDataObject );
    }

    // Add to the parent object
    if( continueLoading )
    {
        // Add to the parents' animation set
        animation->frameAnimations.push_back( frameAnimation );
    }
    else
    {
        DEBUG_WARNING( "Failed to completely process bone animation %s", name.c_str() );

        SAFE_DELETE_ARRAY( frameAnimation->scalingKeys );
        SAFE_DELETE_ARRAY( frameAnimation->translationKeys );
        SAFE_DELETE_ARRAY( frameAnimation->rotationKeys );
        SAFE_DELETE_ARRAY( frameAnimation->transformationKeys );
        SAFE_DELETE( frameAnimation );
    }

    // Return the status flag
    return continueLoading;
}




//------------------------------------------------------------------------------------------------
// Name: loadAnimationOptions
// Desc: Loads animation options into the given object
//------------------------------------------------------------------------------------------------
bool FrameNode::loadAnimationOptions( const std::string& name, ID3DXFileData* dxFileData, AnimationOptions* animationOptions )
{
    // Lock the buffer
    SIZE_T dataSize = 0;
    DWORD* dataPointer = NULL;
    if( APP_ERROR( FAILED( dxFileData->Lock( &dataSize, (LPCVOID*)&dataPointer ) ) )( "Couldn't lock file data to read animation options" ) )
        return false;

    // Get the data
    DWORD openClosed = *dataPointer++;
    DWORD spline0Linear1 = *dataPointer++;

    // Store the info
    animationOptions->valid = true;
    animationOptions->loops = openClosed != 0;
    animationOptions->useSplineInterpolation = spline0Linear1 == 0;

    // Unlock the data object
    dxFileData->Unlock();

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name: loadAnimationKey
// Desc: Loads a key that is used to animate the given frame
//------------------------------------------------------------------------------------------------
bool FrameNode::loadAnimationKey( const std::string& name, ID3DXFileData* dxFileData, FrameAnimation* frameAnimation )
{
    // Lock the data
    SIZE_T dataSize = 0;
    DWORD* dataPointer = NULL;
    if( APP_ERROR( FAILED( dxFileData->Lock( &dataSize, (LPCVOID*)&dataPointer ) ) )( "Couldn't lock data to load animation key" ) )
        return false;

    // Get the key type
    DWORD keyType = *dataPointer++;

    // Get the number of entries in this key
    DWORD numberOfEntries = *dataPointer++;

    // Reduce data left by consumed data amount
    dataSize -= sizeof(DWORD) * 2;

    // Based on which key type we have, load the key
    bool succeeded = true;
    switch( keyType )
    {
        case 0: succeeded = loadRotationKeys( dataPointer, dataSize, numberOfEntries, frameAnimation ); break;
        case 1: succeeded = loadScalingKeys( dataPointer, dataSize, numberOfEntries, frameAnimation ); break;
        case 2: succeeded = loadTranslationKeys( dataPointer, dataSize, numberOfEntries, frameAnimation ); break;
        case 4: succeeded = loadTransformationKeys( dataPointer, dataSize, numberOfEntries, frameAnimation ); break;
        default:
            DEBUG_WARNING( "Unknown key type encountered in %s:  %lu", name.c_str(), keyType );
            break;
    }

    // Unlock
    dxFileData->Unlock();

    // Return the success flag
    return succeeded;
}



//------------------------------------------------------------------------------------------------
// Name: loadRotationKeys
// Desc: Loads keys from the source data pointer into the 
//------------------------------------------------------------------------------------------------
bool FrameNode::loadRotationKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation )
{
#pragma pack( push, 1 )
    struct XFileRotationKey
    {
        DWORD tick;
        DWORD floatEntries; // this should always be 4
        FLOAT w, x, y, z;
    };
#pragma pack( pop )

// Make sure the key is a valid size
const int KEY_SIZE_MOD_DWORD = sizeof(XFileRotationKey)%sizeof(DWORD);
#if (KEY_SIZE_MOD_DWORD!=0)
#error XFileRotationKey should have a multiple of sizeof(DWORD) byte-size
#endif

    // Check to make sure a set of keys doesn't already exist
    if( APP_WARNING( frameAnimation->rotationKeys )( "Multiple sets of rotation keys found for animation %s; overwriting", frameAnimation->frameNames.begin()->c_str() ) )
        delete[] frameAnimation->rotationKeys;

    // Allocate space for the keys
    frameAnimation->rotationKeys = new RotateAnimationKey[numberOfEntries];
    frameAnimation->numberOfRotationKeys = numberOfEntries;

    // Load all of the keys
    for( DWORD i = 0; i < numberOfEntries; ++i )
    {
        // Load the key
        XFileRotationKey key;
        if( APP_ERROR( dataSize < sizeof(key) )( "Not enough space in buffer to load XFileRotationKey" ) ) return false;
        memcpy( &key, dataPointer, sizeof(key) );
        
        // Make sure the key is the right size
        if( APP_ERROR( key.floatEntries != 4 )( "Rotation key should always have 4 float elements" ) ) return false;

        // Copy the key into the animation
        frameAnimation->rotationKeys[i].tick = key.tick;
        frameAnimation->rotationKeys[i].quaternion.w = key.w;
        frameAnimation->rotationKeys[i].quaternion.x = key.x;
        frameAnimation->rotationKeys[i].quaternion.y = key.y;
        frameAnimation->rotationKeys[i].quaternion.z = key.z;

        // Advance the data pointer
        dataPointer += sizeof(XFileRotationKey) / sizeof(DWORD);
        dataSize -= sizeof(XFileRotationKey);
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: loadScalingKeys
// Desc: Loads keys from the source data pointer into the 
//------------------------------------------------------------------------------------------------
bool FrameNode::loadScalingKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation )
{
#pragma pack( push, 1 )
    struct XFileScalingKey
    {
        DWORD tick;
        DWORD floatEntries; // this should always be 3
        FLOAT x, y, z;
    };
#pragma pack( pop )

// Make sure the key is a valid size
const int KEY_SIZE_MOD_DWORD = sizeof(XFileScalingKey)%sizeof(DWORD);
#if (KEY_SIZE_MOD_DWORD!=0)
#error XFileScalingKey should have a multiple of sizeof(DWORD) byte-size
#endif

    // Check to make sure a set of keys doesn't already exist
    if( APP_WARNING( frameAnimation->scalingKeys )( "Multiple sets of scaling keys found for animation %s; overwriting", frameAnimation->frameNames.begin()->c_str() ) )
        delete[] frameAnimation->scalingKeys;

    // Allocate space for the keys
    frameAnimation->scalingKeys = new ScaleAnimationKey[numberOfEntries];
    frameAnimation->numberOfScalingKeys = numberOfEntries;

    // Load all of the keys
    for( DWORD i = 0; i < numberOfEntries; ++i )
    {
        // Load the key
        XFileScalingKey key;
        if( APP_ERROR( dataSize < sizeof(key) )( "Not enough space in buffer to load XFileScalingKey" ) ) return false;
        memcpy( &key, dataPointer, sizeof(key) );
        
        // Make sure the key is the right size
        if( APP_ERROR( key.floatEntries != 3 )( "Scaling key should always have 3 float elements" ) ) return false;

        // Copy the key into the animation
        frameAnimation->scalingKeys[i].tick = key.tick;
        frameAnimation->scalingKeys[i].vector.x = key.x;
        frameAnimation->scalingKeys[i].vector.y = key.y;
        frameAnimation->scalingKeys[i].vector.z = key.z;

        // Advance the data pointer
        dataPointer += sizeof(XFileScalingKey) / sizeof(DWORD);
        dataSize -= sizeof(XFileScalingKey);
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: loadTranslationKeys
// Desc: Loads keys from the source data pointer into the 
//------------------------------------------------------------------------------------------------
bool FrameNode::loadTranslationKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation )
{
#pragma pack( push, 1 )
    struct XFileTranslationKey
    {
        DWORD tick;
        DWORD floatEntries; // this should always be 3
        FLOAT x, y, z;
    };
#pragma pack( pop )

// Make sure the key is a valid size
const int KEY_SIZE_MOD_DWORD = sizeof(XFileTranslationKey)%sizeof(DWORD);
#if (KEY_SIZE_MOD_DWORD!=0)
#error XFileTranslationKey should have a multiple of sizeof(DWORD) byte-size
#endif

    // Check to make sure a set of keys doesn't already exist
    if( APP_WARNING( frameAnimation->translationKeys )( "Multiple sets of translation keys found for animation %s; overwriting", frameAnimation->frameNames.begin()->c_str() ) )
        delete[] frameAnimation->translationKeys;

    // Allocate space for the keys
    frameAnimation->translationKeys = new TranslateAnimationKey[numberOfEntries];
    frameAnimation->numberOfTranslationKeys = numberOfEntries;

    // Load all of the keys
    for( DWORD i = 0; i < numberOfEntries; ++i )
    {
        // Load the key
        XFileTranslationKey key;
        if( APP_ERROR( dataSize < sizeof(key) )( "Not enough space in buffer to load XFileTranslationKey" ) ) return false;
        memcpy( &key, dataPointer, sizeof(key) );
        
        // Make sure the key is the right size
        if( APP_ERROR( key.floatEntries != 3 )( "Translation key should always have 3 float elements" ) ) return false;

        // Copy the key into the animation
        frameAnimation->translationKeys[i].tick = key.tick;
        frameAnimation->translationKeys[i].vector.x = key.x;
        frameAnimation->translationKeys[i].vector.y = key.y;
        frameAnimation->translationKeys[i].vector.z = key.z;

        // Advance the data pointer
        dataPointer += sizeof(XFileTranslationKey) / sizeof(DWORD);
        dataSize -= sizeof(XFileTranslationKey);
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: loadTransformationKeys
// Desc: Loads keys from the source data pointer into the 
//------------------------------------------------------------------------------------------------
bool FrameNode::loadTransformationKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation )
{
#pragma pack( push, 1 )
    struct XFileTransformationKey
    {
        DWORD tick;
        DWORD floatEntries; // this should always be 16
        D3DXMATRIX matrix;
    };
#pragma pack( pop )

// Make sure the key is a valid size
const int KEY_SIZE_MOD_DWORD = sizeof(XFileTransformationKey)%sizeof(DWORD);
#if (KEY_SIZE_MOD_DWORD!=0)
#error XFileTransformationKey should have a multiple of sizeof(DWORD) byte-size
#endif

    // Check to make sure a set of keys doesn't already exist
    if( APP_WARNING( frameAnimation->transformationKeys )( "Multiple sets of transformation keys found for animation %s; overwriting", frameAnimation->frameNames.begin()->c_str() ) )
        delete[] frameAnimation->transformationKeys;

    // Allocate space for the keys
    frameAnimation->transformationKeys = new TransformAnimationKey[numberOfEntries];
    frameAnimation->numberOfTransformationKeys = numberOfEntries;

    // Load all of the keys
    for( DWORD i = 0; i < numberOfEntries; ++i )
    {
        // Load the key
        XFileTransformationKey key;
        if( APP_ERROR( dataSize < sizeof(key) )( "Not enough space in buffer to load XFileTransformationKey" ) ) return false;
        memcpy( &key, dataPointer, sizeof(key) );
        
        // Make sure the key is the right size
        if( APP_ERROR( key.floatEntries != 16 )( "Transformation key should always have 16 float elements" ) ) return false;

        // Copy the key into the animation
        frameAnimation->transformationKeys[i].tick = key.tick;
        memcpy( &frameAnimation->transformationKeys[i].matrix, &key.matrix, sizeof(D3DXMATRIX) );

        // Advance the data pointer
        dataPointer += sizeof(XFileTransformationKey) / sizeof(DWORD);
        dataSize -= sizeof(XFileTransformationKey);
    }

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name: bakeIntoTransformKeys
// Desc: Turns scale/rotate/translate keys into transform keys
//------------------------------------------------------------------------------------------------
void FrameNode::bakeIntoTransformKeys( const D3DXMATRIX* frameTransform, FrameAnimation* animation )
{
    if( APP_WARNING( animation->numberOfTransformationKeys )( "bakeIntoTransformKeys can't handle transform keys that already exist" ) )
        return;

    D3DXQUATERNION lastRotationKey, nextRotationKey;
    D3DXVECTOR3 lastScalingKey( 1.0f, 1.0f, 1.0f ), nextScalingKey( 1.0f, 1.0f, 1.0f );
    D3DXVECTOR3 lastTranslationKey( 0.0f, 0.0f, 0.0f ), nextTranslationKey( 0.0f, 0.0f, 0.0f );

    D3DXQuaternionIdentity( &lastRotationKey );
    D3DXQuaternionIdentity( &nextRotationKey );

    std::set<DWORD> keyTicks;

    // Find all of the rotation, scaling and translation key times
    for( int i = 0; i < animation->numberOfRotationKeys; ++i )
    {
        if( i == 0 ) { nextRotationKey = lastRotationKey = animation->rotationKeys[0].quaternion; }
        keyTicks.insert( animation->rotationKeys[i].tick );
    }
    for( int i = 0; i < animation->numberOfScalingKeys; ++i )
    {
        if( i == 0 ) { nextScalingKey = lastScalingKey = animation->scalingKeys[0].vector; }
        std::pair< std::set<DWORD>::iterator, bool> result = keyTicks.insert( animation->scalingKeys[i].tick );
        APP_WARNING( result.second )( "Scaling key tick exists that is not a rotation key tick" );
    }
    for( int i = 0; i < animation->numberOfTranslationKeys; ++i )
    {
        if( i == 0 ) { nextTranslationKey = lastTranslationKey = animation->translationKeys[0].vector; }
        std::pair< std::set<DWORD>::iterator, bool> result = keyTicks.insert( animation->translationKeys[i].tick );
        APP_WARNING( result.second )( "Translation key tick exists that is not a rotation or scaling key tick" );
    }

    // Bake the keys
    animation->numberOfTransformationKeys = keyTicks.size();
    animation->transformationKeys = new FrameNode::TransformAnimationKey[keyTicks.size()];

    int keyIndex = 0;
    for( std::set<DWORD>::iterator i = keyTicks.begin(); i != keyTicks.end(); ++i, ++keyIndex )
    {
        DWORD keyTick = *i;
        animation->transformationKeys[keyIndex].tick = keyTick;

        // Build the matrix
        if( keyIndex < animation->numberOfScalingKeys )     lastScalingKey     = animation->scalingKeys[keyIndex].vector;
        if( keyIndex < animation->numberOfRotationKeys )    lastRotationKey    = animation->rotationKeys[keyIndex].quaternion;
        if( keyIndex < animation->numberOfTranslationKeys ) lastTranslationKey = animation->translationKeys[keyIndex].vector;

        D3DXMATRIXA16 cumulative, builder;
        D3DXMatrixScaling( &cumulative, lastScalingKey.x, lastScalingKey.y, lastScalingKey.z );

        D3DXQuaternionNormalize( &lastRotationKey, &lastRotationKey );
        D3DXMatrixRotationQuaternion( &builder, &lastRotationKey );
        D3DXMatrixTranspose( &builder, &builder );

        D3DXMatrixMultiply( &cumulative, &cumulative, &builder );
        D3DXMatrixTranslation( &builder, lastTranslationKey.x, lastTranslationKey.y, lastTranslationKey.z );
        D3DXMatrixMultiply( &animation->transformationKeys[keyIndex].matrix, &cumulative, &builder );

        //D3DXMatrixMultiply( &animation->transformationKeys[keyIndex].matrix, frameTransform, &cumulative );
    }

    DEBUG_INFO( "Created %lu transformation keys from SRT keys", keyTicks.size() );
}



//------------------------------------------------------------------------------------------------
// Name: findAllMeshes
// Desc: Recursively searches the frame node hierarchy and builds a table of all of the meshes
//------------------------------------------------------------------------------------------------
void FrameNode::findAllMeshes( std::multimap<FrameNode*,Mesh*>* meshes )
{
    for( std::vector<Mesh*>::iterator i = myMeshes.begin(); i != myMeshes.end(); ++i )
    {
        if( (*i)->skinInfo )
            meshes->insert( std::multimap<FrameNode*,Mesh*>::value_type( this, *i ) );
    }
    for( std::vector<FrameNode*>::iterator i = myChildren.begin(); i != myChildren.end(); ++i )
        (*i)->findAllMeshes( meshes );
}




//------------------------------------------------------------------------------------------------
// Name: findAllFrames
// Desc: Recursively searches the frame node hierarchy for all of the frames
//------------------------------------------------------------------------------------------------
void FrameNode::findAllFrames( std::vector<FrameNode*>* frameNodes )
{
    // Add all of this entry's children
    for( std::vector<FrameNode*>::iterator i = myChildren.begin(); i != myChildren.end(); ++i )
    {
        FrameNode* child = *i;
        frameNodes->push_back( child );
        child->findAllFrames( frameNodes );
    }
}






//------------------------------------------------------------------------------------------------
// Name: getFileDataName
// Desc: Gets the name of a file data element
//------------------------------------------------------------------------------------------------
void FrameNode::getFileDataName( ID3DXFileData* dxFileData, std::string* name )
{
    // Check output parameter; dxFileData can be invalid
    if( APP_ERROR( !name )( "Invalid output name to getFileDataName" ) ) return;

    // Get the name
    SIZE_T nameSize = 0;
    if( dxFileData &&
       !APP_ERROR( FAILED( dxFileData->GetName( NULL, &nameSize ) ) )( "Couldn't get data object's name size" )
        && nameSize )
    {
        name->reserve( nameSize + 1 );
        char nameBuffer[256];
        DWORD nameSize = sizeof(nameBuffer)/sizeof(char);
        dxFileData->GetName( nameBuffer, &nameSize );
        name->assign( nameBuffer );
    }
    else
        name->assign( "(invalid)" );
}

