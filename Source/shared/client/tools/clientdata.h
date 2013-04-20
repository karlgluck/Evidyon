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
#ifndef __SHARED_CLIENT_TOOLS_CLIENTDATA_H__
#define __SHARED_CLIENT_TOOLS_CLIENTDATA_H__
#pragma once


#include "shared/image/tools/image.h"
#include "shared/image/imageindex.h"
#include "shared/sound/tools/wavsound.h"
#include "shared/sound/soundindex.h"
#include "shared/specialfx/tools/specialfx.h"
#include "shared/specialfx/specialfxindex.h"
#include "shared/client/clientimage.h"
#include "shared/client/clientsound.h"
#include "shared/client/clientspecialfx.h"


namespace Evidyon {
namespace Client {
namespace Tools {


  
//----[  ClientData  ]---------------------------------------------------------
class ClientData : public dc::dcResource<ClientData> {
public:
  static std::string staticTypeName() { return "ClientData"; }
public:
  ClientData();
  void compileImage(size_t index, Evidyon::Image::ImageIndex* image);
  void compileSound(size_t index, Evidyon::Sound::SoundIndex* sound);
  void compileSpecialFX(size_t index,
                        Evidyon::SpecialFX::SpecialFXIndex* special_fx);
  void compileFootstepSounds(
      Evidyon::Sound::SoundIndex* land_footstep_sound,
      Evidyon::Sound::SoundIndex* water_footstep_sound);

private:
  Evidyon::Tools::ImageReference images_[NUMBER_OF_CLIENT_IMAGES];
  Evidyon::Tools::SoundReference sounds_[NUMBER_OF_CLIENT_SOUNDS];
  Evidyon::Tools::SpecialFXReference special_fx_[NUMBER_OF_CLIENT_SPECIALFX];
  Evidyon::Tools::SoundReference land_footstep_sound_;
  Evidyon::Tools::SoundReference water_footstep_sound_;
  //Evidyon::Tools::SoundReference footstep_sounds_[NUMBER_OF_NAVIGABILITY_TYPES];
};




}
}
}


#endif