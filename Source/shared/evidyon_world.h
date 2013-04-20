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
#ifndef __SHARED_EVIDYON_WORLD_H__
#define __SHARED_EVIDYON_WORLD_H__
#pragma once



namespace Evidyon {

namespace World {

typedef unsigned long RegionID;
const static int REGION_SIZE = 16;
const static int REGION_BORDER_SIZE = 5;
const static float REGION_RADIUS = REGION_SIZE / 2.0f + REGION_BORDER_SIZE;
const static float REGION_RADIUS_SQ = REGION_RADIUS*REGION_RADIUS;


RegionID regionIDFromPoint(float x, float y);
RegionID regionIDFromPoint(float x, float y, short* region_x, short* region_y);
RegionID regionIDFromPoint(int x, int y);
RegionID regionIDFromPoint(int x, int y, short* region_x, short* region_y);
RegionID regionIDFromRegionCoordinates(short region_x, short region_y);

}

}

#endif