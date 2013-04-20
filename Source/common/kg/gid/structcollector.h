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
#ifndef __COMMON_KG_GID_STRUCTCOLLECTOR_H__
#define __COMMON_KG_GID_STRUCTCOLLECTOR_H__
#pragma once

#include "../kg.h"
#include <vector>

namespace kg {
namespace gid {


  
//----[  StructCollector  ]----------------------------------------------------
template <typename T> struct StructCollector {
  typedef std::vector<T*> Objects;

  ~StructCollector() {
    clear();
  }

  size_t add(const T& t) {
    T* i = new T;
    memcpy(i, &t, sizeof(T));
    objects.push_back(i);
    return objects.size() - 1;
  }

  size_t add(const T& t, T** added) {
    assert(added);
    T* i = new T;
    memcpy(i, &t, sizeof(T));
    objects.push_back(i);
    *added = i;
    return objects.size() - 1;
  }

  void clear() {
    for (Objects::iterator i = objects.begin(); i != objects.end(); ++i) {
      delete[] *i;
    }
    objects.clear();
  }

  void copyOut(size_t index, T* out) {
    assert(index < objects.size());
    memcpy(out, objects.at(index), sizeof(T));
  }

  Objects objects;
};

}
}

#endif