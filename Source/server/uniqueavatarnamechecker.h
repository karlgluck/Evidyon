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