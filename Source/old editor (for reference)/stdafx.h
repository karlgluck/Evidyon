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

#ifndef __STDAFX_H__
#define __STDAFX_H__
#pragma once


// Bring in OS headers
#include <windows.h>    // Main Windows header
#include <windowsx.h>   // Methods to help control Windows controls


// Link Direct3D
#include <d3dx9.h>


// Get the various DirectCraft header files used by the resources
#include <dc/debug>
#include <dc/list>
#include <dc/map>
#include <dc/rangemap>
#include <dc/table>
#include <dc/objects>
#include <dc/dcreference.h>
#include <dcx/vector3>
#include <dcx/win32>

using namespace dc;
using namespace dcx;


// Include the geometry definition header
#include "../shared/geometry.h"

#include "../shared/server-editor/server_gamefile.h"
#include "../shared/avatar/evidyon_avatar.h"
#include "../shared/avatar/evidyon_avatarinventory.h"
#include "../shared/evidyon_accounts.h"
#include "../shared/actor/evidyon_actor.h"
#include "../shared/evidyon_enchantments.h"
#include "../shared/evidyon_magic.h"
#include "../shared/evidyon_map.h"
//#include "../shared/evidyon_network.h"
//#include "../shared/client-server/packets.h"
#include "../shared/evidyon_version.h"
#include "../shared/evidyon_world.h"
#include "../shared/client-editor/client_gamefile.h"
using namespace Evidyon;


#include "resourceactions.h"

#include "../common/unseenskinnedmesh/unseenskinnedmesh.h"
#include "unseenkeyframeanimation.h"


#define SAFE_RELEASE(p)       if (p) { p->Release(); p = NULL; }
#define SAFE_DELETE(p)        if (p) { delete p; p = NULL; } 
#define SAFE_DELETE_ARRAY(p)  if (p) { delete[] p; p = NULL; }
#define SAFE_RELEASE_ELEMENTS(a, n)   {for (size_t i=0;i<n;++i){SAFE_RELEASE(a[i])}}



#endif