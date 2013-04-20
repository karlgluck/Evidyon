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
#ifndef __SHARED_ACTOR_ACTORID_H__
#define __SHARED_ACTOR_ACTORID_H__
#pragma once


#include "actortemplateindex.h"

namespace Evidyon {
namespace Actor {

typedef unsigned int ActorID;
static const ActorID INVALID_ACTOR_ID = ActorID(-1);

// The number of bits in the actor ID reserved for the actor's template index.
// This allows an actor to be instantiated on the first message the client
// receives, since most of the actors are based on templates that do not
// change.
static const int ACTORID_TEMPLATE_BITS = 12;
static const int MAX_ACTOR_TEMPLATES = 1<<ACTORID_TEMPLATE_BITS;

// This contains the number of bits in actor's ID number that hold the actor's
// index in the server's actor memory pool.  This allows actors to be looked
// up by a direct index offset + validation check, instead of having to do
// some sort of search.
static const int ACTORID_ACTOR_INDEX_BITS = 11; // 2^* = # max simultaneous actors
static const int MAX_ACTORS_IN_WORLD = 1<<ACTORID_ACTOR_INDEX_BITS;





//----[  PackActorID  ]--------------------------------------------------------
// This method is used by the server to generate a unique actor ID.
ActorID PackActorID(unsigned int incremented_value,             // just some number
                    unsigned int actor_structure_mempool_index, // must be < MAX_ACTORS_IN_WORLD
                    ActorTemplateIndex actor_template);         // must be < MAX_ACTOR_TEMPLATES

//----[  UnpackActorID  ]------------------------------------------------------
// Used by the client to extract the actor's template type from its ID number.
void UnpackActorID(ActorID actor_id,
                   ActorTemplateIndex* actor_template);



}
}

#endif