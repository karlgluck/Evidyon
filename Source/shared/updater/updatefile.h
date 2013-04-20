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
#ifndef __UPDATEFILE_H__
#define __UPDATEFILE_H__
#pragma once

#include <string>
#include <map>

namespace Evidyon {

struct UpdateFileData {
  std::string md5;
  __int64 file_bytes;
};

struct UpdateFile {
  std::string update_text;
  unsigned long major, minor;
  std::map<std::string, UpdateFileData> files;
};

// This method contacts the update server, then interprets and downloads
// the current file.  If it is invalid, or the server could not be
// contacted, it returns 'false'.
bool DownloadUpdateFile(const char* webaddr, UpdateFile* file);

static const char* UPDATE_FILE_HEADER_TEXT = "EvidyonUpdateFile";


}

#endif