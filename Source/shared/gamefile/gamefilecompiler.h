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
#ifndef __COMPLETE_EDITOR_GAMEFILE_GAMEFILECOMPILER_H__
#define __COMPLETE_EDITOR_GAMEFILE_GAMEFILECOMPILER_H__
#pragma once

#include <dc/dcresource.h>
#include "gamefilewriter.h"

namespace Evidyon {
namespace Tools {
class CompleteEditor;
}
namespace GameFile {
struct ClientGameFileData;
struct ServerGameFileData;
}
}


namespace Evidyon {
namespace GameFile {


//----[  GameFileCompiler  ]---------------------------------------------------
class GameFileCompiler {
public:
  class ProgressCallback {
  public:
    enum Type {
      ECHO_INFO,
      ECHO_WARNING,
      ECHO_ERROR,
    };
  public:
    virtual ~ProgressCallback();
    virtual void echo(Type type, const char* message);
  };

public:
  GameFileCompiler();
  ~GameFileCompiler();

  // Erases all contents of the compiler and deallocates memory
  void clear();

  // Creates the client and server game files from the source data.  The
  // functionoid, if valid, is passed messages about what is happening.
  // Be aware that optimization can take a VERY long time!  However, it should
  // reduce the file size by 10-15% and increase the speed of the game.
  bool compile(Tools::CompleteEditor* source,
               bool optimize,
               ProgressCallback* progress);

  // Outputs the compiled data to the destination files.  This should only
  // be called once compile() has succeeded.
  bool write(const char* client_file,
             const char* server_file);


public:

  // Fills 'resources' with all of the resources--at any level in the
  // hierarchy--that match the given type ID number.  This is used for
  // finding all references to objects in a list; for example, all
  // references to images or all references to actors.
  void getAllResourcesOfType(unsigned long type,
                             dc::dcGenericResource::Array& resources);

private:

  // Prebuild can consist of:
  //  - basic validation on the state of the game
  //  - organizing lists that require it, such as images
  //  - sorting textures based on "omnicient" knowledge of the game state,
  //      such as pairing two textures together that are only both used
  //      by the same scenery object, or putting textures together that are
  //      only used by the map renderer
  //  - building the list of references for each type, so that unreferenced
  //      types are ignored.
  bool prebuild(bool optimize);
  
  // Translates all of the game resources into their compiled format.  This
  // includes removing entries that have no references (where appropriate),
  // atlasing textures, writing images to buffers filled with PNG data,
  // optimizing meshes, and processing each map into its special format.
  // It is in this stage that two separate game files emerge: one for the
  // client and one for the server.
  bool assemble(bool optimize);
  
  // Streams all of the processed data into a single in-memory buffer for
  // each game file.
  bool serialize();


private:
  void zero();
  void info(const char* message, ...);
  void warning(const char* message, ...);
  void error(const char* message, ...);
  bool writeGameFile(const char* file_name, GameFileWriter* writer);

  
private:
  Tools::CompleteEditor* source_;
  ProgressCallback* progress_;
  ClientGameFileData* client_data_;
  ServerGameFileData* server_data_;
  GameFileWriter client_writer_, server_writer_;
};



}
}

#endif