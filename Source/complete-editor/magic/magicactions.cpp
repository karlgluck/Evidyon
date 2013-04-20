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

#include "shared/magic/tools/aoemagicdata.h"
#include "shared/magic/tools/magic.h"
#include "shared/magic/tools/magic_spinattack.h"
#include "shared/magic/tools/magic_attackrun.h"
#include "shared/magic/tools/magic_leap.h"
#include "shared/magic/tools/magic_buff.h"
#include "shared/magic/tools/magic_aoe.h"
#include "shared/magic/tools/magic_aoebuff.h"
#include "shared/magic/tools/magic_dot.h"
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
#include "shared/magic/tools/magic_heal.h"
#include "shared/magic/tools/magic_enchantment_armor.h"
#include "shared/magic/tools/magic_enchantment_shield.h"
#include "shared/magic/tools/magic_enchantment_reactivehealing.h"
#include "shared/magic/tools/magic_enchantment_thorns.h"







//----[  Magic_Portal::Type  ]-------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::Magic::Magic_Portal::Type>)
  ENUM_ACTIONS(Evidyon::Magic::Magic_Portal::Type),
END_DEFINING_ACTIONS()



//----[  Magic_AOE::Location  ]------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::Magic::AOEMagicData::Location>)
  ENUM_ACTIONS(Evidyon::Magic::AOEMagicData::Location),
END_DEFINING_ACTIONS()



BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::AOEMagicData)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_SpinAttack)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_AttackRun)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Leap)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Buff)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_AOE)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_AOEBuff)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_DOT)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_DOTAOEBuff)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_DirectDamage)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Projectile)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Branching)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Branching_Stage)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Portal)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Teleport)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Mutate)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Summon)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Trap_DimensionDoor)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Trap_BlackHole)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Disenchant)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Invisibility)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Heal)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Enchantment_Armor)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Enchantment_Shield)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Enchantment_ReactiveHealing)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Magic::Tools::Magic_Enchantment_Thorns)
END_DEFINING_ACTIONS()






//----[  SetMagicType  ]-------------------------------------------------------
int SetMagicType(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Tools::MagicInterface, magic_interface);


  std::string caption = "Set Magic Type";
  std::list<std::string> type_options;
  type_options.push_back("Spin Attack");
  type_options.push_back("Attack Run");
  type_options.push_back("Leap");
  type_options.push_back("Buff");
  type_options.push_back("AOE");
  type_options.push_back("AOE Buff");
  type_options.push_back("DOT");
  type_options.push_back("DOT - AOE + Buff");
  type_options.push_back("Direct Damage");
  type_options.push_back("Projectile");
  type_options.push_back("Branching");
  type_options.push_back("Portal");
  type_options.push_back("Teleport");
  type_options.push_back("Mutate");
  type_options.push_back("Summon");
  type_options.push_back("Trap - Dimension Door");
  type_options.push_back("Trap - Black Hole");
  type_options.push_back("Disenchant");
  type_options.push_back("Invisibility");
  type_options.push_back("Enchantment - Armor");
  type_options.push_back("Enchantment - Shield");
  type_options.push_back("Enchantment - Reactive Healing");
  type_options.push_back("Enchantment - Thorns");
  type_options.push_back("Heal");
  int selection_index = 0;
  if (!dcx::dcxWin32PromptForSelection(dialog->getWindowHandle(),
                                       caption.c_str(),
                                       "What kind of magic would you like?",
                                       &type_options,
                                       &selection_index,
                                       true)) {
    return 0;
  }

  Evidyon::World::Tools::MapLayer* layer = NULL;
#define IMPLEMENT(type) {magic_interface->implement<\
  Evidyon::Magic::Tools::type>()->setName(*type_iter);}

  std::list<std::string>::iterator type_iter = type_options.begin();
  for (int i = 0; i < selection_index; ++i) {
    ++type_iter;
  }
  switch (selection_index) {
    default:
    case 0: IMPLEMENT(Magic_SpinAttack); break;
    case 1: IMPLEMENT(Magic_AttackRun); break;
    case 2: IMPLEMENT(Magic_Leap); break;
    case 3: IMPLEMENT(Magic_Buff); break;
    case 4: IMPLEMENT(Magic_AOE); break;
    case 5: IMPLEMENT(Magic_AOEBuff); break;
    case 6: IMPLEMENT(Magic_DOT); break;
    case 7: IMPLEMENT(Magic_DOTAOEBuff); break;
    case 8: IMPLEMENT(Magic_DirectDamage); break;
    case 9: IMPLEMENT(Magic_Projectile); break;
    case 10: IMPLEMENT(Magic_Branching); break;
    case 11: IMPLEMENT(Magic_Portal); break;
    case 12: IMPLEMENT(Magic_Teleport); break;
    case 13: IMPLEMENT(Magic_Mutate); break;
    case 14: IMPLEMENT(Magic_Summon); break;
    case 15: IMPLEMENT(Magic_Trap_DimensionDoor); break;
    case 16: IMPLEMENT(Magic_Trap_BlackHole); break;
    case 17: IMPLEMENT(Magic_Disenchant); break;
    case 18: IMPLEMENT(Magic_Invisibility); break;
    case 19: IMPLEMENT(Magic_Enchantment_Armor); break;
    case 20: IMPLEMENT(Magic_Enchantment_Shield); break;
    case 21: IMPLEMENT(Magic_Enchantment_ReactiveHealing); break;
    case 22: IMPLEMENT(Magic_Enchantment_Thorns); break;
    case 23: IMPLEMENT(Magic_Heal); break;
  }
#undef IMPLEMENT

  return 1;
}

//----[  dcInterface<Magic>  ]-------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::Magic::Tools::Magic>)
  { "Set Type...", &SetMagicType },
END_DEFINING_ACTIONS()

