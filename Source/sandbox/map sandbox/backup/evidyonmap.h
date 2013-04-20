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
#ifndef __EVIDYONMAP_H__
#define __EVIDYONMAP_H__


namespace Evidyon {
static const double CAMERA_ASPECT_RATIO = 55.0 / 49.0;
static const double CAMERA_TILT_RATIO = 1.0 / 2.0; // units up / units vertical

namespace Map {

// Limited by the number of tile-bits in the packed actor coordinates sent
// out by the server.  The max dimension is the largest number of tiles in
// either the horizontal or vertical direction.
static const int MAXIMUM_DIMENSION = 2048;


static const int VISIBLE_HORIZONTAL_EXTENT = 8;
static const int VISIBLE_VERTICAL_EXTENT = 8 + 3;
static const int CACHED_HEIGHT_HORIZONTAL_EXTENT = VISIBLE_HORIZONTAL_EXTENT + 1;
static const int CACHED_HEIGHT_VERTICAL_EXTENT = VISIBLE_VERTICAL_EXTENT + 1;
static const int WIDTH = 2*VISIBLE_HORIZONTAL_EXTENT + 1;
static const int HEIGHT = 2*VISIBLE_VERTICAL_EXTENT + 1;
static const int CACHED_HEIGHTS_WIDTH =
  (2*CACHED_HEIGHT_HORIZONTAL_EXTENT+1);
static const int CACHED_HEIGHTS_HEIGHT =
  (2*CACHED_HEIGHT_VERTICAL_EXTENT+1);
static const int CACHED_HEIGHTS =
  CACHED_HEIGHTS_WIDTH * CACHED_HEIGHTS_HEIGHT;


static const int VISIBLE_TILES = WIDTH * HEIGHT;
static const int VERTICES_PER_QUAD = 5;
static const int TRIANGLES_PER_QUAD = 4;


enum TileVisibility {

  // This tile can't be seen at all because it's in a room behind a wall.  The
  // player should be completely unaware that this tile is even present.
  TILEVISIBILITY_INVISIBLE,

  // This tile is behind something that occludes the direct view of the player.
  // These squares are colored slightly darker to show that there might be
  // something hiding there!
  TILEVISIBILITY_OCCLUDED,

  // This tile is completely visible.
  TILEVISIBILITY_NORMAL,
};


// Performs a modulus that is consistent across the negative number boundary 
#define MAP_TO_RANGE(v, r)  ((v + 10000 * r) % r)



}
}


#endif