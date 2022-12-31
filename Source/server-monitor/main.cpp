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
#include <windows.h>
#include <string>
#include <direct.h>

#include "../shared/updater/updatefile.h"
#include "../shared/updater-monitor/serverupdatefile.h"
#include <dcx/dcxwin32internetstream.h>
#include <dc/debug>
#include "../common/md5/md5wrapper.h"

#pragma comment(lib, "wininet.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "dc_d.lib")
#pragma comment(lib, "dcx_d.lib")
#else
#pragma comment(lib, "dc.lib")
#pragma comment(lib, "dcx.lib")
#endif


/// This flag is set to 'false' when the console's window closes
bool globalRunningFlag = true;
HANDLE globalCloseEvent = 0;


//-----------------------------------------------------------------------------
// Name: ConsoleHandler
// Desc: Handles Windows messages to the console window
//-----------------------------------------------------------------------------
BOOL WINAPI ConsoleHandler( DWORD CtrlType )
{
    if( CtrlType == CTRL_SHUTDOWN_EVENT ||
        CtrlType == CTRL_CLOSE_EVENT )
    {
        globalRunningFlag = false;
        SetEvent( globalCloseEvent );
    }

    return TRUE;
}

bool DownloadFile(const std::string& filename) {
  // Open the local file
  HANDLE hDestination;
  hDestination = CreateFile( filename.c_str(),
                             GENERIC_WRITE,
                             FILE_SHARE_READ,
                             NULL,
                             CREATE_ALWAYS,
                             FILE_FLAG_WRITE_THROUGH | 
                             FILE_FLAG_SEQUENTIAL_SCAN,
                             NULL );
  CONFIRM( hDestination ) else return false;

  // Download this file
  dcx::dcxWin32InternetStream reader;
  std::string fileURL = Evidyon::ONLINE_SERVER_UPDATE_FOLDER;
  fileURL.append(filename);
  if( !reader.open( "EvidyonServer", fileURL.c_str() ) )
  {
      printf("couldn't open remote file!");
      CloseHandle( hDestination );
      return false;
  }
  char buffer[512];
  size_t bytesRead = 0;
  size_t bytes;
  size_t hundredsOfKb = 0;
  while( reader.scan( buffer, sizeof(buffer), &bytes ) && !reader.end() )
  {
      // Write this data into the output file
      DWORD bytesWritten;
      BOOL success = WriteFile( hDestination,
                                buffer,
                                bytes,
                               &bytesWritten,
                                NULL );

      // Add to the number of bytes that were read
      bytesRead += bytes;

      // for every 100 kb, print a dot
      if (bytesRead / (100000) > hundredsOfKb) {
        printf(".");
        hundredsOfKb++;
      }
  }

  bool completed = reader.end();

  // Get rid of the output file and internet connection
  reader.close();
  CloseHandle( hDestination );

  CONFIRM(completed) else return false;
  return true;
}


bool UpdateServer() {

  Evidyon::UpdateFile update_file;
  CONFIRM(Evidyon::DownloadUpdateFile(Evidyon::ONLINE_SERVER_UPDATE_FILE, &update_file)) else {
    return false;
  }

  { // make sure all of the files are up-to-date
    printf("\nchecking files:");
    for (std::map<std::string, Evidyon::UpdateFileData>::const_iterator i = update_file.files.begin();
         i != update_file.files.end(); ++i) {
      const std::string& filename = i->first;
      printf("\n  %s", filename.c_str());
      md5wrapper wrappy;
      std::string local_md5 = wrappy.getHashFromFile(filename);
      if (local_md5.compare(i->second.md5) != 0) {  // this file needs to be updated
        printf(" is out of date; downloading %I64d bytes:  ", i->second.file_bytes);
        CONFIRM(DownloadFile(filename)) else {
          printf("  [error]\n");
        }
      }
    }
  }

  return true;
}


//-----------------------------------------------------------------------------
// Name: main
// Desc: Takes argument 2 (the server's EXE file name) and runs the program.  When the program
//       bombs, the time is logged and the server is restarted.
//-----------------------------------------------------------------------------
int main( int argc, char** argv )
{
    if( argc < 2 ) return -1;

    std::string serverEXEFile = argv[1];//std::string("\"") + argv[1] + "\"";

    int timesRestarted = 1;

    std::string directory = serverEXEFile.substr( 0, serverEXEFile.find_last_of( "\\" ) );

    globalCloseEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    while( globalRunningFlag )
    {
        STARTUPINFO startupInfo;
        ZeroMemory( &startupInfo, sizeof(startupInfo) );

        PROCESS_INFORMATION processInfo;

        // Make sure the directory is set
        SetCurrentDirectory( directory.c_str() );
        _chdir( directory.c_str() );

        while (!UpdateServer()) {
          printf("\n\nCouldn't update the server.  Trying again in ");
          for (int i = 0; i < 5; ++i) {
            printf("%i minute%s...", 5-i, (i != 1) ? "s" : "");
            Sleep(1000*60);
          }
          continue;
        }

        // Start the server
        if( !CreateProcessA(serverEXEFile.c_str(),
                            NULL,
                            NULL,
                            NULL,
                            FALSE,
                            CREATE_NEW_CONSOLE|ABOVE_NORMAL_PRIORITY_CLASS,
                            NULL,
                            NULL,
                            &startupInfo,
                            &processInfo))
        {
            DWORD error = GetLastError();
            MessageBox( NULL, "Couldn't restart server!", "Error", MB_OK );
            return -1;
        }

        // Change the process priority
        SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);

        // Wait for the server to close
        HANDLE waitEvents[] = { globalCloseEvent, processInfo.hProcess };
        WaitForMultipleObjects( 2, waitEvents, FALSE, INFINITE );

        CloseHandle( processInfo.hProcess );
        CloseHandle( processInfo.hThread );

        // Restart the server after 15 seconds
        if( globalRunningFlag )
        {
            printf( "\n\nServer restarting (done %lu times)...", ++timesRestarted );
            for (int i = 0; i < 3; ++i) {
              printf( "%i seconds...", (3-i) * 15);
              Sleep( 15000 );
            }
            printf("\n");
        }
    }

    return 0;
}