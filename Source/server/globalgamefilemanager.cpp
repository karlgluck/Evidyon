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
#include "shared/magic/magiclimits.h"
#include "globalgamefilemanager.h"
#include "shared/server-editor/server_gamefile.h"
#include "../common/null.h"
#include <dc/debug>
#include "shared/server-editor/serveratlas.h"
#include "shared/avatar/evidyon_avatar.h"
#include "shared/server-editor/magicdescription.h"
#include "../common/safemacros.h"
#include "shared/server-editor/serverprojectiledescription.h"
#include "globalworldmanager.h"
#include "shared/server-editor/maplocationspecialfunction.h"
#include "..\shared\server-editor\swarmspawndescription.h"
#include "globalactormanager.h"
#include "shared/actor/actordescriptiontemplate.h"
#include "globalitemmanager.h"

#include "uniqueavatarnamechecker.h"
#include "shared/quest/quest-server.h"
#include "globalquestmanager.h"

#include "shared/gamefile/servergamefiledata.h"
#include "shared/gamefile/gamefilereader.h"
#include "shared/magic/magic.h"
#include "shared/spell/spellserverdescription.h"

#include "server/lifeformai/globallifeformaimanager.h"
#include "server/treasure/globaltreasuremanager.h"
#include "shared/design/xptolevel.h"
#include "server/geosid/globalgeosidmanager.h"

namespace Evidyon {


GlobalGamefileManager* GlobalGamefileManager::singleton_ = NULL;




//----[  singleton  ]----------------------------------------------------------
GlobalGamefileManager* GlobalGamefileManager::singleton() {
  return singleton_;
}



//----[  create  ]-------------------------------------------------------------
bool GlobalGamefileManager::create(
    const char* source_file,
    UniqueAvatarNameChecker* unique_avatar_name_checker,
    GlobalItemManager* item_manager,
    GlobalActorManager* actor_manager,
    GlobalWorldManager* world_manager,
    Server::GlobalLifeformAIManager* lifeform_ai_manager,
    Quests::Server::GlobalQuestManager* quest_manager,
    Server::GlobalTreasureManager* treasure_manager,
    Server::GlobalGeosidManager* geosid_manager) {

  using namespace Evidyon::GameFile;

  spell_descriptions_ = NULL;
  magic_descriptions_ = NULL;
  projectile_descriptions_ = NULL;

  GameFileReader reader;
  if (APP_ERROR(!reader.open(source_file))("Couldn't open server game file"))
    goto STARTUP_FAILED;

  ServerGameFileHeader header;
  memcpy(&header, reader.getReadPoint(), sizeof(header));
  reader.setReadPoint(reader.getReadPoint() + sizeof(header));

  ServerDataSerializer::read(&reader, &server_data_);
  AvatarRaceServerSerializer::read(
    &reader,
    &CopiedList<Evidyon::Avatar::AvatarRaceServerDescription>(races_));
  AvatarClassServerSerializer::read(
    &reader,
    &CopiedList<Evidyon::Avatar::AvatarClassServerDescription>(classes_));
  AvatarLevelSerializer::read(
    &reader,
    &CopiedList<Evidyon::Avatar::AvatarLevelDescription>(levels_));

  //===========================================================================
  // Lifeform AI
  //===========================================================================
  assert(reader.getReadPointOffset()
         == header.lifeform_ai.first_byte_offset);
  lifeform_ai_manager->initNumberOfDescriptions(header.lifeform_ai.array_entries);
  for (size_t i = 0; i < header.lifeform_ai.array_entries; ++i) {
    Evidyon::LifeformAI::LifeformAI description;
    LifeformAISerializer::read(
      &reader,
      &description);
    lifeform_ai_manager->initDescription(i, &description);
  }


  //===========================================================================
  // Maps
  //===========================================================================
  assert(reader.getReadPointOffset() == header.maps.first_byte_offset);
  world_manager->create(header.maps.array_entries);
  for (size_t i = 0; i < header.maps.array_entries; ++i) {
    World::MapHash map_hash;
    size_t width, height;
    size_t regions_wide, regions_high;
    size_t number_of_regions, number_of_locations;
    size_t number_of_triggers;

    // Read the array sizes
    size_t map_file_offset = reader.getReadPointOffset();
    ServerMapSerializer::read(
      &reader,
      &map_hash,
      &width,
      &height,
      &PeekArray<ServerGameFileMapLocationData>(&number_of_locations),
      &regions_wide,
      &regions_high,
      &PeekArray<ServerGameFileMapRegionData>(&number_of_regions),
      &PeekArray<Evidyon::Trigger::Trigger>(&number_of_triggers));
    reader.setReadPointOffset(map_file_offset);

    // Check values
    assert(regions_wide * regions_high == number_of_regions);
    assert(width * height == number_of_locations);

    // Allocate arrays
    ServerGameFileMapLocationData* locations
      = new ServerGameFileMapLocationData[number_of_locations];
    ServerGameFileMapRegionData* regions
      = new ServerGameFileMapRegionData[number_of_regions];
    Evidyon::Trigger::Trigger* triggers
      = new Evidyon::Trigger::Trigger[number_of_triggers];

    // Read data
    ServerMapSerializer::read(
      &reader,
      &map_hash,
      &width,
      &height,
      &CopiedArray<ServerGameFileMapLocationData>(
        locations,
        number_of_locations,
        &number_of_locations),
      &regions_wide,
      &regions_high,
      &CopiedArray<ServerGameFileMapRegionData>(
        regions,
        number_of_regions,
        &number_of_regions),
      &CopiedArray<Evidyon::Trigger::Trigger>(
        triggers,
        number_of_triggers,
        &number_of_triggers));

    ////////////////////////////////////////////////////////////////////////
    World::MapIndex map_index = World::MapIndex(i);
    world_manager->setMapHashToMapIndex(map_hash, map_index);
    Map* map = world_manager->getMapFromIndex(map_index);
    map->create(map_index,
                map_hash,
                width,
                height,
                locations,
                regions_wide,
                regions_high,
                regions,
                triggers,
                number_of_triggers);
    ////////////////////////////////////////////////////////////////////////

    // Erase temporary information
    SAFE_DELETE_ARRAY(locations);
    SAFE_DELETE_ARRAY(regions);
    SAFE_DELETE_ARRAY(triggers);
  }

  //===========================================================================
  // Items
  //===========================================================================
  assert(reader.getReadPointOffset() == header.items.first_byte_offset);
  item_manager->setNumberOfItemDescriptions(header.items.array_entries);
  for (size_t i = 0; i < header.items.array_entries; ++i) {
    Evidyon::Item::ItemServerDescription description;
    ItemServerSerializer::read(
      &reader,
      &description);
    item_manager->setItemDescription(i, &description);
  }

  //===========================================================================
  // Magic
  //===========================================================================
  assert(reader.getReadPointOffset() == header.magic.first_byte_offset);
  {
    size_t count = header.magic.array_entries;
    number_of_magic_descriptions_ = count;
    if (count > 0) {
      magic_descriptions_ = new Evidyon::Magic::Magic[count];
      for (size_t i = 0; i < count; ++i) {
        MagicSerializer::read(
          &reader,
          &magic_descriptions_[i]);
      }
    }
  }

  //===========================================================================
  // Enchantments
  //===========================================================================
  ServerEnchantmentSerializer::read(
    &reader,
    &CopiedList<size_t>(enchantments_in_type_));

  //===========================================================================
  // Spells
  //===========================================================================
  assert(reader.getReadPointOffset() == header.spells.first_byte_offset);
  {
    size_t count = header.spells.array_entries;
    number_of_spell_descriptions_ = count;
    if (count > 0) {
      spell_descriptions_ = new SpellDescription[count];
      for (size_t i = 0; i < count; ++i) {
        Evidyon::Spell::SpellServerDescription description;
        ServerSpellSerializer::read(
          &reader,
          &description);
        int j = 0;
        while (j < description.number_of_stages) {
          spell_descriptions_[i].magic[j]
            = getMagic(description.stages[j].magic_index);
          ++j;
        }
        while (j < description.MAXIMUM_STAGES) {
          spell_descriptions_[i].magic[j] = NULL;
          ++j;
        }
        memcpy(&spell_descriptions_[i].description,
               &description,
               sizeof(description));
      }
    }
  }

  //===========================================================================
  // Treasure Manager
  //===========================================================================
  {
    assert(reader.getReadPointOffset() == header.epic_treasure.first_byte_offset);
    treasure_manager->initNumberOfEpicTreasures(
      header.epic_treasure.array_entries);
    for (int i = 0; i < header.epic_treasure.array_entries; ++i) {
      Treasure::EpicTreasure description;
      EpicTreasureSerializer::read(
        &reader,
        &description);
      treasure_manager->initEpicTreasure(i, &description);
    }
  }
  {
    assert(reader.getReadPointOffset() == header.treasure_classes.first_byte_offset);
    treasure_manager->initNumberOfTreasureClasses(
      header.treasure_classes.array_entries);
    for (int i = 0; i < header.treasure_classes.array_entries; ++i) {
      Treasure::TreasureClass description;
      TreasureClassSerializer::read(
        &reader,
        &description);
      treasure_manager->initTreasureClass(i, &description);
    }
  }
  {
    assert(reader.getReadPointOffset() == header.treasure_drop_sequence.first_byte_offset);
    treasure_manager->initDropSequenceLength(
      header.treasure_drop_sequence.array_entries);
    for (int i = 0; i < header.treasure_drop_sequence.array_entries; ++i) {
      Treasure::TreasureDropSequenceElement description;
      TreasureDropSequenceSerializer::read(
        &reader,
        &description);
      treasure_manager->initDropSequenceElement(i, &description);
    }
  }

  //===========================================================================
  // Geosids
  //===========================================================================
  {
    assert(reader.getReadPointOffset() == header.geosids.first_byte_offset);
    geosid_manager->initNumberOfGeosids(header.geosids.array_entries);
    for (int i = 0; i < header.geosids.array_entries; ++i) {
      Geosid::GeosidServerDescription description;
      ServerGeosidSerializer::read(
        &reader,
        &description);
      geosid_manager->initGeosid(&description);
    }
  }


  //===========================================================================
  // Reserved Names
  //===========================================================================
  //{
  //  size_t size = 0;
  //  fread(&size, sizeof(size), 1, fp);
  //  for (size_t s = 0; s < size; ++s) {
  //    size_t length = 0;
  //    fread(&length, sizeof(length), 1, fp);
  //    char* buffer = new char[length+1];
  //    fread(buffer, sizeof(char), length, fp);
  //    buffer[length] = '\0';
  //    unique_avatar_name_checker->addName(buffer);
  //    delete[] buffer;
  //  }
  //}




  //// Get defined effect types
  //fread(&spawn_magic_fx_, sizeof(spawn_magic_fx_), 1, fp);

  ////===========================================================================
  //// Quests
  ////===========================================================================
  //{
  //  // temporarily disabled
  //  ASSERT(quest_manager->create(fp)) else return false;
  //}


  //  //=========================================================================
  //  // Special Locations
  //  //=========================================================================
  //  {
  //    size_t number;
  //    fread(&number, sizeof(number), 1, fp);
  //    CONFIRM(map->createSpecialFunctions(number)) else goto STARTUP_FAILED;
  //    for (size_t index = 0; index < number; ++index) {
  //      Evidyon::CompiledMapLocationSpecialFunction description;
  //      fread(&description, sizeof(description), 1, fp);
  //      MapLocationSpecialFunction* function =
  //        map->setLocationSpecialFunction(index, description.x, description.y);
  //      int map_index = description.map_link.map_index;
  //      function->map_link.target_map =
  //        (map_index < 0 || map_index >= number_of_maps) ? map : world_manager->getMap(map_index);
  //      function->map_link.x = description.map_link.x;
  //      function->map_link.y = description.map_link.y;
  //      function->type = description.type;
  //    }
  //  }

  //  //=========================================================================
  //  // Swarm Spawn
  //  //=========================================================================
  //  {
  //    int regions_wide, regions_high;
  //    fread(&regions_wide, sizeof(regions_wide), 1, fp);
  //    fread(&regions_high, sizeof(regions_high), 1, fp);

  //    for (int y = 0; y < regions_high; ++y) {
  //      for (int x = 0; x < regions_wide; ++x) {
  //        unsigned int treasure_randomizer_mask;
  //        fread(&treasure_randomizer_mask,
  //              sizeof(treasure_randomizer_mask), 1, fp);
  //        size_t local_swarms;
  //        fread(&local_swarms, sizeof(local_swarms),1, fp);
  //        Evidyon::CompiledRegionSwarmSpawn swarms[MAX_SWARM_SPAWNS_PER_WORLD_REGION];
  //        Evidyon::CompiledSwarmSpawnLocationInfo locations[MAX_SWARM_SPAWNS_PER_WORLD_REGION];
  //        ASSERT(local_swarms <= MAX_SWARM_SPAWNS_PER_WORLD_REGION) else return false;
  //        fread(swarms, sizeof(Evidyon::CompiledRegionSwarmSpawn), local_swarms, fp);
  //        fread(locations, sizeof(locations), 1, fp);

  //        //// add to the map's spawner
  //        //CONFIRM(map->createRegionSwarmSpawner(x,
  //        //                                      y,
  //        //                                      treasure_randomizer_mask,
  //        //                                      swarms,
  //        //                                      local_swarms,
  //        //                                      locations)) else return false;
  //      }
  //    }
  //  }

  //  //=========================================================================
  //  // Inhabitants
  //  //=========================================================================
  //  {
  //    size_t number_of_inhabitants = 0;
  //    fread(&number_of_inhabitants, sizeof(number_of_inhabitants), 1, fp);
  //    map->createInhabitants(number_of_inhabitants);
  //    for (size_t i = 0; i < number_of_inhabitants; ++i) {
  //      int x, y;
  //      size_t lifeform_ai_type;
  //      fread(&x, sizeof(x), 1, fp);
  //      fread(&y, sizeof(y), 1, fp);
  //      fread(&lifeform_ai_type, sizeof(lifeform_ai_type), 1, fp);
  //      //map->setInhabitant(i, x, y, lifeform_ai_type);
  //    }
  //  }
  //}



  reader.close();
  singleton_ = this;
  return true;

STARTUP_FAILED:
  reader.close();
  destroy();
  return false;
}



//----[  destroy  ]------------------------------------------------------------
void GlobalGamefileManager::destroy() {
  singleton_ = NULL;
  SAFE_DELETE_ARRAY(projectile_descriptions_);
  SAFE_DELETE_ARRAY(spell_descriptions_);
  SAFE_DELETE_ARRAY(magic_descriptions_);
}




//----[  getCharacterActorTypeIndex  ]-----------------------------------------
size_t GlobalGamefileManager::getCharacterActorTypeIndex(int race_index, int gender_index)
{
  return 0; // this has to be changed for v3 anyway...
}




//----[  getPlayerRespawnLocation  ]-------------------------------------------
void GlobalGamefileManager::getPlayerRespawnLocation(bool is_evil,
                                                     Evidyon::World::MapIndex* map,
                                                     float* x,
                                                     float* y) const {
  if (is_evil) {
    *map = server_data_.evil_spawn_point.map;
    *x = server_data_.evil_spawn_point.x;
    *y = server_data_.evil_spawn_point.y;
  } else {
    *map = server_data_.good_spawn_point.map;
    *x = server_data_.good_spawn_point.x;
    *y = server_data_.good_spawn_point.y;
  }
}




//----[  isValidLevel1Character  ]---------------------------------------------
bool GlobalGamefileManager::isValidLevel1Character(Avatar::AvatarRace race_index,
                                                   Avatar::AvatarClass class_index,
                                                   int attributes[Avatar::NUMBER_OF_AVATAR_ATTRIBUTES]) {
  // Make sure the indices are valid
  if (race_index  < 0 || race_index  >= Evidyon::Avatar::NUMBER_OF_AVATAR_RACES ||
      class_index < 0 || class_index >= Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES) return false;

  // Compare all of the attributes
  int sum = 0;
  for (int a = 0; a < 5; ++a) {
    sum += attributes[a];
    if (attributes[a] < 10 || attributes[a] > races_[race_index].attribute_level_1_maxes[a])
      return false;
  }
  CONFIRM(sum <= (10*5+25)) else return false;

  // This is a valid character
  return true;
}




//----[  getLevelUpExperience  ]-----------------------------------------------
Avatar::AvatarExperience GlobalGamefileManager::getLevelUpExperience(int level) const {
  //return (level > 0 && level <= Evidyon::Avatar::HIGHEST_AVATAR_LEVEL) ? levels_[level-1].experience_to_achieve_next_level : (INT_MAX>>2);
  return (level > 0 && level <= Evidyon::Avatar::HIGHEST_AVATAR_LEVEL) ? Design::AVATAR_LEVEL_UP_EXPERIENCE[level-1] : (INT_MAX>>2);
}




//----[  getExperienceToAchieveLevel  ]----------------------------------------
Avatar::AvatarExperience GlobalGamefileManager::getExperienceToAchieveLevel(int level) const {
  level = (level <= Evidyon::Avatar::HIGHEST_AVATAR_LEVEL)
    ? level : Evidyon::Avatar::HIGHEST_AVATAR_LEVEL;
  level = level <= 1 ? 0 : (level-1);
//  return levels_[level].total_experience_to_achieve_this_level;
  return Design::AVATAR_LEVEL_EXPERIENCE[level];
}







//
////----[  calculateCoreAvatarAttributes  ]--------------------------------------
//bool GlobalGamefileManager::calculateCoreAvatarAttributes(AvatarAttributes* avatar_attributes) {
//
//  // Make sure the class index is valid
//  int class_index = avatar_attributes->class_index;
//  if (avatar_attributes == NULL || class_index < 0 || class_index >= Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES) return false;
//  Evidyon::Avatar::AvatarClassServerDescription* class_desc = &classes_[class_index];
//
//  // build the stats values
//  avatar_attributes->stats[0] = 10;
//  avatar_attributes->stats[1] = 10;
//  avatar_attributes->stats[2] = 10;
//  avatar_attributes->stats[3] = 10;
//  avatar_attributes->stats[4] = 10;
//  for (int i = 0; i < avatar_attributes->base.level; ++i) {
//    avatar_attributes->stats[0] += avatar_attributes->stat_increments[i].strength;
//    avatar_attributes->stats[1] += avatar_attributes->stat_increments[i].agility;
//    avatar_attributes->stats[2] += avatar_attributes->stat_increments[i].constitution;
//    avatar_attributes->stats[3] += avatar_attributes->stat_increments[i].intelligence;
//    avatar_attributes->stats[4] += avatar_attributes->stat_increments[i].wisdom;
//  }
//
//  { // 
//    int str_hp = GameplayDesign::avatarCoreAttribute(avatar_attributes->base.level,
//                                                     0, // str
//                                                     avatar_attributes->stat_increments,
//                                                     class_desc->hp_str_base,
//                                                     class_desc->hp_str_bonus_increment,
//                                                     class_desc->hp_str_modifier,
//                                                     avatar_attributes->stats[0]);
//    int con_hp = GameplayDesign::avatarCoreAttribute(avatar_attributes->base.level,
//                                                     2, // con
//                                                     avatar_attributes->stat_increments,
//                                                     class_desc->hp_con_base,
//                                                     class_desc->hp_con_bonus_increment,
//                                                     class_desc->hp_con_modifier,
//                                                     avatar_attributes->stats[2]);
//    DEBUG_INFO("%i/%i str/con", str_hp, con_hp);
//    avatar_attributes->base.max_hp = str_hp + con_hp;
//  }
//  {
//    int int_mp = GameplayDesign::avatarCoreAttribute(avatar_attributes->base.level,
//                                                     3, // int
//                                                     avatar_attributes->stat_increments,
//                                                     class_desc->mp_int_base,
//                                                     class_desc->mp_int_bonus_increment,
//                                                     class_desc->mp_int_modifier,
//                                                     avatar_attributes->stats[3]);
//    int wis_mp = GameplayDesign::avatarCoreAttribute(avatar_attributes->base.level,
//                                                     4, // str
//                                                     avatar_attributes->stat_increments,
//                                                     class_desc->mp_wis_base,
//                                                     class_desc->mp_wis_bonus_increment,
//                                                     class_desc->mp_wis_modifier,
//                                                     avatar_attributes->stats[4]);
//    DEBUG_INFO("%i/%i int/wis", int_mp, wis_mp);
//    avatar_attributes->base.max_mp = int_mp + wis_mp;
//  }
//
//  const int* stats = avatar_attributes->stats;
//
//  //const int level = avatar_attributes->base.level;
//  int level = avatar_attributes->base.level - avatar_attributes->availableStatPoints; // reduce level until stat pts added
//  level = level < 1 ? 1 : level;
////  double calc_mp = class_desc->mpModifier * GameplayDesign::avatarCoreMP(level, stats[3], stats[4]);
////  double calc_hp = class_desc->hpModifier * GameplayDesign::avatarCoreHP(level, stats[0], stats[2]);
//
//  //avatar_attributes->base.max_hp = (int)calc_hp;
//  //avatar_attributes->base.max_mp = (int)calc_mp;
//
//  avatar_attributes->base.speed = GameplayDesign::avatarMovementSpeed(stats[1]);
//  avatar_attributes->base.attack_delay_time = GameplayDesign::avatarAttackDelay(stats[1]);
//  avatar_attributes->base.hp_regen_rate = GameplayDesign::avatarHPRegenRate(avatar_attributes->base.max_hp, stats[2]);
//  avatar_attributes->base.mp_regen_rate = GameplayDesign::avatarMPRegenRate(avatar_attributes->base.max_mp, stats[2]);
//  avatar_attributes->base.min_attack_damage = GameplayDesign::avatarMeleeDamageBase(stats[0], level);
//  avatar_attributes->base.random_attack_damage = GameplayDesign::avatarMeleeDamageAdditional(stats[0], level);
//
//  { // Calculate the offensive factors
//    avatar_attributes->offensive_magic_multiplier = GameplayDesign::avatarOffensiveMagicMultiplier(level, stats[3], stats[4]);
//    avatar_attributes->defensive_magic_multiplier = GameplayDesign::avatarDefensiveMagicMultiplier(level, stats[4]);
//  }
//
//  avatar_attributes->base.attack_delay_time = GameplayDesign::avatarAttackDelay(stats[1]);
//  avatar_attributes->base.casting_delay_percent = GameplayDesign::avatarCastingDelayPercent(stats[1], stats[3]);
//
//  memcpy(&avatar_attributes->current,
//         &avatar_attributes->base,
//         sizeof(avatar_attributes->current));
//
//
//  // Success
//  return true;
//}
//



//----[  getProjectileDuration  ]----------------------------------------------
float GlobalGamefileManager::getProjectileDuration(int projectile_index) const {
  return projectile_index < number_of_projectile_descriptions_ ?
  projectile_descriptions_[projectile_index].duration : 0.0f;
}




//----[  getSpell  ]-----------------------------------------------------------
const SpellDescription* GlobalGamefileManager::getSpell(int spell_index) const {
  return (spell_index < number_of_spell_descriptions_) ?
    &spell_descriptions_[spell_index] : NULL;
}



//----[  getMagic  ]-----------------------------------------------------------
const Evidyon::Magic::Magic* GlobalGamefileManager::getMagic(Evidyon::Magic::MagicIndex magic) const {
  return (magic < number_of_magic_descriptions_) ? &magic_descriptions_[magic] : NULL;
}



//----[  getClassSpellAvailability  ]------------------------------------------
const Avatar::SpellAvailabilityMask*
GlobalGamefileManager::getClassSpellAvailability(Avatar::AvatarClass class_index,
                                                 Avatar::AvatarLevel level) {
  if (level > Evidyon::Avatar::HIGHEST_AVATAR_LEVEL) level = Evidyon::Avatar::HIGHEST_AVATAR_LEVEL;
  --level;  // level->index in array
  if (level < 0) level = 0;
  return (class_index < Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES) ?
  &classes_[class_index].spell_availability_mask[level] : NULL;
}



//----[  getHair  ]------------------------------------------------------------
unsigned int GlobalGamefileManager::getHair(int race_index,
                                            int gender_index,
                                            int type_select) {
  if (race_index < 0 || race_index >= Evidyon::Avatar::NUMBER_OF_AVATAR_RACES) return -1;
  if (gender_index < 0 || gender_index >= Evidyon::Avatar::NUMBER_OF_AVATAR_GENDERS) return -1;
  //return races_[race_index].gender_data[gender_index].
  return -1;//races_[race_index].hair_styles[gender_index][type_select % Evidyon::AVATAR_RACE_HAIR_STYLES_PER_GENDER];  
}




//----[  getSpawnMagicFX  ]----------------------------------------------------
unsigned int GlobalGamefileManager::getSpawnMagicFX() const {
  return spawn_magic_fx_;
}

unsigned long MultPack(unsigned long p, unsigned long v, unsigned long o) {
  assert(o > v);
  return (p * o + v);
}
//
//unsigned long MultUnpack(unsigned long p, unsigned long o, unsigned long* v) {
//  unsigned long lv = p % o;
//  *v = lv;
//  return p / o;
//}


//----[  packEnchantments  ]---------------------------------------------------
unsigned short GlobalGamefileManager::packEnchantments(
    Evidyon::Magic::EnchantmentTypeIndex enchantments[Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES]) {
  unsigned long pack = 0;
  for (int i = 0; i < Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
    // need to add 1 to 'o' (the "options" parameter) because 0 in an enchantment
    // type needs to be included.
    // ex. enchantments_in_type_[i] = 3 means there are 4 possible indices: 0, 1, 2, 3
    // MultPack requres that o > v, so 3 -> 4
    pack = MultPack(pack, enchantments[i], enchantments_in_type_[i]+1);
  }
  assert(pack <= 0x0000FFFF);
  return unsigned short(pack);
}



//----[  getNudeAvatarActorData  ]---------------------------------------------
void GlobalGamefileManager::getNudeAvatarActorData(
    Evidyon::Avatar::AvatarRace race,
    Evidyon::Avatar::AvatarGender gender,
    Evidyon::Actor::ActorTemplateIndex* actor_template,
    Evidyon::Actor::ActorProfileIndex* actor_profile,
    Evidyon::SkinnedMesh::SkinnedMeshFormIndex* skinned_mesh_form) {
  Evidyon::Avatar::AvatarRaceServerDescription::GenderData* data
    = &races_[race].gender_data[gender];

  if (actor_template) *actor_template = data->actor_template;
  if (actor_profile) *actor_profile = data->actor_profile;
  if (skinned_mesh_form) *skinned_mesh_form = data->skinned_mesh_form;
}


}