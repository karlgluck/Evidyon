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
#include "uniqueavatarnamechecker.h"
#include "shared/avatar/evidyon_avatar.h"

namespace Evidyon {

bool UniqueAvatarNameChecker::addIfAvailable(const std::string& name) {
  std::string reduced_name;
  reduceName(name, &reduced_name);
  if (reduced_names_.find(reduced_name) == reduced_names_.end()) {
    reduced_names_.insert(reduced_name);
    return true;
  }
  return false;
}

void UniqueAvatarNameChecker::addName(const std::string& name) {
  std::string reduced_name;
  reduceName(name, &reduced_name);
  reduced_names_.insert(reduced_name);
}

bool UniqueAvatarNameChecker::isNameAvailable(const std::string& name) const {
  std::string reduced_name;
  reduceName(name, &reduced_name);
  return reduced_names_.find(reduced_name) == reduced_names_.end();
}

void UniqueAvatarNameChecker::reduceName(const std::string& name, std::string* reduced_name) {
  static const int LONGEST_NAME = 256;
  const char* source_name = name.c_str();
  int sentinel = 0;
  char current_character = '\0';
  while ((current_character = *(source_name++)) != '\0' &&
         (sentinel++) < LONGEST_NAME) {
    if ((current_character >= 'A' && current_character <= 'Z')) {
      current_character = current_character + 'a' - 'A'; // move to uppercase
    }
    if (current_character >= 'a' && current_character <= 'z') {
      reduced_name->append(1, current_character);
    }
  }
}


}