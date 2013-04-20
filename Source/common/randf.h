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
// These methods return random values between [0, 1] or [-1, 1] for the
// symmetric methods.
#ifndef __RANDF_H__
#define __RANDF_H__
#pragma once





//----[  randf  ]--------------------------------------------------------------
float randf();



//----[  randf32  ]------------------------------------------------------------
float randf32();



//----[  randf64  ]------------------------------------------------------------
double randf64();



//----[  randfSymmetric  ]-----------------------------------------------------
// Returns a value in the range [-1, +1)
float randfSymmetric();



//----[  randfExclusive  ]-----------------------------------------------------
// Returns a value in the range [0, 1)
float randfExclusive();




//----[  randfAngleRadians  ]--------------------------------------------------
// Returns a value [0, 2*pi)
float randfAngleRadians();


#endif