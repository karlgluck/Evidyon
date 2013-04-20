//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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
#include "guilayer.h"
#include "guitexture.h"
#include "guiborderedfillelement.h"
#include "guiquad.h"


//-----------------------------------------------------------------------------
// Name: GUIBorderedFillElement
// Desc: Initializes the class
//-----------------------------------------------------------------------------
GUIBorderedFillElement::GUIBorderedFillElement()
{
  myTexture = 0;
  myDiffuseColor = D3DCOLOR_XRGB(255,255,255);
  myMarginPixels = 0;
  ZeroMemory(myTexcoords, sizeof(myTexcoords));
  myTexcoordsPerPixelWidth = 0.0f;
  myTexcoordsPerPixelHeight = 0.0f;
}


//-----------------------------------------------------------------------------
// Name: setTexture
// Desc: Stores the texture for this element
//-----------------------------------------------------------------------------
void GUIBorderedFillElement::setTexture(int texture)
{
  myTexture = texture;
}

//-----------------------------------------------------------------------------
void GUIBorderedFillElement::setColor(D3DCOLOR color) {
  myDiffuseColor = color;
}



//-----------------------------------------------------------------------------
// Name: setRegion
// Desc: Calculates the texture coordinates for the the source area for this element using
//     the dimensions of the current texture.  This method will not function unless
//     the internal texture is currently valid.
//-----------------------------------------------------------------------------
void GUIBorderedFillElement::setSourceRegion(const GUIRect* sourceArea, int marginPixels, GUILayer* layer)
{
  GUITexture* gui_texture = layer->getTexture(myTexture);
  if (!gui_texture) return;
  setSourceRegion(sourceArea, marginPixels, gui_texture->getSize());
}



//-----------------------------------------------------------------------------
// Name: setRegion
// Desc: Calculates the texture coordinates for the the source area for this element
//-----------------------------------------------------------------------------
void GUIBorderedFillElement::setSourceRegion(const GUIRect* sourceArea, int marginPixels, GUISize textureDimensions)
{
  // If there is an invalid parameter, exit
  if (!sourceArea)
  {
    // Exit this method
    return;
  }

  // Get the dimensions of the texture
  float textureWidth  = (float)textureDimensions.width;
  float textureHeight = (float)textureDimensions.height;

  // Get the size of the texture
  {
    GUISize sourceSize = sourceArea->calculateSize();
    myTexcoordsPerPixelWidth = 1.0f / textureWidth;
    myTexcoordsPerPixelHeight = 1.0f / textureHeight;
  }

  // Calculate the texture coordinates
  for (int quadRow = 0; quadRow < 3; ++quadRow)
  {
    // Assign the texture coordinates for this row (top and bottom)
    float v1, v2;
    {
      // Find the row's top pixel coordinate
      int rowTop, rowBottom;
      switch(quadRow)
      {
             case 0: rowTop = sourceArea->top;
                 rowBottom = sourceArea->top + marginPixels;  break;
             case 1: rowTop = sourceArea->top + marginPixels;
                 rowBottom = sourceArea->bottom - marginPixels; break;
        default: case 2: rowTop = sourceArea->bottom - marginPixels;
                 rowBottom = sourceArea->bottom;        break;
      }

      // Calculate the texture coordinates from the pixel locations
      v1 = rowTop / textureHeight;
      v2 = rowBottom / textureHeight;
    }

    for (int quadCol = 0; quadCol < 3; ++quadCol)
    {
      // Assign the texture coordinates for this row (top and bottom)
      float u1, u2;
      {
        // Find the row's top pixel coordinate
        int colLeft, colRight;
        switch(quadCol)
        {
               case 0: colLeft  = sourceArea->left;
                   colRight = sourceArea->left + marginPixels;  break;
               case 1: colLeft  = sourceArea->left + marginPixels;
                   colRight = sourceArea->right - marginPixels; break;
          default: case 2: colLeft  = sourceArea->right - marginPixels;
                   colRight = sourceArea->right;        break;
        }

        // Calculate the texture coordinates from the pixel locations
        u1 = colLeft / textureWidth;
        u2 = colRight / textureWidth;
      }

      for (int vertexRow = 0; vertexRow < 2; ++vertexRow)
      {
        // Set the v coordinate
        float v = vertexRow == 0 ? v1 : v2;

        for (int vertexCol = 0; vertexCol < 2; ++vertexCol)
        {
          // Set the u coordinate
          float u = vertexCol == 0 ? u1 : u2;

          // Save this texcoord
          myTexcoords[quadRow][quadCol][vertexRow][vertexCol][0] = u;
          myTexcoords[quadRow][quadCol][vertexRow][vertexCol][1] = v;
        }
      }
    }
  }

  // Save the margin parameter
  myMarginPixels = marginPixels;
}



//-----------------------------------------------------------------------------
// Name: render
// Desc: Renders this text list into the given layer.
//-----------------------------------------------------------------------------
bool GUIBorderedFillElement::render(GUILayer* layer)
{
  // Get the destination area for this element
  if (!mySourceCanvas) return true;
  const GUIRect* destArea = mySourceCanvas->getArea();
  float depth = mySourceCanvas->getDepth();

  // Allocate space for this element
  size_t quads = 9;
  GUIVertex* vertices;
  if (!layer->allocateQuadListGeometry(&quads, &vertices))
  {
    // Try flushing the buffer, and reallocating
    quads = 9;
    if (!layer->flush() || !layer->allocateQuadListGeometry(&quads, &vertices))
      return false; // There isn't enough space in the layer for this element
  }

  // Get this element's clipping region
  const GUIRect* clippingArea = myClippingArea;

  // Local variable used to build vertices
  // TODO: replace vertices[4] with a GUIQuad which can do its own clipping
  //GUIVertex quadVertices[4];
  GUIQuad quad;
  float texcoordsPerPixelWidth = myTexcoordsPerPixelWidth;
  float texcoordsPerPixelHeight = myTexcoordsPerPixelHeight;

  // Initialize vertex colors and depths
  quad.initialize(depth, myDiffuseColor);

  // Calculate the texture coordinates
  int marginPixels = myMarginPixels;
  for (int quadRow = 0; quadRow < 3; ++quadRow)
  {
    // Find the row's top pixel coordinate
    float y1, y2;
    switch(quadRow)
    {
           case 0: y1 = destArea->top;
               y2 = destArea->top + marginPixels;  break;
           case 1: y1 = destArea->top + marginPixels;
               y2 = destArea->bottom - marginPixels; break;
      default: case 2: y1 = destArea->bottom - marginPixels;
               y2 = destArea->bottom;        break;
    }

    for (int quadCol = 0; quadCol < 3; ++quadCol)
    {
      // Find the row's top pixel coordinate
      float x1, x2;
      switch(quadCol)
      {
             case 0: x1 = destArea->left;
                 x2 = destArea->left + marginPixels;  break;
             case 1: x1 = destArea->left + marginPixels;
                 x2 = destArea->right - marginPixels; break;
        default: case 2: x1 = destArea->right - marginPixels;
                 x2 = destArea->right;        break;
      }

      // The index of the vertex that is being initialized
      int v = 0;

      // Set up the quad
      for (int vertexRow = 0; vertexRow < 2; ++vertexRow)
      {
        // Set the y coordinate
        float y = vertexRow == 0 ? y1 : y2;

        for (int vertexCol = 0; vertexCol < 2; ++vertexCol)
        {
          // Set the x coordinate
          float x = vertexCol == 0 ? x1 : x2;

          // Set the vertex
          quad.vertices[v].x = x;
          quad.vertices[v].y = y;
          quad.vertices[v].u = myTexcoords[quadRow][quadCol][vertexRow][vertexCol][0];
          quad.vertices[v].v = myTexcoords[quadRow][quadCol][vertexRow][vertexCol][1];
          v++;
        }
      }

      if (clippingArea)
        quad.clipSquare(clippingArea, texcoordsPerPixelWidth, texcoordsPerPixelHeight);

      // Copy the quad's vertices into the vertex buffer
      vertices = quad.copyTo(vertices);
    }
  }

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name: getTexture
// Desc: Gets the texture used by this fill element
//-----------------------------------------------------------------------------
int GUIBorderedFillElement::getTexture()
{
  return myTexture;
}

