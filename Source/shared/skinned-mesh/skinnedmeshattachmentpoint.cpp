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
#include "skinnedmeshattachmentpoint.h"
#include "common/null.h"


namespace Evidyon {
namespace SkinnedMesh {


//----[  SkinnedMeshAttachmentPointString  ]-----------------------------------
const char* SkinnedMeshAttachmentPointString(
    SkinnedMeshAttachmentPoint value) {
  switch (value) {
    default: return NULL;
    case ATTACHMENTPOINT_PRIMARY_HAND:    return "Primary Hand";
    case ATTACHMENTPOINT_SECONDARY_HAND:  return "Secondary Hand";
    case ATTACHMENTPOINT_HEAD:            return "Head";
    case ATTACHMENTPOINT_SHIELD:          return "Shield";
    case ATTACHMENTPOINT_BACK:            return "Back";
    case ATTACHMENTPOINT_LEFT_FOOT:       return "Left Foot";
    case ATTACHMENTPOINT_RIGHT_FOOT:      return "Right Foot";
  }
}



}
}