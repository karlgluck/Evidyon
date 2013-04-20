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
#include "resource"
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include <dcx/d3d>
#include <dcx/win32>
#include <dcx/eventtimer>
#include <windowsx.h>
#include <set>
#include "commctrl.h"
#include <vector>
#include "atlaseditdialog.h"
#include "atlascompilationcontext.h"
#include "../shared/compiledatlasformat.h"

#define ATLAS_FLOAT_EPSILON 0.1f

bool fALessThanB(float a, float b)
{
    return a < (b - ATLAS_FLOAT_EPSILON);
}

bool fAGreaterThanB(float a, float b)
{
    return a > (b + ATLAS_FLOAT_EPSILON);
}


//------------------------------------------------------------------------------------------------
// Name:  less_LDptr_client::operator()
// Desc:  Compares location description pointers by data that is visible to a client program
//------------------------------------------------------------------------------------------------
bool AtlasCompilationContext::less_LDptr_client::operator() (
    const AtlasCompilationContext::LocationDescription* left,
    const AtlasCompilationContext::LocationDescription* right) const
{
    if (left->navigability < right->navigability) return true;
    if (left->navigability > right->navigability) return false;
    if (left->sceneryIndex < right->sceneryIndex) return true;
    if (left->sceneryIndex > right->sceneryIndex) return false;
    
    // Repeat for each layer
    for (int i = 0; i < 2; ++i)
    {
        const AtlasCompilationContext::LocationTerrainDescription* leftTerrain = &left->terrainLayers[i];
        const AtlasCompilationContext::LocationTerrainDescription* rightTerrain = &right->terrainLayers[i];

        bool leftInvalid = leftTerrain->terrainMeshSubset == TMS_INVALID,
            rightInvalid = rightTerrain->terrainMeshSubset == TMS_INVALID;

        // Check to make sure this layer is valid before anything else
        if (!leftInvalid &&  rightInvalid) return true;
        if ( leftInvalid && !rightInvalid) return false;
        if ( leftInvalid &&  rightInvalid) continue;

        // We're now sure that this layer is valid, so compare its attributes
        if (leftTerrain->terrainMeshSubset < rightTerrain->terrainMeshSubset ) return true;
        if (leftTerrain->terrainMeshSubset > rightTerrain->terrainMeshSubset ) return false;
        if (leftTerrain->textureIndex < rightTerrain->textureIndex ) return true;
        if (leftTerrain->textureIndex > rightTerrain->textureIndex ) return false;
        if (leftTerrain->wallTextureIndex < rightTerrain->wallTextureIndex ) return true;
        if (leftTerrain->wallTextureIndex > rightTerrain->wallTextureIndex ) return false;
        if (fALessThanB(   leftTerrain->elevation, rightTerrain->elevation)) return true;
        if (fAGreaterThanB(leftTerrain->elevation, rightTerrain->elevation)) return false;
        if (fALessThanB(   leftTerrain->height,    rightTerrain->height)) return true;
        if (fAGreaterThanB(leftTerrain->height,    rightTerrain->height)) return false;
    }

    // These are equal
    return false;
}


//------------------------------------------------------------------------------------------------
// Name:  greater_occurrance::operator()
// Desc:  Used to sort by the occurrance term in a collection of OccurranceToDescriptionPair entries
//------------------------------------------------------------------------------------------------
bool AtlasCompilationContext::greater_occurrance::operator() (
    const AtlasCompilationContext::OccurranceToDescriptionPair& left,
    const AtlasCompilationContext::OccurranceToDescriptionPair& right) const
{
    return left.first < right.first;
}

//------------------------------------------------------------------------------------------------
// Name:  convertLocationDescription
// Desc:  Takes fields from a LocationDescription structure and compiles it for the client
//------------------------------------------------------------------------------------------------
void AtlasCompilationContext::convertLocationDescription(LocationDescription* description,
                                       CompiledAtlasClientLocationDescription* clientDescription)
{
    // Check to make sure we got a good parameter
    if (APP_ERROR(!clientDescription)("Invalid destination for convertLocationDescription"))
        return;

    // Check description
    if (!description)
    {
        clientDescription->navigability = MAPNAV_COUNT;
        clientDescription->sceneryIndex = 0xFFFFFFFF;

        for (int i = 0; i < 2; ++i)
        {
            CompiledAtlasClientLocationDescription::TerrainData* clientTerrain = &clientDescription->terrain[i];

            clientTerrain->terrainMeshIndex = 0xFFFFFFFF;
            clientTerrain->textureIndex = 0xFFFFFFFF;
            clientTerrain->wallTextureIndex = 0xFFFFFFFF;
            clientTerrain->elevation = 0.0f;
            clientTerrain->height = 0.0f;
        }
    }
    else
    {
        clientDescription->navigability = description->navigability;
        clientDescription->sceneryIndex = description->sceneryIndex;

        for (int i = 0; i < 2; ++i)
        {
            CompiledAtlasClientLocationDescription::TerrainData* clientTerrain = &clientDescription->terrain[i];
            LocationTerrainDescription* terrain = &description->terrainLayers[i];

            clientTerrain->terrainMeshIndex = terrain->terrainMeshSubset;
            clientTerrain->textureIndex = terrain->textureIndex;
            clientTerrain->wallTextureIndex = terrain->wallTextureIndex;
            clientTerrain->elevation = terrain->elevation;
            clientTerrain->height = terrain->height;
        }
    }
}



//------------------------------------------------------------------------------------------------
// Name:  AtlasCompilationContext
// Desc:  This description defines a terrain layer's attributes
//------------------------------------------------------------------------------------------------
AtlasCompilationContext::AtlasCompilationContext()
{
    setDefaultLocation(0xFFFFFFFF, 0xFFFFFFFF, 0.0f, 0.0f, MAPNAV_WALK);
    myX1 = 0;
    myY1 = 0;
    myX2 = 0;
    myY2 = 0;
    myMap = 0;
    background_sound_loop_index_;
}




//------------------------------------------------------------------------------------------------
// Name:  destroy
// Desc:  Erases everything in the class and deallocates all allocated data
//------------------------------------------------------------------------------------------------
void AtlasCompilationContext::destroy()
{
    SAFE_DELETE_ARRAY(myMap);
    setDefaultLocation(0xFFFFFFFF, 0xFFFFFFFF, 0.0f, 0.0f, MAPNAV_WALK);
    myX1 = 0;
    myY1 = 0;
    myX2 = 0;
    myY2 = 0;
}




//------------------------------------------------------------------------------------------------
// Name:  setDefaultLocation
// Desc:  Initializes the default location's texture
//------------------------------------------------------------------------------------------------
void AtlasCompilationContext::setDefaultLocation(
    size_t textureIndex0, size_t textureIndex1, float elevation0, float elevation1, MapNavigability navigability)
{
    myDefaultLocation.navigability = navigability;
    myDefaultLocation.sceneryIndex = 0xFFFFFFFF;

    // Reset the layers
    for (int i = 0; i < 2; ++i)
    {
        LocationTerrainDescription* terrainLayer = &myDefaultLocation.terrainLayers[i];
        terrainLayer->terrainMeshSubset = TMS_INVALID;
        terrainLayer->textureIndex = 0xFFFFFFFF;
        terrainLayer->wallTextureIndex = 0xFFFFFFFF;
        terrainLayer->height = 0.0f;
        terrainLayer->elevation = 0.0f;
    }

    // Enable the first layer's basic terrain
    myDefaultLocation.terrainLayers[0].terrainMeshSubset = TMS_FLAT_TEXN;
    myDefaultLocation.terrainLayers[0].elevation = elevation0;
    myDefaultLocation.terrainLayers[0].textureIndex = textureIndex0;

    myDefaultLocation.terrainLayers[1].terrainMeshSubset = TMS_FLAT_TEXN;
    myDefaultLocation.terrainLayers[1].elevation = elevation1;
    myDefaultLocation.terrainLayers[1].textureIndex = textureIndex1;
}


//------------------------------------------------------------------------------------------------
// Name:  setSpawnInfo
// Desc:  Initializes character spawning information
//------------------------------------------------------------------------------------------------
void AtlasCompilationContext::setSpawnInfo(int spawnX,
                                           int spawnY,
                                           float spawnRadius,
                                           int evil_spawn_x,
                                           int evil_spawn_y)
{
    mySpawnX = spawnX;
    mySpawnY = spawnY;
    mySpawnRadius = spawnRadius;
    evil_spawn_x_ = evil_spawn_x;
    evil_spawn_y_ = evil_spawn_y;
}




//------------------------------------------------------------------------------------------------
// Name:  reserve
// Desc:  Makes sure there is enough room in the map to include locations in the given region
//------------------------------------------------------------------------------------------------
bool AtlasCompilationContext::reserve(int x1, int y1, int x2, int y2)
{
    // If we have enough room, don't do anything
    if (myX1 <= x1 && myY1 <= y1 && myX2 >= x2 && myY2 >= y2) return true;

    // Calculate new sizes
    int newX1 = min(x1, myX1),
        newX2 = max(x2, myX2),
        newY1 = min(y1, myY1),
        newY2 = max(y2, myY2);
    int newWidth = newX2 - newX1;
    int newHeight = newY2 - newY1;

    // Allocate a new map
    LocationDescription* newMap = new LocationDescription[newWidth*newHeight];

    // Make sure the allocation succeeded
    if (APP_FATAL(!newMap)("Out of memory: couldn't allocate %ix%i map", newWidth, newHeight)) return false;

    //std::set<size_t> indices, srcIndices;

    // Copy old data into the new map
    int newLeftOffset = myX1 - newX1;
    int currentWidth = myX2 - myX1;
    for (int y = newY1; y < newY2; ++y)
    {
        // If this row is out of range of the old map, copy default locations into the row
        if (y < myY1 || y >= myY2)
        {
            for (int x = 0; x < newWidth; ++x)
            {
                memcpy(&newMap[(y-newY1)*newWidth + x], &myDefaultLocation, sizeof(AtlasCompilationContext::LocationDescription));
            }
        }
        else

        // If we get here, the row is in the old range of data, so we need to copy that
        // data into the new map
        {
            // Copy locations that weren't set yet on the left
            for (int x = newX1; x < myX1; ++x)
            {
                memcpy(&newMap[(y-newY1)*newWidth + (x-newX1)], &myDefaultLocation, sizeof(LocationDescription));
            }

            // Copy the old row
            {
                //for (int x = 0; x < currentWidth; ++x)
                //{
                //    indices.insert((y-newY1)*newWidth + newLeftOffset + x);
                //    srcIndices.insert((y-myY1)*currentWidth + x);
                //}

                memcpy(&newMap[(y-newY1)*newWidth + myX1 - newX1], &myMap[(y-myY1)*currentWidth], sizeof(LocationDescription)*currentWidth);
            }

            // Copy locations that weren't set yet on the right
            for (int x = myX1 + currentWidth; x < newX2; ++x)
            {
                memcpy(&newMap[(y-newY1)*newWidth + x-newX1], &myDefaultLocation, sizeof(LocationDescription));
            }
        }
    }

    // Make sure all of the locations were set (for debugging)
    //for (int y = 0; y < newHeight; ++y)
    //    for (int x = 0; x < newWidth; ++x)
    //    {
    //        APP_WARNING(newMap[y*newWidth+x].navigability < 0 || newMap[y*newWidth+x].navigability > MAPNAV_COUNT)
    //            ("Element at %i, %i isn't valid!!!", x, y);
    //    }

    // Get rid of the old map
    SAFE_DELETE(myMap);

    // Save settings and the new map
    myX1 = newX1;
    myX2 = newX2;
    myY1 = newY1;
    myY2 = newY2;
    myMap = newMap;

    // Success
    return true;
}


void AtlasCompilationContext::setBackgroundSoundLoopIndex(unsigned int index) {
  background_sound_loop_index_ = index;
}


//------------------------------------------------------------------------------------------------
// Name:  importDescriptions
// Desc:  Adds all of the terrain descriptions from the given cache into this context
//------------------------------------------------------------------------------------------------
void AtlasCompilationContext::importDescriptions(AtlasEditDialog::MapCacheType* mapCache)
{
    for (AtlasEditDialog::MapCacheType::iterator i = mapCache->begin(); i != mapCache->end(); ++i)
    {
        // Make sure the map has a space for this element
        int x = i->first.first;
        int y = i->first.second;
        if (APP_ERROR(!reserve(x, y, x+1, y+1))("Failed to reserve space to compile %i,%i", x, y))
            return;


        // The description structure being built
        LocationDescription* description = &myMap[y*(myX2-myX1) + (x-myX1)];
        description->navigability = i->second->navigability;
        {
            Scenery::Preview* sceneryPreview = i->second->scenery;
            dc::dcList<Scenery>::Element* sceneryElement = sceneryPreview ? (dcList<Scenery>::Element*)sceneryPreview->scenery : 0;
            description->sceneryIndex = sceneryElement ? sceneryElement->getIndex() : 0xFFFFFFFF;
        }

        // Build each terrain layer
        for (int layerIndex = 0; layerIndex < 2; ++layerIndex)
        {
            LocationTerrainDescription* dest = &description->terrainLayers[layerIndex];
            AtlasEditDialog::MapLocation::TerrainLayer* src = &i->second->layer[layerIndex];

            // Warn if this layer has already been filled in
            APP_WARNING(dest->textureIndex != myDefaultLocation.terrainLayers[layerIndex].textureIndex ||
                         dest->terrainMeshSubset != myDefaultLocation.terrainLayers[layerIndex].terrainMeshSubset)
                         ("Element in terrain being overwritten!!");

            // Reset this layer
            dest->elevation = 0.0f;
            dest->height = 0.0f;
            dest->terrainMeshSubset = TMS_INVALID;
            dest->textureIndex = 0xFFFFFFFF;
            dest->wallTextureIndex = 0xFFFFFFFF;

            // If this isn't a valid layer, move on
            if (!src->terrain || !src->terrain->terrain)
                continue;

            // Copy over basic information
            dest->elevation = src->elevation;
            dest->height = src->height;
            dest->terrainMeshSubset = src->terrainMeshSubset;

            // Get the terrain element for this layer
            Terrain::Preview* terrain = src->terrain;

            // Get the texture element for the top section
            {
                Texture::Preview* texturePreview = terrain ? terrain->topTexturePreview : 0;
                Texture* texture = texturePreview ? texturePreview->texture : 0;
                dc::dcTable<Texture>::Element* textureElement = texture ? ((dcTable<Texture>::Element*)texture->getOwner()) : 0;
                dest->textureIndex = textureElement ? textureElement->getIndex() : 0xFFFFFFFF;
            }

            // Get the texture element for the side, if this is a wall
            if (terrain && terrain->isWall)
            {
                Texture::Preview* texturePreview = terrain ? terrain->sideTexturePreview : 0;
                Texture* texture = texturePreview ? texturePreview->texture : 0;
                dc::dcTable<Texture>::Element* textureElement = texture ? ((dcTable<Texture>::Element*)texture->getOwner()) : 0;
                dest->wallTextureIndex = textureElement ? textureElement->getIndex() : 0xFFFFFFFF;

                // Set the terrain mesh subset type to display flat
                dest->terrainMeshSubset = TMS_FLAT(src->terrain->direction);
            }
            else
                dest->wallTextureIndex = 0xFFFFFFFF;
        }        
    }
}






//------------------------------------------------------------------------------------------------
// Name:  getSingleQuadrantMapOffset
// Desc:  Gets the offset values that will put all of the map data into the positive-x,
//------------------------------------------------------------------------------------------------
void AtlasCompilationContext::getSingleQuadrantMapOffset(int* xOffset, int* yOffset)
{
    *xOffset = myX1;
    *yOffset = myY1;
}




//------------------------------------------------------------------------------------------------
// Name:  compile
// Desc:  Writes out the data to compile this map layer into the given stream
//------------------------------------------------------------------------------------------------
bool AtlasCompilationContext::compile(LPDIRECT3DDEVICE9 d3dDevice,
                                      dc::dcStreamOut* clientStream,
                                      dc::dcStreamOut* serverStream,
                                      int* width_out,
                                      int* height_out)
{
  // Make sure the map is on a 2^k boundary size in each direction and at least as large
  // as a single chunk.
  int currentWidth = (int)(myX2 - myX1), width = currentWidth;
  int currentHeight = (int)(myY2 - myY1), height = currentHeight;

  // Increase the width and height to be as large as a chunk
  if (width < COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS)   width = COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS;
  if (height < COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS) height = COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS;

  // Find the next power of two higher than the width
  width = width - 1;
  width = width | (width >> 1);
  width = width | (width >> 2);
  width = width | (width >> 4);
  width = width | (width >> 8);
  width = width | (width >> 16);
  width = width + 1;

  // Find the next power of two higher than the height
  height = height - 1;
  height = height | (height >> 1);
  height = height | (height >> 2);
  height = height | (height >> 4);
  height = height | (height >> 8);
  height = height | (height >> 16);
  height = height + 1;

  // Make sure the height/width are equal; otherwise, resize this map
  if (APP_INFO(width != currentWidth || height != currentHeight)
          ("Map is not a power of two:  %i x %i is being resized to %i x %i", currentWidth, currentHeight, width, height))
      if (APP_ERROR(!reserve(myX1, myY1, myX1 + width, myY1 + height))("Unable to resize map!"))
          return false;

  // For the map to compile correctly, the world size must be a multiple of
  // the world region size.  This doesn't cause compilation to fail, but it
  // can potentially cause the swarm spawner not to work correctly.
  ASSERT(width%Evidyon::World::REGION_SIZE == 0 &&
        height%Evidyon::World::REGION_SIZE == 0);

  // As of the new version, the map can't be larger than
  // 2 ^ (16-5+1) in either direction.  This is because actor
  // positions are encoded as 16-bit integers with 5 points of
  // decimal precision per square.
  ASSERT(width<=1<<(16-5+1) && height <= 1<<(16-5+1));


  // Dec 2008: Set output size, used by the swarm spawner compiler
  *width_out = width;
  *height_out = height;

  //=======================================================================================
  // Client - Header Information
  //=======================================================================================
  // Create the clientHeader information
  CompiledAtlasClientHeader clientHeader;

  // Compile the clientHeader data we already know
  clientHeader.mapBytesPerPixel = COMPILEDATLAS_MAP_BYTES_PER_PIXEL;
  clientHeader.mapImageFormat = COMPILEDATLAS_MAP_FORMAT;
  clientHeader.width = (size_t)width;
  clientHeader.height = (size_t)height;
  clientHeader.defaultNavigability = myDefaultLocation.navigability;
  clientHeader.defaultTextureIndex = myDefaultLocation.terrainLayers[0].textureIndex;
  clientHeader.fileLocations.start = clientStream->tell();
  clientHeader.background_sound_loop = background_sound_loop_index_;

  // Save this clientHeader, so that the place is reserved (we'll write in the final version later)
  if (APP_ERROR(!clientStream->write(&clientHeader, sizeof(clientHeader)))("Failed writing atlas clientHeader"))
      return false;


  //=======================================================================================
  // Client - Terrain Meshes
  //=======================================================================================
  {
    // Save the current location
    clientHeader.fileLocations.terrainMeshes = clientStream->tell();

    // Generate the terrain mesh
    LPD3DXMESH terrainMesh;
    if (APP_ERROR(!CreateTerrainMesh(d3dDevice, &terrainMesh))("Couldn't create terrain mesh"))
        return false;

    // Get geometry for this mesh
    SubsetGeometry subsetGeometry;
    bool builtGeometry = XMesh::buildGeometryFromD3DXMesh(terrainMesh, &subsetGeometry, TMS_COUNT);

    // Release the original X mesh
    terrainMesh->Release();

    // Make sure the geometry could be constructed
    if (APP_ERROR(!builtGeometry)("Unable to construct the terrain mesh's geometry"))
        return false;

    // Save the number of subsets
    size_t subsets = subsetGeometry.size();
    if (APP_ERROR(!clientStream->write(&subsets, sizeof(subsets)))("Unable to save subset count"))
    {
        DeallocateGeometry(&subsetGeometry);
        return false;
    }

    // TODO: it is expected that the geometry for the terrain is linear
    // and indexable by a normal array--this assumption should be checked
    // during compilation

    // Save all of the mesh's geometry
    for (SubsetGeometry::iterator i = subsetGeometry.begin(); i != subsetGeometry.end(); ++i)
    {
      // August 2008: Switch the winding order of the indices--it's reversed after building the geometry for some reason
      {
        GeometryIndex* indices = i->second->pIndices;
        DWORD numberOfTriangles = i->second->NumIndices / 3;
        for (DWORD triangleIndex = 0; triangleIndex < numberOfTriangles; ++triangleIndex) {
          GeometryIndex temp = indices[0];
          indices[0] = indices[1];
          indices[1] = temp;
          indices += 3;
        }
      }


      // Save the geometry for this subset in the following format:
      //      SubsetIndex     subsetIndex;
      //      DWORD           numVertices;
      //      DWORD           numIndices;
      //      GeometryVertex  vertexArray[numVertices];
      //      GeometryIndex   indexArray[numIndices];
      bool failed = !clientStream->write(&i->first, sizeof(SubsetIndex)) ||
                    !clientStream->write(&i->second->NumVertices, sizeof(DWORD)) ||
                    !clientStream->write(&i->second->NumIndices, sizeof(DWORD)) ||
                    ((i->second->NumVertices > 0) && !clientStream->write(i->second->pVertices, sizeof(GeometryVertex) * i->second->NumVertices)) ||
                    ((i->second->NumIndices > 0) && !clientStream->write(i->second->pIndices, sizeof(GeometryIndex) * i->second->NumIndices));

      // Make sure the subset's geometry was saved correctly
      if (APP_ERROR(failed)("Unable to save terrain geometry")) {
        DeallocateGeometry(&subsetGeometry);
        return false;
      }
    }

    // Deallocate the geometry
    DeallocateGeometry(&subsetGeometry);
  }



  //=======================================================================================
  // Client - Description Table
  //=======================================================================================
  
  // Save the current location
  clientHeader.fileLocations.descriptionTable = clientStream->tell();

  // Build a table with each of the unique descriptions and their occurrance rates
  DescriptionOccurranceTable descriptionOccurranceTable;
  size_t mapLocations = clientHeader.width * clientHeader.height;
  for (size_t i = 0; i < mapLocations; ++i)
  {
      // Find this element's equivalent in the map
      DescriptionOccurranceTable::iterator element = descriptionOccurranceTable.find(&myMap[i]);

      // If we found the element in the map, increment its occurrance count; otherwise,
      // create a new entry for it
      if (element != descriptionOccurranceTable.end())
          element->second++;
      else
          descriptionOccurranceTable.insert(DescriptionOccurranceTable::value_type(&myMap[i], 1));
  }

  // Report on the number of entries
  DEBUG_INFO("Compiled atlas as %lu unique location types", descriptionOccurranceTable.size());

  // Add all of the entries based on their number of occurrances to the queue
  DescriptionsSortedByOccurranceQueue descriptionsSortedByOccurrance;
  for (DescriptionOccurranceTable::iterator i = descriptionOccurranceTable.begin();
                                            i != descriptionOccurranceTable.end(); ++i)
      descriptionsSortedByOccurrance.push(OccurranceToDescriptionPair(i->second, i->first));

  // We don't need this table anymore
  descriptionOccurranceTable.clear();

  // Build a list of the entries that are going to be cached
  DescriptionToLookupTableIndexTable descriptionToIndexTable;
  size_t index = 0, totalIndexedOccurrances = 0;
  while(index < COMPILEDATLAS_MAP_LOOKUPTABLE_SIZE && !descriptionsSortedByOccurrance.empty())
  {
      // Get information about how many times this description is used
      size_t occurrances = descriptionsSortedByOccurrance.top().first;
      totalIndexedOccurrances += occurrances;

      // Add to the index-lookup table
      descriptionToIndexTable.insert(
          DescriptionToLookupTableIndexTable::value_type(descriptionsSortedByOccurrance.top().second, index));

      // Get rid of this index
      descriptionsSortedByOccurrance.pop();

      // Display occurrances in debug mode
      DEBUG_INFO("Occurrance for index %lu:\t%lu", index, occurrances);

      // Increment the index
      index++;
  }

  // We no longer use the sorted queue
  while(!descriptionsSortedByOccurrance.empty())
      descriptionsSortedByOccurrance.pop();

  // Write out the descriptions table.  This won't necessairily have the most used one
  // first, but it will have the top most used entries in the table.  We have to update
  // the index here, however, to make sure that it is referenced correctly!
  index = 0;
  for (DescriptionToLookupTableIndexTable::iterator i  = descriptionToIndexTable.begin();
                                                    i != descriptionToIndexTable.end(); ++i)
  {
      // Convert to the type that is saved to the file
      CompiledAtlasClientLocationDescription cacld;
      convertLocationDescription(i->first, &cacld);
      i->second = index;

      // Save this entry
      if (APP_ERROR(!clientStream->write(&cacld, sizeof(cacld)))("Couldn't write CACLD"))
          return false;

      // Move to the next entry in the array
      ++index;
  }

  // Pad unknown entries until we have enough in the array
  {
    CompiledAtlasClientLocationDescription emptyCACLD;
    convertLocationDescription(NULL, &emptyCACLD);
    while(index < COMPILEDATLAS_MAP_LOOKUPTABLE_SIZE) {
      // Write this entry
      if (APP_ERROR(!clientStream->write(&emptyCACLD, sizeof(emptyCACLD)))("Couldn't write CACLD"))
          return false;

      // Advance to the next (unavailable) index
      index++;
    }
  }

  //=======================================================================================
  // Client - Map
  //=======================================================================================

  // Save the file location
  clientHeader.fileLocations.map = clientStream->tell();

  // Create a texture to hold the map's data
  LPDIRECT3DTEXTURE9 mapImage = NULL;
  if (APP_ERROR(FAILED(
      D3DXCreateTexture(d3dDevice, width, height, 1, 0,
                         COMPILEDATLAS_MAP_FORMAT, D3DPOOL_SCRATCH, &mapImage)))
         ("Couldn't create map texture"))
     return false;

  // Lock the image buffer
  D3DLOCKED_RECT mapImageLockedRect;
  if (APP_ERROR(FAILED(mapImage->LockRect(0, &mapImageLockedRect, NULL, 0)))
          ("Unable to lock the map texture"))
  {
      SAFE_RELEASE(mapImage);
      return false;
  }

  // Save the locked bits pointer as a usable pointer, and adjust the pitch to reflect
  // increment values in our datatype and not in bytes.  The pitch will always be a
  // power of two, so as long as the pitch is > the size of a single table entry index,
  // this will allow us to prevent having to adjust the code below this when changing
  // the size of the index table.
  mapImageLockedRect.Pitch /= COMPILEDATLAS_MAP_BYTES_PER_PIXEL;
  COMPILEDATLAS_MAP_PIXEL_DATATYPE* mapImageBits = (COMPILEDATLAS_MAP_PIXEL_DATATYPE*)mapImageLockedRect.pBits;

  // These vectors hold descriptions that aren't in the main body
  std::vector<size_t> chunkToNontableDescriptionIndices;
  std::vector<LocationDescription*> nontableDescriptions;

  // Keep track of the relative frequencies of table vs. nontable entries
  int tableLocations = 0, nontableLocations = 0;

  // Fill the texture with data!  This requires a chunk-based approach!
  int mapChunksWide = width / COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS,
      mapChunksHigh = height / COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS;
  for (int chunkX = 0; chunkX < mapChunksWide; ++chunkX)
      for (int chunkY = 0; chunkY < mapChunksHigh; ++chunkY)
      {
          // Get the top-left corner location in map units
          int left = chunkX * COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS;
          int top  = chunkY * COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS;

          // Set where this chunk starts in the nontable description array
          chunkToNontableDescriptionIndices.push_back(nontableDescriptions.size());

          // Repeat for each entry in this chunk
          for (int x = 0; x < COMPILEDATLAS_CHUNK_WIDTH_IN_LOCATIONS; ++x)
              for (int y = 0; y < COMPILEDATLAS_CHUNK_HEIGHT_IN_LOCATIONS; ++y)
              {
                  // Get the map description at this location
                  LocationDescription* data = &myMap[(y+top)*width+(x+left)];

                  // Try to find this index in the lookup table
                  DescriptionToLookupTableIndexTable::iterator i = descriptionToIndexTable.find(data);

                  // If this element was found, write its corresponding index
                  // (none of these indices will ever be zero!).  Because of the adjustment
                  // done earlier, this pitch is correct for whichever datatype pngBits happens
                  // to be in.
                  COMPILEDATLAS_MAP_PIXEL_DATATYPE locationEntry =
                      i != descriptionToIndexTable.end() ?
                          (COMPILEDATLAS_MAP_PIXEL_DATATYPE)i->second
                      :
                          COMPILEDATLAS_MAP_INDEX_NOTINTABLE;

                  // Because of the adjustment done earlier, this pitch is correct for
                  // whichever datatype pngBits happens to be in.
                  mapImageBits[((y+top)*mapImageLockedRect.Pitch+(x+left))] = locationEntry;

                  // In the slightly more complex situation when the entry is not in the table,
                  // put the description into the nontable description array so it can be
                  // accessed later.
                  if (locationEntry == COMPILEDATLAS_MAP_INDEX_NOTINTABLE)
                  {
                      // Add data to the nontable description array
                      nontableDescriptions.push_back(data);

                      // This is just for reporting purposes; it serves no function during this process
                      nontableLocations++;
                  }
                  else
                  {
                      // This is just for reporting purposes; it serves no function during this process
                      tableLocations++;
                  }
              }
      }

  // Unlock the map's texture
  mapImage->UnlockRect(0);

  // Report the status of this map
  DEBUG_INFO("Finished writing atlas's map image:  %lu locations indexed,  %lu excess (%.02f%%)",
              tableLocations, nontableLocations, 100 * nontableLocations / (float)(tableLocations + nontableLocations));

  // Write this image into the output clientStream
  {
      LPD3DXBUFFER mapImageInMemory;
      HRESULT hr = D3DXSaveTextureToFileInMemory(&mapImageInMemory, D3DXIFF_PNG, mapImage, NULL);

      // Get rid of the source texture
      SAFE_RELEASE(mapImage);

      // If the library failed to save the image, this process fails
      if (APP_ERROR(FAILED(hr))("D3DX was unable to compile the map image")) return false;

      // Write the map image into the clientStream
      size_t imageSize = mapImageInMemory->GetBufferSize();
      bool failedWritingImage = !clientStream->write(&imageSize, sizeof(imageSize)) ||
                                !clientStream->write(mapImageInMemory->GetBufferPointer(), imageSize);

      // Get rid of the buffer
      SAFE_RELEASE(mapImageInMemory);

      // Make sure the writing was successful
      if (APP_ERROR(failedWritingImage)("Couldn't write map image to output file")) return false;
  }

  // Done with the lookup table
  descriptionToIndexTable.clear();


  //=======================================================================================
  // Client - Chunk to Nontable Description Index Table
  //=======================================================================================

  // Mark the start
  clientHeader.fileLocations.chunkToNontableDescriptionIndicesTable = clientStream->tell();

  // TODO: BEWARE!!! this makes the assumption that the std::vector type
  // stores its elements in a linear array!!!!
  size_t chunkLookupSize = chunkToNontableDescriptionIndices.size();
  if (APP_ERROR(!clientStream->write(&chunkLookupSize, sizeof(chunkLookupSize)) ||
                 (chunkLookupSize > 0) && !clientStream->write(&chunkToNontableDescriptionIndices[0], sizeof(size_t) * chunkLookupSize))
          ("Couldn't write nontable description array indices"))
      return false;


  //=======================================================================================
  // Client - Nontable Descriptions
  //=======================================================================================

  // Mark this section
  clientHeader.fileLocations.nontableDescriptions = clientStream->tell();

  // Save the array's size
  size_t nontableDescriptionArraySize = nontableDescriptions.size();
  if (APP_ERROR(!clientStream->write(&nontableDescriptionArraySize, sizeof(nontableDescriptionArraySize)))
          ("Couldn't write nontableDescriptionArraySize")) return false;

  // Save each of the descriptions
  for (std::vector<LocationDescription*>::iterator i  = nontableDescriptions.begin();
                                                   i != nontableDescriptions.end(); ++i)
  {
      // Convert this entry to the client format
      CompiledAtlasClientLocationDescription cacld;
      convertLocationDescription(*i, &cacld);

      // Save the map data
      if (APP_ERROR(!clientStream->write(&cacld, sizeof(cacld)))
              ("Couldn't write nontable description array entry"))
          return false;
  }

  // Save the end of the map
  clientHeader.fileLocations.end = clientStream->tell();

  // Rewrite the clientHeader
  if (APP_ERROR(!clientStream->seek(clientHeader.fileLocations.start) ||
                 !clientStream->write(&clientHeader, sizeof(clientHeader)) ||
                 !clientStream->seek(clientHeader.fileLocations.end))("Unable to write map clientHeader")) return false;

  //=======================================================================================
  // Server - Header
  //=======================================================================================
  CompiledAtlasServerHeader serverHeader;
  serverHeader.width = width;
  serverHeader.height = height;
  serverHeader.spawnX = mySpawnX;
  serverHeader.spawnY = mySpawnY;
  serverHeader.spawnRadius = mySpawnRadius;
  serverHeader.evil_spawn_x = evil_spawn_x_;
  serverHeader.evil_spawn_y = evil_spawn_y_;
  serverHeader.defaultNavigability = myDefaultLocation.navigability;

  // Write out the header
  if (APP_ERROR(!serverStream->write(&serverHeader, sizeof(serverHeader)))("Couldn't write server header"))
      return false;

  // Allocate a temporary array to hold navigability information
  size_t navigabilityArraySize = width * height;
  MapNavigability* navigabilityArray = new MapNavigability[navigabilityArraySize];
  if (APP_FATAL(!navigabilityArray)("Unable to allocate navigability array for server"))
      return false;

  // Fill the array
  for (size_t y = 0; y < height; ++y)
      for (size_t x = 0; x < width; ++x)
          navigabilityArray[y*width+x] = myMap[y*width+x].navigability;

  // Write the array
  if (APP_ERROR(!serverStream->write(&navigabilityArraySize, sizeof(navigabilityArraySize)) ||
                 !serverStream->write(navigabilityArray, sizeof(MapNavigability)*navigabilityArraySize))
          ("Couldn't write navigability array"))
  {
      SAFE_DELETE_ARRAY(navigabilityArray);
      return false;
  }

  // Deallocate the array
  SAFE_DELETE_ARRAY(navigabilityArray);

  // Success
  return true;
}
