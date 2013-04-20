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
#ifndef __DCXASYNCFILEWRITER_H__
#define __DCXASYNCFILEWRITER_H__

#include "../dc/dcstream.h"
#include <windows.h>
#include "dcxasynchronoussisothreaddatacontroller.h"

namespace dcx {


class dcxAsyncFileWriter : public dc::dcStreamOut {
public:
  dcxAsyncFileWriter();
  virtual ~dcxAsyncFileWriter();
  bool create(const char* output_file, bool binary);
  void destroy();
  virtual bool seek(size_t location);
  virtual bool move(int amount);
  virtual size_t tell();
  virtual bool write(const void * buffer, size_t size );

public:
  void executeOutputThread();

private:
  dcx::dcxAsynchronousSISOThreadDataController<const char*, 16> output_queue_;
  HANDLE thread_handle_;
  HANDLE wait_event_;
  std::string output_file_;
  bool binary_;
};


}


#endif