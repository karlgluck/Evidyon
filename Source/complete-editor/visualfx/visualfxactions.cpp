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
#include "shared/visualfx/tools/visualfx.h"
#include "shared/visualfx/tools/visualfx_orbitingparticles.h"
#include "shared/visualfx/tools/visualfx_beam.h"
#include "shared/visualfx/tools/visualfx_scenery.h"
#include "shared/visualfx/tools/visualfx_grounddecal.h"
#include "shared/visualfx/tools/visualfx_fireflycloud.h"
#include "shared/visualfx/tools/visualfx_particleshooter.h"
#include "shared/visualfx/tools/visualfx_flickeringlight.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "complete-editor/visualfx/visualfxeditingdialog.h"


BEGIN_DEFINING_ACTIONS(Evidyon::VisualFX::Tools::VisualFX_OrbitingParticles)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::VisualFX::Tools::VisualFX_Beam)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::VisualFX::Tools::VisualFX_Scenery)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::VisualFX::Tools::VisualFX_GroundDecal)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::VisualFX::Tools::VisualFX_FireflyCloud)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::VisualFX::Tools::VisualFX_ParticleShooter)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::VisualFX::Tools::VisualFX_FlickeringLight)
END_DEFINING_ACTIONS()


//----[  CreateVisualFXTableEntry  ]-------------------------------------------
template <typename T>
int CreateVisualFXTableEntry(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Evidyon::VisualFX::Tools::VisualFX>, table);
  std::string name = "";
  if (!dcx::dcxWin32PromptTextInput(dialog->getWindowHandle(), 
                                   "Resource Name?",
                                   "Please enter a name for this resource:",
                                   &name)) {
    return -1;
  }
  T* entry = (T*)table->create<T>(name);
  return -1;
}


//----[  EditVisualFX  ]-------------------------------------------------------
int EditVisualFX(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  typedef dc::dcTable<Evidyon::VisualFX::Tools::VisualFX> Table;
  CAST_ACTION_RESOURCE(Table, table);

  Evidyon::VisualFXEditingDialog* visual_fx_editing_dialog
    = new Evidyon::VisualFXEditingDialog();
  if (!visual_fx_editing_dialog->begin(tool,
                                       tool->getVisualFXTable()->getMembers())) {
    delete visual_fx_editing_dialog;
    return 0;
  }

  // Dialog will be changing the fx
  return 1;
}



//----[  SelectVisualFX  ]-----------------------------------------------------
int SelectVisualFX(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::Tools::VisualFXReference, reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select VisualFX",
        "Pick the visual fx to reference",
        tool->getVisualFX(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}



//----[  VisualFX  ]-----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::VisualFX::Tools::VisualFX>)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::VisualFX::Tools::VisualFX>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Preview && Edit...", &EditVisualFX },
  ACTION_LIST_SEPARATOR,
  { "Create Orbiting Particles",
    &CreateVisualFXTableEntry<Evidyon::VisualFX::Tools::VisualFX_OrbitingParticles> },
  { "Create Scenery FX",
    &CreateVisualFXTableEntry<Evidyon::VisualFX::Tools::VisualFX_Scenery> },
  { "Create Beam",
    &CreateVisualFXTableEntry<Evidyon::VisualFX::Tools::VisualFX_Beam> },
  { "Create Ground Decal",
    &CreateVisualFXTableEntry<Evidyon::VisualFX::Tools::VisualFX_GroundDecal> },
  { "Create Firefly Cloud",
    &CreateVisualFXTableEntry<Evidyon::VisualFX::Tools::VisualFX_FireflyCloud> },
  { "Create Particle Shooter",
    &CreateVisualFXTableEntry<Evidyon::VisualFX::Tools::VisualFX_ParticleShooter> },
  { "Create Flickering Light",
    &CreateVisualFXTableEntry<Evidyon::VisualFX::Tools::VisualFX_FlickeringLight> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::VisualFX::Tools::VisualFX>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::VisualFX::Tools::VisualFX>::Reference)
  { "Select...", &SelectVisualFX },
  LIST_REFERENCE_ACTIONS(NULL),
END_DEFINING_ACTIONS()
