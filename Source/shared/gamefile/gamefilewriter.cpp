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
#include "gamefilewriter.h"
#include "common/null.h"
#include "common/safemacros.h"
#include <assert.h>
#include <memory.h> // for memcpy
#include "gamefileheader.h"


namespace Evidyon {
namespace GameFile {




  
//----[  GameFileWriter  ]-----------------------------------------------------
GameFileWriter::GameFileWriter() {
  zero();
}




//----[  ~GameFileWriter  ]----------------------------------------------------
GameFileWriter::~GameFileWriter() {
  destroy();
}







//----[  create  ]-------------------------------------------------------------
bool GameFileWriter::create(size_t initial_size) {
  assert(initial_size);
  destroy();
  buffer_ = new char[initial_size];
  if (!buffer_) return false;
  buffer_capacity_ = initial_size;
  write_point_ = buffer_;
  return true;
}







//----[  grow  ]---------------------------------------------------------------
bool GameFileWriter::grow() {
  size_t new_capacity = buffer_capacity_ << 1;
  char* new_buffer = new char[new_capacity];
  if (!new_buffer) return false;
  memcpy(new_buffer, buffer_, bytes_consumed_);
  size_t write_point_offset = getWritePointOffset();
  SAFE_DELETE_ARRAY(buffer_);
  buffer_ = new_buffer;
  setWritePointOffset(write_point_offset);
  buffer_capacity_ = new_capacity;
  return true;
}





//----[  destroy  ]------------------------------------------------------------
void GameFileWriter::destroy() {
  SAFE_DELETE_ARRAY(buffer_);
  zero();
}





//----[  getWritePoint  ]------------------------------------------------------
char* GameFileWriter::getWritePoint() {
  return write_point_;
}






//----[  advanceWritePoint  ]--------------------------------------------------
void GameFileWriter::advanceWritePoint(char* point) {
  write_point_ = point;
  assert(point >= buffer_ && (point < buffer_ + buffer_capacity_));
  bytes_consumed_ = point - buffer_;
}





//----[  getWritePointOffset  ]------------------------------------------------
size_t GameFileWriter::getWritePointOffset() {
  return write_point_ - buffer_;
}





//----[  setWritePointOffset  ]------------------------------------------------
void GameFileWriter::setWritePointOffset(size_t offset) {
  write_point_ = buffer_ + offset;
}






//----[  getWriteLimit  ]------------------------------------------------------
const char* GameFileWriter::getWriteLimit() const {
  return buffer_ + buffer_capacity_;
}







//----[  finalize  ]-----------------------------------------------------------
bool GameFileWriter::finalize(const char** data,
                              size_t* length) {
  assert(data);
  assert(length);
  if (!bytes_consumed_) return false;

  // nothing left to do!
  *data = buffer_;
  *length = bytes_consumed_;

  return true;
}








//----[  zero  ]---------------------------------------------------------------
void GameFileWriter::zero() {
  buffer_ = NULL;
  buffer_capacity_ = 0;
  write_point_ = NULL;
  bytes_consumed_ = 0;
}








}
}

