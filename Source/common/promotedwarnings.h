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
#ifndef __PROMOTEDWARNINGS_H__
#define __PROMOTEDWARNINGS_H__
#pragma once


#pragma warning(disable : 4530)
#pragma warning(disable : 4355)

#pragma warning(error : 4800)
#pragma warning(error : 4804)
#pragma warning(error : 4101) // unreferenced local variable
#pragma warning(error : 4146) // unary minus operator applied to unsigned type

// check operator precedence for possible error; use parentheses to clarify precedence
#pragma warning(error : 4554)
#pragma warning(error : 4715) // not all control paths return a value
#pragma warning(error : 4700)


#endif