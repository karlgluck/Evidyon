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
#include "confirm.h"
#include <stdio.h>  // printf

#ifdef WIN32
// OutputDebugString, MessageBox, and Clipboard functions
#include <windows.h>
#endif


//----[  _confirm_failed_fn  ]-------------------------------------------------
void _confirm_failed_fn(const char* expression,
                        const char* file,
                        unsigned int line,
                        bool* sentinel) {
  char message[4096];
#ifdef WIN32
  // this is a Visual Studio-compatible message that will take the programmer
  // directly to the problem line when the user double-clicks on the the
  // message in the output pane
  sprintf_s(message,
            sizeof(message),
            "%s(%lu):  !confirm(%s)\n",
            file,
            line,
            expression);
  OutputDebugStringA(message);
#endif

  // this message prints to a console window
  sprintf_s(message,
            sizeof(message),
            "confirm(%s)\nFile:\t%s\nLine:\t%lu\n", expression, file, line);
  printf("Debug Error:  %s", message);

  if (*sentinel) {
#ifdef WIN32
    sprintf_s(
      message,
      sizeof(message),
      "Debug Error!\r\n\r\nThis program has encountered an error:"\
      "\r\n\r\nFailed:\t%s\r\nFile:\t%s\r\nLine:\t%lu\r\n\r\n\r\n"\
      "Press Ignore to continue normally and avoid seeing this message "\
      "again.\r\n\r\nTo open a debugger and examine the problem, press Retry."\
      "Select Abort to immediately end the program.\r\n\r\nThis text will be "\
      "copied to your clipboard.",
      expression,
      file,
      line);
    int rv = MessageBoxA(NULL,
                         message,
                         "Debug Error",
                         MB_ICONHAND|MB_ABORTRETRYIGNORE);

    // this code adopted from an article by Steve Rabin in
    // Game Programming Gems (c) 2000 pp 113-114
    if (OpenClipboard(NULL)) {
      HGLOBAL memory;
      size_t bytes = strlen(message)+1;
      memory = GlobalAlloc(GHND|GMEM_DDESHARE, bytes);
      if (memory) {
        char* buffer = (char*)GlobalLock(memory);
        memcpy(buffer, message, bytes);
        GlobalUnlock(memory);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, memory);
      }
      CloseClipboard();
    }

    switch (rv) {
    default:
    case IDIGNORE:
      // ignore this in the future
      *sentinel = false;
      break;
    case IDABORT:
      // exit the application
      exit(0);
      break;
    case IDRETRY:
      // continue on our way; the macro will invoke a breakpoint
      break;
    }
#endif
  }
}
