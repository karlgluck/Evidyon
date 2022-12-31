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
#include <dc/enum>
#include "shared/magic/magic_portal.h"
#include "common/null.h"


//----[  getString  ]----------------------------------------------------------
template<>
const char* dc::dcEnum<Evidyon::Magic::Magic_Portal::Type>
    ::getString(Evidyon::Magic::Magic_Portal::Type value) {
  using namespace Evidyon::Magic;
  switch (value) {
    case Magic_Portal::SELF_ONLY_MARK1: return "Self-Only to Mark 1";
    case Magic_Portal::SELF_ONLY: return "Self-Only";
    case Magic_Portal::OPEN_PORTAL_MARK1: return "Open Portal to Mark 1";
    case Magic_Portal::OPEN_PORTAL: return "Open Portal";
    case Magic_Portal::DIMENSION_DOOR: return "Dimension Door";
    case Magic_Portal::REVERSE_DIMENSION_DOOR: return "Reverse Dimension Door";
    case Magic_Portal::OPEN_PORTAL_TO_DESTINATION: return "Open Portal to Destination";
    case Magic_Portal::SELF_ONLY_TO_SPAWN_POINT: return "Self-Only to Spawn Point";
    default:  return NULL;
  }
}



//----[  staticTypeName  ]-----------------------------------------------------
std::string dc::dcEnum<Evidyon::Magic::Magic_Portal::Type>::staticTypeName()
{
    return "Enum-Magic-Magic_Portal-Type";
}
