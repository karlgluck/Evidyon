//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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