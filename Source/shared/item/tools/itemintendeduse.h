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
#ifndef __SHARED_ITEM_TOOLS_ITEMINTENDEDUSE_H__
#define __SHARED_ITEM_TOOLS_ITEMINTENDEDUSE_H__
#pragma once

namespace Evidyon {
namespace Item {
namespace Tools {



  
//----[  ItemIntendedUse  ]----------------------------------------------------
enum ItemIntendedUse {
  INTENDEDUSE_EQUIPMENT_COMMON,
  INTENDEDUSE_EQUIPMENT_UNCOMMON,
  INTENDEDUSE_EQUIPMENT_RARE,
  INTENDEDUSE_EPIC,
  INTENDEDUSE_CONSUMABLE_SELF,
  INTENDEDUSE_CONSUMABLE_OTHER,
  INTENDEDUSE_CONSUMABLE_BUFF,
  INTENDEDUSE_CONSUMABLE_ATTACK,
  INTENDEDUSE_CONSUMABLE_DEFEND,
  INTENDEDUSE_CONSUMABLE_TRANSPORT,
  INTENDEDUSE_CONSUMABLE_RESTORE,
};

}
}
}

#endif