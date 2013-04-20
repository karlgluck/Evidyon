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
#include "shared/sound/tools/wavsound.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"





//----[  WAVSound  ]-----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Sound::Tools::WAVSound)
END_DEFINING_ACTIONS()


  
//----[  AddWAVSounds  ]-------------------------------------------------------
int AddWAVSounds(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Sound::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<WAVSound>, list);
  std::string path;
  std::list<std::string> files;
  if (!dcx::dcxOpenMultipleFilesDialog(
        tool->topWindow(),
        "Select Sounds",
        "WAV Sound Files (*.wav)\0"\
          "*.WAV\0\0",
        &path,
        &files)) return 0;
  for (std::list<std::string>::iterator i = files.begin();
       i != files.end();
       ++i) {
    WAVSound* sound = (WAVSound*)list->generateElement(*i);
    sound->setSourceFile(path + "\\" + *i);
  }

  return 1;
}



//----[  SelectWAVSoundReference  ]--------------------------------------------
int SelectWAVSoundReference(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Sound::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<WAVSound>::Reference, reference);

  dc::dcGenericResource* selection = reference->getReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Animation",
        "Pick the animation to reference",
        tool->getSounds(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}



//----[  dcList<WAVSound>  ]---------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Sound::Tools::WAVSound>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add WAV Sounds...", &AddWAVSounds },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Sound::Tools::WAVSound>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Sound::Tools::WAVSound>::Reference)
  { "Select...", &SelectWAVSoundReference },
  LIST_REFERENCE_ACTIONS(Evidyon::Sound::Tools::WAVSound),
END_DEFINING_ACTIONS()

