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
#ifndef __UNIQUEAVATARNAMECHECKER_H__
#define __UNIQUEAVATARNAMECHECKER_H__
#pragma once



#include <string>
#include <set>


namespace Evidyon {


  
//----[  UniqueAvatarNameChecker  ]--------------------------------------------
// Ensures that avatar names are unique.  This removes a bunch of formatting
// and special characters from an avatar's name so that KARL is equivalent to
// kArL, and both are equivalent to "k   a.rL".
class UniqueAvatarNameChecker {
public:
  // returns 'true' if the name was unique and could be added
  bool addIfAvailable(const std::string& name);
  void addName(const std::string& name);
  bool isNameAvailable(const std::string& name) const;

public:

  // translates a name into a standardized form that can be
  // compared with other names for uniqueness
  static void reduceName(const std::string& name, std::string* reduced_name);

private:
  std::set<std::string> reduced_names_;
};

}

#endif