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
#ifndef __COMPLETE_EDITOR_GAMEFILE_GAMEFILEWRITER_H__
#define __COMPLETE_EDITOR_GAMEFILE_GAMEFILEWRITER_H__
#pragma once

namespace Evidyon {
namespace GameFile {



//----[  GameFileWriter  ]-----------------------------------------------------
class GameFileWriter {
public:
  GameFileWriter();
  ~GameFileWriter();

  // Allocates the first chunk of data the writer will use, and returns
  // whether or not the allocation succeeded.  The write point is set to the
  // start of the buffer.
  bool create(size_t initial_size);

  // Expands the capacity of the writer.  If a greater sized allocation would
  // fail, this method returns false.  The write-point and all data remain
  // unchanged.
  bool grow();

  // Frees all data used by the writer
  void destroy();

  // Returns the current write-point
  char* getWritePoint();

  // Changes the current write-point.  This will update the number of
  // bytes consumed.
  void advanceWritePoint(char* point);

  // Gets the current location of the write point.  This can be used to mark
  // locations in the writer to come back and overwrite later using
  // setWritePointOffset.  This is frequently necessary for storing things
  // like hash codes and file sizes, which are only known after the
  // file has been written.
  size_t getWritePointOffset();
  void setWritePointOffset(size_t offset);

  // Returns the point up to which data can be written.  Call grow() to
  // expand this limit.
  const char* getWriteLimit() const;

  // When the file is ready to be written to disk, this method is invoked.
  // It process the data internally and returns the pointer to a
  // buffer containing all of the information in the writer.  If written
  // directly to a file, this data can then be read by a GameFileReader.
  // This method should only be called once no more writing is to be done.
  // You should only invoke this method once; subsequent calls will fail.
  bool finalize(const char** data,
                size_t* length);


private:
  void zero();

private:
  char* buffer_;
  size_t buffer_capacity_;
  char* write_point_;
  size_t bytes_consumed_;
};




}
}

#endif