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
#ifndef __BUFFERSERIALIZER_H__
#define __BUFFERSERIALIZER_H__
#pragma once



#include "serializer.h"

#include "serializer_begin.h"


#define WRITEBUFFER_NOPARAMS()  
#define WRITEBUFFER(base,plus)  template <TYPES(base,plus)> static char* writeBuffer(char* buffer, const char* end, W_PARAMS(base,plus)) { W_BODY(base,plus); return buffer; }
#define READBUFFER_NOPARAMS()   
#define READBUFFER(base,plus)   template <TYPES(base,plus)> static const char* readBuffer(const char* buffer, const char* end, R_PARAMS(base,plus)) { R_BODY(base,plus); return buffer; }


template <TLIST_64> class BufferSerializer {
public: SERIALIZER_READ64(BUFFER);
public: SERIALIZER_WRITE64(BUFFER);
};



#include "serializer_end.h"


#endif