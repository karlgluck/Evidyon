//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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

