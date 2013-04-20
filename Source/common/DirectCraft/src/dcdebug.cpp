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
#include <string>
#include "../dc/filestream"
#include "../dc/dcdebug.h"
#include <string.h>
#include <stdarg.h>
#include <time.h>       // For getting formatted time
#define WIN32_LEAN_AND_MEAN
#include <windows.h>    // For OutputDebugStringA
#include <direct.h>


using namespace dc;



class dcDebugStreamOut : public dcStreamOut {
public:
  dcDebugStreamOut() {
    file_ = 0;
  }

  ~dcDebugStreamOut() {
    close();
  }

  /**
   * Opens a file on fixed media
   *   @param name Path to the file
   *   @param how Combination of access specifiers
   *   @return Whether or not the operation succeeded
   */
  bool open( const char * name, int how ) {
    close();
    const char * mask;
    switch( how ) {
      case STREAM_OVERWRITE|STREAM_TEXT:  mask = "w+t"; break;
      case STREAM_OVERWRITE:              mask = "w+b"; break;
      case STREAM_TEXT:                   mask = "r+t"; break;
      default:                            mask = "r+b"; break;
    }

    // Open the file
    return 0 == fopen_s( &file_, name, mask );
  }

  /**
   * Closes the file stream
   */
  void close() {
    if (file_) {
      fclose(file_);
      file_ = NULL;
    }
  }

  /**
   * Moves to a place within the file
   *   @param location Number of bytes offset from the start of the file
   *   @return 'true' if the location was valid
   */
  bool seek( size_t location ) {
    if (!file_) return false;
    return 0 == fseek( file_, location, SEEK_SET );
  }

  /**
   * Moves the file's read/write cursor
   *   @param amount How many bytes to move
   *   @return 'true' if the move succeeded
   */
  bool move( int amount ) {
    if (!file_) return false;
    return 0 == fseek( file_, amount, SEEK_CUR );
  }

  /**
   * Gets the current position of the file stream, such that
   * calling seek(tell()) will not move the cursor.
   * @return The current stream location
   */
  size_t tell() {
    if (!file_) return false;
    return static_cast<size_t>(ftell(file_));
  }

  /**
   * Puts data into the file
   *   @param buffer Buffer of data to write
   *   @param size How many bytes to write
   *   @return Whether or not the writing was successful
   */
  bool write( const void * buffer, size_t size ) {
    OutputDebugString((LPCSTR)buffer);
    if (!file_) return false;
    if (1 == fwrite(buffer, size, 1, file_)) {
      fflush(file_); // force changes to update
      return true;
    } else {
      return false;
    }
  }


private:

  /// Source file
  FILE* file_;
};



dcDebugStreamOut* dcDebugDefaultStream() {
    static dcDebugStreamOut stream;
    return &stream;
}

dcStreamOut* dcDebugDefaultStreamOpen(const char* file) {
    dcDebugStreamOut* stream = dcDebugDefaultStream();
    stream->open(file, STREAM_TEXT);
    return stream;
}

void dcDebugDefaultStreamClose() {
    dcDebugStreamOut* stream = dcDebugDefaultStream();
    stream->close();
}



//-----------------------------------------------------------------------------
// Name:  global
// Desc:  Obtains a dcDebug interface that will output data from the project
//-----------------------------------------------------------------------------
dcDebug* dcDebug::global( const char* file, unsigned long line )
{
    static dcDebug globaldcDebug( false );

    // Set the source file and line
    globaldcDebug.myFile = file;
    globaldcDebug.myLine = line;

    // Return the object
    return &globaldcDebug;
}




//-----------------------------------------------------------------------------
// Name:  nullGlobal
// Desc:  The null-global object is a disabled dcDebug object
//-----------------------------------------------------------------------------
dcDebug* dcDebug::nullGlobal()
{
    static dcDebug globalNulldcDebug( true );
    return &globalNulldcDebug;
}




//-----------------------------------------------------------------------------
// Name:  dcDebug
// Desc:  Initializes this class
//-----------------------------------------------------------------------------
dcDebug::dcDebug( bool isNull )
{
    // Reset internal values
    myFile = 0;
    myLine = 0;
    thisIsNull = isNull;
    myOutputStream = 0;

    // If this is the dummy log, just exit because it doesn't need to do anything
    if( isNull ) return;

    _mkdir("./logs/");

    // Generate the file name for the dcDebug log
    char file[256], mask[256];
    sprintf_s( mask, sizeof(mask), "./logs/#debug - %%s.txt" );

    // Find the current time
    tm * loc;
    time_t ltime;
    time( &ltime );
//    localtime_s( loc, &ltime );
	loc = localtime( &ltime );

    // Print the date and time
    char dateTime[32];
	asctime_s( dateTime, sizeof(dateTime), loc );
    //strcpy_s( dateTime, sizeof(dateTime), asctime( loc ) );
    dateTime[strlen(dateTime)-1] = '\0';

    // Create the file to open
    sprintf_s( file, sizeof(file), mask, dateTime );

    // Remove invalid characters
    for( int i = 0; file[i] != '\0'; ++i )
        if( file[i] == ':' )
            file[i] = '\'';

    // Save the file name
    myOutputLogFile = file;

    // Open the output stream
    myOutputStream = dcDebugDefaultStreamOpen(file);
}



//-----------------------------------------------------------------------------
// Name:  print
// Desc:  Just writes directly to the output
//-----------------------------------------------------------------------------
bool dcDebug::print( const char* message )
{
    if( thisIsNull ) return false;

    // Write to the output file
    if( myOutputStream != 0 )
    {
        myOutputStream->write( message, strlen(message) );
    }

    // Done
    return true;
}






//-----------------------------------------------------------------------------
// Name:  info
// Desc:  Logs an information message to the dcDebug output
//-----------------------------------------------------------------------------
bool dcDebug::info( const char* message, ... )
{
    if( thisIsNull  ) return false;

    // The buffer for compiling the message
    char messageBuffer[2048], outputBuffer[2048];

    // Compile the message to the string
    va_list args;
    va_start( args, message );
    _vsnprintf_s( messageBuffer, sizeof(messageBuffer), sizeof(messageBuffer), message, args );  // Output to the buffer
    messageBuffer[sizeof(messageBuffer)-1] = '\0';
    va_end( args );

    // Format the output message
    sprintf_s( outputBuffer, "%s(%u): [INFO]  %s\n", myFile, myLine, messageBuffer );
    
    // Output the message
    print( outputBuffer );

    // Done
    return true;
}




//-----------------------------------------------------------------------------
// Name:  warning
// Desc:  Logs a warning message to the dcDebug output
//-----------------------------------------------------------------------------
bool dcDebug::warning( const char* message, ... )
{
    if( thisIsNull  ) return false;

    // The buffer for compiling the message
    char messageBuffer[2048], outputBuffer[2048];

    // Compile the message to the string
    va_list args;
    va_start( args, message );
    _vsnprintf_s( messageBuffer, sizeof(messageBuffer), sizeof(messageBuffer), message, args );  // Output to the buffer
    messageBuffer[sizeof(messageBuffer)-1] = '\0';
    va_end( args );

    // Format the output message
    sprintf_s( outputBuffer, "%s(%u): [WARNING]  %s\n", myFile, myLine, messageBuffer );
    
    // Output the message
    print( outputBuffer );

    // Done
    return true;
}




//-----------------------------------------------------------------------------
// Name:  error
// Desc:  Logs an error message to the dcDebug output
//-----------------------------------------------------------------------------
bool dcDebug::error( const char* message, ... )
{
    if( thisIsNull  ) return false;

    // The buffer for compiling the message
    char messageBuffer[2048], outputBuffer[2048];

    // Compile the message to the string
    va_list args;
    va_start( args, message );
    _vsnprintf_s( messageBuffer, sizeof(messageBuffer), sizeof(messageBuffer), message, args );  // Output to the buffer
    messageBuffer[sizeof(messageBuffer)-1] = '\0';
    va_end( args );

    // Format the output message
    sprintf_s( outputBuffer, "%s(%u): [ERROR]  %s\n", myFile, myLine, messageBuffer );
    
    // Output the message
    print( outputBuffer );

    // Done
    return true;
}




//-----------------------------------------------------------------------------
// Name:  fatal
// Desc:  be sent to the user via a message box during dcDebug::atExit(), and the dcDebug
//-----------------------------------------------------------------------------
bool dcDebug::fatal( const char* message, ... )
{
    if( thisIsNull  ) return false;

    // The buffer for compiling the message
    char messageBuffer[2048], outputBuffer[2048];

    // Compile the message to the string
    va_list args;
    va_start( args, message );
    _vsnprintf_s( messageBuffer, sizeof(messageBuffer), sizeof(messageBuffer), message, args );  // Output to the buffer
    messageBuffer[sizeof(messageBuffer)-1] = '\0';
    va_end( args );

    // Format the output message
    sprintf_s( outputBuffer, "%s(%u): [FATAL]  %s\n", myFile, myLine, messageBuffer );
    
    // Output the message
    print( outputBuffer );

    // Add this to the fatal error buffer
    myFatalError.append( outputBuffer );

    // Done
    return true;
}




//-----------------------------------------------------------------------------
// Name:  changeOutputStream
// Desc:  Switches the output stream
//-----------------------------------------------------------------------------
void dcDebug::changeOutputStream( dc::dcStreamOut* stream ) {
  if( myOutputStream != 0 ) {
    dcDebugDefaultStreamClose();
    myOutputStream = NULL;
  }

  if (stream == 0) {
    myOutputStream = dcDebugDefaultStreamOpen(myOutputLogFile.c_str());
  } else {
    myOutputStream = stream;
  }
}



const std::string& dcDebug::getLogFilename() const {
    return myOutputLogFile;
}





//-----------------------------------------------------------------------------
// Name:  atExit
// Desc:  Call this method when the program is shutting down to prompt the user
//-----------------------------------------------------------------------------
void dcDebug::atExit()
{
    // feb 2009: disabled because this causes problems shutting down the Evidyon server
    //// If the fatal error flag was triggered, show the error
    //if( !myFatalError.empty() )
    //{
    //    // Show the user a message box
    //    MessageBox( GetDesktopWindow(), myFatalError.c_str(), "Fatal Error - Evidyon Crashed!", MB_OK );

    //    // Bring up the dcDebug log
    //    char commandLine[512], currentDirectory[512];
    //    GetCurrentDirectory( sizeof(currentDirectory), currentDirectory );
    //    sprintf_s( commandLine, sizeof(commandLine), "NOTEPAD.EXE \"%s\\%s\"", currentDirectory, myOutputLogFile.c_str() );

    //    // This structure just needs to exist to create the process
    //    STARTUPINFO startupInfo;
    //    ZeroMemory( &startupInfo, sizeof(startupInfo) );
    //    startupInfo.cb = sizeof(STARTUPINFO);

    //    // Receives information about the process
    //    PROCESS_INFORMATION processInfo;
    //    ZeroMemory( &processInfo, sizeof(processInfo) );

    //    // Create the process
    //    CreateProcess( NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo );
    //}

    myOutputStream = NULL;
}
