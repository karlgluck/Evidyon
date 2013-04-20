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


#include <enet/enet.h>
#include <dcx/win32>
#include <dcx/inifilereader>
#include <dcx/statemachine>
#include <dcx/memorypool>

#include <list>

//using namespace dc;
using namespace dcx;

#include <d3dx9.h>
#include <stdio.h>
#include "cleanup.h"
#include <string>
#include <dc/debug>
#include <map>
#include <dsound.h>
#include "common/promotedwarnings.h"


#include "shared/avatar/evidyon_avatarinventory.h"
#include "shared/evidyon_accounts.h"
#include "shared/actor/evidyon_actor.h"
#include "shared/avatar/evidyon_avatar.h"
#include "shared/evidyon_enchantments.h"
#include "shared/evidyon_map.h"
#include "shared/evidyon_network.h"
#include "shared/client-server/packets.h"
#include "shared/evidyon_version.h"
#include "shared/evidyon_world.h"
#include <d3dx9.h> // TODO: remove this dependency
#include <mmsystem.h> // TODO: remove this dependency

#include "shared/actor/evidyon_actor.h"
#include "shared/avatar/evidyon_avatar.h"

#include "encodepackets.h"
#include "decodepackets.h"

#include "shared/geometry.h"
#include "common/isometriccamera.h"

// Include generic GUI implementations
#include "common/GUI/guipoint.h"
#include "common/GUI/guisize.h"
#include "common/GUI/guirect.h"
#include "common/GUI/guicanvas.h"
#include "common/GUI/guielement.h"
#include "common/GUI/guivertex.h"
#include "common/GUI/guiindex.h"
#include "common/GUI/guilayer.h"
#include "common/GUI/guifont.h"
#include "common/GUI/guiquad.h"

#include "common/GUI/guistretchfillelement.h"
#include "common/GUI/guiborderedfillelement.h"
#include "common/GUI/guipictureelement.h"
#include "common/GUI/guitextelement.h"
#include "common/GUI/guitextlabelelement.h"

#include "common/GUI/guimouse.h"
#include "common/GUI/guikeyboard.h"

#include "common/GUI/guicomponent.h"
#include "common/GUI/guibutton.h"
#include "common/GUI/guislider.h"
#include "common/GUI/guieditabletextline.h"
#include "common/GUI/guilist.h"

#include "common/GUI/guilayerbasic.h"

#include "useractionqueue.h"

// Include Evidyon-specific GUI implementations
#include "evidyonguibutton.h"
#include "evidyonguititledpanel.h"
#include "evidyonguilistentrytext.h"
#include "evidyonguislider.h"
#include "evidyonguilist.h"
#include "evidyonguikeyboardinputsource.h"
#include "evidyonguimouseinputsource.h"
#include "evidyonguieditabletextline.h"
#include "evidyonguistatdisplay.h"
#include "evidyonguichatlog.h"
#include "evidyonguidefs.h"
#include "chattextline.h"

#include "common/unseenskinnedmesh/unseenskinnedmesh.h"



#include "evidyonclient.h"
#include "strings.h"
#include "winmain.h"

#include "common/safemacros.h"


#endif