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
#ifndef __GUIQUAD_H__
#define __GUIQUAD_H__
#pragma once


/**
 * Used to edit a quad of vertices
 */
struct GUIQuad
{
  /// The vertices of this quad.  They are arranged in the following pattern:
  ///
  ///   0----1
  ///   |  / |
  ///   | /  |
  ///   2----3
  GUIVertex vertices[4];

  /**
   * Sets the depth and color of all 4 vertices
   */
  void initialize(float depth, D3DCOLOR color);

  void createRotated(float center_x, float center_y, float radius, float angle);

  /**
   * Clips the vertices so that they don't go outside the bounds of the given rectangle.  If
   * the whole quad is outside of the rectangle, it is guaranteed to have at least one
   * zero dimension.
   */
  void clipSquare(const GUIRect* visibleArea, float texcoordsPerPixelOfWidth, float texcoordsPerPixelOfHeight);

  /**
   * Copies the vertices in this quad to the given pointer
   */
  GUIVertex* copyTo(GUIVertex* vertexBuffer);
};


#endif