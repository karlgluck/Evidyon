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
#include "clientdata.h"

// there is a problem with linking: vc++ will consider two files with the same
// name to be the same, even if they are in different folders!  so this file
// produces a linker error if it is named without [].


namespace Evidyon {
namespace Client {




  
//----[  clientImage  ]--------------------------------------------------------
Image::ImageIndex ClientData::clientImage(ClientImage image) {
  return Image::INVALID_IMAGE_INDEX;
}





//----[  clientSound  ]--------------------------------------------------------
Sound::SoundIndex ClientData::clientSound(ClientSound sound) {
  return Sound::INVALID_SOUND_INDEX;
}






//----[  clientSpecialFX  ]----------------------------------------------------
SpecialFX::SpecialFXIndex ClientData::clientSpecialFX(ClientSpecialFX special_fx) {
  return SpecialFX::INVALID_SPECIALFX_INDEX;
}




//----[  locationIsVisible  ]--------------------------------------------------
bool ClientData::locationIsVisible(int x, int z) {
  return true;
}





//----[  navigability  ]-------------------------------------------------------
World::Navigability ClientData::navigability(int x, int z) {
  return World::NAVIGABILITY_NORMAL;
}








//----[  footstepSound  ]------------------------------------------------------
Sound::SoundIndex ClientData::footstepSound(int x, int z) {
  return Sound::INVALID_SOUND_INDEX;
}


}
}

