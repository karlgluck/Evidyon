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
#include "soundutils.h"
#include <dsound.h>
#include "sound.h"
#include <assert.h>
#include <dc/filestream>
#include "common/safemacros.h"


// When reading data from the disk, every .WAV file begins with this header.
// It specifies information about the rest of the file and is used when
// loading audio information.
struct WAVFileHeader
{
  CHAR  RiffSig[4];         // 'RIFF'
  LONG  WaveformChunkSize;  // 8
  CHAR  WaveSig[4];         // 'WAVE'
  CHAR  FormatSig[4];       // 'fmt ' (notice space after)
  LONG  FormatChunkSize;    // 16
  SHORT FormatTag;          // WAVE_FORMAT_PCM
  SHORT Channels;           // # of channels
  LONG  SampleRate;         // sampling rate
  LONG  BytesPerSec;        // bytes per second
  SHORT BlockAlign;         // sample block alignment
  SHORT BitsPerSample;      // bits per second
  CHAR  DataSig[4];         // 'data'
  LONG  DataSize;           // size of waveform data
};


namespace Evidyon {
namespace Sound {


//----[  EvidyonSoundUtil_LoadSoundFromWAV  ]----------------------------------
bool EvidyonSoundUtil_LoadSoundFromWAV(const char* path,
                                       Sound* sound) {
  assert(path);
  assert(sound);

  dc::dcFileStream file;
  file.open(path, 0);

  // Read source information from the WAV file
  WAVFileHeader wav_header;
  if (!file.read(&wav_header, sizeof(wav_header))) {
    file.close();
    return false;
  }

  // Check the signature fields to make sure the header was read correctly and that it's
  // actually a WAV file.
  if (0 != memcmp(wav_header.RiffSig,   "RIFF", 4) ||
      0 != memcmp(wav_header.WaveSig,   "WAVE", 4) ||
      0 != memcmp(wav_header.FormatSig, "fmt ", 4) ||
      0 != memcmp(wav_header.DataSig,   "data", 4)) {
    file.close();
    return false;
  }

  // Build the wave format header
  sound->wave_format.wFormatTag = WAVE_FORMAT_PCM;
  sound->wave_format.nChannels = wav_header.Channels;
  sound->wave_format.nSamplesPerSec = wav_header.SampleRate;
  sound->wave_format.wBitsPerSample = wav_header.BitsPerSample;
  sound->wave_format.nBlockAlign = wav_header.BitsPerSample / 8 * wav_header.Channels;
  sound->wave_format.nAvgBytesPerSec = wav_header.SampleRate * sound->wave_format.nBlockAlign;

  // Set the data size
  sound->waveform_size = wav_header.DataSize;
  sound->waveform = new BYTE[wav_header.DataSize];
  if (!file.read(sound->waveform, wav_header.DataSize)) {
    SAFE_DELETE_ARRAY(sound->waveform);
    file.close();
    return false;
  }

  // Success
  file.close();
  return true;
}

}
}