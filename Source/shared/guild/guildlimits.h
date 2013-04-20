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
#ifndef __SHARED_GUILD_GUILDLIMITS_H__
#define __SHARED_GUILD_GUILDLIMITS_H__
#pragma once

#include "shared/avatar/avatarlimits.h"

namespace Evidyon {
namespace Guild {

static const int MAX_NAME_LENGTH = 24;
static const int MAX_MEMBERS = 16;
static const int MAX_GUILD_MEMBER_NAME_TAG_LENGTH
  = MAX_NAME_LENGTH + sizeof("Founder of ") + 1;

// Since the founder is a "leader", setting this to N makes the number of
// people who can be promoted to "Leader of..." N-1
static const size_t MAX_NUMBER_OF_LEADERS = 4;

// max length of text that can be sent in response to g/members
static const int MAX_GUILD_MEMBERS_TEXT_LENTH = MAX_MEMBERS * (3+Avatar::MAX_AVATAR_NAME_LENGTH);

// NETMSG_GUILD_MESSAGE's text max length
static const int MAX_GUILD_MESSAGE_LENGTH = MAX_GUILD_MEMBERS_TEXT_LENTH;

// A guild can own no more than this # of geosids at a time
static const int MAX_GEOSIDS_OWNED = MAX_MEMBERS / 2;

}
}

#endif