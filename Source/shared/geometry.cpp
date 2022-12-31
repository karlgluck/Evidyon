//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#include <windows.h>
#include <map>
#include <dc/debug>
#include "geometry.h"




//-----------------------------------------------------------------------------
// Name: AllocateGeometry
// Desc: Creates a new geometry structure with the requested buffer sizes
//-----------------------------------------------------------------------------
HRESULT AllocateGeometry(DWORD Vertices, DWORD Indices, Geometry** ppGeometry)
{
  // Make sure the geometry pointer is valid
  if (APP_ERROR(!ppGeometry)("Invalid geometry reference"))
		return false;

  // Create a new geometry obect
  Geometry* pGeometry = new Geometry;
  if (APP_ERROR(!pGeometry)("Couldn't allocate geometry")) return false;

  // Allocate the vertex buffer
  pGeometry->pVertices = new GeometryVertex[Vertices];
  if (APP_ERROR(!pGeometry->pVertices)("Couldn't allocate vertices for geometry"))
  {
		// Get rid of the allocated geometry
    delete pGeometry;

		// Failure
		return false;
  }

	// Store the number of vertices
  pGeometry->NumVertices = Vertices;

  // Allocate the index buffer
  pGeometry->pIndices = new GeometryIndex[Indices];
  if (APP_ERROR(!pGeometry->pVertices)("Couldn't allocate geometry vertex data"))
  {
		// Erase the allocated vertices
    delete[] pGeometry->pVertices;

		// Get rid of the geometry structure
    delete pGeometry;

		// No more memory!
    return E_OUTOFMEMORY;
  }

	// Save the index count
  pGeometry->NumIndices = Indices;

  // Return the geometry pointer
  *ppGeometry = pGeometry;

  // Success
  return S_OK;
}



//-----------------------------------------------------------------------------
// Name: GetGeometryTotals
// Desc: Calculates the total number of vertices/indices in a set of geometry
//-----------------------------------------------------------------------------
VOID GetGeometryTotals(SubsetGeometry* geometry, DWORD* pTotalVertices, DWORD* pTotalIndices)
{
  // Make sure the pointer is valid
  if (APP_ERROR(!geometry || !pTotalVertices || !pTotalIndices)("GetGeometryTotals was provided a NULL pointer!"))
		return;

  // Go through the geometry
  DWORD vertices = 0, indices = 0;
  for (SubsetGeometry::iterator i = geometry->begin(); i != geometry->end(); ++i)
  {
    vertices += i->second->NumVertices;
    indices  += i->second->NumIndices;
  }

  // Store the output values
  *pTotalVertices = vertices;
  *pTotalIndices = indices;
}


//-----------------------------------------------------------------------------
// Name: DeallocateGeometry
// Desc: Erases a geometry pointer
//-----------------------------------------------------------------------------
VOID DeallocateGeometry(Geometry* pGeometry)
{
  // Make sure the pointer is valid
  if (APP_ERROR(!pGeometry)("DeallocateGeometry was provided a NULL pointer!"))
		return;

  // Deallocate all of the buffers
  if (pGeometry->pVertices) delete[] pGeometry->pVertices;
  if (pGeometry->pIndices) delete[] pGeometry->pIndices;

  // Get rid of the geometry itself
  delete pGeometry;
}


//-----------------------------------------------------------------------------
// Name: DeallocateGeometryGroup
// Desc: Erases all of the geometry in a group
//-----------------------------------------------------------------------------
template <typename GeometryGroup> VOID DeallocateGeometryGroup(GeometryGroup* pGroup)
{
  // Make sure the pointer is valid
  if (APP_ERROR(!pGroup)("DeallocateGeometry was provided a NULL pointer!"))
		return;

  // Iterate through the collection
  for (GeometryGroup::iterator i = pGroup->begin(); i != pGroup->end(); ++i)
  {
    // Get the geometry pointer
    Geometry* pGeometry = i->second;

    // Get rid of this element
    DeallocateGeometry(pGeometry);
  }

  // Clear the list
  pGroup->clear();
}


//-----------------------------------------------------------------------------
// Name: DeallocateGeometry
// Desc: Erases all of the geometry in a group
//-----------------------------------------------------------------------------
VOID DeallocateGeometry(SubsetGeometryCollection* pGroup)
{
  DeallocateGeometryGroup<SubsetGeometryCollection>(pGroup);
}


//-----------------------------------------------------------------------------
// Name: DeallocateGeometry
// Desc: Erases all of the geometry in a group
//-----------------------------------------------------------------------------
VOID DeallocateGeometry(SubsetGeometry* pGroup)
{
  DeallocateGeometryGroup<SubsetGeometry>(pGroup);
}



//-----------------------------------------------------------------------------
// Name: CollectGeometry
// Desc: Compresses a collection of geometry with multiple sets of geometry
//-----------------------------------------------------------------------------
template <typename MultiGroup, typename Group> HRESULT CollectGeometryGroup(
  const MultiGroup* pSourceGeometry,
  Group* pDestGeometry)
{
  // Make sure the pointers are valid
  if (APP_ERROR(!pSourceGeometry)("Invalid pSourceGeometry pointer for CollectGeometry") ||
		APP_ERROR(!pDestGeometry)("Invalid pDestGeometry pointer for CollectGeometry"))
		return E_INVALIDARG;

  // Iterate through each geometry entry
  for (MultiGroup::const_iterator source = pSourceGeometry->begin();
                  source != pSourceGeometry->end();) // no need to increment the source
  {
    // Find the range of this key type
    MultiGroup::const_iterator begin = source;
    MultiGroup::const_iterator end = pSourceGeometry->upper_bound(source->first);

    // Accumulates vertex/index counts for this key type
    DWORD totalVertices = 0, totalIndices = 0;

    // Iterate through the range to determine how much geometry these contain
    for (MultiGroup::const_iterator i = begin; i != end; ++i)
    {
      // Get this geometry pointer
      const Geometry* pGeometry = i->second;

      // Make sure the geometry exists
      if (APP_WARNING(!pGeometry)("Blank entry in geometry collection; ignoring"))
				continue;

      // Add this geometry's data to the list
      totalVertices += pGeometry->NumVertices;
      totalIndices += pGeometry->NumIndices;
    }

    // Now that we have the totals for the geometry, create a new instance to hold it all
    Geometry* pCollectedEntry;
    if (APP_ERROR(AllocateGeometry(totalVertices, totalIndices, &pCollectedEntry))("Couldn't allocate geometry"))
    {
      // Get rid of all of the geometry we created
      DeallocateGeometry(pDestGeometry);
      return false;
    }

    // Reset the accumulators
    totalVertices = 0;
    totalIndices = 0;

    // Go through the source range again, this time adding geometry to the new buffer
    for (MultiGroup::const_iterator i = begin; i != end; ++i)
    {
      // Get this geometry pointer
      const Geometry* pGeometry = i->second;

      // Make sure the geometry exists (we already output a warning above so don't
      // worry about letting the user know any more)
      if (!pGeometry) continue;

      // Copy the verties into the buffer at the current location
      memcpy_s(pCollectedEntry->pVertices + totalVertices,                // destination for copy
           (pCollectedEntry->NumVertices - totalVertices) * sizeof(GeometryVertex),  // how many bytes are left in the buffer
            pGeometry->pVertices,                          // source vertices
            pGeometry->NumVertices * sizeof(GeometryVertex));             // how many bytes to copy


      // Optimize the index copy operation by dereferencing pointers earlier
      const GeometryIndex* pSrcIndices = pGeometry->pIndices;
      GeometryIndex* pDestIndices = pCollectedEntry->pIndices + totalIndices;
      DWORD numIndices = pGeometry->NumIndices;

      // Copy the indices and add the vertex offset
      for (GeometryIndex index = 0; index < numIndices; ++index)
        pDestIndices[index] = pSrcIndices[index] + totalVertices;

      // Increment the total amount of geometry
      totalVertices += pGeometry->NumVertices;
      totalIndices += numIndices;
    }

    // Add this new entry to the output set
    pDestGeometry->insert(Group::value_type(source->first, pCollectedEntry));

    // Advance the source iterator
    source = end;
  }

  // Success
  return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CollectGeometry
// Desc: Compresses a collection of geometry with multiple sets of geometry
//-----------------------------------------------------------------------------
HRESULT CollectGeometry(const SubsetGeometryCollection* pSourceGeometry,
             SubsetGeometry* pDestGeometry)
{
  return CollectGeometryGroup<SubsetGeometryCollection,SubsetGeometry>(pSourceGeometry, pDestGeometry);
}