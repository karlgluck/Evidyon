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
#include "soundmanager.h"
#include "common/safemacros.h"
#include <assert.h>


namespace Evidyon {
namespace Sound {


  
//----[  SoundManager  ]-------------------------------------------------------
SoundManager::SoundManager() {
  direct_sound_ = NULL;
  listener_x_ = 0.0f;
  listener_z_ = 0.0f;
  for (int i = 0; i < NUMBER_OF_SOUND_BUFFERS; ++i) {
    sound_buffers_[i] = NULL;
  }
  background_sound_buffer_ = NULL;
}




//----[  ~SoundManager  ]------------------------------------------------------
SoundManager::~SoundManager() {
  destroy();
}





//----[  create  ]-------------------------------------------------------------
bool SoundManager::create(HWND focus_window) {
  if (FAILED(DirectSoundCreate8(NULL, &direct_sound_, NULL)) ||
      FAILED(direct_sound_->SetCooperativeLevel(focus_window, DSSCL_NORMAL))) {
    return false;
  }
  return true;
}





//----[  destroy  ]------------------------------------------------------------
void SoundManager::destroy() {
  for (SoundArray::iterator i = sounds_.begin(); i != sounds_.end(); ++i) {
    SAFE_DELETE_ARRAY(i->waveform);
  }
  sounds_.clear();


  // Go through the sound buffer array and release everything
  for (int i = 0; i < NUMBER_OF_SOUND_BUFFERS; ++i) {
    if (sound_buffers_[i]) {
      sound_buffers_[i]->Stop();
      sound_buffers_[i]->Release();
      sound_buffers_[i] = NULL;
    }
  }

  if (background_sound_buffer_ != NULL) {
    background_sound_buffer_->Stop();
    background_sound_buffer_->Release();
    background_sound_buffer_ = NULL;
  }

  SAFE_RELEASE(direct_sound_);
}





//----[  addSound  ]-----------------------------------------------------------
bool SoundManager::addSound(size_t index, const Sound& sound) {
  if (!direct_sound_) return false;  // don't do anything if DSound doesn't exist
  if (sounds_.size() != index) return false;
  sounds_.push_back(sound);
  return true;
}





//----[  clearSounds  ]--------------------------------------------------------
void SoundManager::clearSounds() {
  for (SoundArray::iterator i = sounds_.begin(); i != sounds_.end(); ++i) {
    SAFE_DELETE_ARRAY(i->waveform);
  }
  sounds_.clear();
}





//----[  playSound  ]----------------------------------------------------------
bool SoundManager::playSound(SoundPriority priority, SoundIndex index) {
  if (!direct_sound_) return true;
  if (index >= sounds_.size()) return false;

  int buffer_index = getAvailableBufferIndex(priority);
  if (buffer_index < 0) return false; // couldn't play this sound

  LPDIRECTSOUNDBUFFER8 sound_buffer = NULL;
  if (!fillSoundBuffer(&sounds_.at(index), &sound_buffer)) return false;
  sound_buffer_priority_[buffer_index] = priority;
  sound_buffers_[buffer_index] = sound_buffer;

  // Start the sound playing
  sound_buffer->SetCurrentPosition(0);
  sound_buffer->SetVolume(0);
  sound_buffer->Play(0, 0, 0);

  // Success
  return true;
}




//----[  playSound  ]----------------------------------------------------------
bool SoundManager::playSound(SoundPriority priority, float x, float z, SoundIndex index) {
  if (!direct_sound_) return true;
  if (index >= sounds_.size()) return false;

  // Find distance to the client
  float dx = x - listener_x_;
  float dz = z - listener_z_;
  float dd_sq = dx*dx+dz*dz;
  static const float MAX_DISTANCE_SQ = 16.0f*16.0f;

  // If this distance is too high, we don't need to play it
  if (dd_sq > MAX_DISTANCE_SQ) return true; 

  int buffer_index = getAvailableBufferIndex(priority);
  if (buffer_index < 0) return false; // couldn't play this sound

  LPDIRECTSOUNDBUFFER8 sound_buffer = NULL;
  if (!fillSoundBuffer(&sounds_.at(index), &sound_buffer)) return false;
  sound_buffer_priority_[buffer_index] = priority;
  sound_buffers_[buffer_index] = sound_buffer;

  // Start the sound playing
  sound_buffer->SetCurrentPosition(0);
  static const int DB_HIGH = 0, DB_LOW = -3000;
  static const int DB_PAN = 1000;
  sound_buffer->SetVolume((DB_HIGH - DB_LOW) * (MAX_DISTANCE_SQ / dd_sq) + DB_LOW);
  sound_buffer->SetPan((long)(dx * DB_PAN));
  sound_buffer->Play(0, 0, 0);

  // Success
  return true;
}





//----[  loopBackgroundSound  ]------------------------------------------------
bool SoundManager::loopBackgroundSound(SoundIndex index) {
  if (!direct_sound_) return true;
  if (background_sound_buffer_ != NULL) {
    background_sound_buffer_->Stop();
    background_sound_buffer_->Release();
    background_sound_buffer_ = NULL;
  }
  if (index >= sounds_.size()) return true; // turn off sound
  if (!fillSoundBuffer(&sounds_.at(index),
                       &background_sound_buffer_)) return false;
  background_sound_buffer_->SetCurrentPosition(0);
  background_sound_buffer_->SetVolume(-500); // a bit lower than natural volume (down .5 decibels)
  background_sound_buffer_->Play(0, 0, DSBPLAY_LOOPING);
  return true;
}






//----[  update  ]-------------------------------------------------------------
void SoundManager::update(float listener_x, float listener_z) {
  listener_x_ = listener_x;
  listener_z_ = listener_z;
}







//----[  getAvailableBufferIndex  ]--------------------------------------------
int SoundManager::getAvailableBufferIndex(SoundPriority priority) {

  // find a buffer that has stopped or is empty
  for (int index = 0; index < NUMBER_OF_SOUND_BUFFERS; ++index) {
    LPDIRECTSOUNDBUFFER8 buffer = sound_buffers_[index];
    if (buffer == NULL) return index;
    DWORD status = DSBSTATUS_BUFFERLOST;
    if (FAILED(buffer->GetStatus(&status)) || !(status & DSBSTATUS_PLAYING)) {
      // recover this buffer since it's finished
      buffer->Stop();
      sound_buffers_[index]->Release();
      sound_buffers_[index] = NULL;
      sound_buffer_priority_[index] = SOUNDPRIORITY_AMBIENT;
      return index;
    }
  }

  // preempt a buffer of lower priority
  for (int index = 0; index < NUMBER_OF_SOUND_BUFFERS; ++index) {
    if (sound_buffer_priority_[index] > priority) continue;
    LPDIRECTSOUNDBUFFER8 buffer = sound_buffers_[index];
    buffer->Stop();
    buffer->Release();
    sound_buffers_[index] = NULL;
    sound_buffer_priority_[index] = SOUNDPRIORITY_AMBIENT;
    return index;
  }

  // no buffer could be found
  return -1;
}





//----[  fillSoundBuffer  ]----------------------------------------------------
bool SoundManager::fillSoundBuffer(const Sound* sound, LPDIRECTSOUNDBUFFER8* buffer) {
  assert(direct_sound_);
  assert(sound);
  assert(buffer);

  // Create the sound buffer description.  We want to be able to control the
  // frequency, volume and pan of this sound.
  DSBUFFERDESC dsbd;
  ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN; //  | DSBCAPS_CTRLFREQUENCY
  dsbd.dwBufferBytes = sound->waveform_size;
  dsbd.lpwfxFormat = const_cast<LPWAVEFORMATEX>(&sound->wave_format);

  // Generate the sound buffer using a temporary interface
  IDirectSoundBuffer* pDSB;
  if (FAILED(direct_sound_->CreateSoundBuffer(&dsbd, &pDSB, NULL)))
    return false;

  // Query the temporary interface for the version 8 sound buffer
  LPDIRECTSOUNDBUFFER8 internal_buffer = NULL;
  if (FAILED(pDSB->QueryInterface(IID_IDirectSoundBuffer8,
                                  (VOID**)&internal_buffer))) {
    pDSB->Release();
    return false;
  }

  // Release the newer interface
  pDSB->Release();

  // Copy data into the buffer
  BYTE* ptr1;
  DWORD size1;
  BYTE* ptr2;
  DWORD size2;
  if (FAILED(internal_buffer->Lock(0, sound->waveform_size,
            (VOID**)&ptr1, &size1, (VOID**)&ptr2, &size2, 0)))
  {
    internal_buffer->Release();
    return false;
  }

  // Copy the data
  memcpy(ptr1, sound->waveform, size1);
  if (ptr2 != NULL)
    memcpy(ptr2, ((BYTE*)sound->waveform) + size1, size2);

  // Unlock the buffer
  internal_buffer->Unlock((VOID*)ptr1, size1, (VOID*)ptr2, size2);

  // Save the pointer
  *buffer = internal_buffer;

  // Success
  return true;
}




}
}