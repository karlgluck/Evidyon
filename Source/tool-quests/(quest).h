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
// This file locally includes a remote file that relies on definitions which
// are external to this project.  Specifically, Item, Atlas and LifeformAI
// objects are referenced by Quests, but this tool has no concept of their
// data attributes and shouldn't need to have one.  The important thing is
// that this tool can load a list of the ones that have been created and
// link references.  In order to avoid code duplication, the quest objects
// are shared and require the employing app to provide definitions for
// these objects.
//-----------------------------------------------------------------------------
#ifndef __QUEST_LOCALIZED_H__
#define __QUEST_LOCALIZED_H__
#pragma once

#include <dc/string>
#include <dc/boolean>
#include <dc/list>
#include <dc/dcreference.h>
#include <dc/integer>

#include "../shared/avatar/evidyon_avatar.h"
#include "../shared/calendar/calendar.h"


//----[  Item  ]---------------------------------------------------------------
class Item : public dc::dcResource<Item> {
public:
  static std::string staticTypeName() { return "Item"; }
};



//----[  Atlas  ]--------------------------------------------------------------
class Atlas : public dc::dcResource<Item> {
public:
  static std::string staticTypeName() { return "Atlas"; }
};




//----[  LifeformAI  ]---------------------------------------------------------
class LifeformAI : public dc::dcResource<LifeformAI> {
public:
  static std::string staticTypeName() { return "LifeformAI"; }
};



#include "../shared/quest/quest-tools.h"




#endif