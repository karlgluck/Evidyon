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
#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "skin.h"
#include "mesh.h"
#include "scenery.h"
#include "avatarmodifier.h"
#include "wavsound.h"

#include "avatarmodifier.h"
#include "item.h"
#include "magic.h"

//#include "../shared/client-editor/client_gamefile.h"
#include "../shared/server-editor/server_gamefile.h"



//------------------------------------------------------------------------------------------------
// Name:  getString
// Desc:  Implements the enum-index-to-string conversion for dc::dcEnum<EquipmentSlot>
//------------------------------------------------------------------------------------------------
const char* dc::dcEnum<EquipmentSlot>::getString(EquipmentSlot value)
{
    switch(value)
    {
        case EQUIPMENTSLOT_NONE:        return "None";
        case EQUIPMENTSLOT_ACCESSORY:   return "Accessory";
        case EQUIPMENTSLOT_RING:        return "Ring";
        case EQUIPMENTSLOT_2HWEAPON:    return "2H Weapon";
        case EQUIPMENTSLOT_1HWEAPON:    return "1H Weapon";
        case EQUIPMENTSLOT_BELT:        return "Belt";
        case EQUIPMENTSLOT_BOOTS:       return "Boots";
        case EQUIPMENTSLOT_BODYARMOR:   return "Body Armor";
        case EQUIPMENTSLOT_GLOVES:      return "Gloves";
        case EQUIPMENTSLOT_HELMET:      return "Helmet";
        case EQUIPMENTSLOT_SHIELD:      return "Shield";
        default: return 0;
    }
}




//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Implements the class name for dc::dcEnum<EquipmentSlot>
//------------------------------------------------------------------------------------------------
std::string dc::dcEnum<EquipmentSlot>::staticTypeName()
{
    return "EquipmentSlot";
}






//------------------------------------------------------------------------------------------------
// Name: Item
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
Item::Item() {
  member("Displayed Name", &displayed_name_);
  member("Equipment Slot", &myEquipmentSlot);
  member("Description", &myDescription);
  member("Scenery Representation", &mySceneryRepresentation);
  member("Max Stack Size", &max_stack_size_);
  member("Treasure Level", &treasure_level_);
  member("Geonite Value", &geonite_value_);
  member("Consumable: Magic", &consumable_magic_);
  member("Consumable: Magic Needs Target?", &consumable_needs_target_);
  member("Equipped Modifier", &avatar_modifier_);

  max_stack_size_.setValue(1);
  geonite_value_.setValue(1);
}


unsigned int HashString(const std::string& string) {

  // Make sure the length is aligned on a 16-bit boundary
  std::string ts = string;
  if( ts.length() % 2 == 1 )
      ts += '%';

  const unsigned short *data = (unsigned short*)ts.data();  // Pointer to the data to be summed
  size_t len = ts.length() / 2;      // Length in 16-bit words
  unsigned long sum1 = 0xffff, sum2 = 0xffff;

  while (len) {
      size_t tlen = len > 360 ? 360 : len;
      len -= tlen;
      do {
          sum1 += *data++;
          sum2 += sum1;
      } while (--tlen);
      sum1 = (sum1 & 0xffff) + (sum1 >> 16);
      sum2 = (sum2 & 0xffff) + (sum2 >> 16);
  }

  // Second reduction step to reduce sums to 16 bits
  sum1 = (sum1 & 0xffff) + (sum1 >> 16);
  sum2 = (sum2 & 0xffff) + (sum2 >> 16);

  // Return the code
  return sum2 << 16 | sum1;
}


//------------------------------------------------------------------------------------------------
// Name: fillServerDescription
// Desc: Enters data into the server effect structure
//------------------------------------------------------------------------------------------------
bool Item::fillServerDescription(ServerItemDescription* description) const
{
    // Generate the hash value using this structure's name
    description->permanent_storage_hash = HashString(getName());

    description->geonite_value = geonite_value_.getValue();

    description->equipment_slot_mask.item_initialize();
    description->fills_multiple_slots = false;
    description->equipped_attachment_point = ACTORATTACHMENTPOINT_INVALID;

    // Save the scenery index
    const dc::dcList<Scenery>::Element* referencedElement = mySceneryRepresentation.getReferencedResource();
    description->equipped_scenery_index = referencedElement ? referencedElement->getIndex() : 0xFFFFFFFF;

    // For now, just use the scenery as the ground type
    description->ground_type_index = description->equipped_scenery_index;

    // Set the equipment slot
    bool is_weapon = false;
    switch(myEquipmentSlot.getValue())
    {
        default: DEBUG_WARNING("Item equipment slot was invalid; defaulting to NONE");

        case EQUIPMENTSLOT_NONE: // this item can't be equipped
          break;

        case EQUIPMENTSLOT_ACCESSORY:  
                description->equipment_slot_mask.fields.accessory0 = 1;
                description->equipment_slot_mask.fields.accessory1 = 1;
                description->equipment_slot_mask.fields.accessory2 = 1;
            break;

        case EQUIPMENTSLOT_RING:
                description->equipment_slot_mask.fields.ring0 = 1;
                description->equipment_slot_mask.fields.ring1 = 1;
                description->equipment_slot_mask.fields.ring2 = 1;
                description->equipment_slot_mask.fields.ring3 = 1;
                description->equipment_slot_mask.fields.ring4 = 1;
                description->equipment_slot_mask.fields.ring5 = 1;
            break;

        case EQUIPMENTSLOT_2HWEAPON:
                is_weapon = true;
                description->fills_multiple_slots = true; // this weapon fills multiple slots
                description->equipment_slot_mask.fields.shieldHand = 1;
                description->equipment_slot_mask.fields.weaponHand = 1;
                description->equipped_attachment_point = ACTORATTACHMENTPOINT_WEAPON;
            break;

        case EQUIPMENTSLOT_1HWEAPON:    is_weapon = true;
                                        description->equipment_slot_mask.fields.weaponHand = 1;
                                        description->equipped_attachment_point = ACTORATTACHMENTPOINT_WEAPON;
                                        break;
        case EQUIPMENTSLOT_BELT:        description->equipment_slot_mask.fields.belt = 1;       break;
        case EQUIPMENTSLOT_BOOTS:       description->equipment_slot_mask.fields.boots = 1;      break;
        case EQUIPMENTSLOT_BODYARMOR:   description->equipment_slot_mask.fields.bodyArmor = 1;  break;
        case EQUIPMENTSLOT_GLOVES:      description->equipment_slot_mask.fields.gloves = 1;     break;
        case EQUIPMENTSLOT_HELMET:      description->equipment_slot_mask.fields.helmet = 1;
                                        description->equipped_attachment_point = ACTORATTACHMENTPOINT_HELMET; break;
        case EQUIPMENTSLOT_SHIELD:      description->equipment_slot_mask.fields.shieldHand = 1;
                                        description->equipped_attachment_point = ACTORATTACHMENTPOINT_SHIELD;
                                        break;
    }

    // generate the item's effect
    CONFIRM(avatar_modifier_.compileForServer(is_weapon,
                                             &description->avatar_modifier)) else return false;

    { // set up the consumable effect
      description->consumable_magic = consumable_magic_.getReferencedResourceIndex();
      //const dc::dcList<Spell>::Element* spell_element = consumable_effect_.getReferencedResource();
      //if (NULL == spell_element) {
      //  description->consumable_spell = -1;
      //} else {
      //  description->consumable_spell = spell_element->getIndex();
      //}
    }

    // Save other information
    description->max_stack_size = max_stack_size_.getValue();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool Item::compileForClient(ClientItemDescription* description) const
{
    // Save text strings
    strcpy_s(description->name,
             CLIENTGAMEFILE_ITEM_NAME_STRING_LENGTH,
             displayed_name_.getValue().c_str());

    // Get the item's type
    std::string descriptionText;
    bool is_weapon = false;
    switch(myEquipmentSlot.getValue())
    {
        default:    DEBUG_ERROR("Equipment slot type unknown!  defaulting to accessory");
        case EQUIPMENTSLOT_NONE:
        case EQUIPMENTSLOT_ACCESSORY:   descriptionText = ""; break;
        case EQUIPMENTSLOT_RING:        descriptionText = "Ring\n"; break;
        case EQUIPMENTSLOT_2HWEAPON:    descriptionText = "2H Weapon\n"; is_weapon = true; break;
        case EQUIPMENTSLOT_1HWEAPON:    descriptionText = "1H Weapon\n"; is_weapon = true; break;
        case EQUIPMENTSLOT_BELT:        descriptionText = "Belt\n"; break;
        case EQUIPMENTSLOT_BOOTS:       descriptionText = "Boots\n"; break;
        case EQUIPMENTSLOT_BODYARMOR:   descriptionText = "Body Armor\n"; break;
        case EQUIPMENTSLOT_GLOVES:      descriptionText = "Gloves\n"; break;
        case EQUIPMENTSLOT_HELMET:      descriptionText = "Helmet\n"; break;
        case EQUIPMENTSLOT_SHIELD:      descriptionText = "Shield\n"; break;
    }

    bool has_description = false == myDescription.getValue().empty();
    if (has_description) {
      descriptionText.append(myDescription.getValue().c_str());
      descriptionText.append("\n");
    }

    descriptionText.append("" + geonite_value_.toString() + " geonite\n");

    { // save the description of this item and its props
      std::string modifier_description;
      std::string prop_field_descriptions[8];
      CONFIRM(avatar_modifier_.compileForClient(is_weapon,
                                               &modifier_description,
                                                prop_field_descriptions)) else return false;
      descriptionText.append(modifier_description);

      strcpy_s(description->description,
               CLIENTGAMEFILE_ITEM_DESC_STRING_LENGTH,
               descriptionText.c_str());

      for (int i = 0; i < 8; ++i) {
        strcpy_s(description->prop_field_descriptions[i],
                 CLIENTGAMEFILE_ITEM_PROP_FIELD_DESC_LENGTH,
                 prop_field_descriptions[i].c_str());
      }
    }

    { // save whether or not this is a consumable item
      const dc::dcTable<Magic>::Element* magic_element = consumable_magic_.getReferencedResource();
      if (NULL == magic_element) {
        description->consumable = false;
        description->consuming_requires_target = false;
      } else {
        description->consumable = true;
        description->consuming_requires_target = consumable_needs_target_.getValue();
      }
    }

    // Save the scenery index
    const dc::dcList<Scenery>::Element* referencedElement = mySceneryRepresentation.getReferencedResource();
    description->sceneryRepresentationIndex = referencedElement ? referencedElement->getIndex() : 0xFFFFFFFF;


    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: createCSVLine
// Desc: 
//------------------------------------------------------------------------------------------------
void Item::createCSVLine(char* buffer, size_t buffer_size) const {
  std::string update_line;
  update_line += displayed_name_.toString() + ",";
  update_line += myEquipmentSlot.toString() + ",";
  update_line += myDescription.toString() + ",";
  update_line += mySceneryRepresentation.toString() + ",";
  update_line += max_stack_size_.toString() + ",";
  update_line += treasure_level_.toString() + ",";
  update_line += geonite_value_.toString() + ",";
  update_line += consumable_magic_.toString() + ",";
  update_line += consumable_needs_target_.toString() + ",";
  std::string avatar_modifier_csv;
  avatar_modifier_.getCSVLine(&avatar_modifier_csv);
  update_line += avatar_modifier_csv;

  strcpy_s(buffer, buffer_size, update_line.c_str());
}


//------------------------------------------------------------------------------------------------
// Name: setSceneryRepresentation
// Desc: Sets the scenery visualization of this item
//------------------------------------------------------------------------------------------------
void Item::setSceneryRepresentation(dcList<Scenery>::Element* scenery)
{
    mySceneryRepresentation.setReferencedResource(scenery);
}



//------------------------------------------------------------------------------------------------
// Name: compareTo
// Desc: Used to sort this list.  If the static "compiling" flag is set, the items are
//       sorted by type ID.  Otherwise, they are sorted by their equipment slot.
//------------------------------------------------------------------------------------------------
int Item::compareTo(const dc::dcGenericResource* other) const
{
    if (other == 0 || getTypeID() != other->getTypeID())
        return dc::dcGenericResource::compareTo(other);
    else {
       int compare = myEquipmentSlot.compareTo(&((const Item*)other)->myEquipmentSlot);
       if (compare != 0) return compare;
       else return dc::dcGenericResource::compareTo(other);
    }
}





//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string Item::staticTypeName()
{
    return "Item";
}

