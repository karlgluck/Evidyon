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

// Fiber--it's good for you, and it's also good for preventing context switches.


#define WINVER          0x400
#define _WIN32_WINNT    0x400


#include <windows.h>
#include <stdio.h>

struct FiberData
{
    
};

#define FIBER_PRIMARY 0
#define FIBER1 1
#define FIBER2 2
LPVOID g_pFibers[3];

TCHAR buffer[32];
int i;


VOID __stdcall Fiber1( LPVOID lpParameter )
{
    buffer[i++] = '1';
    while( i < sizeof(buffer) - 1 )
    {
        buffer[i++] = 'b';
        SwitchToFiber( g_pFibers[FIBER2] );
    }

    SwitchToFiber( g_pFibers[FIBER_PRIMARY] );
}

VOID __stdcall Fiber2( LPVOID lpParameter )
{
    buffer[i++] = '2';
    while( i < sizeof(buffer) - 1 )
    {
        buffer[i++] = 'a';
        SwitchToFiber( g_pFibers[FIBER1] );
    }

    SwitchToFiber( g_pFibers[FIBER_PRIMARY] );
}

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
  // Reset data
  i = 0;
  memset( buffer, 0, sizeof(buffer) );

  // Convert this thread
  g_pFibers[FIBER_PRIMARY] = ConvertThreadToFiber( NULL );

  // Create the fibers
  g_pFibers[FIBER1] = CreateFiber( 0, Fiber1, NULL );
  g_pFibers[FIBER2] = CreateFiber( 0, Fiber2, NULL );

  // Execute the fibers
  SwitchToFiber( g_pFibers[FIBER1] );

  // Terminate the input
  buffer[i++] = '\0';

  DeleteFiber( g_pFibers[FIBER1] );
  DeleteFiber( g_pFibers[FIBER2] );

  OutputDebugString( buffer );
}


/*

use a MMF for the resource file (CreateFileMapping, MapViewOfFile) and just
pass pointers.  when the loading task exits, destroy the mapping



two kinds of task relationships: hierarchy and sequence.

some tasks contain other tasks,
some tasks must be done in a particular order


for simplicity, all tasks can only have one prereq; however,
a task can contain as many other tasks as it wants.

a task is only complete once its children are complete

ex.
  StartGame
    ReadConfigFile
    AcquireWindow
    AcquireGraphics(AcquireWindow)
    LoadLoginGUIImages(AcquireGraphics)
    LoadImages(AcquireGraphics)
    LoadTextures(LoadImages)
    AcquireInput(AcquireWindow)
    AcquireNetwork
    AcquireAudio
    LoadSounds(AcquireAudio)

   -> EstablishServerConnection(AcquireNetwork)
   -> RenderConnectToServer(AcquireLoginImages)

 EstablishServerConnection
  


*/