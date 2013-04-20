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
#include "../dcx/dcxinifilewriter.h"
#include "../dcx/dcxinifilereader.h"
#include <dc/stream>
#include <dc/debug>
#include <dc/filestream>

namespace dcx {



  
//----[  scan  ]---------------------------------------------------------------
void dcxIniFileWriter::scan(dcx::dcxIniFileReader* reader) {
  dcxIniFileReader::SettingMap::iterator settings_end = reader->mySettings.end();
  for (dcxIniFileReader::SettingMap::iterator i = reader->mySettings.begin(); i != settings_end; ++i) {
    std::string section = i->first.first;
    const std::string& setting = i->first.second;
    const std::string& value = i->second;
    setSetting(section, setting, value);
  }
}



//----[  setSetting  ]---------------------------------------------------------
void dcxIniFileWriter::setSetting(const std::string& section, const std::string& setting, int value) {
  char buffer[64];
  sprintf_s(buffer, 64, "%i", value);
  setSetting(section, setting, buffer);
}


void dcxIniFileWriter::setSetting(const std::string& section, const std::string& setting, float value) {
  char buffer[64];
  sprintf_s(buffer, 64, "%f", value);
  setSetting(section, setting, buffer);
}

void dcxIniFileWriter::setSetting(const std::string& section, const std::string& setting, const std::string& value) {
  Data::iterator section_iter = data_.find(section);
  if (section_iter == data_.end()) {
    Data::_Pairib ib = data_.insert(Data::value_type(section, Data::mapped_type()));
    section_iter = ib.first;
  }
  Data::mapped_type& settings = section_iter->second;
  Data::mapped_type::iterator setting_iter = settings.find(setting);
  if (setting_iter == settings.end()) {
    settings.insert(Data::mapped_type::value_type(setting, value));
  } else {
    setting_iter->second.assign(value);
  }
}




//----[  write  ]--------------------------------------------------------------
bool dcxIniFileWriter::write(dc::dcStreamOut* stream) {
  char data[512];
  for (Data::iterator i = data_.begin(); i != data_.end(); ++i) {
    sprintf_s(data, "%s[%s]\n", (i == data_.begin() ? "" : "\n\n"), i->first.c_str());
    CONFIRM(stream->write(data, strlen(data))) else return false;
    for (Data::mapped_type::iterator j = i->second.begin(); j != i->second.end(); ++j) {
      sprintf_s(data, "%s=%s\n", j->first.c_str(), j->second.c_str());
      CONFIRM(stream->write(data, strlen(data))) else return false;
    }
  }

  return true;
}




//----[  write  ]--------------------------------------------------------------
bool dcxIniFileWriter::write(const char* file_name) {
  dc::dcFileStream file_stream;
  if (!file_stream.open(file_name, STREAM_OVERWRITE|STREAM_TEXT)) return false;
  bool return_value = write(&file_stream);
  file_stream.close();
  return return_value;
}

}