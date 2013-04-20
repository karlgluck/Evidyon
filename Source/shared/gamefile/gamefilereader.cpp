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
#include "gamefilereader.h"
#include <windows.h>    // for file I/O
#include <assert.h>
#include "common/safemacros.h"
#include "gamefileheader.h"


namespace Evidyon {
namespace GameFile {




  


  
//----[  GameFileReader  ]-----------------------------------------------------
GameFileReader::GameFileReader() {
  zero();
}




//----[  ~GameFileReader  ]----------------------------------------------------
GameFileReader::~GameFileReader() {
  close();
}





//----[  open  ]---------------------------------------------------------------
bool GameFileReader::open(const char* file) {
  HANDLE game_file_handle = CreateFile(file,
                                       GENERIC_READ,
                                       FILE_SHARE_READ,
                                       NULL,
                                       OPEN_EXISTING,
                                       FILE_ATTRIBUTE_READONLY
                                        | FILE_FLAG_SEQUENTIAL_SCAN,
                                       NULL);
  if (game_file_handle == NULL) return false;

  // Read the file into memory
  LARGE_INTEGER large_size;
  if (!GetFileSizeEx(game_file_handle, &large_size)) {
    CloseHandle(game_file_handle);
    return false;
  }

  char* game_file = new char[large_size.u.LowPart];
  DWORD bytes_read = 0;
  if (FALSE == ReadFile(game_file_handle,
                        game_file,
                        large_size.u.LowPart,
                        &bytes_read,
                        NULL) ||
      bytes_read != large_size.u.LowPart) {
    SAFE_DELETE_ARRAY(game_file);
    CloseHandle(game_file_handle);
    return false;
  }
  CloseHandle(game_file_handle);
  game_file_handle = NULL;

  file_ = game_file;
  read_point_ = game_file;
  file_size_ = bytes_read;

  return true;
}





//----[  close  ]--------------------------------------------------------------
void GameFileReader::close() {
  SAFE_DELETE_ARRAY(file_);
  zero();
}





//----[  getReadPoint  ]-------------------------------------------------------
const char* GameFileReader::getReadPoint() {
  return read_point_;
}





//----[  getReadPointOffset  ]-------------------------------------------------
size_t GameFileReader::getReadPointOffset() const {
  return read_point_ - file_;
}





//----[  setReadPoint  ]-------------------------------------------------------
void GameFileReader::setReadPoint(const char* point) {
  read_point_ = point;
}




//----[  setReadPointOffset  ]-------------------------------------------------
void GameFileReader::setReadPointOffset(size_t offset) {
  read_point_ = file_ + offset;
}




//----[  getReadLimit  ]-------------------------------------------------------
const char* GameFileReader::getReadLimit() {
  return file_ + file_size_;
}




//----[  zero  ]---------------------------------------------------------------
void GameFileReader::zero() {
  file_ = NULL;
  file_size_ = 0;
  read_point_ = NULL;
}








}
}

