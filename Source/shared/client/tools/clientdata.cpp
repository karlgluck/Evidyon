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
#include "clientdata.h"
#include "shared/client/clientdata.h"

namespace Evidyon {
namespace Client {
namespace Tools {



  
//----[  ClientData  ]---------------------------------------------------------
ClientData::ClientData() {
  for (int i = 0; i < NUMBER_OF_CLIENT_IMAGES; ++i) {
    member(Evidyon::Client::ClientImageString(Evidyon::Client::ClientImage(i)),
           &images_[i]);
  }
  for (int i = 0; i < NUMBER_OF_CLIENT_SOUNDS; ++i) {
    member(Evidyon::Client::ClientSoundString(Evidyon::Client::ClientSound(i)),
           &sounds_[i]);
  }
  for (int i = 0; i < NUMBER_OF_CLIENT_SPECIALFX; ++i) {
    member(Evidyon::Client::ClientSpecialFXString(Evidyon::Client::ClientSpecialFX(i)),
           &special_fx_[i]);
  }
  member("Land Footstep Sound", &land_footstep_sound_);
  member("Water Footstep Sound", &water_footstep_sound_);
}


//----[  compileImage  ]-------------------------------------------------------
void ClientData::compileImage(size_t index,
                              Evidyon::Image::ImageIndex* image) {
  *image =images_[index].getReferencedResourceIndex();
}


//----[  compileSound  ]-------------------------------------------------------
void ClientData::compileSound(size_t index,
                              Evidyon::Sound::SoundIndex* sound) {
  *sound = sounds_[index].getReferencedResourceIndex();
}


//----[  compileSpecialFX  ]---------------------------------------------------
void ClientData::compileSpecialFX(
    size_t index,
    Evidyon::SpecialFX::SpecialFXIndex* special_fx) {
  *special_fx = special_fx_[index].getReferencedResourceIndex();
}


//----[  compileFootstepSounds  ]----------------------------------------------
void ClientData::compileFootstepSounds(
    Evidyon::Sound::SoundIndex* land_footstep_sound,
    Evidyon::Sound::SoundIndex* water_footstep_sound) {
  *land_footstep_sound = land_footstep_sound_.getReferencedResourceIndex();
  *water_footstep_sound = water_footstep_sound_.getReferencedResourceIndex();
}




}
}
}
