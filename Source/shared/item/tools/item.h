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
#ifndef __SHARED_ITEM_TOOLS_ITEM_H__
#define __SHARED_ITEM_TOOLS_ITEM_H__
#pragma once

#include <dc/enum>
#include <dc/string>
#include <dc/integer>
#include "shared/item/itemequipmenttype.h"
#include "shared/scenery/tools/scenery.h"
#include "shared/actor/tools/actorformreference.h"
#include "shared/avatar/avatargender.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"
#include "shared/projectile/tools/projectile.h"
#include "itemintendeduse.h"
#include "itemintendeduser.h"
#include "shared/magic/tools/magic.h"
#include "shared/avatar/avatarattribute.h"
#include "shared/treasure/tools/treasureweightvalue.h"
#include "shared/treasure/treasuregroup.h"
#include <map>


namespace Evidyon {
namespace Item {
struct ItemClientDescription;
struct ItemServerDescription;
}
namespace Magic {
namespace Tools {
class MagicCompiler;
}
}
namespace Treasure {
namespace Tools {
class TreasureCompiler;
}
}
namespace Avatar {
namespace Tools {
class AvatarRaceList;
}
}
}

namespace Evidyon {
namespace Item {
namespace Tools {


  
//----[  Item  ]---------------------------------------------------------------
class Item : public dc::dcResource<Item> {
public:
  static std::string staticTypeName() { return "Item"; }
public:
  Item();

  // treasure compiler - this item will be added as an epic if its "intended use"
  //  field is set as such
  // magic compiler - if this item is consumable or is a projectile weapon, the
  //  effect will be added
  void compile(Evidyon::Treasure::Tools::TreasureCompiler* treasure_compiler,
               Evidyon::Magic::Tools::MagicCompiler* magic_compiler,
               Evidyon::Item::ItemClientDescription* client_description,
               Evidyon::Item::ItemServerDescription* server_description);

  void setScenery(Evidyon::Tools::SceneryElement* scenery);
  bool hasScenery() const;

  // sorts items by equipment slot
  int compareTo(const dc::dcGenericResource* other) const;

  // Obtains the settings that allow this item to be categorized as a specific
  // kind of treasure.
  void getTreasureSettings(ItemIntendedUser* intended_user,
                           ItemIntendedUse* intended_use,
                           ItemEquipmentType* equipment_type) const;

  // Returns the weight to assign to this treasure if the treasure is assigned
  // to the given monster.
  Treasure::Tools::TreasureWeightValue getTreasureDropWeight(int monster_level) const;

  // Returns 'false' if this item cannot be classified into one of the treasure
  // groups.  Epics are an example of an item that has no treasure group
  // classification.
  bool getTreasureGroup(Treasure::TreasureGroup* group) const;


///////////////////////////////////////////////////////////////////////////////
  size_t getGeoniteValue() const { return geonite_value_.getValue(); }
///////////////////////////////////////////////////////////////////////////////

  // used to configure this class based on values in the file imported from the
  // master design document.
  void set(std::map<std::string,std::string>* values,
           Avatar::Tools::AvatarRaceList* avatar_races);

private:
  dc::dcString name_;
  dc::dcEnum<ItemIntendedUse> intended_use_;
  dc::dcEnum<ItemIntendedUser> intended_user_;
  dc::dcInteger treasure_level_low_;
  dc::dcInteger treasure_level_high_;
  dc::dcEnum<ItemEquipmentType> equipment_type_;
  dc::dcString description_;
  Evidyon::Tools::SceneryReference scenery_;
  dc::dcInteger max_stack_size_;
  dc::dcInteger geonite_value_;
  dc::dcFloat weapon_range_; // how far from the target this weapon reaches
  dc::dcInteger weapon_attack_duration_;

  // What to do when this item is consumed
  Evidyon::Tools::MagicInterface consumable_magic_;
  dc::dcBoolean consuming_requires_target_, consuming_requires_slot_;

  // This amount is added to the % chance to not drop any items on death
  dc::dcInteger wishing_percent_;

  // This type of magic is used if the player dies with this item equipped.
  // item is consumed when this happens.
  Evidyon::Tools::MagicInterface on_death_magic_;

  // Can this item be dropped when the player dies?
  dc::dcBoolean drop_on_death_;

  // Requirements for use
  dc::dcInteger required_avatar_level_;
  dc::dcInteger required_avatar_attribute_[Avatar::NUMBER_OF_AVATAR_ATTRIBUTES];

  // Avatar modifiers
  dc::dcInteger min_attack_damage_;
  dc::dcInteger max_attack_damage_;
  dc::dcInteger physical_defense_;
  dc::dcInteger hp_regen_rate_;
  dc::dcInteger mp_regen_rate_;
  dc::dcFloat hp_modifier_;
  dc::dcFloat mp_modifier_;
  dc::dcInteger hp_added_;
  dc::dcInteger mp_added_;
  dc::dcFloat movement_speed_modifier_;
  dc::dcFloat attack_speed_modifier_;
  dc::dcFloat hp_steal_, mp_steal_;
  Evidyon::Actor::Tools::ActorFormReference
    actor_templates_[Avatar::NUMBER_OF_AVATAR_GENDERS]
                    [Avatar::NUMBER_OF_AVATAR_RACES]
                    [Avatar::NUMBER_OF_AVATAR_CLASSES];

  // Allow projectile weapons to specify the type of projectile they shoot
  Projectile::Tools::Projectile projectile_;
};



}
}
}



#include <dc/list>

namespace Evidyon {
namespace Tools {
typedef dc::dcList<Evidyon::Item::Tools::Item> ItemList;
typedef dc::dcList<Evidyon::Item::Tools::Item>::Element ItemElement;
typedef dc::dcList<Evidyon::Item::Tools::Item>::Reference ItemReference;
}
}


#endif