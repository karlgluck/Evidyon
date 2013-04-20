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
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include <d3dx9.h>
#include <map>
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guitexture.h"
#include "guilayer.h"



//-----------------------------------------------------------------------------
// Name: GUILayer
// Desc: Resets the class
//-----------------------------------------------------------------------------
GUILayer::GUILayer()
{
  zero();
}

//-----------------------------------------------------------------------------
// Name: GUILayer
// Desc: Allow subclasses to destruct
//-----------------------------------------------------------------------------
GUILayer::~GUILayer()
{
  destroy();
}



//-----------------------------------------------------------------------------
// Name: create
// Desc: Initializes the buffers for this layer
//-----------------------------------------------------------------------------
bool GUILayer::create(LPDIRECT3DDEVICE9 d3dDevice, size_t vertexCapacity, size_t triangleCapacity)
{
  // Validate parameters
  if (!d3dDevice || !vertexCapacity || !triangleCapacity) return false;

  // Make sure the class is clean
  destroy();

  // Create a vertex buffer
  if (FAILED(d3dDevice->CreateVertexBuffer(
          sizeof(GUIVertex) * vertexCapacity,
          D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
          GUIVERTEX_D3DBUFFERFORMAT,
          D3DPOOL_DEFAULT,
          &myVB, 0)))
  {
    return false;
  }

  // Create an index buffer
  if (FAILED(d3dDevice->CreateIndexBuffer(
          sizeof(GUIIndex) * triangleCapacity * 3,
          D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
          GUIINDEX_D3DBUFFERFORMAT,
          D3DPOOL_DEFAULT,
          &myIB, 0)))
  {
    // Clear out the VB
    myVB->Release();

    // Failed
    return false;
  }

  // Save the values internally
  myVertexCapacity = vertexCapacity;
  myTriangleCapacity = triangleCapacity;

  // Store the device pointer internally
  (d3d_device_ = d3dDevice)->AddRef();

  // Set up the default viewport
  setFullscreenViewport(0.0f, 1.0f);

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name: destroy
// Desc: Erases this class
//-----------------------------------------------------------------------------
void GUILayer::destroy()
{
  // Remove all of the elements
  myElements.clear();

  // Release D3D object references
  if (myVB) myVB->Release();
  if (myIB) myIB->Release();
  if (d3d_device_) d3d_device_->Release();

  // Reset class members
  zero();
}



//-----------------------------------------------------------------------------
// Name: setFullscreenViewport
// Desc: Initializes the rendering depth utilized for this layer.  The layer will not
//     render beyond this Z range.  Note that elements are positioned according to
//     a 'depth' value, which is not the same thing as the 'z' value.  The depth value
//     ranges from 0.0f to 1.0f always (anything else is not rendered), and maps into
//     the range of Z values specified by this method when rendered.
//     This method fails silently if the layer has not been created successfully.
//-----------------------------------------------------------------------------
void GUILayer::setFullscreenViewport(float minZ, float maxZ)
{
  // Store the Z-coordinates in the viewport
  myViewport.MinZ = minZ;
  myViewport.MaxZ = maxZ;

  // Fail here unless we have a device pointer
  if (!d3d_device_) return;

  // Get the device's size
  LPDIRECT3DSURFACE9 backBufferSurface;
  if (FAILED(d3d_device_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBufferSurface)))
    return;

  // Get the back buffer description, and release the surface
  D3DSURFACE_DESC backBufferDesc;
  backBufferSurface->GetDesc(&backBufferDesc);
  backBufferSurface->Release();

  // Initialize the rest of the viewport
  myViewport.X = 0;
  myViewport.Y = 0;
  myViewport.Width = backBufferDesc.Width;
  myViewport.Height = backBufferDesc.Height;

  // Make sure that the primary canvas mirrors this information
  myPrimaryCanvas.alignXLeft(0);
  myPrimaryCanvas.alignYTop(0);
  myPrimaryCanvas.absoluteWidth(backBufferDesc.Width);
  myPrimaryCanvas.absoluteHeight(backBufferDesc.Height);
  //myPrimaryCanvas.realign();

  // Set up the projection matrix.  This sets the bottom as the minimum Y-coordinate so that
  // the matrix will be created in the right Y-orientation (increasing Y coordinates from top to bottom)
  D3DXMatrixOrthoOffCenterLH(&myProjectionMatrix,
    0.0f, (float)backBufferDesc.Width, (float)backBufferDesc.Height, 0.0f,
    0.0f, 1.0f);
}



//-----------------------------------------------------------------------------
// Name: registerElement
// Desc: Registers an element with the manager so that it will be drawn by the rendering procedure
//-----------------------------------------------------------------------------
void GUILayer::registerElement(GUIElement* element)
{
  myElements.insert(TextureToGUIElementTable::value_type(element->getTexture(), element));
}



//-----------------------------------------------------------------------------
// Name: unregisterElement
// Desc: Removes the element from the manager
//-----------------------------------------------------------------------------
void GUILayer::unregisterElement(GUIElement* element)
{
  // Look through each of the elements to find the one to remove
  for (TextureToGUIElementTable::iterator i = myElements.begin(); i != myElements.end(); ++i)
  {
    if (i->second == element)
    {
      // Erase this element
      myElements.erase(i);

      // Exit
      return;
    }
  }
}



//-----------------------------------------------------------------------------
// Name: clearElements
// Desc: Erases all of the elements from the manager
//-----------------------------------------------------------------------------
void GUILayer::clearElements()
{
  myElements.clear();
}



//-----------------------------------------------------------------------------
// Name: getPrimaryCanvas
// Desc: Obtains the primary canvas for this layer.  The primary canvas always has a depth of 1.0
//-----------------------------------------------------------------------------
GUICanvas* GUILayer::getPrimaryCanvas()
{
  return &myPrimaryCanvas;
}



//-----------------------------------------------------------------------------
// Name: advance
// Desc: Updates all of the elements that are contained in this layer
//-----------------------------------------------------------------------------
void GUILayer::advance(double currentTime, double timeSinceLastUpdate)
{
  // Go through all of the elements and update them
  for (TextureToGUIElementTable::iterator i = myElements.begin(); i != myElements.end(); ++i)
    i->second->advance(currentTime, timeSinceLastUpdate);
}



//-----------------------------------------------------------------------------
// Name: render
// Desc: Draws all of the components in this layer to the screen
//-----------------------------------------------------------------------------
bool GUILayer::render()
{
  if (d3d_device_ == NULL) return true; // not initialized yet

  // We haven't intialized the device this rendering cycle
  myInitializedD3DDeviceFlag = false;

  // Ensure there are elements in the list
  if (!myElements.empty())
  {
    // Get an invalid texture index
    int currentTextureIndex = myElements.begin()->first - 1;
    GUITexture* currentTexture = NULL;

    // Render each of the components
    for (TextureToGUIElementTable::iterator i = myElements.begin(); i != myElements.end(); ++i)
    {
      // Update the texture, if necessary
      if (i->first != currentTextureIndex)
      {
        // Draw the current set of geometry
        flush();

        // Get rid of the old texture
        if (currentTexture != NULL)
          currentTexture->cleanStage0Texture(d3d_device_);
        else
          d3d_device_->SetTexture(0, NULL);
        currentTextureIndex = i->first;

        // Set up the new texture
        currentTexture = getTexture(currentTextureIndex);
        if (currentTexture != NULL)
          currentTexture->initStage0Texture(d3d_device_);
      }

      // Render the element
      if (!i->second->render(this)) return false;
    }
  }

  // Make sure all of the elements' geometry is drawn
  return flush();
}



//-----------------------------------------------------------------------------
// Name: allocateQuadListGeometry
// Desc: Locks the vertex buffer, and fills indices into the index buffer for the given number of
//     quads.  If this method fails, it makes no allocations, outputs the number of quads
//     that can be successfully allocated and returns false.  If it succeeds, the vertices pointer
//     is set to a valid location for the vertices and it returns true.
//     This method assumes that vertices for the quads are specified in the shape of the letter Z
//     (top left, top right, bottom left, bottom right).
//-----------------------------------------------------------------------------
bool GUILayer::allocateQuadListGeometry(size_t* quads, GUIVertex** vertexBuffer)
{
  // Check the parameters
  if (!quads || !vertexBuffer) return false;

  GUIIndex offset;
  GUIIndex* indices;

  // Allocate the geometry for this quad list
  size_t numQuadsRequested = *quads;
  size_t numVertices = numQuadsRequested * 4;
  size_t numTriangles = numQuadsRequested * 2;
  if (!allocateGeometry(&numVertices, &numTriangles, vertexBuffer, &indices, &offset))
  {
    // Calculate the maximum number of quads allowed by the number of vertices/triangles available
    size_t vertexLimitedQuads = numVertices / 4;
    size_t triangleLimitedQuads = numTriangles / 2;

    // Return the minimum of the number of quads that can be created
    *quads = vertexLimitedQuads < triangleLimitedQuads ? vertexLimitedQuads : triangleLimitedQuads;

    // Fail
    return false;
  }
  else
  {
    // Fill in the allocated indices with geometry data for quads
    for (size_t i = 0; i < numQuadsRequested; ++i)
    {
      indices[0] = offset + 0;
      indices[1] = offset + 1;
      indices[2] = offset + 2;
      indices[3] = offset + 2;
      indices[4] = offset + 1;
      indices[5] = offset + 3;

      indices += 6;
      offset += 4;
    }

    // Success
    return true;
  }
}



//-----------------------------------------------------------------------------
// Name: allocateGeometry
// Desc: Locks the vertex and index buffers so that the calling class can fill in index information about the
//     geometry.  This method returns a pointer to the internal indices as well as an index-offset.  The
//     index offset should be added to EACH of the indices in order to make sure that the vertices that are
//     written are referenced correctly.
//     If this method cannot allocate space for the requested number of triangles, nothing is allocated,
//     the number of available triangles/vertices is output and the method returns false.  If it
//     succeeds, 'true' is returned.
//-----------------------------------------------------------------------------
bool GUILayer::allocateGeometry(size_t* vertices, size_t* triangles, GUIVertex** vertexBuffer, GUIIndex** indexBuffer, GUIIndex* indexOffset)
{
  // Fail if any of the parameters are invalid
  if (!vertices || !triangles || !vertexBuffer || !indexBuffer || !indexOffset) return false;

  // Reset output variables
  *indexOffset = 0;
  *vertexBuffer = 0;
  *indexBuffer = 0;

  // If the buffers aren't locked, lock them
  if (!myLockedVertices)
  {
    myVB->Lock(0, 0, (void**)&myLockedVertices, D3DLOCK_DISCARD);
    myConsumedVertices = 0;
  }
  if (!myLockedIndices)
  {
    myIB->Lock(0, 0, (void**)&myLockedIndices, D3DLOCK_DISCARD);
    myConsumedTriangles = 0;
  }

  // If the buffers couldn't be locked, fail
  if (!myLockedVertices || !myLockedIndices)
  {
    // Unlock a buffer, if it was locked
    if (myLockedVertices)
    {
      myVB->Unlock();
      myLockedVertices = 0;
    }
    else
    {
      myIB->Unlock();
      myLockedIndices = 0;
    }

    // Fail
    return false;
  }

  // Get the allocation requests
  size_t verticesRequested = *vertices;
  size_t trianglesRequested = *triangles;

  // Does the buffer have enough room?
  size_t verticesRemaining = myVertexCapacity - myConsumedVertices;
  size_t trianglesRemaining = myTriangleCapacity - myConsumedTriangles;
  if (verticesRequested > verticesRemaining || trianglesRequested > trianglesRemaining)
  {
    // Output the available geometry sizes
    *vertices = verticesRemaining;
    *triangles = trianglesRemaining;

    // Failed
    return false;
  }

  // Set the output variables
  *indexOffset = myConsumedVertices;
  *vertexBuffer = myLockedVertices + myConsumedVertices;
  *indexBuffer = myLockedIndices + myConsumedTriangles * 3;

  // Increment the number of consumed vertices
  myConsumedVertices += verticesRequested;
  myConsumedTriangles += trianglesRequested;

  // Exit
  return true;
}



//-----------------------------------------------------------------------------
// Name: flush
// Desc: Draws the geometry in this layer to the screen
//-----------------------------------------------------------------------------
bool GUILayer::flush()
{

  // Make sure the buffers are unlocked
  if (myLockedVertices)
  {
    myVB->Unlock();
    myLockedVertices = 0;
  }
  if (myLockedIndices)
  {
    myIB->Unlock();
    myLockedIndices = 0;
  }

  // If there is no geometry, exit here
  if (!myConsumedVertices || !myConsumedTriangles) return true;

  // If the device hasn't been set up, do so
  if (!myInitializedD3DDeviceFlag)
  {
    // Set up the device for rendering
    if (!initializeD3DDevice()) return false;

    // The device has been initialized
    myInitializedD3DDeviceFlag = true;
  }

  // Draw all of the geometry in the buffers
  HRESULT hr = d3d_device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, myConsumedVertices,
                          0, myConsumedTriangles);

  // Reset the geometry
  myConsumedVertices = 0;
  myConsumedTriangles = 0;

  // Return based on the result code
  return SUCCEEDED(hr);
}



//-----------------------------------------------------------------------------
// Name: reinitializeD3DDeviceState
// Desc: This should be called if doing rendering from outside of the scope of a render() call
//     (i.e. externally to an element).  Causes the rendering procedure to reset the device's
//     state during flush()
//-----------------------------------------------------------------------------
void GUILayer::reinitializeD3DDeviceState()
{
  myInitializedD3DDeviceFlag = false;
}




//-----------------------------------------------------------------------------
// Name: initializeD3DDevice
// Desc: Sets up the target D3D device for rendering.  This doesn't check or modify
//     the initialized device flag.
//-----------------------------------------------------------------------------
bool GUILayer::initializeD3DDevice()
{
  // TODO: clamp?
  d3d_device_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  d3d_device_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
  d3d_device_->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);

  // Don't resample the GUI
  d3d_device_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  d3d_device_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  d3d_device_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

  // GUIs don't need to be affected by lights
  d3d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);

  // Blend textures over the current ones
  d3d_device_->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
  d3d_device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  d3d_device_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  d3d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
  d3d_device_->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  d3d_device_->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

  // Enable alpha-testing to remove pixels.  This prevents clear parts of images from
  // overlaying other elements with transparent effects by writing to the z-buffer. If
  // we mess with the alpharef value, this can either take more pixels away or add them
  // on the edges.
  d3d_device_->SetRenderState(D3DRS_ALPHATESTENABLE,  TRUE);
  d3d_device_->SetRenderState(D3DRS_ALPHAREF,     0x08);
  d3d_device_->SetRenderState(D3DRS_ALPHAFUNC,    D3DCMP_GREATEREQUAL);
  d3d_device_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

  // Blend with the current texture depending on this texture's alpha value
  d3d_device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  d3d_device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  d3d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);
  d3d_device_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

  // Set up the rendering buffers
  d3d_device_->SetFVF(GUIVERTEX_D3DBUFFERFORMAT);
  d3d_device_->SetStreamSource(0, myVB, 0, sizeof(GUIVertex));
  d3d_device_->SetIndices(myIB);

  // Set up the viewport
  d3d_device_->SetViewport(&myViewport);

  // Set the camera
  d3d_device_->SetTransform(D3DTS_PROJECTION, &myProjectionMatrix);

  // Reset other transform matrices
  D3DXMATRIXA16 identity;
  D3DXMatrixIdentity(&identity);
  d3d_device_->SetTransform(D3DTS_VIEW, &identity);
  d3d_device_->SetTransform(D3DTS_WORLD, &identity);

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name: zero
// Desc: Resets everything in this class
//-----------------------------------------------------------------------------
void GUILayer::zero()
{
  d3d_device_ = 0;
  myElements.clear();
  ZeroMemory(&myViewport, sizeof(myViewport));
  D3DXMatrixIdentity(&myProjectionMatrix);
  myVertexCapacity = 0;
  myTriangleCapacity = 0;
  myVB = 0;
  myIB = 0;
  myConsumedVertices = 0;
  myConsumedTriangles = 0;
  myLockedVertices = 0;
  myLockedIndices = 0;
  myInitializedD3DDeviceFlag = false;
}

