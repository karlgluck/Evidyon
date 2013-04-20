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
#ifndef __SHARED_AVATAR_AVATARRACE_H__
#define __SHARED_AVATAR_AVATARRACE_H__
#pragma once


namespace Evidyon {
namespace Avatar {


  
//----[  AVATAR_RACE_HAIR_STYLES_PER_GENDER  ]---------------------------------
static const int AVATAR_RACE_HAIR_STYLES_PER_GENDER = 3;



//----[  AvatarRace  ]---------------------------------------------------------
enum AvatarRace {
  AVATARRACE_GEONAN,
  AVATARRACE_LUCAN,
  AVATARRACE_DANTALION,
  AVATARRACE_VOLUCRIS,
  AVATARRACE_HUMAN,
  NUMBER_OF_AVATAR_RACES,
  INVALID_AVATAR_RACE,
};



//----[  AvatarRaceString  ]---------------------------------------------------
const char* AvatarRaceString(AvatarRace value);


//bool IsValidLevel1Avatar(AvatarRace race,
//                         AvatarAttributeValue attributes[NUMBER_OF_AVATAR_ATTRIBUTES]);


}
}

#endif