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
#include "../dc/dcstream.h"
#include "../dc/dcbufferstream.h"
#include <memory.h>



using namespace dc;



//-----------------------------------------------------------------------------
// Name: dcBufferStream
// Desc: Resets the class's internal variables
//-----------------------------------------------------------------------------
dcBufferStream::dcBufferStream()
{
    myBuffer = 0;
    myBufferSize = 0;
    myAccessPtr = 0;
}


//-----------------------------------------------------------------------------
// Name: dcBufferStream
// Desc: Cleans up the class
//-----------------------------------------------------------------------------
dcBufferStream::~dcBufferStream()
{
    if( myBuffer )
        delete[] myBuffer;
}



//-----------------------------------------------------------------------------
// Name: seek
// Desc: Moves to a place within the stream
//-----------------------------------------------------------------------------
bool dcBufferStream::seek( size_t location )
{
    if( location < 0 || location > myBufferSize ) return false;
    myAccessPtr = location;
    return true;
}



//-----------------------------------------------------------------------------
// Name: move
// Desc: Moves the buffer's read/write cursor
//-----------------------------------------------------------------------------
bool dcBufferStream::move( int Amount )
{
    return seek( (int)myAccessPtr + Amount );
}



//-----------------------------------------------------------------------------
// Name: read
// Desc: Gets data from the buffer
//-----------------------------------------------------------------------------
bool dcBufferStream::read( void* buffer, size_t size )
{
    // Don't read past the end of the buffer
    if( (myBuffer == 0) || (myAccessPtr + size ) > myBufferSize ) return false;

    // Scan the information
    memcpy_s( buffer, size, myBuffer + myAccessPtr, size );

    // Advance the pointer
    myAccessPtr += size;

    // Success
    return true;
}



//-----------------------------------------------------------------------------
// Name: end
// Desc: Determines EOF condition
//-----------------------------------------------------------------------------
bool dcBufferStream::end()
{
    return myAccessPtr == myBufferSize;
}



//-----------------------------------------------------------------------------
// Name: write
// Desc: Puts data into the buffer
//-----------------------------------------------------------------------------
bool dcBufferStream::write( const void* buffer, size_t size )
{
    // If we would write past the end of the buffer, reallocate it
    if( (myAccessPtr + size ) > myBufferSize )
    {
        // Calculate a new buffer size that is double the current size, padded a little bit
        size_t newBufferSize = (size_t)((myBufferSize + myAccessPtr + size) * 2);

        // Create a new buffer 
        unsigned char* pNewBuffer = new unsigned char[newBufferSize];
        if( !pNewBuffer ) return false;

        // If there was anything in our current buffer, save it to the new one
        if( myBuffer != 0 )
        {
            memcpy( pNewBuffer, myBuffer, myBufferSize );
            delete[] myBuffer;
        }

        // Set the buffer pointer
        myBuffer = pNewBuffer;

        // Save the new size
        myBufferSize = newBufferSize;
    }

    // Scan the information into the buffer
    memcpy_s( myBuffer + myAccessPtr, myBufferSize - myAccessPtr, buffer, size );

    // Advance the pointer
    myAccessPtr += size;

    // Success
    return true;
}



//-----------------------------------------------------------------------------
// Name: tell
// Desc: Gets the current position of the stream, such that
//-----------------------------------------------------------------------------
size_t dcBufferStream::tell()
{
    return myAccessPtr;
}
