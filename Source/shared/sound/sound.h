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
#ifndef __SOUND_H__
#define __SOUND_H__
#pragma once



#define WIN32_LEAN_AND_MEAN
#include <windef.h>
#include <mmsystem.h>
#undef WIN32_LEAN_AND_MEAN

namespace Evidyon {
namespace Sound {

struct Sound {

  // Format information used to create the buffer for this sound
  WAVEFORMATEX wave_format;

  // How many bytes to load into the waveform data buffer
  size_t waveform_size;

  // Data to load into the sound buffer
  void* waveform;
};

}
}

#endif