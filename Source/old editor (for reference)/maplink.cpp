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

