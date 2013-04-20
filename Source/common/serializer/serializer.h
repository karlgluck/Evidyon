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
#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__
#pragma once



#include <memory.h>


//----[  SerializerWrite  ]----------------------------------------------------
template <typename T> char* SerializerWrite(T t, char* at, const char* limit) {
  char* next;
  if (!at || ((next = at + sizeof(T)) > limit)) return 0;
  *((T*)at) = t;
  return next;
}



//----[  SerializerRead  ]-----------------------------------------------------
template <typename T> const char* SerializerRead(const char* at, const char* end, T* out) {
  const char* next;
  if (!at || ((next = at + sizeof(T)) > end) || !out) return 0;
  *out = *((T*)at);
  return next;
}




//----[  List  ]---------------------------------------------------------------
template <typename _E, const size_t Length> class List {
public:
  typedef _E ElementType;
  static const size_t length = Length;
};



//----[  CopiedList  ]---------------------------------------------------------
template <typename _E> class CopiedList {
public:
  typedef _E ElementType;

public:

  // Constructor for writing elements
  explicit CopiedList(const _E* elements)
    : elements_(const_cast<_E*>(elements)) {}

  // Constructor for reading elements
  explicit CopiedList(_E* elements)
    : elements_(elements) {}

  // Writes the source list into the output by doing a memcpy.  Produces a
  // compile error if the element types are not identical.
  template <typename ListType> inline char* write(char* at, const char* limit) {
    const size_t length = ListType::length;
    const size_t bytes = length * sizeof(ListType::ElementType);
    char* next = at + bytes;
    if (!at || (at >= limit) || (next > limit)) return 0;

     // compile error if array types aren't the same
    const _E* src = (const ListType::ElementType*)elements_;
    
    // copy data
    memcpy(at, src, bytes);
    return next; // return pointer to byte after last written position
  }

  // Reads source data into the 'elements' by doing a memcpy.  This method
  // ignores NULL element reference.
  template <typename ListType> inline const char* read(const char* at, const char* limit) {
    if (!at || (at >= limit)) return 0;
    const size_t bytes = ListType::length * sizeof(ListType::ElementType);
    const char* next = at + bytes;
    if (next > limit) return 0; // not enough source data?
    if (elements_) memcpy(elements_, at, bytes);  // copy data
    return next;            // next data element
  }


private:
  _E* elements_;

  // disallow copy & assign
  CopiedList(const CopiedList& rhs);
  CopiedList& operator = (const CopiedList& rhs);
};


//----[  Array  ]--------------------------------------------------------------
// put in the declaration field to specify that an array should be serialized
template <typename _E, typename _C> class Array {
public:
  typedef _E ElementType; // what kind of stuff is stored here
  typedef _C CountType;   // variable type to use for # of elements
};

//----[  CopiedArray  ]--------------------------------------------------------
// Provide this type in the Serializer::write parameter list if the data you
// want to copy is available as a straight buffer and can be copied directly
// into the serialized stream.
// If this is used in Serializer::read, the 'count' parameter is the size
// of the output buffer before invoking the function, and will be assigned
// to the number of elements read afterward.
template <typename _E> class CopiedArray {
public:
  typedef _E ElementType;

public:

  // Constructor for writing elements
  explicit CopiedArray(const _E* elements, size_t count)
    : elements_(const_cast<_E*>(elements)), count_(count) {}

  // Constructor for reading elements; number read is stored in *count_out
  explicit CopiedArray(_E* elements, size_t count, size_t* count_out)
    : elements_(elements), count_(count), count_out_(count_out) {}

  // Writes the source data into the array by doing a memcpy.  Produces a
  // compile error if the element types are not identical.
  template <typename ArrayType> inline char* write(char* at, const char* limit) {
    typedef ArrayType::CountType _C;
    const size_t count = count_;
    const size_t bytes = count * sizeof(ArrayType::ElementType);
    char* next = at + sizeof(_C) + bytes;
    if (!at || (at >= limit) || (next > limit)) return 0;

     // compile error if array types aren't the same
    const _E* src = (const ArrayType::ElementType*)elements_;
    
    // convert ptr to count type, store count, write data
    *(((_C*)at)) = _C(count);
    memcpy(at + sizeof(_C), elements_, bytes);
    return next; // return pointer to byte after last written position
  }

  // Reads source data into the constructed array by doing a memcpy.
  template <typename ArrayType> inline const char* read(const char* at, const char* limit) {
    typedef ArrayType::CountType _C;
    if (!at || (at >= limit)) return 0;
    size_t count = size_t(*((_C*)at));
    *count_out_ = count;    // save # of elements
    const size_t bytes = count * sizeof(ArrayType::ElementType);
    const char* next = at + sizeof(_C) + bytes;
    if ((count > count_) || (next > limit)) return 0; // not enough buffer space?
    const void* src = at + sizeof(_C);
    memcpy(elements_, src, bytes);  // copy data
    count_ = 0;             // prevent overwrites by resetting buffer size
    return next;            // next data element
  }

private:
  _E* elements_;
  size_t count_;
  size_t* count_out_;

  // disallow copy & assign
  CopiedArray(const CopiedArray& rhs);
  CopiedArray& operator = (const CopiedArray& rhs);
};


//----[  ConvertedArray  ]-----------------------------------------------------
// A converted array uses operator = (LHS is destination type) to convert
// one kind of element into another.
template <typename _E> class ConvertedArray {
public:
  typedef _E ElementType;

  // Constructor for writing elements
  explicit ConvertedArray(_E* elements, size_t count)
    : elements_(elements), count_(count) {}

  // Constructor for reading elements; number read is stored in *count_out
  explicit ConvertedArray(_E* elements, size_t count, size_t* count_out)
    : elements_(elements), count_(count), count_out_(count_out) {}

  // Writes the source data into the array by doing an assigned copy.
  // Produces a compile error if the element types are not identical.
  template <typename ArrayType> inline char* write(char* at, const char* limit) {
    typedef ArrayType::CountType _C;
    size_t count = count_;
    const size_t bytes = count * sizeof(ArrayType::ElementType);
    char* next = at + sizeof(_C) + bytes;
    if (!at || (at >= limit) || (next > limit)) return 0;

    // iterate backward through all elements
    *(((_C*)at)) = _C(count);
    at = next;
    while (count > 0) {
      --count;
      at -= sizeof(ArrayType::ElementType);
      *static_cast<ArrayType::ElementType*>(at) = elements_[count];
    }
    
    return next; // return pointer to byte after last written position
  }


  // Reads source data into the constructed array by doing an assigned copy.
  template <typename ArrayType> inline const char* read(const char* at, const char* limit) {
    typedef ArrayType::CountType _C;
    if (!at || (at >= limit)) return 0;
    size_t count = size_t(*((_C*)at));
    const size_t bytes = count * sizeof(ArrayType::ElementType);
    const char* next = at + sizeof(_C) + bytes;
    *count_out_ = count;    // save # of elements
    if ((count > count_) || (next > limit)) return 0; // not enough buffer space?
    const ArrayType::ElementType* src =
        static_cast<const ArrayType::ElementType*>(at + sizeof(_C));
    _E* elements = elements_;
    while (count > 0) {
      --count;
      *(elements++) = *(src++);
    }
    count_ = 0;             // prevent overwrites by resetting buffer size
    return next;            // next data element
  }

private:
  _E* elements_;
  size_t count_;
  size_t* count_out_;


  // disallow copy & assign
  ConvertedArray(const ConvertedArray& rhs);
  ConvertedArray& operator = (const ConvertedArray& rhs);
};





//----[  DynamicArray  ]-------------------------------------------------------
template <typename _E> class DynamicArray {
public:
  typedef _E ElementType;

public:
  class ElementFunctionoid {
  public:
    virtual ~ElementFunctionoid() {}

    // Returns 'true' if fill() should be called again.
    virtual bool hasMoreElements() = 0;

    // Returns 'true' if the element was filled, or 'false' to terminate the
    // array.  *failed is false by default; if it is set to 'true' and the
    // return value is 'false', the array serialization fails.
    virtual bool fill(size_t count, ElementType* element, bool* failed) = 0;
  };


  // Constructor for writing elements
  explicit DynamicArray(ElementFunctionoid* elements)
    : elements_(elements) {}

  // Writes the source data into the array by using the functionoid.  This
  // Produces a compile error if the element types are not identical.
  template <typename ArrayType> inline char* write(char* at, const char* limit) {
    typedef ArrayType::CountType _C;
    size_t count = 0;
    char* start = at;
    at += sizeof(_C);
    bool failed = false;
    while (elements_->hasMoreElements()) {
      if (at + sizeof(ElementType) < limit &&
          elements_->fill(count, (ArrayType::ElementType*)at, &failed)) {
        at += sizeof(ElementType);
        ++count;
      } else {
        if (failed) {
          return 0; // serialization error
        } else {
          break;
        }
      }
    }
    *(((_C*)start)) = _C(count);
    return at; // return pointer to byte after last written position
  }



private:
  ElementFunctionoid* elements_;

  // disallow copy & assign
  DynamicArray(const DynamicArray& rhs);
  DynamicArray& operator = (const DynamicArray& rhs);
};






//----[  PeekArray  ]----------------------------------------------------------
// Provide this type in a Serializer::read parameter list to obtain the size
// of an array in the stream and read past it without actually doing anything
// with the data.  This is intended to be used in order to find the array's
// size before reading it.
template <typename _E> class PeekArray {
public:
  typedef _E ElementType;

public:

  // Constructor for reading elements; number read is stored in *count_out
  explicit PeekArray(size_t* count_out)
    : elements_out_(0), count_out_(count_out) {}

  // Constructor for reading elements; number read is stored in *count_out
  // and a pointer to the array of elements is stored in elements_out
  explicit PeekArray(const ElementType** elements_out, size_t* count_out)
    : elements_out_(elements_out), count_out_(count_out) {}

  // Reads the size of the element and advances the buffer pointer
  template <typename ArrayType> inline const char* read(const char* at, const char* limit) {
    typedef ArrayType::CountType _C;
    const char* next = at + sizeof(_C);
    if (!at || (at >= limit)) return 0;
    size_t count = size_t(*((_C*)at));
    *count_out_ = count;    // save # of elements
    if (elements_out_) *elements_out_ = (const ArrayType::ElementType*)(next);
    const size_t bytes = count * sizeof(ArrayType::ElementType);
    next += bytes;
    return (next > limit) ? 0 : next; // not enough data left? return null
  }

private:
  size_t* count_out_;
  const ElementType** elements_out_;

  // disallow copy & assign
  PeekArray(const PeekArray& rhs);
  PeekArray& operator = (const PeekArray& rhs);
};




//----[  Serializer  ]---------------------------------------------------------
template <typename SerializedType> class Serializer {
public:

  template <typename SourceElementType> static char* write(CopiedList<SourceElementType>* in, char* at, const char* limit) {
    return in->write<SerializedType>(at, limit);
  }

  template <typename SourceElementType> static char* write(CopiedArray<SourceElementType>* in, char* at, const char* limit) {
    return in->write<SerializedType>(at, limit);
  }

  template <typename SourceElementType> static char* write(ConvertedArray<SourceElementType>* in, char* at, const char* limit) {
    return in->write<SerializedType>(at, limit);
  }

  template <typename SourceElementType> static char* write(DynamicArray<SourceElementType>* in, char* at, const char* limit) {
    return in->write<SerializedType>(at, limit);
  }

  // Write using a copy constructor or primitive conversion from the source
  // type to the serialized type.
  template <typename SourceType> static char* write(SourceType& in, char* at, const char* limit) {
    char* next = at + sizeof(SerializedType);
    if (!at || (next > limit)) return 0;
    *((SerializedType*)at) = SerializedType(in);
    return next;
  }

  // Write using a memcpy to the buffer from the input pointer, unless the type
  // is particularly small (<4 bytes)
  static char* write(SerializedType* in, char* at, const char* limit) {
    char* next = at + sizeof(SerializedType);
    if (!at || (next > limit)) return 0;
    memcpy(at, in, sizeof(SerializedType));
    return next;
  }

public:

  template <typename DestElementType> static const char* read(const char* at, const char* limit, CopiedList<DestElementType>* out) {
    return out->read<SerializedType>(at, limit);
  }

  template <typename DestElementType> static const char* read(const char* at, const char* limit, CopiedArray<DestElementType>* out) {
    return out->read<SerializedType>(at, limit);
  }

  template <typename DestElementType> static const char* read(const char* at, const char* limit, ConvertedArray<DestElementType>* out) {
    return out->read<SerializedType>(at, limit);
  }

  template <typename DestElementType> static const char* read(const char* at, const char* limit, PeekArray<DestElementType>* out) {
    return out->read<SerializedType>(at, limit);
  }

  // Read using copy constructor (or primitive conversion) to the output type
  template <typename DestType> static const char* read(const char* at, const char* limit, DestType* out) {
    const char* next = at + sizeof(SerializedType);
    if (!at || (next > limit) || !out) return 0;
    *out = DestType(*((SerializedType*)at));
    return next;
  }

  // Read using a memcpy to the output pointer
  static const char* read(const char* at, const char* limit, SerializedType* out) {
    const char* next = at + sizeof(SerializedType);
    if (!at || (next > limit) || !out) return 0;
    memcpy(out, at, sizeof(SerializedType));
    return next;
  }
};





#endif