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
#include "geosidtype.h"
#include "common/null.h"

namespace Evidyon {
namespace Geosid {

//----[  GeosidTypeString  ]---------------------------------------------------
const char* GeosidTypeString(GeosidType value) {
  switch (value) {
  default: return NULL;
  case GEOSID_TOWN_WHITE: return "Town - White";
  case GEOSID_WILD_RED_LARGE: return "Wild - Red Large";
  case GEOSID_WILD_RED_MEDIUM: return "Wild - Red Medium";
  case GEOSID_WILD_RED_SMALL: return "Wild - Red Small";
  case GEOSID_WILD_BLUE_LARGE: return "Wild - Blue Large";
  case GEOSID_WILD_BLUE_MEDIUM: return "Wild - Blue Medium";
  case GEOSID_WILD_BLUE_SMALL: return "Wild - Blue Small";
  case GEOSID_WILD_YELLOW_LARGE: return "Wild - Yellow Large";
  case GEOSID_WILD_YELLOW_MEDIUM: return "Wild - Yellow Medium";
  case GEOSID_WILD_YELLOW_SMALL: return "Wild - Yellow Small";
  }
}




//----[  GeosidHPBonus  ]------------------------------------------------------
float GeosidHPBonus(GeosidType value) {
  switch (value) {
  default: return 0.0;
  case GEOSID_WILD_RED_LARGE:  return 0.20;
  case GEOSID_WILD_RED_MEDIUM: return 0.10;
  case GEOSID_WILD_RED_SMALL:  return 0.05;
  }
}



//----[  GeosidMPBonus  ]------------------------------------------------------
float GeosidMPBonus(GeosidType value) {
  switch (value) {
  default: return 0.0;
  case GEOSID_WILD_BLUE_LARGE:  return 0.20;
  case GEOSID_WILD_BLUE_MEDIUM: return 0.10;
  case GEOSID_WILD_BLUE_SMALL:  return 0.05;
  }
}



//----[  GeosidXPBonus  ]------------------------------------------------------
float GeosidXPBonus(GeosidType value) {
  switch (value) {
  default: return 0.0;
  case GEOSID_WILD_YELLOW_LARGE:  return 0.20;
  case GEOSID_WILD_YELLOW_MEDIUM: return 0.10;
  case GEOSID_WILD_YELLOW_SMALL:  return 0.05;
  }
}



}
}

