//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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
#include "../shared/actor/evidyon_actor.h"
#include <dc/enum>


template<> const char* dc::dcEnum<Evidyon::ActorAttack>::getString(
    Evidyon::ActorAttack value) {
  using namespace Evidyon;
  switch (value) {
    case Evidyon::ACTORATTACK_1: return "Attack 1";
    case Evidyon::ACTORATTACK_2: return "Attack 2";
    case Evidyon::ACTORATTACK_3: return "Attack 3";
    case Evidyon::ACTORATTACK_4: return "Attack 4";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::ActorAttack>::staticTypeName()
{
    return "Enum-ActorAttack";
}
