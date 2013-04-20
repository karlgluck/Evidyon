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
#ifndef __COMMON_KG_GID_STR_H__
#define __COMMON_KG_GID_STR_H__
#pragma once


#include <algorithm>
#include <sstream>

inline std::string &rtrimbv(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}


inline std::string &ltrimbv(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(isspace))));
    return s;
}

inline std::string &trimbv(std::string &s) {
    return ltrimbv(rtrimbv(s));
}


inline void split_str(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while(std::getline(ss, item, delim)) {
      elems.push_back(trimbv(item));
  }
}

template <typename C1, typename C2>
inline void zip(const C1& keys,
                const C2& values,
                std::map<typename C1::value_type, typename C2::value_type>& output) {
  typename C1::const_iterator a = keys.begin();
  typename C2::const_iterator b = values.begin();
  for (;
       a != keys.end();
       ++a) {
    if (b != values.end()) {
      output.insert(std::pair<C1::value_type, C2::value_type>(*a, *b));
      ++b;
    } else {
      output.insert(std::pair<C1::value_type, C2::value_type>(*a, C2::value_type()));
    }
  }
}



#endif