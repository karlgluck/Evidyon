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
#include "shared/specialfx/tools/specialfx.h"
#include "shared/specialfx/tools/specialfx_soundand3visualfx.h"
#include "shared/specialfx/tools/specialfx_soundandvisualfx.h"
#include "shared/specialfx/tools/specialfx_soundvisualfxandemitter.h"
#include "shared/specialfx/tools/specialfxbinding_offset.h"
#include "shared/specialfx/tools/specialfxbinding_oscillation.h"
#include "shared/specialfx/tools/specialfxbinding_orbit.h"
#include "shared/specialfx/tools/specialfxbinding_slider.h"
#include "shared/specialfx/tools/specialfxbinding_sourcelocation.h"
#include "shared/specialfx/tools/specialfxbinding_targetlocation.h"
#include "shared/specialfx/tools/specialfxbinding_sourceobject.h"
#include "shared/specialfx/tools/specialfxbinding_targetobject.h"
#include "shared/specialfx/tools/specialfxemitter.h"
#include "shared/specialfx/tools/specialfxemitter_interval.h"
#include "specialfxeditingdialog.h"




//----[  EditSpecialFX  ]------------------------------------------------------
int EditSpecialFX(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  typedef dc::dcTable<Evidyon::SpecialFX::Tools::SpecialFX> Table;
  CAST_ACTION_RESOURCE(Table, table);

  Evidyon::SpecialFXEditingDialog* special_fx_editing_dialog
    = new Evidyon::SpecialFXEditingDialog();
  if (!special_fx_editing_dialog->begin(
        tool,
        tool->getSpecialFX())) {
    delete special_fx_editing_dialog;
    return 0;
  }

  // Dialog will be changing the fx
  return 1;
}





BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFX_SoundAndVisualFX)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFX_SoundAnd3VisualFX)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFX_SoundVisualFXAndEmitter)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFXBinding_Offset)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFXBinding_Oscillation)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFXBinding_Orbit)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFXBinding_SourceLocation)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFXBinding_TargetLocation)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFXBinding_SourceObject)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFXBinding_TargetObject)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFXBinding_Slider)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::SpecialFX::Tools::SpecialFXEmitter_Interval)
END_DEFINING_ACTIONS()



//----[  SelectSpecialFX  ]------------------------------------------------------
int SelectSpecialFX(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::SpecialFX::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<SpecialFX>::Reference, reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select SpecialFX",
        "Pick the special fx to reference",
        tool->getSpecialFX(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}


//----[  SpecialFXTable  ]-----------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::SpecialFXTable)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Edit...", &EditSpecialFX },
  ACTION_LIST_SEPARATOR,
  { "Add Sound && Visual FX...",
  &DirectCraftActions::GenerateTableElement<
    Evidyon::SpecialFX::Tools::SpecialFX,
    Evidyon::SpecialFX::Tools::SpecialFX_SoundAndVisualFX> },
  { "Add Sound && 3 Visual FX...",
  &DirectCraftActions::GenerateTableElement<
    Evidyon::SpecialFX::Tools::SpecialFX,
    Evidyon::SpecialFX::Tools::SpecialFX_SoundAnd3VisualFX> },
  { "Add Sound, Visual FX && Emitter...",
  &DirectCraftActions::GenerateTableElement<
    Evidyon::SpecialFX::Tools::SpecialFX,
    Evidyon::SpecialFX::Tools::SpecialFX_SoundVisualFXAndEmitter> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::SpecialFXElement)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(Evidyon::Tools::SpecialFXReference)
  { "Select...", &SelectSpecialFX },
  { "Clear", &DirectCraftActions::ClearReference },
END_DEFINING_ACTIONS()



//----[  SetSpecialFXType  ]---------------------------------------------------
int SetSpecialFXType(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  typedef dc::dcInterface<Evidyon::SpecialFX::Tools::SpecialFX> SpecialFXInterface;
  CAST_ACTION_RESOURCE(SpecialFXInterface, specialfx_interface);


  std::string caption = "Set SpecialFX Type";
  std::list<std::string> type_options;
  type_options.push_back("Sound and Visual FX");
  type_options.push_back("Sound and 3 Visual FX");
  type_options.push_back("Sound, Visual FX and Emitter");
  int selection_index = 0;
  if (!dcx::dcxWin32PromptForSelection(dialog->getWindowHandle(),
                                       caption.c_str(),
                                       "What kind of special effect would you like to use?",
                                       &type_options,
                                       &selection_index,
                                       true)) {
    return 0;
  }

  Evidyon::World::Tools::MapLayer* layer = NULL;
#define IMPLEMENT(type) {specialfx_interface->implement<\
  Evidyon::SpecialFX::Tools::type>()->setName(*type_iter);}

  std::list<std::string>::iterator type_iter = type_options.begin();
  for (int i = 0; i < selection_index; ++i) {
    ++type_iter;
  }
  switch (selection_index) {
    default:
    case 0: IMPLEMENT(SpecialFX_SoundAndVisualFX); break;
    case 1: IMPLEMENT(SpecialFX_SoundAnd3VisualFX); break;
    case 2: IMPLEMENT(SpecialFX_SoundVisualFXAndEmitter); break;
  }
#undef IMPLEMENT

  return 1;
}





BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::SpecialFX::Tools::SpecialFX>)
  { "Set Type...", &SetSpecialFXType },
END_DEFINING_ACTIONS()






//----[  SetSpecialFXBindingType  ]--------------------------------------------
int SetSpecialFXBindingType(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  typedef dc::dcInterface<Evidyon::SpecialFX::Tools::SpecialFXBinding> SpecialFXBindingInterface;
  CAST_ACTION_RESOURCE(SpecialFXBindingInterface, specialfx_interface);


  std::string caption = "Set SpecialFX Binding Type";
  std::list<std::string> type_options;
  type_options.push_back("Offset");
  type_options.push_back("Orbit");
  type_options.push_back("Oscillation");
  type_options.push_back("Slider");
  type_options.push_back("Source Location");
  type_options.push_back("Source Object");
  type_options.push_back("Target Location");
  type_options.push_back("Target Object");
  int selection_index = 0;
  if (!dcx::dcxWin32PromptForSelection(dialog->getWindowHandle(),
                                       caption.c_str(),
                                       "What kind of binding would you like to use?",
                                       &type_options,
                                       &selection_index,
                                       true)) {
    return 0;
  }

  Evidyon::World::Tools::MapLayer* layer = NULL;
#define IMPLEMENT(type) {specialfx_interface->implement<\
  Evidyon::SpecialFX::Tools::type>()->setName(*type_iter);}

  std::list<std::string>::iterator type_iter = type_options.begin();
  for (int i = 0; i < selection_index; ++i) {
    ++type_iter;
  }
  switch (selection_index) {
    default:
    case 0: IMPLEMENT(SpecialFXBinding_Offset); break;
    case 1: IMPLEMENT(SpecialFXBinding_Orbit); break;
    case 2: IMPLEMENT(SpecialFXBinding_Oscillation); break;
    case 3: IMPLEMENT(SpecialFXBinding_Slider); break;
    case 4: IMPLEMENT(SpecialFXBinding_SourceLocation); break;
    case 5: IMPLEMENT(SpecialFXBinding_SourceObject); break;
    case 6: IMPLEMENT(SpecialFXBinding_TargetLocation); break;
    case 7: IMPLEMENT(SpecialFXBinding_TargetObject); break;
  }
#undef IMPLEMENT

  return 1;
}






//----[  dcInterface<SpecialFXBinding>  ]--------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::SpecialFX::Tools::SpecialFXBinding>)
  { "Set Type...", &SetSpecialFXBindingType },
END_DEFINING_ACTIONS()








//----[  SetSpecialFXEmitterType  ]--------------------------------------------
int SetSpecialFXEmitterType(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  typedef dc::dcInterface<Evidyon::SpecialFX::Tools::SpecialFXEmitter>
      SpecialFXEmitterInterface;
  CAST_ACTION_RESOURCE(SpecialFXEmitterInterface, emitter_interface);


  std::string caption = "Set SpecialFX Emitter Type";
  std::list<std::string> type_options;              
  type_options.push_back("Interval");
  int selection_index = 0;
  if (!dcx::dcxWin32PromptForSelection(dialog->getWindowHandle(),
                                       caption.c_str(),
                                       "What kind of emitter would you like to use?",
                                       &type_options,
                                       &selection_index,
                                       true)) {
    return 0;
  }

  Evidyon::World::Tools::MapLayer* layer = NULL;
#define IMPLEMENT(type) {emitter_interface->implement<\
  Evidyon::SpecialFX::Tools::type>()->setName(*type_iter);}

  std::list<std::string>::iterator type_iter = type_options.begin();
  for (int i = 0; i < selection_index; ++i) {
    ++type_iter;
  }
  switch (selection_index) {
    default:
    case 0: IMPLEMENT(SpecialFXEmitter_Interval); break;
  }
#undef IMPLEMENT

  return 1;
}

BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::SpecialFX::Tools::SpecialFXEmitter>)
  { "Set Type...", &SetSpecialFXEmitterType },
END_DEFINING_ACTIONS()