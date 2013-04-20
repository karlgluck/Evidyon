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
#ifndef __DCSTREAM_H__
#define __DCSTREAM_H__

#include <string>

namespace dc
{
    /**
     * Basic input/output stream declaration.  It isn't too useful on its own, but it prevents
     * code duplication further down the inheritance tree.
     *   @author Karl Gluck
     */
    class dcStream
    {
        public:

            /**
             * Virtual destructor to let subclasses run their destructors
             */
            virtual ~dcStream();

            /**
             * Moves the IO cursor to a specific point in the file
             *   @param location Byte index to move to
             *   @return Whether or not this location could be moved to
             */
            virtual bool seek( size_t location ) = 0;

            /**
             * Shifts the cursor in the file relative to the current position
             *   @param amount Number of bytes to shift
             *   @return Whether or not the IO point could be changed
             */
            virtual bool move( int amount ) = 0;

            /**
             * Gets the current position of the stream, such that
             * calling seek(tell()) will not move the cursor.
             * @return The current stream location
             */
            virtual size_t tell() = 0;
    };


    /**
     * Standard set of methods for all input streams
     *   @author Karl Gluck
     */
    class dcStreamIn : public dcStream
    {
        public:

            /**
             * Gets data from this stream
             *   @param buffer Destination for the data
             *   @param size How many bytes to read
             *   @return Operation success flag
             */
            virtual bool read( void * buffer, size_t size ) = 0;

            /**
             * Determines whether or not the stream has reached the end of its input
             *   @return 'true' if there is no more data to be read
             */
            virtual bool end() = 0;

        public:

            /**
             * Reads a string written with writeString from the stream.  This method
             * confirms that the string is no longer than the given number of characters,
             * otherwise the method fails.
             * @param string The destination for the string being read
             * @param lengthLimit Upper limit on the number of characters
             *                    in the string; the default is no limit.
             * @return Whether or not the string could be read
             */
            bool readString( std::string* string, size_t lengthLimit = -1 );
    };

    /**
     * Standard set of methods for all output streams
     *   @author Karl Gluck
     */
    class dcStreamOut : public dcStream
    {
        public:

            /**
             * Transfers data into the stream
             * @param buffer Source data
             * @param size How many bytes to write
             * @return Success flag
             */
            virtual bool write( const void * buffer, size_t size ) = 0;

        public:

            /**
             * Writes a string into the stream to be read by dcStreamIn::readString 
             * @param string The string to write
             * @return Whether or not the string could be written
             */
            bool writeString( const std::string& string );

            /**
             * Writes an instance of any parameter type into this stream
             */
            template <typename T> inline bool write(const T& type) {
              return write((const void*)((const T*)&type), sizeof(T));
            }
    };
}



#endif
