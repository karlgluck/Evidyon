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
// Desc:    Includes all of the game resources used in the editor.  This isn't a part of stdafx.h
//          because any one change in a single header will cause a complete recompile.  Some files
//          need to know about every resource, though (like the resource actions file and compile
//          method declarations file) so this file is still useful in some regard.
#ifndef __ALLGAMERESOURCES_H__
#define __ALLGAMERESOURCES_H__
#pragma once

// Declare types
class ProjectVolucris;


#include "magic.h"
#include "detailedmagic.h"


#include "colorkey.h"

#include "image.h"
#include "d3dximage.h"

#include "texture.h"
#include "animatedtexture.h"
#include "circlingtexture.h"
#include "slidingtexture.h"
#include "statictexture.h"

#include "skin.h"

#include "mesh.h"
#include "xmesh.h"
#include "mdjmesh.h"


#include "scenery.h"

#include "location.h"

#include "terrain.h"
#include <dc/table>

#include "terrainrule.h"

#include "../shared/compiledatlasformat.h"

#include "maplayer.h"

#include "atlasterrainmesh.h"
#include <dcx/win32>
#include <dcx/d3d>
#include "../common/isometriccamera.h"  // The common camera for viewing the world
#include "atlas.h"
#include "atlaseditdialog.h"
#include "atlascompilationcontext.h"


#include "contributor.h"

#include "resourcepack.h"
#include "packimage.h"
#include "packmesh.h"


#include "wavsound.h"

#include "visualfximage.h"
#include "visualfx.h"
#include "magicfx.h"
#include "visualfx_orbitingparticles.h"
#include "visualfx_particletrail.h"

#include "avatarmodifier.h"

#include "item.h"

#include "unseenskinnedmeshresource.h"
#include "unseenkeyframeanimation.h"
#include "skinnedmeshattachmentpoint.h"
#include "actortype.h"

#include "aicharactertype.h"

#include "playercharacterrace.h"
#include "playercharacterclasslevelspells.h"
#include "playercharacterclass.h"

#include "leveldefinitions.h"

#include "projectile.h"

#include "enchantment.h"
#include "enchantmentclass.h"
#include "enchantmentclassimpl.h"
#include "enchantmentclasslist.h"

#include "lifeformai.h"


// This one is special--it is the main resource that contains everything in the editor!
#include "projectvolucris.h"



#endif