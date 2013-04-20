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
#include "updatefile.h"
#include <dcx/dcxwin32internetstream.h>

namespace Evidyon {

bool DownloadUpdateFile(const char* webaddr, UpdateFile* file) {

  std::string update_file;
  {
    dcx::dcxWin32InternetStream stream;
    if (false == stream.open("EvidyonUpdateUploader", webaddr)) {
      return false;
    }
    static const int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE+1];
    size_t bytes_read = 0;
    while (stream.scan(buffer, BUFFER_SIZE, &bytes_read) && !stream.end()) {
      buffer[bytes_read] = '\0';
      update_file.append(buffer);
    }
    stream.close();

    if (update_file.find(Evidyon::UPDATE_FILE_HEADER_TEXT) != 0) return false;
    update_file = update_file.substr(strlen(Evidyon::UPDATE_FILE_HEADER_TEXT));
  }

  // Find locations
  size_t major_end = update_file.find_first_of("\r\n");
  size_t minor_end = update_file.find_first_of("\r\n", major_end + 2);

  sscanf_s(update_file.substr(0, major_end).c_str(), "%lu", &file->major);
  sscanf_s(update_file.substr(major_end+2, minor_end-2).c_str(), "%lu", &file->minor);

  update_file = update_file.substr(minor_end+2);

  while (false == update_file.empty()) {
    size_t filename_end = update_file.find_first_of("\r\n");
    size_t file_size_end = update_file.find_first_of("\r\n", filename_end + 2);
    size_t md5_end = update_file.find_first_of("\r\n", file_size_end + 2);
    std::string filename = update_file.substr(0, filename_end);
    std::string file_size = update_file.substr(filename_end + 2, file_size_end - filename_end - 2);
    std::string md5 = update_file.substr(file_size_end+2, md5_end-file_size_end-2);
    update_file = update_file.substr(md5_end+2);
    {
      UpdateFileData file_data;
      file_data.md5 = md5;
      sscanf_s(file_size.c_str(), "%I64d", &file_data.file_bytes);
      file->files.insert(std::map<std::string,UpdateFileData>::value_type(filename, file_data));
    }
  }

  return true;
}

}