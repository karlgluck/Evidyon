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