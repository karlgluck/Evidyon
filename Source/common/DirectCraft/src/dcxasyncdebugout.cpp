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