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
#include <string>
#include "../dc/dcstream.h"     // Basic stream interfaces
#include <stdio.h>              // For the standard file declaration and methods
#include "../dc/dcfilestream.h"


using namespace dc;


//-----------------------------------------------------------------------------
// Name:  dcFileStream
// Desc:  
//-----------------------------------------------------------------------------
dcFileStream::dcFileStream()
{
    myFile = 0;
}


//-----------------------------------------------------------------------------
// Name:  ~dcFileStream
// Desc:  
//-----------------------------------------------------------------------------
dcFileStream::~dcFileStream()
{
    close();
}


//-----------------------------------------------------------------------------
// Name:  open
// Desc:  
//-----------------------------------------------------------------------------
bool dcFileStream::open( const char * name, int how )
{
    // Close a currently open file
    close();

    // Which access specifier to use when opening the file
    const char * mask;

    // Build the access specifier
    switch( how )
    {
        case STREAM_OVERWRITE|STREAM_TEXT:  mask = "w+t"; break;
        case STREAM_OVERWRITE:              mask = "w+b"; break;
        case STREAM_TEXT:                   mask = "r+t"; break;
        default:                            mask = "r+b"; break;
    }

    // Open the file
    return 0 == fopen_s( &myFile, name, mask );
}


//-----------------------------------------------------------------------------
// Name:  close
// Desc:  
//-----------------------------------------------------------------------------
void dcFileStream::close()
{
    if( myFile )
    {
        fclose( myFile );
        myFile = 0;
    }
}


//-----------------------------------------------------------------------------
// Name:  seek
// Desc:  
//-----------------------------------------------------------------------------
bool dcFileStream::seek( size_t location )
{
    return 0 == fseek( myFile, location, SEEK_SET );
}


//-----------------------------------------------------------------------------
// Name:  move
// Desc:  
//-----------------------------------------------------------------------------
bool dcFileStream::move( int amount )
{
    if( myFile )
        return 0 == fseek( myFile, amount, SEEK_CUR );
    else
        return false;
}


//-----------------------------------------------------------------------------
// Name:  tell
// Desc:  
//-----------------------------------------------------------------------------
size_t dcFileStream::tell()
{
    if( myFile )
        return ftell( myFile );
    else
        return -1;
}


//-----------------------------------------------------------------------------
// Name:  read
// Desc:  
//-----------------------------------------------------------------------------
bool dcFileStream::read( void * buffer, size_t size )
{
    if( myFile )
        return 1 == fread( buffer, size, 1, myFile );
    else
        return false;
}


//-----------------------------------------------------------------------------
// Name:  end
// Desc:  
//-----------------------------------------------------------------------------
bool dcFileStream::end()
{
    if( myFile )
        return 0 != feof(myFile);
    else
        return true;
}


//-----------------------------------------------------------------------------
// Name:  write
// Desc:  
//-----------------------------------------------------------------------------
bool dcFileStream::write( const void * buffer, size_t size )
{
    if( myFile )
        return 1 == fwrite( buffer, size, 1, myFile );
    else
        return false;
}



//-----------------------------------------------------------------------------
// Name:  flush
// Desc:  Forces this file stream to write to the disk
//-----------------------------------------------------------------------------
void dcFileStream::flush()
{
    if( myFile ) fflush( myFile );
}



//-----------------------------------------------------------------------------
// Name:  size
// Desc:  
//-----------------------------------------------------------------------------
size_t dcFileStream::size() {
  if (myFile) {
    long current = ftell(myFile);
    fseek(myFile, 0, SEEK_END);
    size_t size = ftell(myFile);
    fseek(myFile, current, SEEK_SET);
    return size;
  } else {
    return 0;
  }
}