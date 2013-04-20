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
#include "common/enet/enet.h"
#include "adminconsole.h"
#include <stdio.h>
#include "shared/client-server/packets.h"
#include "decodepackets.h"

namespace Evidyon {

#ifdef ENABLE_ADMIN_CONSOLE
AdminConsole* console = NULL;

BOOL WINAPI ConsoleHandler(DWORD CtrlType)
{
  if (CtrlType == CTRL_SHUTDOWN_EVENT ||
    CtrlType == CTRL_CLOSE_EVENT) {
      if (console) {
        console->destroy();
      }
  }

  return TRUE;
}



void AdminConsole::create() {
  AllocConsole();
  SetConsoleTitle("Evidyon Admin Console");
  SetConsoleCtrlHandler(ConsoleHandler, FALSE);
  out_ = GetStdHandle(STD_OUTPUT_HANDLE);
  in_ = GetStdHandle(STD_INPUT_HANDLE);

  COORD size = { WIDTH, HEIGHT };
  SetConsoleScreenBufferSize(out_, size);

  command_buffer_[0] = '\0';

  scrollDown(1); // rewrite display area

  { // Create a bar above the scrolling area
    COORD coord = { 0, FIRST_SCROLLING_OUTPUT_LINE - 1 };
    DWORD written = 0;
    FillConsoleOutputCharacterA(out_, '=', WIDTH, coord, &written);
  }
  { // create a bar below the input area
    COORD coord = { 0, (COMMAND_BUFFER_SIZE / WIDTH) + 1 };
    DWORD written = 0;
    FillConsoleOutputCharacterA(out_, '=', WIDTH, coord, &written);
  }
}


void AdminConsole::destroy() {
  FreeConsole();
  in_ = NULL;
  out_ = NULL;
}


bool AdminConsole::update(AdminConsole::Command* command, std::string* output_parameters) {
  if (!readInput()) return false;

  char command_string[MAX_COMMAND_STRING_LENGTH];
  sscanf_s(command_buffer_,
           "%[^>]",
           command_string,
           MAX_COMMAND_STRING_LENGTH);

  int i;
  for (i = 0; i < COMMAND_COUNT; ++i) {
    const char* command_string_i = getCommandString(Command(i));
    if (_stricmp(command_string, command_string_i) != 0) continue;
    *command = AdminConsole::Command(i);
    const char* parameters = &command_buffer_[strlen(command_string_i)];

    // find the first non-whitespace character (or the end-of-command)
    char ch = *parameters;
    while (ch=='>' || ch==' ') { ch = *(++parameters); }
    *output_parameters = parameters;

    // finished
    break;
  }

  clearCommandBuffer();

  return i != COMMAND_COUNT; // i == COMMAND_COUNT if no command was found
}



bool AdminConsole::examineNetworkMessage(Network::Packet::Message message, ENetPacket* enet_packet) {
  using namespace Evidyon::Network::Packet;
  using namespace Client;
  switch (message) {
    case NETMSG_ADMIN_SERVERMESSAGE: {
      std::string reply;
      using namespace Evidyon::Network::Packet::Client;
      if (Decode::admin_serverMessage(enet_packet, &reply)) {
        print(reply.c_str());
      }
    } return true;
  }

  return false;
}




bool AdminConsole::readInput() {
  if (in_ == NULL) return false;
  DWORD events;
  GetNumberOfConsoleInputEvents(in_, &events);
  while (events > 0) {
    events--;

    // Read the event from the queue
    INPUT_RECORD record;
    DWORD events_read = 0;
    ReadConsoleInput(in_,
                    &record,
                     1,
                    &events_read);
    if (events_read == 1) {
      switch (record.EventType) {
        case KEY_EVENT:
          if (record.Event.KeyEvent.bKeyDown) {
            switch (record.Event.KeyEvent.wVirtualKeyCode) {
            case VK_ESCAPE:
            case VK_RETURN: return true;
            case VK_BACK: removeCharacter(); break;
            default: appendCharacter(record.Event.KeyEvent.uChar.AsciiChar); break;
            }

            rewriteCommandBuffer(strlen(command_buffer_));
          }
          break;
      }
    }
  }
  return false;
}


void AdminConsole::pushCommand(Command command,
                               const char* parameters) {
  sprintf_s(command_buffer_,
            COMMAND_BUFFER_SIZE,
            "%s>%s",
            getCommandString(command),
            parameters);
  rewriteCommandBuffer(strlen(command_buffer_));

  // Fake a 'return' key-press
  if (in_ != NULL) {
    INPUT_RECORD record;
    record.EventType = KEY_EVENT;
    record.Event.KeyEvent.bKeyDown = true;
    record.Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
    DWORD written = 0;
    WriteConsoleInput(in_, &record, 1, &written);
  }
}

void AdminConsole::rewriteCommandBuffer(int input_length) {
  if (out_ == NULL) return;

  COORD write_position = { 0, 0 };

  // Write the characters to the screen
  DWORD written = 0;
  WriteConsoleOutputCharacter(
      out_,             // Console output handle
      command_buffer_,  // Buffer to print
      input_length,     // Number of characters to write
      write_position,   // Coordinate to write data to
      &written);        // How many characters were written


  write_position.X += input_length;

  const char spaces[16+1] = "                ";
  WriteConsoleOutputCharacter(
      out_,
      spaces,
      16,
      write_position,
      &written);

  // scroll cursor
  while (write_position.X > WIDTH) {
    write_position.X -= WIDTH;
    write_position.Y++;
  }

  SetConsoleCursorPosition(out_, write_position);
}

void AdminConsole::scrollDown(int lines) {
  SMALL_RECT area = { 0, FIRST_SCROLLING_OUTPUT_LINE, WIDTH, HEIGHT };
  COORD dest = { 0, FIRST_SCROLLING_OUTPUT_LINE + lines };
  CHAR_INFO fill = { ' ', FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED };
  ScrollConsoleScreenBuffer(out_,
                            &area,
                            &area,
                            dest,
                            &fill);
}


void AdminConsole::appendCharacter(char c) {
  size_t length = strlen(command_buffer_);
  if (length + 1 < COMMAND_BUFFER_SIZE) {
    command_buffer_[length] = c;
    command_buffer_[++length] = '\0';
  }
}

void AdminConsole::clearCommandBuffer() {
  for (int i = 0; i < COMMAND_BUFFER_SIZE; ++i) command_buffer_[i] = ' ';
  rewriteCommandBuffer(COMMAND_BUFFER_SIZE);
  command_buffer_[0] = '\0';
  COORD write_position = { 0, 0 };
  SetConsoleCursorPosition(out_, write_position);
}

void AdminConsole::removeCharacter() {
  size_t length = strlen(command_buffer_);
  if (length > 0) {
    command_buffer_[--length] = '\0';
  }
}

const char* AdminConsole::getCommandString(Command command) {
  switch (command) {
    default:  return "";
    case COMMAND_DEBUG: return "dbg";
    case COMMAND_EXIT: return "exit";
    case COMMAND_ACTORS:  return "print actors";
    case COMMAND_SQLQUERY: return "sql";
    case COMMAND_CREATEITEM: return "create item";
    case COMMAND_CHARACTERS: return "characters";
    case COMMAND_INVENTORY: return "inventory";
    case COMMAND_BACKUP: return "backup";
    case COMMAND_TURNOFF: return "turn off server";
    case COMMAND_ONLINE: return "online";
    case COMMAND_STORAGE: return "storage";
    case COMMAND_COMMIT:  return "commit";
    case COMMAND_RESPAWNALL: return "respawn all";
    case COMMAND_UNITTEST_DUPEITEM: return "ut-dupe";
    case COMMAND_RESOURCES: return "resources";
    case COMMAND_RESETATTRIBUTES: return "reset attributes";
    case COMMAND_QUERYCHARACTERSTATISTICS: return "average character stats";
    case COMMAND_STRESSTESTS: return "stress test";
    case COMMAND_GODMODE: return "god mode";
    case COMMAND_REPACKITEMS: return "repack items";
    case COMMAND_UNITTEST_DROPONDEATH: return "ut-deathdrop";
    case COMMAND_SETALIGNMENT: return "set alignment";
    case COMMAND_RESETALLPKS: return "reset all pks";
  }
}


void AdminConsole::print(const char* text) {
  int lines = getLinesToPrint(text);
  scrollDown(lines);

  // Write the characters to the screen
  COORD write_position = { 0, FIRST_SCROLLING_OUTPUT_LINE };
  DWORD written = 0;
  WriteConsoleOutputCharacter(
      out_,             // Console output handle
      text,             // Buffer to print
      strlen(text),     // Number of characters to write
      write_position,   // Coordinate to write data to
      &written);        // How many characters were written
}


int AdminConsole::getLinesToPrint(const char* text) {
  int lines = 1;
  int current_x = 0;

  // count newlines and simple wrapping
  char ch = *text;
  while (ch != '\0') {
    if (ch == '\n') {
      current_x = 0;
      ++lines;
    } else {
      if (++current_x > WIDTH) {
        current_x = 0;
        ++lines;
      }
    }

    ch = *(++text);
  }

  return lines;
}

#endif

}
