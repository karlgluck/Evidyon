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
#include "randf.h"
#include "randint32.h"

//----[  randf  ]--------------------------------------------------------------
float randf() {
  return randf32();
}



//----[  randf32  ]------------------------------------------------------------
float randf32() {
  return float(double(rand_uint32()) / double(0xFFFFFFFF));
}



//----[  randf64  ]------------------------------------------------------------
double randf64() {
  return double(rand_uint32()) / double(0xFFFFFFFF);
}


//----[  randfSymmetric  ]-----------------------------------------------------
float randfSymmetric() {
  return float((double(rand_uint32()) / double(0x7FFFFFFF)) - 1.0);
}



//----[  randfExclusive  ]-----------------------------------------------------
float randfExclusive() {
  return float(double(rand_uint32()) / double(0x100000000));
}



//----[  randfAngleRadians  ]--------------------------------------------------
float randfAngleRadians() {
  return randfExclusive() * 3.141592654f * 2.0f;
}