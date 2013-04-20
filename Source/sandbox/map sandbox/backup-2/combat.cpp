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


class Actor {

  // should invoke terminate() on the projectile if it successfully collided
  virtual void checkCollision(Projectile* projectile);
  
  void applySpellToTarget(Actor* target)
}

namespace Evidyon {
typedef unsigned int Time;

namespace Magic {
namespace Server {

struct Magic {
  int mp_cost;
  bool root, freeze, face_target;
  bool can_be_canceled;

  enum Type {
    INVALID,    // Nothing to do in this stage

    WEAPON,       // apply the effect of the caster's weapon, plus damage of int_param
    HP,           // change target HP by int_param modified by target's magic defense
    HP_NOARMOR,   // change target HP by int_param, don't apply armor
    MP,           // change target MP by int_param
    TELEPORT,
    TELEPORT_SWAP,
    DISENCHANT,   // remove all enchantments
    ENCHANT_HP, // change target HP by int_param every [regenerate period]
    ENCHANT_MP, // change target MP by int_param every [regenerate period]
    ENCHANT_MUTATE, // change actor type to that specified in int_param
    ENCHANT_MUTATE_GASFORM,   // mutate to cloud, speed=float_param
    ENCHANT_MUTATE_COPY,    // copy the form of the targeted actor
    ENCHANT_ATTACK_RUN,     // run at the target with speed float_param
    ENCHANT_MAXHP,  // change target max HP by float_param percent of current
    ENCHANT_MAXMP,  // change target max MP by float_param percent of current
    ENCHANT_PHYSICAL_DAMAGE,  // add int_param to physical damage
    ENCHANT_PHYSICAL_DEFENSE, // add int_param to physical defense
    ENCHANT_MAGICAL_DEFENSE,  // add int_param to magical defense
    ENCHANT_MOVEMENT_SPEED, // multiply speed by float_param
    ENCHANT_ATTACK_SPEED,   // multiply speed by float_param

    // these abilities use geonite
    GEOSID_RECALL,      // move player back to their bound geosid (for now, just respawns)
    GEOSID_RESTOREHPMP, // 
  } type;

  union {
    struct {
      float damage_modifier;
      int added_damage;
    } weapon;

    struct {
      int change;
    } hp;

    struct {
    } mp;

    struct {
      Time delay;
    } teleport;
  };
};

struct ProjectileMagic {
  
};



}
}

namespace Spells {
namespace Server {


  
//----[  SerializedSpell  ]----------------------------------------------------
// Description of a spell that is stored in the game file.
struct SerializedSpell {
  int mp_cost;
  unsigned int serialized_magic_id;
};



//----[  Spell  ]--------------------------------------------------------------
struct Spell {
  int mp_cost;
  const Magic* magic;
};



}
}
}

namespace Evidyon {

typedef int ActorPointer;
typedef float Coordinate;

struct ActorActivity {
  enum Type {
    ACTIVITY_WALKING,             // moving to targeted location, turns to swimming in water
    ACTIVITY_SWIMMING,            // moving through water
    ACTIVITY_FLYING,              // this player is in gas-form
    ACTIVITY_ATTACK_RUN,          // running at a target location
    ACTIVITY_USE_ITEM,            // use the spell associated with the item
    ACTIVITY_CAST_SPELL,          // find the spell associated with magic and use it
    ACTIVITY_CASTING,             // casting a direct magic effect
    ACTIVITY_MELEE,               // started attack, waiting for delay to apply damage
    ACTIVITY_RANGED,              // 
    ACTIVITY_MAGIC_PROJECTILES,   // creating projectiles for a spell
    ACTIVITY_DEAD,                // actor is dead; can't do anything else
    ACTIVITY_NONE,                // idle
  } type;

  // Holds the data for this activity
  union {
    struct {
      Coordinate x, y;
    } walking;

    struct {
      Coordinate x, y;
    } swimming;

    struct {
      Coordinate x, y;
    } flying;

    struct {
      ActorPointer target;
    } attack_run;
    
    struct {
      int item_inventory_index;
      ActorPointer target;
      Coordinate x, y;
    } use_item;
    
    struct {
      const Spells::Server::Spell* spell;
      ActorPointer target;
      Coordinate x, y;
    } cast_spell;
    
    struct {
      Time next_allowed_action_time;
    } casting;
    
    struct {
      Time time_of_next_attack;
      ActorPointer target;
    } melee;
    
    struct {
      Time time_of_next_attack;
      ActorPointer target;
    } ranged;
    
    struct {
      int number_left_to_generate;
      const ProjectileMagicDescription* description;
    } magic_projectiles;

    struct {
      bool can_reanimate; // life-saving amulet effect
      Time time_of_respawn;
    } dead;
  };
};

}
