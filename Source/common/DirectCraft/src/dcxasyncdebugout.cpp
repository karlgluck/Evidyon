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
#include "../dcx/dcxasyncdebugout.h"
#include "../dc/dcdebug.h"

namespace dcx {


dcxAsyncDebugOut::~dcxAsyncDebugOut() {
  releaseDebugStream();
}


bool dcxAsyncDebugOut::write(const void* buffer, size_t size ) {
  return dcxAsyncFileWriter::write(buffer, size);
}

bool dcxAsyncDebugOut::acquireDebugStream() {
  std::string output_filename = dc::dcDebug::global(__FILE__,__LINE__)->getLogFilename();
  output_filename = output_filename.substr(0, output_filename.find_last_of(".")) + " - async.txt";
  if (!create(output_filename.c_str(), false)) return false;
  dc::dcDebug::global(__FILE__,__LINE__)->changeOutputStream(this);
  return true;
}

void dcxAsyncDebugOut::releaseDebugStream() {
  dc::dcDebug::global(__FILE__,__LINE__)->changeOutputStream(NULL);
  destroy();
}



}