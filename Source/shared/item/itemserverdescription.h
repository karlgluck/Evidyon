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
#ifndef __SHARED_ITEM_ITEMSERVERDESCRIPTION_H__
#define __SHARED_ITEM_ITEMSERVERDESCRIPTION_H__
#pragma once


#include "shared/actor/actorequipmentslot.h"
#include "shared/scenery/sceneryindex.h"
#include "itemindex.h"
#include "itemhash.h"
#include "shared/actor/actorprofileindex.h"
#include "shared/skinned-mesh/skinnedmeshformindex.h"
#include "itemequipmenttype.h"
#include "shared/avatar/avatargender.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarattribute.h"
#include "shared/projectile/projectile.h"
#include "common/number.h"
#include "shared/magic/magicindex.h"
#include "shared/geosid/geonite.h"

namespace Evidyon {
namespace Item {



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//----[  ItemServerDescription  ]----------------------------------------------
struct ItemServerDescription {

  // necessary for logging purposes; this is the non-display ("true") name that
  // generates the unique permanent storage hash
  static const int NAME_LENGTH = 128;
  char true_name[NAME_LENGTH];

  // this is the index of this item in the item list
  ItemIndex type_index;

  // This is a unique value that is deterministically generated for each item
  // based on its name as defined in the game file.  Since items can change
  // order between compilations of the game file, when an avatar is saved to
  // the server's database, all of its items are typed using this hash value.
  // When the avatar is retrieved, the item manager looks up this value to
  // find the item index.
  ItemHash permanent_storage_hash;

  // The slots that this item can be placed into on the avatar
  // Note: the actor profile specialization and the slot where this item is
  //       attached to the actor are defined by this value.
  ItemEquipmentType equipment_type;

  // If this is a weapon, 
  struct {
    // this value specifies the range at which an attack
    // with it equipped should be allowed to hit.
    Number range;
    // this value indicates how long a complete attack should last.  the
    // time until the weapon hits is specified by the design (should be about
    // 25% of this value, the other 75% is waiting between attacks)
    Time::Milliseconds attack_duration;
  } weapon;

  // Defines values that must be met in order to get this item's full
  // effectiveness.
  struct {
    Avatar::AvatarLevel level;
    int attribute_value[Avatar::NUMBER_OF_AVATAR_ATTRIBUTES];
  } requirements;

  // These fields define what happens to the actor when the item is equipped.
  struct {
    Evidyon::Scenery::SceneryIndex equipped_scenery;

    // A table indexed by gender, race and class that defines which actor
    // profile and skinned mesh form to change to when this is equipped.
    struct {
      Evidyon::Actor::ActorProfileIndex actor_profile;
      Evidyon::SkinnedMesh::SkinnedMeshFormIndex skinned_mesh_form;
    } actor_templates[Avatar::NUMBER_OF_AVATAR_GENDERS]
                     [Avatar::NUMBER_OF_AVATAR_RACES]
                     [Avatar::NUMBER_OF_AVATAR_CLASSES];
  } equipped_actor_modifiers;

  // These fields define how the avatar is affected by equipping the item.  All
  // entries are applied.
  struct {
    int min_attack_damage, max_attack_damage;
    int hp_regen_rate;
    int mp_regen_rate;
    float max_hp_multiplier, max_mp_multiplier;
    int max_hp_added, max_mp_added;
    float movement_speed_multiplier, attack_delay_multiplier;
    int physical_defense;
    int spells_to_make_available[2]; // TODO
    float hp_steal, mp_steal; // TODO
  } equipped_avatar_modifiers;

  // How many units of this item can be stored on top of one another
  int max_stack_size;

  // This amount is added to the % chance to not drop any items on death
  int wishing_percent;

  // The magic type to activate when the user consumes this item.  If this is 
  // INVALID_MAGIC_INDEX, this item cannot be consumed.
  Magic::MagicIndex consumable_magic;

  // Used when the player dies with this item equipped; consumes the item.
  Magic::MagicIndex on_death_magic;

  // Can this item be dropped from the player when they die?  Almost all items
  // are droppable; non-drop items are rare.
  bool drop_on_death;

  // When showing this item on the ground, this is the value passed to the
  // client in the network message.  Maximum of 16 bits.
  Evidyon::Scenery::SceneryIndex ground_scenery;

  // How much geonite this item is worth when sacrificed
  Geosid::Geonite geonite_value;

  // If this is a projectile weapon, this data member is filled with the kind
  // of projectiles that it shoots.
  Evidyon::Projectile::Projectile projectile;
};
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX



}
}

#endif