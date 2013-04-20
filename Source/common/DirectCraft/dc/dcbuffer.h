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
#ifndef __DCBUFFER_H__
#define __DCBUFFER_H__


#include "dcstream.h"


namespace dc {


class dcBuffer
{
    public:

        class Reader : public dc::dcStreamIn
        {
            public:

                Reader();

                /**
                 * Sets up this reader to scan from a buffer
                 */
                explicit Reader(dcBuffer* buffer);

                void reset(dcBuffer* buffer);

                /**
                 * Moves the IO cursor to a specific point in the file
                 *   @param location Byte index to move to
                 *   @return Whether or not this location could be moved to
                 */
                bool seek(size_t location);

                /**
                 * Shifts the cursor in the file relative to the current position
                 *   @param amount Number of bytes to shift
                 *   @return Whether or not the IO point could be changed
                 */
                bool move(int amount);

                /**
                 * Gets the current position of the stream, such that
                 * calling seek(tell()) will not move the cursor.
                 * @return The current stream location
                 */
                size_t tell();

                /**
                 * Gets data from this stream
                 *   @param buffer Destination for the data
                 *   @param size How many bytes to read
                 *   @return Operation success flag
                 */
                bool read(void * buffer, size_t size);

                /**
                 * Determines whether or not the stream has reached the end of its input
                 *   @return 'true' if there is no more data to be read
                 */
                bool end();


            protected:

                /// The buffer being scanned
                dcBuffer* myBuffer;

                /// Where in the buffer we currently are reading from
                size_t myAccessPtr;
        };

    public:

        /**
         * Initializes the buffer
         */
        dcBuffer();

        /**
         * Cleans up when the buffer is no longer needed
         */
        ~dcBuffer();

        /**
         * Reads data from a source stream into the buffer
         */
        bool readFrom(dcStreamIn* stream, size_t bytes);
        bool readFrom(const void * buffer, size_t bytes);

        /**
         * Gets the raw data associated with this buffer so that it can be manipulated
         * directly.
         */
        void* getDataPointer() const;

        /**
         * Obtains the number of bytes of information stored in the buffer
         */
        size_t getDataSize() const;


    protected:

        /// The allocated memory the buffer uses
        unsigned char* myMemory;

        /// How much data is stored in this buffer
        size_t myDataSize;

        /// How many bytes of memory are allocated
        size_t myBufferSize;
};


}




#endif