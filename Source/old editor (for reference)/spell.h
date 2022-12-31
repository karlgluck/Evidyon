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