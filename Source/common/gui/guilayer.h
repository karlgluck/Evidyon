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
#ifndef __GUILAYER_H__
#define __GUILAYER_H__
#pragma once


#include <d3dx9.h>
#include <map>
#include "guicanvas.h"
#include "guiindex.h"

struct GUIVertex;
class GUIElement;
class GUITexture;


/**
 * A layer is a set of GUI elements that render into a given range of Z coordinates
 */
class GUILayer {

  /// This structure holds all of the GUI elements that are members of this layer
  typedef std::multimap<int,GUIElement*> TextureToGUIElementTable;

  public:

    /**
     * Resets the class
     */
    GUILayer();

    /**
     * Allow subclasses to destruct
     */
    virtual ~GUILayer();

    /**
     * Initializes the buffers for this layer
     */
    bool create(LPDIRECT3DDEVICE9 d3dDevice, size_t vertexCapacity, size_t triangleCapacity);

    /**
     * Erases this class
     */
    void destroy();

    /**
     * Initializes the rendering depth utilized for this layer.  The layer will not
     * render beyond this Z range.  Note that elements are positioned according to
     * a 'depth' value, which is not the same thing as the 'z' value.  The depth value
     * ranges from 0.0f to 1.0f always (anything else is not rendered), and maps into
     * the range of Z values specified by this method when rendered.
     * This method fails silently if the layer has not been created successfully.
     */
    void setFullscreenViewport(float minZ, float maxZ);

    /**
     * Registers an element with the manager so that it will be drawn by the rendering procedure
     */
    void registerElement(GUIElement* element);

    /**
     * Removes the element from the manager
     */
    void unregisterElement(GUIElement* element);

    /**
     * Erases all of the elements from the manager
     */
    void clearElements();

    /**
     * Obtains the primary canvas for this layer.  The primary canvas always has a depth of 1.0
     */
    GUICanvas* getPrimaryCanvas();

    /**
     * Updates all of the elements that are contained in this layer
     */
    void advance(double currentTime, double timeSinceLastUpdate);

    /**
     * Draws all of the components in this layer to the screen
     */
    bool render();

    /**
     * Locks the vertex buffer, and fills indices into the index buffer for the given number of
     * quads.  If this method fails, it makes no allocations, outputs the number of quads
     * that can be successfully allocated and returns false.  If it succeeds, the vertices pointer
     * is set to a valid location for the vertices and it returns true.
     * This method assumes that vertices for the quads are specified in the shape of the letter Z
     * (top left, top right, bottom left, bottom right).
     */
    bool allocateQuadListGeometry(size_t* quads, GUIVertex** vertexBuffer);

    /**
     * Locks the vertex and index buffers so that the calling class can fill in index information about the
     * geometry.  This method returns a pointer to the internal indices as well as an index-offset.  The
     * index offset should be added to EACH of the indices in order to make sure that the vertices that are
     * written are referenced correctly.
     * If this method cannot allocate space for the requested number of triangles, nothing is allocated,
     * the number of available triangles/vertices is output and the method returns false.  If it
     * succeeds, 'true' is returned.
     */
    bool allocateGeometry(size_t* vertices, size_t* triangles, GUIVertex** vertexBuffer, GUIIndex** indexBuffer, GUIIndex* indexOffset);

    /**
     * Draws the geometry in this layer to the screen
     */
    bool flush();

    /**
     * This should be called if doing rendering from outside of the scope of a render() call
     * (i.e. externally to an element).  Causes the rendering procedure to reset the device's
     * state during flush()
     */
    void reinitializeD3DDeviceState();

    /**
     * Obtains the texture to use to render elements with the given ID
     */
    virtual GUITexture* getTexture(int id) = 0;


  protected:

    /**
     * Sets up the target D3D device for rendering.  This doesn't check or modify
     * the initialized device flag.
     */
    bool initializeD3DDevice();

    /**
     * Resets everything in this class
     */
    void zero();


  protected:

    /// The device to use to render the GUI
    LPDIRECT3DDEVICE9 d3d_device_;

    /// The rendering viewport for this layer
    D3DVIEWPORT9 myViewport;

    /// The rendering projection matrix for this layer
    D3DXMATRIX myProjectionMatrix;

    /// The canvas that takes up the whole renderable area
    GUICanvas myPrimaryCanvas;

    /// Stores the elements of the GUI so that they can be batch-rendered by texture
    TextureToGUIElementTable myElements;

    /// The number of vertices that the vertex buffer can hold
    size_t myVertexCapacity;

    /// The number of triangles that the index buffer can hold
    size_t myTriangleCapacity;

    /// This buffer is dynamic and is used to render the elements in this layer
    LPDIRECT3DVERTEXBUFFER9 myVB;

    /// This buffer is dynamic (like the VB) and is used to render the elements in this layer
    LPDIRECT3DINDEXBUFFER9 myIB;

    /// The number of vertices that have been written in the vertex buffer
    size_t myConsumedVertices;

    /// The number of triangles that have been written into the index buffer
    size_t myConsumedTriangles;

    /// The vertices that have been locked for the buffer.  This pointer is valid only
    /// after an allocate* method has been called, and is invalidated after a flush()
    GUIVertex* myLockedVertices;

    /// The indices that have been locked for the buffer.  This pointer is valid only
    /// after an allocate* method has been called, and is invalidated after a flush()
    GUIIndex* myLockedIndices;

    /// Whether or not the layer has initialized the device for this render() call.  This
    /// variable is used by flush() to determine whether or not the device needs to be
    /// initialized.
    bool myInitializedD3DDeviceFlag;
};



#endif