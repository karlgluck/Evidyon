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
#ifndef __COMPLETE_EDITOR_GAMEFILE_GAMEFILEREADER_H__
#define __COMPLETE_EDITOR_GAMEFILE_GAMEFILEREADER_H__
#pragma once


namespace Evidyon {
namespace GameFile {



//----[  GameFileReader  ]-----------------------------------------------------
class GameFileReader {
public:
  GameFileReader();
  ~GameFileReader();

  // Accesses the chosen game file and loads its data into memory
  bool open(const char* file);

  // Free all data used by the reader
  void close();

  // Obtains the current read point
  const char* getReadPoint();

  // Gets the byte-offset of the current read point
  size_t getReadPointOffset() const;

  // Advances the read point
  void setReadPoint(const char* point);

  // Changes the read point
  void setReadPointOffset(size_t offset);

  // Returns the first invalid byte after the pointer returned by
  // getReadPoint.
  const char* getReadLimit();

private:
  void zero();

private:
  const char* file_;
  const char* read_point_;
  size_t file_size_;
};



}
}

#endif