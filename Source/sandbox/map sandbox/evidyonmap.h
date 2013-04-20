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
#ifndef __EVIDYONMAP_H__
#define __EVIDYONMAP_H__


namespace Evidyon {
static const double CAMERA_ASPECT_RATIO = 55.0 / 49.0;
static const double CAMERA_TILT_RATIO = 1.0 / 2.0; // units up / units vertical

static const int VISIBLE_HORIZONTAL_EXTENT = 8;
static const int VISIBLE_VERTICAL_EXTENT = 8 + 3;
static const int VISIBLE_WIDTH = 2*VISIBLE_HORIZONTAL_EXTENT+1;
static const int VISIBLE_HEIGHT = 2*VISIBLE_VERTICAL_EXTENT+1;
static const int VISIBLE_TILES = VISIBLE_WIDTH * VISIBLE_HEIGHT;


}

#endif