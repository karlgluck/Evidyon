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
#ifndef __DCRESOURCESTORAGE2_H__
#define __DCRESOURCESTORAGE2_H__

#include "dcresource.h"

typedef struct sqlite3 sqlite3;

namespace dc {


//----[  dcResourceStorage  ]------------------------------------------------
// This class is used to read and write a hierarchial permanent storage of
// resource objects.  It's intended to be used with a database file, but
// the implementation is left virtualized.
class dcResourceStorage2 {
public:
  dcResourceStorage2();

  // Opens up the given file for editing.  Segments with different names won't
  // be able to interfere with each other.
  bool openFile(const char* file_name,
                const char* segment);
  bool openMemory();

  // Closes the storage and cleans up memory
  void close();

  // Reads the resource from the given path into the provided parameter. The
  // storage structure supports any number of root resources as long as
  // none of them conflict.
  // For most purposes, the load should be recursive.  However, if a resource
  // is being loaded for which the children's properties should be ignored,
  // set recursive to false.  Be very careful how this is used--it is almost
  // never a good idea to save a resource that has been loaded non-recursively.
  bool load(const char* owner_path,
            dcGenericResource* resource,
            bool recursive);

  // Updates a given resource and all of its children in the file, leaving
  // other resources intact.
  bool save(const char* owner_path,
            dcGenericResource* resource);

  // Erases all resource records from the storage
  void clear();


private:

  // Initiates a transaction and erases all descendents of the given resource
  // from the database.  The resource itself will be overwritten anyway, so it
  // is not erased.
  bool begin(const char* owner_path, const char* member_name);

  // Finalizes changes to the database if 'commit' is true, otherwise, it
  // returns the database to the state it was at before begin() was called.
  void end(bool commit);

  // Sets the output value in record_id to 0 if resource_path refers to the
  // root object (resource_path == "" or is null), the record ID of the
  // resource with the given path, or -1 if the given resource doesn't exist.
  void getRecordID(const char* resource_path, __int64* record_id);

private:
  sqlite3* storage_;
  std::string segment_;

private:
  dcResourceStorage2(dcResourceStorage2&);
  void operator =(dcResourceStorage2&);
};


}




#endif