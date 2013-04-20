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
#ifndef __COMMON_KG_KG_H__
#error "must be included by kg.h"
#endif

namespace kg {

//----[  CharBuffer, SafeCharBuffer  ]----------------------------------------
template <const size_t _SIZE> struct CLASSNAME(CharBuffer) {
  IFSAFE(CLASSNAME(CharBuffer)() {characters[0] = '\0';})

  static const size_t SIZE = _SIZE IFSAFE(+1);
  static const size_t CAPACITY = _SIZE-1;
  char characters[SIZE];

  inline void clear() { characters[0] = '\0'; }

  inline bool match(const char* text, size_t* location) const {
    const char* ch = characters;
    const char* t = text;
    size_t at = 0;
    while (at < SIZE) {
      if (*ch != *t) {
        at++;
        ch = characters + at;
        t = text;
      } else {
        ++ch;
        ++t;
        if (*t == '\0') {
          *location = at;
          return true;
        } else if (*ch == '\0') {
          break;
        }
      }
    }
    return false;
  }

  inline char& operator[](size_t i) { assert(i<SIZE); return characters[i]; }
  inline char& operator[](size_t i) const { assert(i<SIZE); return characters[i]; }

  inline void copyFrom(const char* source) {
    confirm(source) else return;
    strcpy_s(characters, SIZE, source);
  }
  inline void copyMaxFrom(const char* source) {
    strncpy_s(characters, SIZE, source, CAPACITY);
  }
  inline void copyTo(char* dest, size_t size) const {
    confirm(dest) else return;
    strcpy_s(dest, len, characters);
  }
  inline void copyMaxTo(char* dest, size_t size) const {
    confirm(dest) else return;
    strncpy_s(dest, size, characters, size-1);
  }
  inline void appendTo(char* dest, size_t size) const {
    confirm(dest) else return;
    strncat_s(dest, size, characters, size-1);
  }
  inline void appendWith(const char* source, char* dest, size_t size) const {
    confirm(source && dest) else return;
    size_t source_len = strlen(source) + 1;
    size_t this_len = strlen(characters);
    size_t room = size - source_len;
#if SAFE
    confirm(source_len < size) else { strncpy_s(dest, size, source, size - 1); return; }
    confirm(this_len < SIZE) else { this_len[SIZE-1] = '\0'; }
    strcpy_s(dest, size, source);
    strncat_s(dest, size, characters, room < this_len ? room : this_len);
#endif
#if NOTSAFE
    memcpy(dest, source, sizeof(char) * source_len);
    this_len = room < this_len ? room : this_len;
    memcpy(dest + source_len, characters, sizeof(char) * this_len);
    dest[source_len + this_len] = '\0';
#endif
  }
};


}
