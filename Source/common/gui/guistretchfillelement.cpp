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

