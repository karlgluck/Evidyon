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
#ifndef __SPELL_H__
#define __SPELL_H__
#pragma once

#include <dc/string>
#include <dc/integer>
#include "magic.h"
#include <dc/table>
#include <dc/boolean>

namespace Evidyon {
struct ClientSpellDescription;
struct ServerSpellDescription;

// A spell lets magic be used by clients in the world
class Spell : public dc::dcResource<Spell> {
public:
  Spell();
  void compileForClient(ClientSpellDescription* description) const;
  void compileForServer(ServerSpellDescription* description) const;
  bool castingNeedsTarget() const;

public:
  static std::string staticTypeName();

private:
  dc::dcString description_; // Displayed to the player to let them know what this effect does
  dc::dcInteger mp_cost_;    // How much MP it takes to cast this spell
  dc::dcTable<Magic>::Reference magic_; // Magic this spell creates when cast
  dc::dcBoolean casting_needs_target_;
};

}

#endif