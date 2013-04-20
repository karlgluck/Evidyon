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
#ifndef __ADMINCONSOLE_H__
#define __ADMINCONSOLE_H__
#pragma once


#include <windows.h>
#include <string>


//#define ENABLE_ADMIN_CONSOLE


typedef struct _ENetPacket ENetPacket;


namespace Evidyon {
namespace Network {
namespace Packet {
enum Message;
}
}

#ifdef ENABLE_ADMIN_CONSOLE

class AdminConsole {
public:
  static const int WIDTH  = 80;
  static const int HEIGHT = 120;
  static const int FIRST_SCROLLING_OUTPUT_LINE = 25;
  static const int COMMAND_BUFFER_SIZE = 512;
  static const int MAX_COMMAND_STRING_LENGTH = 64;
  
  enum Command {
    COMMAND_DEBUG,
    COMMAND_EXIT,
    COMMAND_ACTORS,
    COMMAND_SQLQUERY,
    COMMAND_CREATEITEM,
    COMMAND_CHARACTERS, // get characters on account
    COMMAND_INVENTORY,  // get inventory of character (inventory> 115)
    COMMAND_STORAGE,    // get storage on account (storage> 1234567)
    COMMAND_BACKUP,     // invoke a server memory -> disk database backup
    COMMAND_TURNOFF,    // shut down the server (should auto-restart after self update)
    COMMAND_ONLINE,     // list clients currently online
    COMMAND_COMMIT,     // commits own items to database & checks them out again
    COMMAND_RESPAWNALL, // puts all characters at spawn point
    COMMAND_UNITTEST_DUPEITEM, // dupes the first item in inventory
    COMMAND_RESOURCES,  // display server resource information
    COMMAND_RESETATTRIBUTES,
    COMMAND_QUERYCHARACTERSTATISTICS,
    COMMAND_STRESSTESTS,
    COMMAND_GODMODE,
    COMMAND_REPACKITEMS,
    COMMAND_UNITTEST_DROPONDEATH, // drops items on ground as if character had died  (ut-deathdrop)
    COMMAND_SETALIGNMENT,
    COMMAND_RESETALLPKS,
    COMMAND_COUNT,
    COMMAND_INVALID,
  };

public:

  void create();
  void destroy();

  // Adds output text to the window.
  void print(const char* text);

  // Call every frame to keep the console updated
  bool update(Command* command,
              std::string* output_parameters);

  // Checks to see if the incoming message is an admin message.  If it is,
  // the message was processed and this method returns 'true'.  The caller
  // should still deallocate the packet as usual.
  bool examineNetworkMessage(Network::Packet::Message message, ENetPacket* enet_packet);

  // Pushes a command onto the console to be processed
  void pushCommand(Command command,
                   const char* parameters);

private:
  bool readInput();
  void rewriteCommandBuffer(int input_length);
  void scrollDown(int lines);
  void appendCharacter(char c);
  void clearCommandBuffer();
  void removeCharacter();
  const char* getCommandString(Command command);
  int getLinesToPrint(const char* text);

private:
  HANDLE in_, out_;
  char command_buffer_[COMMAND_BUFFER_SIZE];
};

#endif


}

#endif