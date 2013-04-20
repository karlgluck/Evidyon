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
#ifndef __MAGICDESCRIPTION_H__
#define __MAGICDESCRIPTION_H__
#pragma once



#include "shared/actor/actoraction.h"

namespace Evidyon {

// When loading this structure, it is ABSOLUTELY ESSENTIAL that the program
// check to make sure that targets <= MagicTargeterTimer::MAX_TARGETS
struct MagicDescriptionStageTargeting {
  enum Type {
    SELF,
    PROJECTILE,     // ex. fireball
    ANY_TARGET_ACTOR,     // ex. burn target
    NONCASTER_TARGET_ACTOR,
    AOE_ACTORBOUND_CASTER,  // ex. buff group (targets self)
    AOE_ACTORBOUND_CASTER_OFFENSIVE,  // ex. evil beam, doesn't affect caster
    AOE_ACTORBOUND_OTHER_OFFENSIVE, // only hits enemies, but will always hit target
    AOE_LOCATIONBOUND_OFFENSIVE, // ex. explosion - finds only enemies
    PERIODIC_AOE_LOCATIONBOUND_OFFENSIVE, // ex. poison cloud; unlimited targets, doesn't create aoe visualization
    //CONE_ACTORBOUND_OFFENSIVE,  // ex. flame wind, always faces in actor's direction
    //SELF_ATTACKRUN
    //SELF_LEAP
  } type;

  struct Parameters {

    // SELF: nothing
    // PROJECTILE: number of projectiles to generate
    // TARGET_ACTOR: nothing
    // AOE_***: max actors to target
    // PERIODIC_AOE_***: number of times to apply AOE
    int targets;

    // SELF: time until applied
    // PROJECTILE: time between generations (stage 0 only)
    // TARGET_ACTOR: time until applied
    // AOE_***: time until applied
    // PERIODIC_AOE_***: delay between applications
    float delay;

    // SELF: nothing
    // PROJECTILE: half angle-width of the cone to generate projectiles in (rad)
    // TARGET_ACTOR: nothing
    // AOE_***: square-radius of the AOE
    // PERIODIC_AOE_***: square-radius of the AOE
    float spread;

  } parameters;
};

struct MagicDescriptionStageEffect {
  enum Type {
    INVALID,    // Nothing to do in this stage

    WEAPON,   // apply the effect of the caster's weapon, plus damage of int_param
    HP,       // change target HP by int_param modified by target's magic defense
    HP_NOARMOR, // change target HP by int_param, don't apply armor
    MP,       // change target MP by int_param
    TELEPORT, // jump to target location after float_param seconds
              // can also be used as a portal by making the targeter a periodic AOE
    TELEPORT_SWAP,  // change locations with target actor after float_param seconds
    DISENCHANT,   // remove all enchantments

    // All enchantments last for 'duration' seconds
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
    //GEOSID_RESURRECT,

    FIRST_ENCHANTMENT_INDEX = ENCHANT_HP,

  } type;

  // If this is an enchantment, this field references an index that defines
  // how it behaves.
  unsigned int enchantment_class_index;
  unsigned int enchantment_type_bits;
  unsigned int enchantment_inv_class_mask;

  // These are the base parameters for the effect.  A version adjusted for
  // character attributes is created by each ActorInstance when they cast a spell.
  struct Parameters {
    float duration;
    float float_param;
    int int_param;
  } parameters;
};

struct MagicDescriptionStageParameters {
  MagicDescriptionStageTargeting::Parameters targeting;
  MagicDescriptionStageEffect::Parameters effect;
};

// Describes the properties of a given stage of a magic spell
struct MagicDescriptionStage {
  MagicDescriptionStageTargeting targeting;
  MagicDescriptionStageEffect effect;
  int aoe_magic_fx_type;

  // For projectiles, this is a projectile type.  For other spells, this
  // is a magic FX type.
  int per_targeter_fx_type;
};

// Parameters for how a spell is cast
struct MagicDescriptionCastingBehavior {
  // true: freeze the source after casting, and don't allow
  //     voluntary moving or turning
  // false: prevent voluntary moving, but allow turning
  bool freeze;

  // Caster can't move for this amount of time.  Set to <0 if no delay
  // is desired.
  float movement_delay;

  // The caster must absolutely not do anything else until this amount of
  // time has elapsed after casting.  This is usually to account for some
  // kind time-duration generating effect, such as creating fireballs.
  float essential_action_delay;

  // Factored based on the caster's abilities, then added to the essential
  // action delay to create the total action delay.
  float balancing_action_delay;

  // The action the client should perform when using this effect
  Evidyon::Actor::ActorAction actor_action;
};


// Stored in the game configuration file.  Gives the overall magic description
// and the script for its progression (stages).
// Note that this is not the same thing as a spell:  a spell creates a magic
// instance, but magic can also be instantiated by other means (such as items)
struct MagicDescription {
  static const int NUM_STAGES = 3;

  // Created when casting occurs
  int magic_fx_type;

  // Defines how the power of this magic's stages are affected with regard to
  // the stats of the casting actor.
  enum PowerType {
    NO_BONUSES, // spell is not affected at any stage by anything.  ex: night vision
    GEONITE,    // spell derives its power from geonite
  } power_type;

  // Information about how the spell is cast
  MagicDescriptionCastingBehavior casting;

  // Describes the progression of this spell
  MagicDescriptionStage stages[NUM_STAGES];
};






}

#endif
