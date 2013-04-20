//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#include "evidyonguisounds.h"
#include "shared/sound/soundmanager.h"


namespace Evidyon {

EvidyonGUISounds* EvidyonGUISounds::singleton_ = NULL;



//----[  EvidyonGUISounds  ]---------------------------------------------------
EvidyonGUISounds::EvidyonGUISounds() {
  singleton_ = this;
  global_sound_manager_ = NULL;
}




//----[  EvidyonGUISounds  ]---------------------------------------------------
EvidyonGUISounds::~EvidyonGUISounds() {
  singleton_ = NULL;
}



//----[  assignSounds  ]-------------------------------------------------------
void EvidyonGUISounds::assignSounds(Sound::SoundManager* global_sound_manager,
                                    Sound::SoundIndex button_sound) {
  global_sound_manager_ = global_sound_manager;
  button_sound_ = button_sound;
  singleton_ = this;
}



//----[  buttonClick  ]--------------------------------------------------------
void EvidyonGUISounds::buttonClick() {
  if (singleton_ && singleton_->global_sound_manager_) {
    singleton_->global_sound_manager_->playSound(Sound::SOUNDPRIORITY_SELF,
                                                 singleton_->button_sound_);
  }
}

}