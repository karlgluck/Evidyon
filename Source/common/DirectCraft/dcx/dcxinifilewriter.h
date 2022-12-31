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
#ifndef __DCXINIFILEWRITER_H__
#define __DCXINIFILEWRITER_H__


#include <map>


namespace dc {
class dcStreamOut;
}

namespace dcx {

class dcxIniFileReader;

/**
 * Provides a simple interface for writing a standard INI file
 */
class dcxIniFileWriter {
  // Map of Section Name -> (Map of Name->Parameter)
  typedef std::map<std::string,std::map<std::string,std::string>> Data;

public:

  // Interprets data from the file reader
  void scan(dcx::dcxIniFileReader* reader);

  // Changes or adds values to be written
  void setSetting(const std::string& section, const std::string& setting, int value);
  void setSetting(const std::string& section, const std::string& setting, float value);
  void setSetting(const std::string& section, const std::string& setting, const std::string& value);

  /**
   * Scans a source file that contains an INI that provides settings information.  This
   * method clears any information that has been previously obtained using read().  This
   * method returns 'false' only if the file doesn't exist.
   * @param file The path to the source file
   */
  bool write(dc::dcStreamOut* stream);

  bool write(const char* file_name);

private:
  Data data_;
};

}



#endif
