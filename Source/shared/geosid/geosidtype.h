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
#ifndef __SHARED_GEOSID_GEOSIDTYPE_H__
#define __SHARED_GEOSID_GEOSIDTYPE_H__
#pragma once

namespace Evidyon {
namespace Geosid {


//----[  GeosidType  ]---------------------------------------------------------
enum GeosidType {
  GEOSID_TOWN_WHITE,
  GEOSID_WILD_RED_LARGE,
  GEOSID_WILD_RED_MEDIUM,
  GEOSID_WILD_RED_SMALL,
  GEOSID_WILD_BLUE_LARGE,
  GEOSID_WILD_BLUE_MEDIUM,
  GEOSID_WILD_BLUE_SMALL,
  GEOSID_WILD_YELLOW_LARGE,
  GEOSID_WILD_YELLOW_MEDIUM,
  GEOSID_WILD_YELLOW_SMALL,
};





//----[  GeosidTypeString  ]---------------------------------------------------
const char* GeosidTypeString(GeosidType value);


float GeosidHPBonus(GeosidType value);
float GeosidMPBonus(GeosidType value);
float GeosidXPBonus(GeosidType value);



}
}

#endif