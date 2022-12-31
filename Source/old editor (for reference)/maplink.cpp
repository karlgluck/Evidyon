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
#include "maplink.h"
#include "../shared/server-editor/maplocationspecialfunction.h"


dc::dcClass<MapLink>::Implements<MapLocationSpecialFunction> MapLink::implementsSpecialFunction;

MapLink::MapLink() {
  member("Source Location", &source_location_);
  member("Destination Map", &dest_map_);
  member("Dest Location", &dest_location_);
}

bool MapLink::compileForServer(Evidyon::CompiledMapLocationSpecialFunction* description) {
  description->type = Evidyon::MAP_LINK;
  description->x = source_location_.getX();
  description->y = source_location_.getY();
  description->map_link.x = dest_location_.getX()+0.5f;
  description->map_link.y = dest_location_.getY()+0.5f;
  description->map_link.map_index = dest_map_.getReferencedResourceIndex();
  return true;
}


std::string MapLink::staticTypeName() {
  return "MapLink";
}

