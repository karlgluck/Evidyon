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
#ifndef __COMPLETE_EDITOR_GAMEFILE_SERVERGAMEFILEDATA_H__
#define __COMPLETE_EDITOR_GAMEFILE_SERVERGAMEFILEDATA_H__
#pragma once




#include "servergamefileserverdata.h"
#include "servergamefilemapdata.h"
#include "shared/avatar/avatarraceserverdescription.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclassserverdescription.h"
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatarleveldescription.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/lifeformai/lifeformai.h"
#include "shared/item/itemserverdescription.h"
#include "shared/magic/magic.h"
#include "shared/magic/enchantmenttype.h"
#include "shared/spell/spellserverdescription.h"
#include "shared/treasure/epictreasure.h"
#include "shared/treasure/treasureclass.h"
#include "shared/treasure/treasuredropsequenceelement.h"
#include "shared/geosid/geosidserverdescription.h"
#include "gamefiledata.h"


namespace Evidyon {
namespace GameFile {

typedef EvidyonGameFileSerializer<
    Evidyon::GameFile::ServerGameFileServerData
  >ServerDataSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::LifeformAI::LifeformAI
  >LifeformAISerializer;

typedef EvidyonGameFileSerializer<
  List<Evidyon::Avatar::AvatarRaceServerDescription,
       Evidyon::Avatar::NUMBER_OF_AVATAR_RACES>
>AvatarRaceServerSerializer;

typedef EvidyonGameFileSerializer<
  List<Evidyon::Avatar::AvatarClassServerDescription,
       Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES>
>AvatarClassServerSerializer;

typedef EvidyonGameFileSerializer<
  List<Evidyon::Avatar::AvatarLevelDescription,
       Evidyon::Avatar::HIGHEST_AVATAR_LEVEL>
>AvatarLevelSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::World::MapHash,                      // unique # stored in db
    size_t,                                       // width
    size_t,                                       // height
    Array<ServerGameFileMapLocationData, size_t>, // locations
    size_t,                                       // regions wide
    size_t,                                       // regions high,
    Array<ServerGameFileMapRegionData, size_t>,   // regions
    Array<Evidyon::Trigger::Trigger, size_t>      // triggers
  >ServerMapSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::Item::ItemServerDescription
  >ItemServerSerializer;

typedef EvidyonGameFileSerializer<
  Evidyon::Magic::Magic
  > MagicSerializer;

typedef EvidyonGameFileSerializer<
  Evidyon::Spell::SpellServerDescription
  > ServerSpellSerializer;

typedef EvidyonGameFileSerializer<
  Treasure::EpicTreasure
  > EpicTreasureSerializer;

typedef EvidyonGameFileSerializer<
  Treasure::TreasureClass
  > TreasureClassSerializer;
  
typedef EvidyonGameFileSerializer<
  Treasure::TreasureDropSequenceElement
  > TreasureDropSequenceSerializer;
  
typedef EvidyonGameFileSerializer<
  Evidyon::Geosid::GeosidServerDescription
  > ServerGeosidSerializer;
  
typedef EvidyonGameFileSerializer<
  List<size_t, Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES>
  > ServerEnchantmentSerializer;



//----[  ServerGameFileData  ]-------------------------------------------------
// Members are listed in the order in which they are serialized in the file
struct ServerGameFileData {
  ServerGameFileServerData server_data;
  Evidyon::Avatar::AvatarRaceServerDescription races[Evidyon::Avatar::NUMBER_OF_AVATAR_RACES];
  Evidyon::Avatar::AvatarClassServerDescription classes[Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES];
  Evidyon::Avatar::AvatarLevelDescription levels[Evidyon::Avatar::HIGHEST_AVATAR_LEVEL];
  GAME_FILE_ARRAY(LifeformAI, Evidyon::LifeformAI::LifeformAI, lifeform_ai);
  GAME_FILE_ARRAY(Map, Evidyon::GameFile::ServerGameFileMapData, maps);
  GAME_FILE_ARRAY(Item, Evidyon::Item::ItemServerDescription, items);
  GAME_FILE_ARRAY(Magic, Evidyon::Magic::Magic, magic);
  GAME_FILE_ARRAY(Spell, Evidyon::Spell::SpellServerDescription, spells);
  GAME_FILE_ARRAY(EpicTreasure, Evidyon::Treasure::EpicTreasure, epic_treasure);
  GAME_FILE_ARRAY(TreasureClass, Evidyon::Treasure::TreasureClass, treasure_classes);
  GAME_FILE_ARRAY(TreasureDropSequenceElement, Evidyon::Treasure::TreasureDropSequenceElement, treasure_drop_sequence);
  size_t enchantments_in_type[Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES];
  GAME_FILE_ARRAY(Geosid, Evidyon::Geosid::GeosidServerDescription, geosids);
};



//----[  ServerGameFileHeader  ]-----------------------------------------------
struct ServerGameFileHeader {
  struct Array {
    size_t array_entries;
    size_t first_byte_offset;
  };
  Array lifeform_ai;
  Array avatar_races;
  Array avatar_classes;
  Array maps;
  Array items;
  Array magic;
  Array spells;
  Array epic_treasure;
  Array treasure_classes;
  Array treasure_drop_sequence;
  Array geosids;
};

}
}

#endif