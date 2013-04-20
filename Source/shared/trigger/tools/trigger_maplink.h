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
#ifndef __SHARED_TRIGGER_TOOLS_TRIGGER_MAPLINK_H__
#define __SHARED_TRIGGER_TOOLS_TRIGGER_MAPLINK_H__
#pragma once


#include "trigger.h"
#include "shared/world/tools/map.h"


namespace Evidyon {
namespace Trigger {
namespace Tools {



//----[  Trigger_MapLink  ]----------------------------------------------------
class Trigger_MapLink : public dc::dcResource<Trigger_MapLink,Trigger> {
public:
  static std::string staticTypeName() { return "Trigger_MapLink"; }
  static dc::dcClass<Trigger_MapLink>::Implements<Trigger> implements_trigger_;
public:
  Trigger_MapLink();
  virtual bool compile(Evidyon::Trigger::Trigger* map_link);
private:
  Evidyon::Tools::MapReference destination_map_;
  dc::dcInteger x_, y_;
};



}
}
}

#endif