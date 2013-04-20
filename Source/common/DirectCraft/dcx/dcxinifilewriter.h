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
