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
#ifndef __STDAFX_H__
#define __STDAFX_H__
#pragma once



#include <d3dx9.h>
#include <dc/debug>
#include <dc/filestream>

#include <set>
#include <list>
#include <string>
#include <vector>
#include <map>


#include "dxfile.h"
#include "../common/unseenskinnedmesh/unseenskinnedmesh.h"
#include "framenode.h"


#define SAFE_RELEASE( p )   if( p ) { p->Release(); p = 0; }
#define SAFE_DELETE( p )    if( p ) { delete p; p = 0; }
#define SAFE_DELETE_ARRAY( p )    if( p ) { delete[] p; p = 0; }




#endif