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
#ifndef __SHARED_SOUND_SOUNDMANAGER_H__
#define __SHARED_SOUND_SOUNDMANAGER_H__
#pragma once




#include <dsound.h>
#include "sound.h"
#include "soundindex.h"
#include <vector>

namespace Evidyon {
namespace Sound {

  
//----[  SoundPriority  ]------------------------------------------------------
enum SoundPriority {
  SOUNDPRIORITY_AMBIENT,    // owl hooting, footsteps, sloshing through water
  SOUNDPRIORITY_NEARBY,     // ex: spell
  SOUNDPRIORITY_SELF,       // always played; ex: own attack
};



//----[  SoundManager  ]-------------------------------------------------------
class SoundManager {
  typedef std::vector<Evidyon::Sound::Sound> SoundArray;
  static const int NUMBER_OF_SOUND_BUFFERS = 5;

public:
  SoundManager();
  ~SoundManager();

  // Initializes internal objects
  bool create(HWND focus_window);

  // Cleans up allocated memory
  void destroy();

  // Adds a new sound type
  bool addSound(size_t index, const Sound& sound);

  // Remove all sound descriptions, as well as clearing all instances
  void clearSounds();

  // Used to play sounds
  bool playSound(SoundPriority priority, SoundIndex index);
  bool playSound(SoundPriority priority, float x, float z, SoundIndex index);
  //bool playSound(SoundPriority priority, float x, float z, float vx, float vy, SoundIndex index);
  bool loopBackgroundSound(SoundIndex index);

  // Syncs the manager with the listener's position so that spatial sounds can
  // be panned appropriately.
  void update(float listener_x, float listener_z);

private:
  // Obtains the index of a sound buffer that can be overwritten
  // in order to play a sound of the requested priority.  If the
  // return value is negative, no buffer could be found.
  int getAvailableBufferIndex(SoundPriority priority);

  // Loads the given sound into the buffer structure
  bool fillSoundBuffer(const Sound* sound, LPDIRECTSOUNDBUFFER8* buffer);


private:
  IDirectSound8* direct_sound_;

  SoundArray sounds_;

  // Array of sound buffers that are currently in use.  When
  // a sound buffer is inactive, its value is NULL.
  LPDIRECTSOUNDBUFFER8 sound_buffers_[NUMBER_OF_SOUND_BUFFERS];
  SoundPriority sound_buffer_priority_[NUMBER_OF_SOUND_BUFFERS];
  int looping_sound_unique_id[NUMBER_OF_SOUND_BUFFERS];
  LPDIRECTSOUNDBUFFER8 background_sound_buffer_;

  // This is the last-updated location of the listener, used
  // to calculate how to play spatial sounds.
  float listener_x_, listener_z_;
};

}
}

#endif