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
#include "itemavatarmodifier.h"
#include "../shared/server-editor/server_gamefile.h"
#include "../shared/client-editor/client_gamefile.h"

ItemAvatarModifier::ItemAvatarModifier() {
  member("Min Attack Damage", &min_attack_damage_);
  member("Max Attack Damage", &max_attack_damage_);
  member("Physical Defense", &physical_defense_);
  member("Magical Defense", &magical_defense_);
  member("HP Regen Rate", &hp_regen_rate_);
  member("MP Regen Rate", &mp_regen_rate_);
  member("HP Multiplier", &hp_multiplier_);
  member("MP Multiplier", &mp_multiplier_);
  member("HP Added", &hp_added_);
  member("MP Added", &mp_added_);
  member("Level Added", &level_added_);
  member("Movement Speed Multiplier", &movement_speed_multiplier_);
  member("Spell to Make Available (0)", &spell_to_make_available_[0]);
  member("Spell to Make Available (1)", &spell_to_make_available_[1]);
  member("Attack Speed Multiplier", &attack_speed_multiplier_);
  member("Actor Type", &actor_type_);
  member("Steal HP Percent", &steal_hp_percent_);
  member("Steal MP Percent", &steal_mp_percent_);
}

bool ItemAvatarModifier::compileForClient(bool is_weapon,
                                          std::string* description_text,
                                          std::string prop_field_descriptions[8]) const {
  using namespace Evidyon;

  int fields_added = 0;
  int prop_fields = 0;

  for (int i = 0; i < Evidyon::CompiledItemAvatarModifierMask::NUMBER_OF_FIELDS; ++i) {
    ItemAvatarModifierFieldState field_state;
    char type_string[256];
    switch ((CompiledItemAvatarModifierMask::FieldType)i) {
      case CompiledItemAvatarModifierMask::MIN_ATTACK_DAMAGE: {
          bool print_min_and_max = false;

          if (is_weapon) {
            field_state = FIELDSTATE_INNATE_ATTRIBUTE;
            print_min_and_max = true;
          } else {
            field_state = min_attack_damage_.getFieldStateValue();
            print_min_and_max =
                field_state == max_attack_damage_.getFieldStateValue();
          }

          if (print_min_and_max) {
            sprintf_s(type_string,
                      sizeof(type_string),
                      "%s%i-%i damage",
                      is_weapon ? "" : "+", // weapons arent "+" damage, they ARE damage
                      min_attack_damage_.getValue().getValue(),
                      max_attack_damage_.getValue().getValue());
          } else {
            sprintf_s(type_string,
                      sizeof(type_string),
                      "%+i min damage",
                      min_attack_damage_.getValue().getValue());
          }
        } break;

      case CompiledItemAvatarModifierMask::MAX_ATTACK_DAMAGE: {
        field_state = max_attack_damage_.getFieldStateValue();
        if (is_weapon || field_state == min_attack_damage_.getFieldStateValue()) {
          // don't print this since it was already printed in the min damage
          field_state = FIELDSTATE_DISABLED;
        } else {
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i max damage",
                    max_attack_damage_.getValue().getValue());
        }
      } break;

      case CompiledItemAvatarModifierMask::PHYSICAL_DEFENSE:
          field_state = physical_defense_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i armor",
                    physical_defense_.getValue().getValue());
          break;

      case CompiledItemAvatarModifierMask::MAGICAL_DEFENSE:
          field_state = magical_defense_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i magic defense",
                    magical_defense_.getValue().getValue());
          break;

      case CompiledItemAvatarModifierMask::HP_REGEN_RATE:
          field_state = hp_regen_rate_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i regenerate HP",
                    hp_regen_rate_.getValue().getValue());
          break;

      case CompiledItemAvatarModifierMask::MP_REGEN_RATE:
          field_state = mp_regen_rate_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i regenerate MP",
                    mp_regen_rate_.getValue().getValue());
          break;

      case CompiledItemAvatarModifierMask::MAX_HP_MULTIPLIER:
          field_state = hp_multiplier_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i%% max HP",
                    (int)(100.0f*(hp_multiplier_.getValue().getValue() - 1.0f)));
          break;

      case CompiledItemAvatarModifierMask::MAX_MP_MULTIPLIER:
          field_state = mp_multiplier_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i%% max MP",
                    (int)(100.0f*(mp_multiplier_.getValue().getValue() - 1.0f)));
          break;

      case CompiledItemAvatarModifierMask::MAX_HP_ADDED:
          field_state = hp_added_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i max HP",
                    hp_added_.getValue().getValue());
          break;

      case CompiledItemAvatarModifierMask::MAX_MP_ADDED:
          field_state = mp_added_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i max MP",
                    mp_added_.getValue().getValue());
          break;

      case CompiledItemAvatarModifierMask::LEVEL:
          field_state = level_added_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i ability level%s",
                    level_added_.getValue().getValue(),
                    level_added_.getValue().getValue() == 1 ? "" : "s");
          break;

      case CompiledItemAvatarModifierMask::MOVEMENT_SPEED:
          field_state = movement_speed_multiplier_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i%% speed",
                    (int)((movement_speed_multiplier_.getValue().getValue() - 1.0f)*100.0f));
          break;

      case CompiledItemAvatarModifierMask::SPELL0:
          field_state = spell_to_make_available_[0].getFieldStateValue();
          {
            const dc::dcList<Spell>::Element* spell_element = 
              spell_to_make_available_[0].getValue().getReferencedResource();
            if (spell_element == NULL) {
              field_state = FIELDSTATE_DISABLED;
            } else {
              sprintf_s(type_string,
                        sizeof(type_string),
                        "bestows '%s'",
                        spell_element->getName().c_str());
            }
          }         
          break;

      case CompiledItemAvatarModifierMask::SPELL1:
          field_state = spell_to_make_available_[1].getFieldStateValue();
          {
            const dc::dcList<Spell>::Element* spell_element = 
              spell_to_make_available_[1].getValue().getReferencedResource();
            if (spell_element == NULL) {
              field_state = FIELDSTATE_DISABLED;
            } else {
              sprintf_s(type_string,
                        sizeof(type_string),
                        "bestows '%s'",
                        spell_element->getName().c_str());
            }
          }         
          break;

      case CompiledItemAvatarModifierMask::ATTACK_SPEED:
          field_state = attack_speed_multiplier_.getFieldStateValue();
          sprintf_s(type_string,
                    sizeof(type_string),
                    "%+i%% attack speed",
                    (int)(-(attack_speed_multiplier_.getValue().getValue() - 1.0f)*100.0f));
          break;

      case CompiledItemAvatarModifierMask::ACTOR_TYPE:
          // there is nothing to display for this parameter
          field_state = FIELDSTATE_DISABLED;
          break;

      case CompiledItemAvatarModifierMask::STEAL_HP:
        field_state = steal_hp_percent_.getFieldStateValue();
        sprintf_s(type_string,
                  sizeof(type_string),
                  "steal %+i%% HP",
                  (int)(steal_hp_percent_.getValue().getValue()));
        break;

      case CompiledItemAvatarModifierMask::STEAL_MP:
        field_state = steal_mp_percent_.getFieldStateValue();
        sprintf_s(type_string,
                  sizeof(type_string),
                  "steal %+i%% MP",
                  (int)(steal_mp_percent_.getValue().getValue()));
        break;

      default:
        DEBUG_ERROR("Unknown type in CompiledItemAvatarModifierMask:  %i", i);
        return false;
    }

    // Add to the output description
    if (field_state == FIELDSTATE_INNATE_ATTRIBUTE) {
      if (fields_added > 0) {
        description_text->append("\n");
      }
      description_text->append(type_string);
      ++fields_added;
    } else if (field_state == FIELDSTATE_PROPPED_ATTRIBUTE) {
      if (APP_ERROR(prop_fields >= 8)("Item has too many props (8 maximum")) {
        return false;
      }

      prop_field_descriptions[prop_fields].assign(type_string);
      ++prop_fields;
    }
  }

  // Success
  return true;
}


bool ItemAvatarModifier::compileForServer(bool is_weapon,
                                          Evidyon::CompiledItemAvatarModifier* modifier) const {
  using namespace Evidyon;

  int prop_fields = 0;

  modifier->always_applied.value = 0;
  modifier->props.value = 0;

  for (int i = 0; i < Evidyon::CompiledItemAvatarModifierMask::NUMBER_OF_FIELDS; ++i) {
    ItemAvatarModifierFieldState field_state;

    // Make sure the bit isn't already set.  This checks to make sure that the field type
    // entry in the enumeration is matched correctly with the bit index in the mask.
    ASSERT(0 == ((modifier->always_applied.value >> i) & 1)) else {
      DEBUG_ERROR("Field %i in CompiledItemAvatarModifierMask has the wrong index", i);
      return false;
    }

    switch ((CompiledItemAvatarModifierMask::FieldType)i) {
      case CompiledItemAvatarModifierMask::MIN_ATTACK_DAMAGE:
          field_state = is_weapon ? FIELDSTATE_INNATE_ATTRIBUTE : min_attack_damage_.getFieldStateValue();
          modifier->min_attack_damage = min_attack_damage_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::MAX_ATTACK_DAMAGE:
          if (is_weapon) {
            field_state = FIELDSTATE_INNATE_ATTRIBUTE;
            modifier->max_attack_damage = max_attack_damage_.getValue().getValue() -
                                          min_attack_damage_.getValue().getValue();
          } else {
            field_state = max_attack_damage_.getFieldStateValue();
            modifier->max_attack_damage = max_attack_damage_.getValue().getValue();
          }
          break;

      case CompiledItemAvatarModifierMask::PHYSICAL_DEFENSE:
          field_state = physical_defense_.getFieldStateValue();
          modifier->physical_defense = physical_defense_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::MAGICAL_DEFENSE:
          field_state = magical_defense_.getFieldStateValue();
          modifier->magical_defense = magical_defense_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::HP_REGEN_RATE:
          field_state = hp_regen_rate_.getFieldStateValue();
          modifier->hp_regen_rate = hp_regen_rate_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::MP_REGEN_RATE:
          field_state = mp_regen_rate_.getFieldStateValue();
          modifier->mp_regen_rate = mp_regen_rate_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::MAX_HP_MULTIPLIER:
          field_state = hp_multiplier_.getFieldStateValue();
          modifier->max_hp_multiplier = hp_multiplier_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::MAX_MP_MULTIPLIER:
          field_state = mp_multiplier_.getFieldStateValue();
          modifier->max_mp_multiplier = mp_multiplier_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::MAX_HP_ADDED:
          field_state = hp_added_.getFieldStateValue();
          modifier->max_hp_added = hp_added_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::MAX_MP_ADDED:
          field_state = mp_added_.getFieldStateValue();
          modifier->max_mp_added = mp_added_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::LEVEL:
          field_state = level_added_.getFieldStateValue();
          modifier->level = level_added_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::MOVEMENT_SPEED:
          field_state = movement_speed_multiplier_.getFieldStateValue();
          modifier->movement_speed_multiplier =
              movement_speed_multiplier_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::SPELL0:
          field_state = spell_to_make_available_[0].getFieldStateValue();
          {
            const dc::dcList<Spell>::Element* spell_element = 
              spell_to_make_available_[0].getValue().getReferencedResource();
            if (spell_element == NULL) {
              field_state = FIELDSTATE_DISABLED;
            } else {
              modifier->spells_to_make_available[0] = spell_element->getIndex();
            }
          }         
          break;

      case CompiledItemAvatarModifierMask::SPELL1:
          field_state = spell_to_make_available_[1].getFieldStateValue();
          {
            const dc::dcList<Spell>::Element* spell_element = 
              spell_to_make_available_[1].getValue().getReferencedResource();
            if (spell_element == NULL) {
              field_state = FIELDSTATE_DISABLED;
            } else {
              modifier->spells_to_make_available[1] = spell_element->getIndex();
            }
          }         
          break;

      case CompiledItemAvatarModifierMask::ATTACK_SPEED:
          field_state = attack_speed_multiplier_.getFieldStateValue();
          modifier->attack_speed_multiplier = attack_speed_multiplier_.getValue().getValue();
          break;

      case CompiledItemAvatarModifierMask::ACTOR_TYPE:
          // there is nothing to display for this parameter
          field_state = actor_type_.getFieldStateValue();
          actor_type_.getValue().compileForServer(modifier->actor_indices);
          break;

      case CompiledItemAvatarModifierMask::STEAL_HP:
          field_state = steal_hp_percent_.getFieldStateValue();
          modifier->steal_hp = steal_hp_percent_.getValue().getValue() / 100.0f;
          break;

      case CompiledItemAvatarModifierMask::STEAL_MP:
          field_state = steal_mp_percent_.getFieldStateValue();
          modifier->steal_mp = steal_mp_percent_.getValue().getValue() / 100.0f;
          break;

      default:
        DEBUG_ERROR("Unknown type in CompiledItemAvatarModifierMask:  %i", i);
        return false;
    }

    // Add to the output description
    if (field_state == FIELDSTATE_INNATE_ATTRIBUTE) {
      modifier->always_applied.value |= 1 << i;
    } else if (field_state == FIELDSTATE_PROPPED_ATTRIBUTE) {
      if (APP_ERROR(prop_fields >= 8)("Item has too many props (8 maximum")) {
        return false;
      }
      modifier->props.value |= 1 << i;
      ++prop_fields;
    }
  }

  // Success
  return true;
}



void ItemAvatarModifier::getCSVLine(std::string* output) const {
  std::string csv;
  csv += min_attack_damage_.toString() + ",";
  csv += max_attack_damage_.toString() + ",";
  csv += physical_defense_.toString() + ",";
  csv += magical_defense_.toString() + ",";
  csv += hp_regen_rate_.toString() + ",";
  csv += mp_regen_rate_.toString() + ",";
  csv += hp_multiplier_.toString() + ",";
  csv += mp_multiplier_.toString() + ",";
  csv += hp_added_.toString() + ",";
  csv += mp_added_.toString() + ",";
  csv += level_added_.toString() + ",";
  csv += movement_speed_multiplier_.toString() + ",";
  csv += spell_to_make_available_[0].toString() + ",";
  csv += spell_to_make_available_[1].toString() + ",";
  csv += attack_speed_multiplier_.toString() + ",";
  csv += actor_type_.toString() + ",";
  csv += steal_hp_percent_.toString() + ",";
  csv += steal_mp_percent_.toString();
  output->assign(csv);
}


std::string ItemAvatarModifier::staticTypeName() {
  return "ItemAvatarModifier";
}