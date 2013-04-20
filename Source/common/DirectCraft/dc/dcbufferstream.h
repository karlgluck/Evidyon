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
#ifndef __DCBUFFERSTREAM_H__
#define __DCBUFFERSTREAM_H__


namespace dc {

    /**
     * Implements a stream interface that writes to a buffer
     *   @author Karl Gluck
     */
    class dcBufferStream : public dcStreamIn, public dcStreamOut
    {
        public:

            /**
             * Resets the class's internal variables
             */
            dcBufferStream();

            /**
             * Cleans up the class
             */
            ~dcBufferStream();

            /**
             * Moves to a place within the stream
             *   @param location Number of bytes offset from the start of the file
             *   @return 'true' if the location was valid
             */
            bool seek( size_t location );

            /**
             * Moves the buffer's read/write cursor
             *   @param amount How many bytes to move
             *   @return 'true' if the move succeeded
             */
            bool move( int amount );

            /**
             * Gets data from the buffer
             *   @param buffer Destination for the bytes
             *   @param size How many bytes to read
             *   @return Operation success flag
             */
            bool read( void* buffer, size_t size );

            /**
             * Determines EOF condition
             *   @return 'true' if there is no more data to be read
             */
            bool end();

            /**
             * Puts data into the buffer
             *   @param buffer Buffer of data to write
             *   @param size How many bytes to write
             *   @return Whether or not the writing was successful
             */
            bool write( const void* buffer, size_t size );

            /**
             * Gets the current position of the stream, such that
             * calling seek(tell()) will not move the cursor.
             * @return The current stream location
             */
            size_t tell();


        private:

            /// The internal expanding buffer
            unsigned char* myBuffer;

            /// How large the buffer is
            size_t myBufferSize;

            /// The location of the read/write pointer
            size_t myAccessPtr;
    };

}

#endif
