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
#include <d3dx9.h>
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guivertex.h"
#include "guiquad.h"



//-----------------------------------------------------------------------------
// Name: initialize
// Desc: Sets the depth and color of all 4 vertices
//-----------------------------------------------------------------------------
void GUIQuad::initialize(float depth, D3DCOLOR color)
{
  for (int i = 0; i < 4; ++i)
  {
    vertices[i].depth = depth;
    vertices[i].color = color;
  }
}

void GUIQuad::createRotated(float center_x, float center_y, float radius, float angle) {
  for (int r = 0; r < 2; ++r) {
  float base_angle = r * D3DX_PI;
  for (int c = 0; c < 2; ++c) {
    float vtx_angle = base_angle + D3DX_PI/4.0f + D3DX_PI/2.0f*c;
    float cos_angle = cosf(vtx_angle + angle);
    float sin_angle = sinf(vtx_angle + angle);
    GUIVertex* vertex = &vertices[r == 0 ? c : r*2 + 1 - c];
    vertex->x = center_x + cos_angle * radius;
    vertex->y = center_y + sin_angle * radius;
    vertex->u = (r^c) * 1.0f; // flip 'c' if r == 1
    vertex->v = r * 1.0f;
  }
  }
}



//-----------------------------------------------------------------------------
// Name: clipSquare
// Desc: Clips the vertices so that they don't go outside the bounds of the given rectangle.  If
//     the whole quad is outside of the rectangle, it is guaranteed to have at least one
//     zero dimension.
//-----------------------------------------------------------------------------
void GUIQuad::clipSquare(const GUIRect* visibleArea, float texcoordsPerPixelOfWidth, float texcoordsPerPixelOfHeight)
{
  if (!visibleArea) return;

  // Check the left vertices for left-clipping
  for (int i = 0; i < 4; i += 2)
  {
    if (vertices[i].x < visibleArea->left)
    {
      float newX = min(visibleArea->left, vertices[i+1].x);
      vertices[i].u += (newX - vertices[i].x) * texcoordsPerPixelOfWidth;
      vertices[i].x = newX;
    }
  }

  // Check the right vertices for right-clipping
  for (int i = 1; i < 4; i += 2)
  {
    if (vertices[i].x > visibleArea->right)
    {
      float newX = max(visibleArea->right, vertices[i-1].x);
      vertices[i].u += (newX - vertices[i].x) * texcoordsPerPixelOfWidth;
      vertices[i].x = newX;
    }
  }

  // Check the top vertices for top-clipping
  for (int i = 0; i < 2; i++)
  {
    if (vertices[i].y < visibleArea->top)
    {
      float newY = min(visibleArea->top, vertices[i+2].y);
      vertices[i].v += (newY - vertices[i].y) * texcoordsPerPixelOfHeight;
      vertices[i].y = newY;
    }
  }

  // Check the bottom vertices for bottom-clipping
  for (int i = 2; i < 4; i++)
  {
    if (vertices[i].y > visibleArea->bottom)
    {
      float newY = max(visibleArea->bottom, vertices[i-2].y);
      vertices[i].v += (newY - vertices[i].y) * texcoordsPerPixelOfHeight;
      vertices[i].y = newY;
    }
  }
}



//-----------------------------------------------------------------------------
// Name: copyTo
// Desc: Copies the vertices in this quad to the given pointer
//-----------------------------------------------------------------------------
GUIVertex* GUIQuad::copyTo(GUIVertex* vertexBuffer)
{
  memcpy(vertexBuffer, &vertices, sizeof(vertices));
  return vertexBuffer + 4;
}

