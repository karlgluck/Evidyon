#ifndef __EVIDYON_NPC_H__
#define __EVIDYON_NPC_H__
#pragma once

namespace Evidyon {

// different things that happen when a client clicks on an interactive NPC
enum NPCInteractionType {
  // provide access to the storage area for this account
  NPCINTERACTION_STORAGE,

  // open the bazaar menu
  NPCINTERACTION_BAZAAR,

  // open the geosid menu
  NPCINTERACTION_GEOSID,

  // this npc gives quests.  client polls the server for the quests, then displays
  // a menu with options
  NPCINTERACTION_QUEST,

  // npc sells items for gold.  client polls the server for sales, then displays
  // a menu with the options.
  NPCINTERACTION_MERCHANT,
};

}


#endif