//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#ifndef __SHARED_MAGIC_TOOLS_MAGICCOMPILER_H__
#define __SHARED_MAGIC_TOOLS_MAGICCOMPILER_H__
#pragma once

#include <vector>
#include "shared/magic/magicindex.h"
#include "shared/magic/enchantmenttype.h"
#include "shared/magic/enchantmenttypeindex.h"
#include "shared/event/eventindex.h"

namespace Evidyon {
namespace Magic {

struct Magic;
struct Magic_AOE;
struct Magic_Buff;
struct Magic_SpinAttack;
struct Magic_AttackRun;
struct Magic_Leap;
struct Magic_AOEBuff;
struct Magic_DirectDamage;
struct Magic_DOT;
struct Magic_DOTAOEBuff;
struct Magic_Projectile;
struct Magic_Branching;
struct Magic_Portal;
struct Magic_Teleport;
struct Magic_Mutate;
struct Magic_Summon;
struct Magic_Trap_DimensionDoor;
struct Magic_Trap_BlackHole;
struct Magic_Disenchant;
struct Magic_Invisibility;
struct Magic_Heal;
struct Magic_Enchantment_Armor;
struct Magic_Enchantment_Shield;
struct Magic_Enchantment_ReactiveHealing;
struct Magic_Enchantment_Thorns;

}
namespace Event {
namespace Tools {
class EventCompiler;
}
}
}

namespace Evidyon {
namespace Magic {
namespace Tools {

  
//----[  MagicCompiler  ]------------------------------------------------------
class MagicCompiler {
  typedef std::vector<Evidyon::Magic::Magic*> MagicList;
  typedef std::vector<Evidyon::Event::EventIndex> EventIndexList;
public:
  MagicCompiler(Event::Tools::EventCompiler* event_compiler);
  ~MagicCompiler();
  void clear();

  MagicIndex add(const Evidyon::Magic::Magic* description);

  MagicIndex add(const Evidyon::Magic::Magic_AttackRun* description);
  MagicIndex add(const Evidyon::Magic::Magic_AOE* description);
  MagicIndex add(const Evidyon::Magic::Magic_AOEBuff* description);
  MagicIndex add(const Evidyon::Magic::Magic_Branching* description);
  MagicIndex add(const Evidyon::Magic::Magic_Buff* description);
  MagicIndex add(const Evidyon::Magic::Magic_DirectDamage* description);
  MagicIndex add(const Evidyon::Magic::Magic_Disenchant* description);
  MagicIndex add(const Evidyon::Magic::Magic_DOT* description);
  MagicIndex add(const Evidyon::Magic::Magic_DOTAOEBuff* description);
  MagicIndex add(const Evidyon::Magic::Magic_Invisibility* description);
  MagicIndex add(const Evidyon::Magic::Magic_Leap* description);
  MagicIndex add(const Evidyon::Magic::Magic_Mutate* description);
  MagicIndex add(const Evidyon::Magic::Magic_Projectile* description);
  MagicIndex add(const Evidyon::Magic::Magic_Portal* description);
  MagicIndex add(const Evidyon::Magic::Magic_SpinAttack* description);
  MagicIndex add(const Evidyon::Magic::Magic_Summon* description);
  MagicIndex add(const Evidyon::Magic::Magic_Teleport* description);
  MagicIndex add(const Evidyon::Magic::Magic_Trap_DimensionDoor* description);
  MagicIndex add(const Evidyon::Magic::Magic_Trap_BlackHole* description);
  MagicIndex add(const Evidyon::Magic::Magic_Heal* description);

  // Enchantments have their index types generated automatically, but are
  // special in that the client needs to know the event type (not the
  // server!)
  MagicIndex add(Event::EventIndex event_index, const Evidyon::Magic::Magic_Enchantment_Armor* description, int index); // index = 0, 1, 2
  MagicIndex add(Event::EventIndex event_index, const Evidyon::Magic::Magic_Enchantment_Shield* description);
  MagicIndex add(Event::EventIndex event_index, const Evidyon::Magic::Magic_Enchantment_ReactiveHealing* description);
  MagicIndex add(Event::EventIndex event_index, const Evidyon::Magic::Magic_Enchantment_Thorns* description);

  Event::Tools::EventCompiler* eventCompiler();
  
  size_t numberOfMagics() const;
  void compileMagic(size_t index, Evidyon::Magic::Magic* magic);

  size_t numberOfEnchantmentsOfType(EnchantmentType type);
  void compileEnchantmentTypeEvents(EnchantmentType type,
                                    Event::EventIndex* event_indices);

private:
  MagicList magics_;
  Event::Tools::EventCompiler* event_compiler_;
  EventIndexList enchantment_events_[NUMBER_OF_ENCHANTMENT_TYPES];
};

}
}
}

#endif