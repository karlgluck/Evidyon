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