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
#ifndef __DCXASYNCHRONOUSSISOTHREADDATACONTROLLER_H__
#define __DCXASYNCHRONOUSSISOTHREADDATACONTROLLER_H__

#include "dcxbitmask.h"

namespace dcx {

template <typename DataType, const size_t Entries> class dcxAsynchronousSISOThreadDataController
{
public:

  dcxAsynchronousSISOThreadDataController()
  {
      myDataArray = 0;
      myWritingThreadDataMask.clearAll();
      myReadingThreadDataMask.clearAll();
  }

  bool create()
  {
      myDataArray = new DataType[Entries];
      return myDataArray != 0;
  }
  
  void destroy()
  {
      if( myDataArray ) delete[] myDataArray;
      myDataArray = 0;
  }

  bool writeByAssignment( const DataType data )
  {
      size_t index = nextWriteableIndex();
      if( index < Entries )
      {
          // We found a location!  Write here and update the mask
          myDataArray[index] = data;
          myWritingThreadDataMask.toggle( index );
          return true;
      }
      else
          return false;
  }
  
  bool readByAssignment( DataType* returnValue )
  {
      size_t index = nextReadableIndex();
      if( index < Entries )
      {
          // We found a location!  Copy the data into a temporary variable and return it
          *returnValue = myDataArray[index];
          myReadingThreadDataMask.toggle( index );
          return true;
      }
      else
          return false;
  }

  bool isValid() const
  {
      return myDataArray != 0;
  }
  
protected:

  size_t nextWriteableIndex() const
  {
      if( !isValid() ) return ((size_t)(-1));

      for( size_t i = 0; i < dcxBitMask<Entries>::MASK_SIZE; ++i )
      {
          dcxBitMask<Entries>::WORD_TYPE writing, reading;
          writing = myWritingThreadDataMask.values[i];
          reading = myReadingThreadDataMask.values[i];
          
          // If a bit is the same for both entries, it is available
          // to be written.  Find an entry with this quality.
          dcxBitMask<Entries>::WORD_TYPE available = ~(writing ^ reading);
          
          size_t currentEntry = i * dcxBitMask<Entries>::BITS_IN_WORD;
          while( available )
          {
              // If this entry can be written, return its index
              if( 1 == (available & 1) ) return currentEntry;

              ++currentEntry;
              available >>= 1;
          }
      }
      
      // Couldn't find an entry
      return ((size_t)(-1));
  }

  size_t nextReadableIndex() const
  {
      if( !isValid() ) return ((size_t)(-1));

      for( size_t i = 0; i < dcxBitMask<Entries>::MASK_SIZE; ++i )
      {
          dcxBitMask<Entries>::WORD_TYPE writing, reading;
          writing = myWritingThreadDataMask.values[i];
          reading = myReadingThreadDataMask.values[i];
          
          // If a bit is different between entries, it is available
          // for reading.
          dcxBitMask<Entries>::WORD_TYPE available = (writing ^ reading);
          
          size_t currentEntry = i * dcxBitMask<Entries>::BITS_IN_WORD;
          while( available ) // todo: use a bit twiddling hack to make this faster
          {
              // If this entry can be written, return its index
              if( 1 == (available & 1) ) return currentEntry;

              ++currentEntry;
              available >>= 1;
          }
      }
      
      // Couldn't find an entry
      return ((size_t)(-1));
  }
  
protected:

  volatile DataType* myDataArray;
  dcx::dcxBitMask<Entries> myWritingThreadDataMask;
  dcx::dcxBitMask<Entries> myReadingThreadDataMask;
};

}

#endif