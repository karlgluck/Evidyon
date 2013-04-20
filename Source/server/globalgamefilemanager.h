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
#ifndef __GLOBALGAMEFILEMANAGER_H__
#define __GLOBALGAMEFILEMANAGER_H__
#pragma once


#include "shared/actor/evidyon_actor.h"
#include "shared/avatar/evidyon_avatar.h"

#include "shared/gamefile/servergamefileserverdata.h"
#include "shared/avatar/avatarraceserverdescription.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclassserverdescription.h"
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatarleveldescription.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarexperience.h"
#include "shared/magic/magicindex.h"
#include "shared/magic/enchantmenttype.h"
#include "shared/magic/enchantmenttypeindex.h"

// for the SpellDescription...
#include "shared/spell/spellserverdescription.h"

namespace Evidyon {

struct ServerEffectDescription;
struct AvatarRaceDescription;
struct AvatarClassDescription;
enum MapNavigability;
class GlobalWorldManager;
class GlobalActorManager;
class GlobalItemManager;
class UniqueAvatarNameChecker;

namespace Server {
class GlobalLifeformAIManager;
class GlobalTreasureManager;
class GlobalGeosidManager;
}
namespace Quests {
namespace Server {
class GlobalQuestManager;
}
}
namespace Magic {
struct Magic;
}



//----[  SpellDescription  ]---------------------------------------------------
struct SpellDescription {
  const Evidyon::Magic::Magic* magic[Spell::SpellServerDescription::MAXIMUM_STAGES];
  Spell::SpellServerDescription description;
};

struct ServerProjectileDescription;
struct AvatarAttributes;
class ActorInstance;
class UniqueAvatarNameChecker;




//----[  GlobalGamefileManager  ]----------------------------------------------
class GlobalGamefileManager {
public:
  static GlobalGamefileManager* singleton();


public:
  bool create(const char* source_file,
              UniqueAvatarNameChecker* unique_avatar_name_checker,
              GlobalItemManager* item_manager,
              GlobalActorManager* actor_manager,
              GlobalWorldManager* world_manager,
              Server::GlobalLifeformAIManager* lifeform_ai_manager,
              Quests::Server::GlobalQuestManager* quest_manager,
              Server::GlobalTreasureManager* treasure_manager,
              Server::GlobalGeosidManager* geosid_manager);
  void destroy();

  ActorTypeIndex getCharacterActorTypeIndex(int race_index, int gender_index);
  void getPlayerRespawnLocation(
    bool is_evil,
    Evidyon::World::MapIndex* map,
    float* x,
    float* y) const;


  // Determines whether or not the character specified by the given values is
   // valid for a newly-created level 1 character
  bool isValidLevel1Character(Avatar::AvatarRace race_index, Avatar::AvatarClass class_index, int attributes[Avatar::NUMBER_OF_AVATAR_ATTRIBUTES]);

  Avatar::AvatarExperience getLevelUpExperience(int level) const;
  Avatar::AvatarExperience getExperienceToAchieveLevel(int level) const;

  float getProjectileDuration(int projectile_index) const;
  const SpellDescription* getSpell(int spell_index) const;
  const Evidyon::Magic::Magic* getMagic(Evidyon::Magic::MagicIndex magic) const;
  const Avatar::SpellAvailabilityMask* getClassSpellAvailability(
    Avatar::AvatarClass class_index, Avatar::AvatarLevel level);

  unsigned int getHair(int race_index, int gender_index, int type_select);

  unsigned int getSpawnMagicFX() const;

  // Turns the enchantments into the short version that is set to the client
  unsigned short packEnchantments(
    Evidyon::Magic::EnchantmentTypeIndex enchantments[Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES]);

private:
  ServerProjectileDescription* projectile_descriptions_;
  size_t number_of_projectile_descriptions_;

  SpellDescription* spell_descriptions_;
  size_t number_of_spell_descriptions_;

  Evidyon::Magic::Magic* magic_descriptions_;
  size_t number_of_magic_descriptions_;

  unsigned int spawn_magic_fx_;

  size_t enchantments_in_type_[Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES];


private:
  static GlobalGamefileManager* singleton_;


public:

  // Obtains the descriptive data for a nude avatar of the given
  // type.  A NULL parameter is ignored.
  void getNudeAvatarActorData(
    Evidyon::Avatar::AvatarRace race,
    Evidyon::Avatar::AvatarGender gender,
    Evidyon::Actor::ActorTemplateIndex* actor_template,
    Evidyon::Actor::ActorProfileIndex* actor_profile,
    Evidyon::SkinnedMesh::SkinnedMeshFormIndex* skinned_mesh_form);
  Scenery::SceneryIndex getGoldScenery() { return server_data_.gold_scenery; }

private:
  Evidyon::GameFile::ServerGameFileServerData server_data_;
  Evidyon::Avatar::AvatarRaceServerDescription
    races_[Evidyon::Avatar::NUMBER_OF_AVATAR_RACES];
  Evidyon::Avatar::AvatarClassServerDescription
    classes_[Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES];
  Evidyon::Avatar::AvatarLevelDescription
    levels_[Evidyon::Avatar::HIGHEST_AVATAR_LEVEL];
};

}

#endif