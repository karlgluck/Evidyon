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
#include "item.h"
#include "shared/item/itemclientdescription.h"
#include "shared/item/itemserverdescription.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/treasure/tools/treasurecompiler.h"
#include "shared/treasure/epictreasure.h"
#include "shared/avatar/tools/avatarracelist.h"

namespace Evidyon {
namespace Item {
namespace Tools {



  
//----[  Item  ]---------------------------------------------------------------
Item::Item() {
  member("Displayed Name", &name_);
  member("Treasure Category", &intended_use_);
  member("Intended User", &intended_user_);
  member("Treasure Level - Low", &treasure_level_low_);
  member("Treasure Level - High", &treasure_level_high_);
  member("Equipment Slot", &equipment_type_);
  member("Description", &description_);
  member("Scenery Representation", &scenery_);
  member("Max Stack Size", &max_stack_size_);
  member("Geonite Value", &geonite_value_);
  member("Consumable: Magic", &consumable_magic_);
  member("Consumable: Magic Needs Target?", &consuming_requires_target_);
  member("Consumable: Magic Needs Slot?", &consuming_requires_slot_);
  member("% Added to Chance to Keep Items on Death", &wishing_percent_);
  member("On Death Magic", &on_death_magic_);
  member("Drop on Death?", &drop_on_death_);
  member("Required Avatar Level", &required_avatar_level_);
  for (int i = 0; i < Avatar::NUMBER_OF_AVATAR_ATTRIBUTES; ++i) {
    member(std::string("Required Avatar ")
      + Avatar::AvatarAttributeString(Avatar::AvatarAttribute(i)),
      &required_avatar_attribute_[i]);
  }
  member("Min Attack Damage", &min_attack_damage_);
  member("Max Attack Damage", &max_attack_damage_);
  member("Physical Defense", &physical_defense_);
  member("HP Regen Rate", &hp_regen_rate_);
  member("MP Regen Rate", &mp_regen_rate_);
  member("HP Modifier (%)", &hp_modifier_);
  member("MP Modifier (%)", &mp_modifier_);
  member("HP Added", &hp_added_);
  member("MP Added", &mp_added_);
  member("Movement Speed Modifier (%)", &movement_speed_modifier_);
  member("Attack Speed Modifier (%)", &attack_speed_modifier_);
  member("Weapon Attack Duration (milliseconds)", &weapon_attack_duration_);
  member("Weapon Range", &weapon_range_);
  member("HP Steal", &hp_steal_);
  member("MP Steal", &mp_steal_);
  //member("Spell to Make Available (0)", &spell_to_make_available_[0]);
  //member("Spell to Make Available (1)", &spell_to_make_available_[1]);
  for (int gender_index = 0; gender_index < Avatar::NUMBER_OF_AVATAR_GENDERS; ++gender_index) {
    std::string gender
      = Avatar::AvatarGenderString((Avatar::AvatarGender)gender_index);
    for (int race_index = 0; race_index < Avatar::NUMBER_OF_AVATAR_RACES; ++race_index) {
      std::string race
        = Avatar::AvatarRaceString((Avatar::AvatarRace)race_index);
      member(gender + " " + race,
             &actor_templates_[gender_index][race_index][0]);
      /*
      for (int class_index = 0; class_index < Avatar::NUMBER_OF_AVATAR_CLASSES; ++class_index) {
        std::string class_name
          = Avatar::AvatarClassString((Avatar::AvatarClass)class_index);
        member(gender + " " + race + " " + class_name,
               &actor_templates_[gender_index][race_index][class_index]);
      }*/
    }
  }
  member("Projectile", &projectile_);

  intended_use_.setValue(INTENDEDUSE_EQUIPMENT_COMMON);
  intended_user_.setValue(INTENDEDUSER_NONSPECIFIC);
  treasure_level_low_.setValue(1);
  treasure_level_high_.setValue(10);
  max_stack_size_.setValue(1);
  geonite_value_.setValue(1);
  hp_modifier_.setValue(0.0f);
  mp_modifier_.setValue(0.0f);
  movement_speed_modifier_.setValue(0.0f);
  weapon_attack_duration_.setValue(1000);
  drop_on_death_.setValue(true);
}


//----[  compile  ]------------------------------------------------------------
void Item::compile(Evidyon::Treasure::Tools::TreasureCompiler* treasure_compiler,
                   Evidyon::Magic::Tools::MagicCompiler* magic_compiler,
                   Evidyon::Item::ItemClientDescription* client_description,
                   Evidyon::Item::ItemServerDescription* server_description) {

  ItemRarity rarity;
  switch (intended_use_.getValue()) {
    default:
    case INTENDEDUSE_EQUIPMENT_COMMON:
      rarity = ITEMRARITY_COMMON;
      break;
    case INTENDEDUSE_EQUIPMENT_UNCOMMON:
      rarity = ITEMRARITY_UNCOMMON;
      break;
    case INTENDEDUSE_EQUIPMENT_RARE:
      rarity = ITEMRARITY_RARE;
      break;
    case INTENDEDUSE_EPIC:
      rarity = ITEMRARITY_EPIC;
      break;
  }

  if (client_description) {
    strncpy_s(client_description->name,
              Evidyon::Item::ItemClientDescription::NAME_LENGTH,
              name_.getValue().c_str(),
              Evidyon::Item::ItemClientDescription::NAME_LENGTH-1);

    client_description->scenery
      = scenery_.getReferencedResourceIndex();
    client_description->consumable = consumable_magic_.isImplemented();
    client_description->consuming_requires_target
      = consuming_requires_target_.getValue();
    client_description->consuming_requires_slot
      = consuming_requires_slot_.getValue();
    client_description->rarity = rarity;
  }

  bool is_weapon = false, is_ranged_weapon = false, is_armor = false;
  const char* item_type = NULL;
  switch (equipment_type_.getValue()) {
    case ITEMEQUIPMENTTYPE_1H:
    case ITEMEQUIPMENTTYPE_MAINHAND:
      is_weapon = true;
      item_type = "One-Handed Weapon\n";
      item_type = "One-Handed Weapon\n";
      break;
    case ITEMEQUIPMENTTYPE_OFFHAND:
      item_type = "Off-Hand Item\n";
      break;
    case ITEMEQUIPMENTTYPE_SHIELD:
      item_type = "Shield\n";
      is_armor = true;
      break;
    case ITEMEQUIPMENTTYPE_JAVELIN:
      item_type = "Javelin\n";
      is_weapon = true;
      is_ranged_weapon = true;
      break;
    case ITEMEQUIPMENTTYPE_THROWN:
      item_type = "Thrown\n";
      is_weapon = true;
      is_ranged_weapon = true;
      break;
    case ITEMEQUIPMENTTYPE_2H:
      is_weapon = true;
      item_type = "Two-Handed Weapon\n";
      break;
    case ITEMEQUIPMENTTYPE_POLEARM:
      is_weapon = true;
      item_type = "Polearm\n";
      break;
    case ITEMEQUIPMENTTYPE_BOW:
      is_weapon = true;
      is_ranged_weapon = true;
      item_type = "Bow\n";
      break;
    case ITEMEQUIPMENTTYPE_CROSSBOW:
      is_weapon = true;
      is_ranged_weapon = true;
      item_type = "Crossbow\n";
      break;
    case ITEMEQUIPMENTTYPE_DUALWIELD:
      is_weapon = true;
      item_type = "Dual-Wielded Weapon\n";
      break;
    case ITEMEQUIPMENTTYPE_UNARMED:
      is_weapon = true;
      item_type = "Fist Weapon\n";
      break;
    case ITEMEQUIPMENTTYPE_SPEAR:
      is_weapon = true;
      item_type = "Spear\n";
      break;
    case ITEMEQUIPMENTTYPE_ARMOR:
      item_type = "Armor\n";
      is_armor = true;
      break;
    case ITEMEQUIPMENTTYPE_HELMET:
      item_type = "Helmet\n";
      is_armor = true;
      break;
    case ITEMEQUIPMENTTYPE_BELT:
      item_type = "Belt\n";
      is_armor = true;
      break;
    case ITEMEQUIPMENTTYPE_RING:
      item_type = "Ring\n";
      break;
    case ITEMEQUIPMENTTYPE_ACCESSORY:
      item_type = "Accessory\n"; // is_consumable ? "Consumable";
      break;
    case ITEMEQUIPMENTTYPE_BOOTS:
      item_type = "Boots\n";
      is_armor = true;
      break;
    case ITEMEQUIPMENTTYPE_GLOVES:
      item_type = "Gloves\n";
      is_armor = true;
      break;
    case ITEMEQUIPMENTTYPE_NONE:
      item_type = "";
      break;

    default:
      item_type = "";
      break;
  }

  if (treasure_compiler &&
      (INTENDEDUSE_EPIC == intended_use_.getValue())) {
    // If this item is marked as an epic, compile the epic description
    Treasure::EpicTreasure epic_treasure;
    epic_treasure.lowest_level = treasure_level_low_.getValue();
    epic_treasure.highest_level = treasure_level_high_.getValue();
    epic_treasure.item = getIndex();
    treasure_compiler->add(&epic_treasure);
  }

  if (magic_compiler) {
    if (is_ranged_weapon) {
      // compile the projectile types for ranged weapons
      projectile_.compile(magic_compiler->eventCompiler(),
                          &server_description->projectile);
    }
    Magic::Tools::Magic* consumable_magic
      = consumable_magic_.getImplementation();
    Magic::Tools::Magic* on_death_magic
      = on_death_magic_.getImplementation();
    if (server_description) {
      server_description->consumable_magic
        = consumable_magic ? consumable_magic->compile(magic_compiler)
                    : Magic::INVALID_MAGIC_INDEX;
      server_description->on_death_magic
        = on_death_magic ? on_death_magic->compile(magic_compiler)
                    : Magic::INVALID_MAGIC_INDEX;
    }
  }

  char property_description
    [Evidyon::Item::ItemClientDescription::DESCRIPTION_LENGTH];
  char temp
    [Evidyon::Item::ItemClientDescription::DESCRIPTION_LENGTH];

  strcpy_s(property_description,
           sizeof(property_description),
           item_type);

  // add rarity
  switch (rarity) {
    case ITEMRARITY_COMMON:
      strcat_s(property_description,
               sizeof(property_description), "Common\n"); break;
    case ITEMRARITY_UNCOMMON:
      strcat_s(property_description,
               sizeof(property_description), "Uncommon\n"); break;
    case ITEMRARITY_EPIC:
      strcat_s(property_description,
               sizeof(property_description), "Epic\n"); break;
    case ITEMRARITY_RARE:
      strcat_s(property_description,
               sizeof(property_description), "Rare\n"); break;
  }

  strcat_s(property_description,
           sizeof(property_description),
           description_.getValue().c_str());
  sprintf_s(temp,
            sizeof(temp),
            "%sSacrifice for %i Geonite",
            property_description[0] != '\0' ? "\n" : "",
            geonite_value_.getValue());
  strcat_s(property_description, sizeof(property_description), temp);

  if (required_avatar_level_.getValue() > 0) {
    sprintf_s(temp,
              sizeof(temp),
              "\nRequires Level %i",
              required_avatar_level_.getValue());
    strcat_s(property_description, sizeof(property_description), temp);
  }
  for (int i = 0; i < Avatar::NUMBER_OF_AVATAR_ATTRIBUTES; ++i) {
    if (required_avatar_attribute_[i].getValue() > 0) {
    sprintf_s(temp,
              sizeof(temp),
              "\nRequires %i %s",
              required_avatar_attribute_[i].getValue(),
              Avatar::AvatarAttributeString(Avatar::AvatarAttribute(i)));
    strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  strcat_s(property_description,
    sizeof(property_description), "\n");

  { // Attack values
    int min_damage = min_attack_damage_.getValue();
    int max_damage = max_attack_damage_.getValue();
    if (is_weapon) {
      sprintf_s(temp, sizeof(temp), "\n%i - %i damage",
                min_damage,
                max_damage);
      strcat_s(property_description, sizeof(property_description), temp);
    } else {
      if (min_damage > 0) {
        sprintf_s(temp, sizeof(temp), "\n+%i min damage", min_damage);
        strcat_s(property_description, sizeof(property_description), temp);
      }
      if (max_damage > 0) {
        sprintf_s(temp, sizeof(temp), "\n+%i max damage", max_damage);
        strcat_s(property_description, sizeof(property_description), temp);
      }
    }
  }
  { // Defense value
    int defense = physical_defense_.getValue();
    if (is_armor) {
      sprintf_s(temp, sizeof(temp), "\n%i armor", defense);
      strcat_s(property_description, sizeof(property_description), temp);
    } else {
      if (defense > 0) {
        sprintf_s(temp, sizeof(temp), "\n+%i armor", defense);
        strcat_s(property_description, sizeof(property_description), temp);
      }
    }
  }

  strcat_s(property_description,
    sizeof(property_description), "\n");

  {
    int value = hp_regen_rate_.getValue();
    if (value != 0) {
      sprintf_s(temp,
                sizeof(temp),
                "\n%+i regenerate HP",
                value);
      strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    int value = mp_regen_rate_.getValue();
    if (value != 0) {
      sprintf_s(temp,
                sizeof(temp),
                "\n%+i regenerate MP",
                value);
      strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    float value = hp_modifier_.getValue();
    if (value != 0.0f) {
      sprintf_s(temp,
                sizeof(temp),
                "\n%+i%% max HP",
                (int)(value));
      strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    float value = mp_modifier_.getValue();
    if (value != 0.0f) {
      sprintf_s(temp,
                sizeof(temp),
                "\n%+i%% max MP",
                (int)(value));
      strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    int value = hp_added_.getValue();
    if (value != 0) {
        sprintf_s(temp,
                  sizeof(temp),
                  "\n%+i max HP",
                  value);
        strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    int value = mp_added_.getValue();
    if (value != 0) {
        sprintf_s(temp,
                  sizeof(temp),
                  "\n%+i max MP",
                  value);
        strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    float value = movement_speed_modifier_.getValue();
    if (value != 0.0f) {
      sprintf_s(temp,
                sizeof(temp),
                "\n%+i%% speed",
                (int)(value));
      strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    float value = attack_speed_modifier_.getValue();
    if (value != 0.0f) {
      sprintf_s(temp,
                sizeof(temp),
                "\n%+i%% attack speed",
                (int)(value));
      strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    float value = hp_steal_.getValue();
    if (value != 0.0f) {
      sprintf_s(temp,
                sizeof(temp),
                "\n%+i%% hp steal",
                (int)(value*100.0f));
      strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    float value = mp_steal_.getValue();
    if (value != 0.0f) {
      sprintf_s(temp,
                sizeof(temp),
                "\n%+i%% mp steal",
                (int)(value*100.0f));
      strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    int value = wishing_percent_.getValue();
    if (value != 0) {
      sprintf_s(temp,
                sizeof(temp),
                "\n%+i%% chance to keep all equipment on death",
                value);
      strcat_s(property_description, sizeof(property_description), temp);
    }
  }

  {
    if (drop_on_death_.getValue() == false) {
      strcat_s(property_description,
               sizeof(property_description),
               "\nWill not drop on death");
    }
  }

  if (client_description) {
    strncpy_s(client_description->description,
              Evidyon::Item::ItemClientDescription::DESCRIPTION_LENGTH,
              property_description,
              Evidyon::Item::ItemClientDescription::DESCRIPTION_LENGTH-1);
  }


  // server description
  if (server_description) {

    // copy this structure's name--not the displayed name!
    strncpy_s(server_description->true_name,
              Evidyon::Item::ItemServerDescription::NAME_LENGTH,
              getName().c_str(),
              Evidyon::Item::ItemServerDescription::NAME_LENGTH-1);

    server_description->weapon.range = weapon_range_.getValue();
    server_description->weapon.attack_duration
      = Time::Milliseconds(weapon_attack_duration_.getValue());
    server_description->requirements.level = required_avatar_level_.getValue();
    for (int i = 0; i < Avatar::NUMBER_OF_AVATAR_ATTRIBUTES; ++i) {
      server_description->requirements.attribute_value[i] =
        required_avatar_attribute_[i].getValue();
    }
    assert(magic_compiler);
    //server_description->consumable_magic = // already initialized
    //server_description->on_death_magic = // already initialized
    server_description->drop_on_death = drop_on_death_.getValue();
    server_description->geonite_value = geonite_value_.getValue();
    server_description->wishing_percent = wishing_percent_.getValue();
    server_description->permanent_storage_hash =
      dc::dcResourceManager::uniqueIDOfTypeName(getName());
    server_description->type_index = getIndex();
    server_description->ground_scenery = scenery_.getReferencedResourceIndex();
    server_description->max_stack_size = max_stack_size_.getValue();
    server_description->equipment_type = equipment_type_.getValue();
    server_description->equipped_actor_modifiers.equipped_scenery
      = scenery_.getReferencedResourceIndex();
    server_description->equipped_avatar_modifiers.hp_steal = hp_steal_.getValue();
    server_description->equipped_avatar_modifiers.mp_steal = mp_steal_.getValue();
    for (int gender_index = 0; gender_index < Avatar::NUMBER_OF_AVATAR_GENDERS; ++gender_index) {
      for (int race_index = 0; race_index < Avatar::NUMBER_OF_AVATAR_RACES; ++race_index) {
        for (int class_index = 0; class_index < Avatar::NUMBER_OF_AVATAR_CLASSES; ++class_index) {
          /*actor_templates_[gender_index][race_index][class_index].compile(
            &server_description->equipped_actor_modifiers.actor_templates
                [gender_index][race_index][class_index].actor_profile,
            &server_description->equipped_actor_modifiers.actor_templates
                [gender_index][race_index][class_index].skinned_mesh_form);*/
          actor_templates_[gender_index][race_index][0].compile(
            &server_description->equipped_actor_modifiers.actor_templates
                [gender_index][race_index][class_index].actor_profile,
            &server_description->equipped_actor_modifiers.actor_templates
                [gender_index][race_index][class_index].skinned_mesh_form);
        }
      }
    }
    server_description->equipped_avatar_modifiers.min_attack_damage
        = min_attack_damage_.getValue();
    server_description->equipped_avatar_modifiers.max_attack_damage
        = max_attack_damage_.getValue();
    server_description->equipped_avatar_modifiers.physical_defense
        = physical_defense_.getValue();
    server_description->equipped_avatar_modifiers.hp_regen_rate
        = hp_regen_rate_.getValue();
    server_description->equipped_avatar_modifiers.mp_regen_rate
        = mp_regen_rate_.getValue();
    server_description->equipped_avatar_modifiers.max_hp_multiplier
        = 1.0f + (hp_modifier_.getValue() / 100.0f);
    server_description->equipped_avatar_modifiers.max_mp_multiplier
        = 1.0f + (mp_modifier_.getValue() / 100.0f);
    server_description->equipped_avatar_modifiers.max_hp_added
        = hp_added_.getValue();
    server_description->equipped_avatar_modifiers.max_mp_added
        = mp_added_.getValue();
    server_description->equipped_avatar_modifiers.movement_speed_multiplier
        = 1.0f + movement_speed_modifier_.getValue() / 100.0f;
    server_description->equipped_avatar_modifiers.attack_delay_multiplier
        = 1.0f / (1.0f + attack_speed_modifier_.getValue() / 100.0f);
  }

}




//----[  setScenery  ]---------------------------------------------------------
void Item::setScenery(Evidyon::Tools::SceneryElement* scenery) {
  scenery_.setReferencedResource(scenery);
}



//----[  hasScenery  ]---------------------------------------------------------
bool Item::hasScenery() const {
  return scenery_.getGenericReferencedResource() != NULL;
}


//----[  compareTo  ]----------------------------------------------------------
int Item::compareTo(const dc::dcGenericResource* other) const {
  if (other == 0 || getTypeID() != other->getTypeID()) {
    return dc::dcGenericResource::compareTo(other);
  } else {
    int compare
     = equipment_type_.compareTo(&((const Item *)other)->equipment_type_);
    if (compare != 0) return compare;
    else return dc::dcGenericResource::compareTo(other);
  }
}



//----[  getTreasureSettings  ]------------------------------------------------
void Item::getTreasureSettings(ItemIntendedUser* intended_user,
                               ItemIntendedUse* intended_use,
                               ItemEquipmentType* equipment_type) const {
  *intended_user = intended_user_.getValue();
  *intended_use = intended_use_.getValue();
  *equipment_type = equipment_type_.getValue();
}



//----[  getTreasureDropWeight  ]----------------------------------------------
Treasure::Tools::TreasureWeightValue Item::getTreasureDropWeight(int monster_level) const {
  Treasure::TreasureGroup group;
  using namespace Evidyon::Treasure;
  if (!getTreasureGroup(&group) || group == TREASUREGROUP_CONSUMABLE) {
    return 100.0;
  }

  int low = treasure_level_low_.getValue();
  int high = treasure_level_high_.getValue();
  if (high <= low) return monster_level == low ? 1.0 : 0.0;
  if (monster_level < low || monster_level > high) return 0.0;
  int diff = 2*(monster_level - low);
  return diff*diff + 10.0;

  //double scale = double(monster_level - low) / double(high-low);
  //assert(scale >= 0.0);
  //assert(scale <= 1.0);

  //// do a quadratic interpolation with the max halfway between low and high;
  //// the ends of the scale are set at 10 and the center is 100
  //scale = (scale - 0.5);
  //scale *= scale;
  //return 10 + 90 * (1.0 - 4.0 * scale);
}



//----[  getTreasureGroup  ]---------------------------------------------------
bool Item::getTreasureGroup(Treasure::TreasureGroup* group) const {
  assert(group);
  using namespace Evidyon::Treasure;
  switch (intended_use_.getValue()) {
  case INTENDEDUSE_EQUIPMENT_COMMON:
    *group = TREASUREGROUP_COMMON_EQUIPMENT;
    return true;
  case INTENDEDUSE_EQUIPMENT_UNCOMMON: 
    *group = TREASUREGROUP_UNCOMMON_EQUIPMENT;
    return true;
  case INTENDEDUSE_EQUIPMENT_RARE: 
    *group = TREASUREGROUP_RARE_EQUIPMENT;
    return true;
  case INTENDEDUSE_EPIC:
    *group = INVALID_TREASURE_GROUP;
    return false;
  default:
    *group = TREASUREGROUP_CONSUMABLE;
    return true;
  }
  assert(false);
}




//----[  set  ]----------------------------------------------------------------
void Item::set(std::map<std::string,std::string>* values,
               Avatar::Tools::AvatarRaceList* avatar_races) {
#define SETVALUE(p, str) p.interpret(values->find(str)->second.c_str());
  SETVALUE(name_, "Name");
  SETVALUE(intended_use_, "Intended Use");
  SETVALUE(intended_user_, "User");
  SETVALUE(treasure_level_low_, "Treasure Level - Low");
  SETVALUE(treasure_level_high_, "Treasure Level - High");
  SETVALUE(equipment_type_, "Equipment Type");
  SETVALUE(description_, "Description");
  //SETVALUE(scenery_, "");
  SETVALUE(max_stack_size_, "Max Stack Size");
  SETVALUE(geonite_value_, "Geonite Value");
  SETVALUE(weapon_range_, "Weapon Range")
  SETVALUE(weapon_attack_duration_, "Weapon Attack Duration");
  //SETVALUE(consumable_magic_, "");
  //SETVALUE(consuming_requires_target_, "");
  SETVALUE(wishing_percent_, "Wishing %");
  //SETVALUE(on_death_magic_, "");
  SETVALUE(drop_on_death_, "Drop on Death");
  SETVALUE(required_avatar_level_, "Required Avatar Level");
  SETVALUE(required_avatar_attribute_[Avatar::ATTRIBUTE_STRENGTH], "Strength");
  SETVALUE(required_avatar_attribute_[Avatar::ATTRIBUTE_AGILITY], "Agility");
  SETVALUE(required_avatar_attribute_[Avatar::ATTRIBUTE_CONSTITUTION], "Constitution");
  SETVALUE(required_avatar_attribute_[Avatar::ATTRIBUTE_INTELLIGENCE], "Intelligence");
  SETVALUE(required_avatar_attribute_[Avatar::ATTRIBUTE_WISDOM], "Wisdom");
  SETVALUE(min_attack_damage_, "Min Attack Damage");
  SETVALUE(max_attack_damage_, "Max Attack Damage");
  SETVALUE(physical_defense_, "Physical Defense");
  SETVALUE(hp_regen_rate_, "HP Regen Rate");
  SETVALUE(mp_regen_rate_, "MP Regen Rate");
  SETVALUE(hp_modifier_, "HP Modifier");
  SETVALUE(mp_modifier_, "MP Modifier");
  SETVALUE(hp_added_, "HP Added");
  SETVALUE(mp_added_, "MP Added");
  SETVALUE(movement_speed_modifier_, "Movement Speed Modifier");
  SETVALUE(attack_speed_modifier_, "Attack Speed Modifier");
  SETVALUE(hp_steal_, "HP Steal");
  SETVALUE(mp_steal_, "MP Steal");

  for (int gender_index = 0; gender_index < Avatar::NUMBER_OF_AVATAR_GENDERS; ++gender_index) {
    for (int race_index = 0; race_index < Avatar::NUMBER_OF_AVATAR_RACES; ++race_index) {
      for (int class_index = 0; class_index < Avatar::NUMBER_OF_AVATAR_CLASSES; ++class_index) {
        if (actor_templates_[gender_index][race_index][class_index].isValid() == false) {
          avatar_races->getForm(
            (Avatar::AvatarRace)race_index,
            (Avatar::AvatarGender)gender_index,
            &actor_templates_[gender_index][race_index][class_index]);
        }
      }
    }
  }
}




}
}
}

