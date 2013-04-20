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
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "shared/treasure/tools/treasureweights.h"
#include "shared/treasure/tools/globaltreasure.h"
#include "shared/treasure/tools/monstertreasureitems.h"
#include "shared/treasure/tools/monstertreasuregroup.h"
#include "shared/treasure/tools/monstertreasure.h"
#include "shared/treasure/tools/treasurecompiler.h"


BEGIN_DEFINING_ACTIONS(Evidyon::Treasure::Tools::TreasureWeights)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::Treasure::Tools::GlobalTreasure)
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(Evidyon::Treasure::Tools::MonsterTreasureGroup)
END_DEFINING_ACTIONS()




//----[  ViewMonsterDropProbabilities  ]---------------------------------------
#include "shared/treasure/tools/treasurecompiler.h"
#include "shared/treasure/treasureclass.h"
#include <map>
int ViewMonsterDropProbabilities(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Treasure;
  using namespace Evidyon::Treasure::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(MonsterTreasure, treasure);

  TreasureCompiler compiler;
  compiler.initialize(tool->getGlobalTreasure());

  treasure->compile(&compiler);

  typedef std::map<Evidyon::Item::ItemIndex, double> DropsMap;
  DropsMap drops;

  // Search the treasure for the actual drop numbers.  Factor drop numbers
  // by the equipment type's drop %.
  float percentage_floats[NUMBER_OF_TREASURE_GROUPS];
  tool->getGlobalTreasure()->getDropPercentages(
    &percentage_floats[TREASUREGROUP_COMMON_EQUIPMENT],
    &percentage_floats[TREASUREGROUP_UNCOMMON_EQUIPMENT],
    &percentage_floats[TREASUREGROUP_RARE_EQUIPMENT],
    &percentage_floats[TREASUREGROUP_CONSUMABLE]);

  double percentages[NUMBER_OF_TREASURE_GROUPS] = {
    percentage_floats[TREASUREGROUP_COMMON_EQUIPMENT]
      / double(Evidyon::Treasure::TreasureClass::COMMON_LENGTH),
    percentage_floats[TREASUREGROUP_UNCOMMON_EQUIPMENT]
      / double(Evidyon::Treasure::TreasureClass::UNCOMMON_LENGTH),
    percentage_floats[TREASUREGROUP_RARE_EQUIPMENT]
      / double(Evidyon::Treasure::TreasureClass::RARE_LENGTH),
    percentage_floats[TREASUREGROUP_CONSUMABLE]
      / double(Evidyon::Treasure::TreasureClass::CONSUMABLE_LENGTH),
  };

  Evidyon::Treasure::TreasureClass treasure_class;
  compiler.compileTreasureClass(0, &treasure_class);

  for (int i = 0; i < NUMBER_OF_TREASURE_GROUPS; ++i) {
    int entries;
    Evidyon::Item::ItemIndex* list;
    switch (i) {
      default: assert(false); return 0;
      case TREASUREGROUP_COMMON_EQUIPMENT:
        entries = Evidyon::Treasure::TreasureClass::COMMON_LENGTH;
        list = treasure_class.common_list;
        break;
      case TREASUREGROUP_UNCOMMON_EQUIPMENT:
        entries = Evidyon::Treasure::TreasureClass::UNCOMMON_LENGTH;
        list = treasure_class.uncommon_list;
        break;
      case TREASUREGROUP_RARE_EQUIPMENT:
        entries = Evidyon::Treasure::TreasureClass::RARE_LENGTH;
        list = treasure_class.rare_list;
        break;
      case TREASUREGROUP_CONSUMABLE:
        entries = Evidyon::Treasure::TreasureClass::CONSUMABLE_LENGTH;
        list = treasure_class.consumable_list;
        break;
    }
    for (int j = 0; j < entries; ++j) {
      DropsMap::iterator d = drops.find(list[j]);
      if (d != drops.end()) {
        d->second += percentages[i];
      } else {
        drops.insert(DropsMap::value_type(list[j], percentages[i]));
      }
    }
  }

  // Turn everything into a string
  std::string drop_string;
  for (DropsMap::iterator i = drops.begin(); i != drops.end(); ++i) {
    char str[512];
    const char* item_name = "(nothing)";
    const dc::dcGenericResource::Array& items = tool->getItems();
    if (items.size() > i->first) {
      item_name = items.at(i->first)->getName().c_str();
    }
    sprintf_s(str, 512, "%2.02f - %s\n", i->second * 100.0, item_name);
    drop_string += str;
  }

  MessageBox(tool->topWindow(), drop_string.c_str(), "Drops", MB_OK);

  return 1;
}


BEGIN_DEFINING_ACTIONS(Evidyon::Treasure::Tools::MonsterTreasure)
  { "View Drops", &ViewMonsterDropProbabilities },
END_DEFINING_ACTIONS()





//----[  AddMonsterTreasure  ]-------------------------------------------------
int AddMonsterTreasure(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Treasure::Tools::MonsterTreasureItems, list);
  dc::dcGenericResource::Array selection;
  if (DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Add Monster Treasure",
        "Pick items to add to this monster's treasure list",
        tool->getItems(),
        &selection,
        false)) {

    for (dc::dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i) {
      Evidyon::Tools::ItemElement* element = (Evidyon::Tools::ItemElement*)(*i);
      Evidyon::Treasure::Tools::MonsterTreasureItems::Element* generated
        = (Evidyon::Treasure::Tools::MonsterTreasureItems::Element*)
          list->generateElement(element->getName());
      generated->getItem()->setReferencedResource(element);
    }
  }

  return 1;
}




BEGIN_DEFINING_ACTIONS(Evidyon::Treasure::Tools::MonsterTreasureItems)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Items...",
    &AddMonsterTreasure },
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(Evidyon::Treasure::Tools::MonsterTreasureItems::Element)
END_DEFINING_ACTIONS()