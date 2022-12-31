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
#include "guifont.h"
#include "guitextelement.h"
#include "guimouse.h"
#include "guikeyboard.h"
#include "guicomponent.h"
#include "guiquad.h"
#include "guistretchfillelement.h"
#include "guitexture.h"


//-----------------------------------------------------------------------------
GUIStretchFillElement::GUIStretchFillElement() {
  texture_ = NULL;
  ZeroMemory(texcoords_, sizeof(texcoords_));
  diffuse_ = D3DCOLOR_XRGB(255,255,255); // TODO: unused
}

//-----------------------------------------------------------------------------
void GUIStretchFillElement::setTexture(int texture) {
  texture_ = texture;
}


//-----------------------------------------------------------------------------
void GUIStretchFillElement::setSourceTextureRegion(const GUIRect* sourceArea, int texture, GUILayer* layer) {
  GUITexture* gui_texture = layer->getTexture(texture);
  setTexture(texture);
  if (!gui_texture) return;
  setSourceRegion(sourceArea, gui_texture->getSize());
}


//-----------------------------------------------------------------------------
void GUIStretchFillElement::setSourceRegion(const GUIRect* sourceArea, GUISize textureDimensions) {
  // Save the dimensions of the texture
  float textureWidth  = (float)textureDimensions.width;
  float textureHeight = (float)textureDimensions.height;

  // Calculate the texture coordinates from the pixel locations
  float u1 = sourceArea->left / textureWidth;
  float u2 = sourceArea->right / textureWidth;

  // Calculate the texture coordinates from the pixel locations
  float v1 = sourceArea->top / textureHeight;
  float v2 = sourceArea->bottom / textureHeight;

  // Write each of the vertices
  for (int vertexRow = 0; vertexRow < 2; ++vertexRow)
  {
    // Set the v coordinate
    float v = vertexRow == 0 ? v1 : v2;

    for (int vertexCol = 0; vertexCol < 2; ++vertexCol)
    {
      // Set the u coordinate
      float u = vertexCol == 0 ? u1 : u2;

      // Save this texcoord
      texcoords_[vertexRow][vertexCol][0] = u;
      texcoords_[vertexRow][vertexCol][1] = v;
    }
  }
}



//-----------------------------------------------------------------------------
bool GUIStretchFillElement::render(GUILayer* layer) {
  // Only render if the source canvas is valid
  if (!mySourceCanvas) return true;


  // Allocate space for this element
  size_t quads = 1;
  GUIVertex* vertices;
  if (!layer->allocateQuadListGeometry(&quads, &vertices))
  {
    // Try flushing the buffer, and reallocating
    quads = 1;
    if (!layer->flush() || !layer->allocateQuadListGeometry(&quads, &vertices))
      return false; // There isn't enough space in the layer for this element
  }

  GUIQuad quad;
  quad.initialize(mySourceCanvas->getDepth(),  // target depth
           diffuse_);          // color

  // The vertex index being initialized
  int v = 0;
  for (int vertexRow = 0; vertexRow < 2; ++vertexRow)
  {
    // Set the y coordinate
    float yc = vertexRow == 0 ? mySourceCanvas->getArea()->top : mySourceCanvas->getArea()->bottom;

    for (int vertexCol = 0; vertexCol < 2; ++vertexCol)
    {
      // Set the x coordinate
      float xc = vertexCol == 0 ? mySourceCanvas->getArea()->left : mySourceCanvas->getArea()->right;

      // Set the vertex
      quad.vertices[v].x = xc;
      quad.vertices[v].y = yc;
      quad.vertices[v].u = texcoords_[vertexRow][vertexCol][0];
      quad.vertices[v].v = texcoords_[vertexRow][vertexCol][1];
      ++v;
    }
  }

  // Clip this picture
  if (myClippingArea)
    quad.clipSquare(myClippingArea, (texcoords_[0][1][0] - texcoords_[0][0][0]) / mySourceCanvas->getArea()->calculateSize().width,
                   (texcoords_[1][0][1] - texcoords_[0][0][1]) / mySourceCanvas->getArea()->calculateSize().height);

  // Copy the quad
  quad.copyTo(vertices);

  // Success
  return true;
}

//-----------------------------------------------------------------------------
int GUIStretchFillElement::getTexture() {
  return texture_;
}

