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