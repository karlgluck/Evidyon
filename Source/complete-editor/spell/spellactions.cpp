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
#include "shared/evidyontool/directcraftactions.h"
#include "shared/world/tools/mapmask.h"
#include "shared/world/tools/mapareareference.h"
#include "shared/world/tools/maplocationreference.h"
#include "shared/world/tools/map.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"

#include <assert.h>

#include "shared/spell/tools/spell.h"
#include "shared/spell/tools/avatarclassspells.h"
#include "common/kg/gid/str.h"



//----[  Spell  ]--------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Spell::Tools::Spell)
END_DEFINING_ACTIONS()

//----[  SelectSpell  ]---------------------------------------------------------
int SelectSpell(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Tools::SpellReference, reference);

  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Spell",
        "Pick the spell to reference",
        tool->getSpells(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}



int AddSpellOfEveryType(dc::dcGenericResource* resource, void* param);
int ImportMasterDesignSpreadsheetSpells(dc::dcGenericResource* resource, void* param);

//----[  dc::dcList<Spell>  ]---------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Spell::Tools::Spell>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Spell...",
  &DirectCraftActions::GenerateCollectionElement<dc::dcList<Evidyon::Spell::Tools::Spell>> },
  { "Add Spell of Every Type",
  &AddSpellOfEveryType },
  { "Import Master Design Spreadsheet",
  &ImportMasterDesignSpreadsheetSpells },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Spell::Tools::Spell>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Spell::Tools::Spell>::Reference)
  { "Select...", &SelectSpell },
  LIST_REFERENCE_ACTIONS(Evidyon::Spell::Tools::Spell),
END_DEFINING_ACTIONS()




//----[  AddSpellsToAvatarClassSpells  ]---------------------------------------
int AddSpellsToAvatarClassSpells(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Spell::Tools::AvatarClassSpells, list);

  dc::dcGenericResource::Array selection;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Select Spells",
        "Pick the spells to add",
        tool->getSpells(),
        &selection,
        false)) {
    return false;
  }

  int level = 0;
  if (!dcx::dcxWin32PromptNumericInput(
        tool->topWindow(),
        "Add Spells - Select Level",
        "At which level should these spells be available?",
        &level,
        false)) {
    return false;
  }

  for (dc::dcGenericResource::Array::iterator i = selection.begin();
        i != selection.end(); ++i) {
    Evidyon::Tools::SpellElement* spell
      = (Evidyon::Tools::SpellElement*)(*i);

    typedef Evidyon::Spell::Tools::AvatarClassSpells::Element Element;
    Element* element
      = (Element*)(Evidyon::Tools::SpellElement*)list->generateElement(spell->getName());

    element->set(level, spell);
  }

  return 0;
}



//----[  AvatarClassSpells  ]--------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Spell::Tools::AvatarClassSpells)
  COLLECTION_ACTIONS,
  { "Add Spells...",
  &AddSpellsToAvatarClassSpells },
END_DEFINING_ACTIONS()
dc::dcAction dc::dcClass<Evidyon::Spell::Tools::AvatarClassSpells::Element>::actions[] = { NULL, 0 };




#include "shared/magic/tools/magic_spinattack.h"
#include "shared/magic/tools/magic_attackrun.h"
#include "shared/magic/tools/magic_leap.h"
#include "shared/magic/tools/magic_buff.h"
#include "shared/magic/tools/magic_aoe.h"
#include "shared/magic/tools/magic_aoebuff.h"
#include "shared/magic/tools/magic_dotaoebuff.h"
#include "shared/magic/tools/magic_directdamage.h"
#include "shared/magic/tools/magic_projectile.h"
#include "shared/magic/tools/magic_branching.h"
#include "shared/magic/tools/magic_portal.h"
#include "shared/magic/tools/magic_teleport.h"
#include "shared/magic/tools/magic_mutate.h"
#include "shared/magic/tools/magic_summon.h"
#include "shared/magic/tools/magic_trap_dimensiondoor.h"
#include "shared/magic/tools/magic_trap_blackhole.h"
#include "shared/magic/tools/magic_disenchant.h"
#include "shared/magic/tools/magic_invisibility.h"
#include "shared/magic/tools/magic_enchantment_armor.h"
#include "shared/magic/tools/magic_enchantment_shield.h"
#include "shared/magic/tools/magic_enchantment_reactivehealing.h"
#include "shared/magic/tools/magic_enchantment_thorns.h"


//----[  AddSpellOfEveryType  ]------------------------------------------------
int AddSpellOfEveryType(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Tools::SpellList, list);
  Evidyon::Tools::SpellElement* element;
#define IMPLEMENT(t)\
  element = (Evidyon::Tools::SpellElement*)list->generateElement(#t);\
  element->implement(#t, dc::dcClass<Evidyon::Magic::Tools::t>::staticResourceManager()->uniqueID());

IMPLEMENT(Magic_SpinAttack)
IMPLEMENT(Magic_AttackRun)
IMPLEMENT(Magic_Leap)
IMPLEMENT(Magic_Buff)
IMPLEMENT(Magic_AOE)
IMPLEMENT(Magic_AOEBuff)
IMPLEMENT(Magic_DOTAOEBuff)
IMPLEMENT(Magic_DirectDamage)
IMPLEMENT(Magic_Projectile)
IMPLEMENT(Magic_Branching)
IMPLEMENT(Magic_Branching_Stage)
IMPLEMENT(Magic_Portal)
IMPLEMENT(Magic_Teleport)
IMPLEMENT(Magic_Mutate)
IMPLEMENT(Magic_Summon)
IMPLEMENT(Magic_Trap_DimensionDoor)
IMPLEMENT(Magic_Trap_BlackHole)
IMPLEMENT(Magic_Disenchant)
IMPLEMENT(Magic_Invisibility)
IMPLEMENT(Magic_Enchantment_Armor)
IMPLEMENT(Magic_Enchantment_Shield)
IMPLEMENT(Magic_Enchantment_ReactiveHealing)
IMPLEMENT(Magic_Enchantment_Thorns)

  return 1;
}


//----[  ImportMasterDesignSpreadsheetSpells  ]--------------------------------
int ImportMasterDesignSpreadsheetSpells(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  using namespace Evidyon::Tools;
  CAST_ACTION_RESOURCE(SpellList, list);

  char file[MAX_PATH];
  if (!dcx::dcxOpenFileDialog(
      tool->topWindow(),
      "Select Spell Spreadsheet",
      "Tab-Delimited Spreadsheet File (*.TXT)\0*.TXT\0\0",
      file,
      MAX_PATH)) {
    return 0;
  }
  
  FILE* fp;
  fp = fopen(file, "rt");
  bool is_header_row = true;
  std::vector<std::string> headers;
  while (!feof(fp)) {
    char line[1024];
    fscanf_s(fp, "%[^\n]\n ", line, 1024);
    if (is_header_row) {
      split_str(line, '\t', headers);
      is_header_row = false;
    } else {
      std::vector<std::string> fields;
      split_str(line, '\t', fields);
      std::map<std::string, std::string> values;
      zip(headers, fields, values);
      const dc::dcGenericResource::Array& spells = list->getMembers();
      dc::dcGenericResource::Array::const_iterator existing_spell = spells.begin();
      const std::string& name = values.find("Name")->second;
      if (name.empty() == false) {
        while (existing_spell != spells.end()) {
          if (0 == stricmp((*existing_spell)->getName().c_str(), name.c_str())) break;
          ++existing_spell;
        }
        SpellElement* spell = (SpellElement*)(
          (existing_spell != spells.end()) ? (*existing_spell) : list->generateElement(name));
        spell->set(&values);
      }
    }
  }
  fclose(fp);
  return 1;
}




//----[  Spell  ]--------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Spell::Tools::SpellStage)
END_DEFINING_ACTIONS()



//----[  dc::dcList<Spell>  ]---------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::SpellStageList)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Stage...",
  &DirectCraftActions::GenerateCollectionElement<Evidyon::Tools::SpellStageList> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::SpellStageElement)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::SpellStageReference)
END_DEFINING_ACTIONS()


