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

#include "shared/trigger/tools/trigger.h"
#include "shared/trigger/tools/trigger_maplink.h"




//----[  SetTriggerType  ]-----------------------------------------------------
int SetTriggerType(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Tools::TriggerInterface, trigger_interface);


  std::string caption = "Set Trigger Type";
  std::list<std::string> type_options;
  type_options.push_back("Map Link");
  int selection_index = 0;
  if (!dcx::dcxWin32PromptForSelection(dialog->getWindowHandle(),
                                       caption.c_str(),
                                       "What kind of trigger would you like?",
                                       &type_options,
                                       &selection_index,
                                       true)) {
    return 0;
  }

#define IMPLEMENT(type) {trigger_interface->implement<\
  Evidyon::Trigger::Tools::type>()->setName(*type_iter);}

  std::list<std::string>::iterator type_iter = type_options.begin();
  for (int i = 0; i < selection_index; ++i) {
    ++type_iter;
  }
  switch (selection_index) {
    default:
    case 0: IMPLEMENT(Trigger_MapLink); break;
  }
#undef IMPLEMENT

  return 1;
}


//----[  Trigger  ]---------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Trigger::Tools::Trigger_MapLink)
END_DEFINING_ACTIONS()





//----[  dc::dcList<Trigger>  ]---------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::Trigger::Tools::Trigger>)
  { "Set Type...", &SetTriggerType },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::TriggerTable)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::TriggerElement)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::TriggerReference)
  LIST_REFERENCE_ACTIONS(Evidyon::Tools::TriggerReference),
END_DEFINING_ACTIONS()
