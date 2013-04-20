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
#ifndef __COMPLETE_EDITOR_GAMEFILE_GAMEFILEDATA_H__
#define __COMPLETE_EDITOR_GAMEFILE_GAMEFILEDATA_H__
#pragma once



#include "gamefilewriter.h"
#include "gamefilereader.h"

#include "../../common/serializer/serializer.h"
#include "../../common/serializer/serializer_begin.h"



#define WRITEGAMEFILE_NOPARAMS()  /*static bool write(GameFileWriter* writer) { return false; }*/
#define WRITEGAMEFILE(base, plus) template <TYPES(base,plus)> static bool write(Evidyon::GameFile::GameFileWriter* writer, W_PARAMS(base,plus)) { char* buffer; do { buffer = writer->getWritePoint(); const char* end = writer->getWriteLimit(); W_BODY(base,plus); if (buffer != 0) break; if (!writer->grow()) return false; } while (true); writer->advanceWritePoint(buffer); return true; }
#define READGAMEFILE_NOPARAMS()   /*static bool read(GameFileReader* reader) { return false; }*/
#define READGAMEFILE(base, plus)  template <TYPES(base,plus)> static bool read(Evidyon::GameFile::GameFileReader* reader, R_PARAMS(base,plus)) { const char* buffer = reader->getReadPoint(); const char* end = reader->getReadLimit(); R_BODY(base,plus); reader->setReadPoint(buffer); return buffer != 0; }


template <TLIST_64> class EvidyonGameFileSerializer {
public: SERIALIZER_READ64(GAMEFILE);
public: SERIALIZER_WRITE64(GAMEFILE);
};


#include "../../common/serializer/serializer_end.h"



namespace Evidyon {
namespace GameFile {

  
//----[  GameFileDataArray  ]--------------------------------------------------
struct GameFileDataArray {
  size_t array_entries;
  size_t first_byte_offset;
  //size_t hash;

  // These byte sizes are the total number of bytes needed to allocate all
  // of the data for all of the entries in this array.
  size_t total_memory_bytes;
  size_t total_disk_bytes;
};



//----[  GAME_FILE_ARRAY  ]----------------------------------------------------
#define GAME_FILE_ARRAY(LocalType, LocalDataType, name) \
  struct LocalType {\
    typedef LocalDataType DataType;\
    LocalDataType data;\
    void byteSize(size_t* memory, size_t* disk) const {*memory =0; *disk=0;}\
  };\
  struct GameFileDataArray name##;\
  LocalType* name##_array;\

}
}

#endif


