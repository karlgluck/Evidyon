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