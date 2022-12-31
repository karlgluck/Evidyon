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
#ifndef __RANDF_H__
#define __RANDF_H__
#pragma once


// Generates a random number [0.0, 1.0)
float randf();

// Generates a random number [0.0, f)
float randf(float max);

// Generates a random number [-f, f]
float randfSymmetric(float magnitude);

// Generates a random number [0, 2 pi)
float randfAngleRadians();

template <const size_t Entries> class RandSymmetricFloatTable {
public:
  void randomize() {
  for (int i = 0; i < Entries; ++i) {
    values_[i] = randfSymmetric(1.0f);
  }
  last_returned_ = 0;
  }

  // Gets the next entry in the float table.  This is theoretically much faster
  // than generating a truly random number each time a random value is needed,
  // and can be safely used in a variety of situations.
  inline float next() {
  last_returned_ = (last_returned_ + 1)%Entries;
  return values_[last_returned_];
  }

private:
  int last_returned_;
  float values_[Entries];
};


#endif