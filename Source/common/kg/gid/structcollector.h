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