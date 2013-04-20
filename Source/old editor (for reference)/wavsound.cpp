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
#include "stdafx.h"
#include "wavsound.h"
#include <dc/filestream>


/**
 * When reading data from the disk, every .WAV file begins with this header.  It specifies
 * information about the rest of the file and is used when loading audio information.
 */
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


//------------------------------------------------------------------------------------------------
// Name: WAVSound
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
WAVSound::WAVSound()
{
    member("Source .WAV File", &mySourceFile);
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Compiles this sound so that the client can load it
//------------------------------------------------------------------------------------------------
bool WAVSound::compileForClient(dcStreamOut* clientStream) const
{
    dc::dcFileStream wavFile;
    wavFile.open(mySourceFile.getValue().c_str(), 0);

    // Read source information from the WAV file
    WAVFileHeader wavHeader;
    if (APP_ERROR(!wavFile.read(&wavHeader, sizeof(wavHeader)))("Couldn't read file header from %s", mySourceFile.getValue().c_str()))
    {
        wavFile.close();
        return false;
    }

    // Check the signature fields to make sure the header was read correctly and that it's
    // actually a WAV file.
    if (APP_ERROR(0 != memcmp(wavHeader.RiffSig,   "RIFF", 4) ||
                   0 != memcmp(wavHeader.WaveSig,   "WAVE", 4) ||
                   0 != memcmp(wavHeader.FormatSig, "fmt ", 4) ||
                   0 != memcmp(wavHeader.DataSig,   "data", 4))("Invalid wave file format"))
    {
        wavFile.close();
        return false;
    }

    // Fill out the output sound structure
    ClientSound::Header clientSoundHeader;
    ZeroMemory(&clientSoundHeader, sizeof(clientSoundHeader));

    // Build the wave format header
    clientSoundHeader.waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    clientSoundHeader.waveFormat.nChannels = wavHeader.Channels;
    clientSoundHeader.waveFormat.nSamplesPerSec = wavHeader.SampleRate;
    clientSoundHeader.waveFormat.wBitsPerSample = wavHeader.BitsPerSample;
    clientSoundHeader.waveFormat.nBlockAlign = wavHeader.BitsPerSample / 8 * wavHeader.Channels;
    clientSoundHeader.waveFormat.nAvgBytesPerSec = wavHeader.SampleRate * clientSoundHeader.waveFormat.nBlockAlign;

    // Set the data size
    clientSoundHeader.waveformSize = wavHeader.DataSize;

    // Store the wave format header in the compiled output file
    if (APP_ERROR(!clientStream->write(&clientSoundHeader, sizeof(clientSoundHeader)))("Couldn't compile header"))
    {
        wavFile.close();
        return false;
    }

    // Copy the data from the source waveform to the client stream
    void* waveformTempBuffer = new BYTE[wavHeader.DataSize];
    if (APP_ERROR(!wavFile.read(waveformTempBuffer, wavHeader.DataSize) ||
                   !clientStream->write(waveformTempBuffer, wavHeader.DataSize))("Couldn't load %lu bytes from source waveform", wavHeader.DataSize))
    {
        SAFE_DELETE_ARRAY(waveformTempBuffer);
        wavFile.close();
        return false;
    }

    // Deallocate the temporary buffer
    SAFE_DELETE_ARRAY(waveformTempBuffer);

    // Close the source file
    wavFile.close();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Gets the string name of this resource type
//       @return Class name
//------------------------------------------------------------------------------------------------
std::string WAVSound::staticTypeName()
{
    return "WAVSound";
}

