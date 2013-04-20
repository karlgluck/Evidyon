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
#include "avatarinstance.h"
#include "shared/design/design.h"
#include "common/randint32.h"
#include <assert.h>
#include "shared/design/newdesign.h"

namespace Evidyon {
namespace Server {

  
//----[  calculateBaseStats  ]-------------------------------------------------
void AvatarInstance::calculateBaseStats() {
  max_hp = Design::AvatarHP(race,
                            class_,
                            level,
                            attribute_values[Avatar::ATTRIBUTE_STRENGTH],
                            attribute_values[Avatar::ATTRIBUTE_CONSTITUTION]);
  max_mp = Design::AvatarMP(race,
                            class_,
                            level,
                            attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE],
                            attribute_values[Avatar::ATTRIBUTE_WISDOM]);
  guild_max_hp = max_hp;
  guild_max_mp = max_mp;
  movement_speed = Design::AvatarMovementSpeed(
    attribute_values[Avatar::ATTRIBUTE_AGILITY]);
  hp_regen = Design::AvatarHPRegen(
    race,
    class_,
    level,
    attribute_values[Avatar::ATTRIBUTE_STRENGTH],
    attribute_values[Avatar::ATTRIBUTE_CONSTITUTION]);
  mp_regen = Design::AvatarMPRegen(
    race,
    class_,
    level,
    attribute_values[Avatar::ATTRIBUTE_INTELLIGENCE],
    attribute_values[Avatar::ATTRIBUTE_WISDOM]);

}







}
}

