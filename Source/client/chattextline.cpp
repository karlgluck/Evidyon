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
#include "stdafx.h"
#include "evidyonguidefs.h"
#include "client/actionqueue.h"
#include "client/clientfacade.h"




//-----------------------------------------------------------------------------
ChatTextLine::KeyboardSentinel::KeyboardSentinel(ChatTextLine* owner) {
  owner_ = owner;
  watching_ = false;
  enabled_ = false;
}



//-----------------------------------------------------------------------------
bool ChatTextLine::KeyboardSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!enabled_) return false;

  if (key == GUIKEY_RETURN) {
  watching_ = true;
  return true;
  }

  // Trap all messages
  return true;
}



//-----------------------------------------------------------------------------
bool ChatTextLine::KeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!enabled_) return false;
  switch (key) {
  case GUIKEY_RETURN:
    if (!watching_) break;
    watching_ = false;
    // changed 3-5-09; exit_ is determined if the entire message was sent
    owner_->exit_ = false;//!(keyboard->isKeyDown(GUIKEY_LSHIFT) || keyboard->isKeyDown(GUIKEY_RSHIFT));
    owner_->send_ = true;
    break;  // Don't propogate this message
  case GUIKEY_ESCAPE:
    watching_ = false;
    owner_->exit_ = true;
    owner_->send_ = false;
    break;
  }

  // Trap all messages
  return true;
}



//-----------------------------------------------------------------------------
bool ChatTextLine::KeyboardSentinel::onTypedCharacter(
  char character,
  GUIKeyboardInputSource* keyboard) {
  if (enabled_) {
    return GUIKeyboardSentinel::onTypedCharacter(character, keyboard);
  } else {
    return false;
  }
}



//-----------------------------------------------------------------------------
ChatTextLine::MouseSentinel::MouseSentinel(ChatTextLine* owner) {
  owner_ = owner;
  enabled_ = false;
}



//-----------------------------------------------------------------------------
bool ChatTextLine::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action,
                            GUIPoint position,
                            GUIMouseInputSource* mouse) {
  if (!enabled_) return false;

  if (GUIMouseSentinel::onMouseButtonAction(action, position, mouse)) return true;
  if (action == GUIMOUSE_CLICK_SINGLE) {
  owner_->exit_ = true;
  owner_->send_ = false;
  return true;
  }
  return false;
}



//-----------------------------------------------------------------------------
ChatTextLine::ChatTextLine() : keyboard_sentinel_(this), mouse_sentinel_(this) {
  exit_ = false;
  send_ = false;
  layer_ = NULL;
}



//-----------------------------------------------------------------------------
bool ChatTextLine::create(GUIFont* font,
                          GUIMouseSentinel* root_mouse_sentinel,
                          GUIKeyboardSentinel* root_keyboard_sentinel,
                          GUISize hpMpXpSize) {
  destroy();
  setFont(font);
  setTextColor(EvidyonGUIDefs::TEXT_INPUT_COLOR);
  setBackgroundColor(D3DCOLOR_ARGB(128,255,255,255)); // make the background translucent
  keyboard_sentinel_.setParentKeyboardSentinel(root_keyboard_sentinel);
  getKeyboardSentinel()->setParentKeyboardSentinel(&keyboard_sentinel_);
  mouse_sentinel_.setParentMouseSentinel(root_mouse_sentinel);
  getMouseSentinel()->setParentMouseSentinel(&mouse_sentinel_);

  GUICanvas* canvas = getCanvas();
  canvas->alignXLeft(hpMpXpSize.width + 5);
  canvas->alignYBottom(-32-6);
  canvas->relativeWidth(-(hpMpXpSize.width + 10));
  canvas->absoluteHeight(35);
  canvas->absoluteDepth(0.75f);

  return true;
}



//-----------------------------------------------------------------------------
void ChatTextLine::destroy() {
  setFont(NULL);
  getCanvas()->setSourceCanvas(NULL);
  keyboard_sentinel_.removeFromKeyboardSentinelChain();
  mouse_sentinel_.removeFromMouseSentinelChain();
}



//-----------------------------------------------------------------------------
void ChatTextLine::show(GUILayer* layer) {
  bool enabled = layer != NULL;
  keyboard_sentinel_.enable(enabled);
  mouse_sentinel_.enable(enabled);

  if (layer_) {
    getCanvas()->setSourceCanvas(NULL);
    unregisterElements(layer_);
  }

  if (layer) {
    getCanvas()->setSourceCanvas(layer->getPrimaryCanvas());
    registerElements(layer);
    getKeyboardSentinel()->obtainKeyboardFocus();
    getMouseSentinel()->obtainMouseFocus();
  }

  layer_ = layer;
}





//----[  SpeechCommand  ]------------------------------------------------------
enum SpeechCommand {
  SPEECHCOMMAND_GUILD_CREATE, // guild/create
  SPEECHCOMMAND_GUILD_LEAD,   // guild/lead
  SPEECHCOMMAND_GUILD_JOIN,
  SPEECHCOMMAND_GUILD_LEAVE,
  SPEECHCOMMAND_GUILD_PROMOTE,
  SPEECHCOMMAND_GUILD_DEMOTE,
  SPEECHCOMMAND_GUILD_KICK,
  SPEECHCOMMAND_GUILD_MEMBERS,
  SPEECHCOMMAND_GUILD_CHAT,
  SPEECHCOMMAND_GUILD_GEOSIDS,
  SPEECHCOMMAND_GUILD_SETHOME,
  SPEECHCOMMAND_GUILD_HOME,
  SPEECHCOMMAND_GUILD_ABANDON,

  SPEECHCOMMAND_PARTY_CHAT,
  SPEECHCOMMAND_PARTY_LEAD,
  SPEECHCOMMAND_PARTY_JOIN,
  SPEECHCOMMAND_PARTY_LEAVE,
  SPEECHCOMMAND_PARTY_KICK,
  SPEECHCOMMAND_PARTY_CLOSE,

  SPEECHCOMMAND_STANDUP,
  SPEECHCOMMAND_RESPAWN,
  SPEECHCOMMAND_MONSTERS,
  SPEECHCOMMAND_ALL,

  SPEECHCOMMAND_MARK,

  NUMBER_OF_SPEECH_COMMANDS,
};



//----[  ParseSpeechCommand  ]-------------------------------------------------
bool ParseSpeechCommand(const char* speech_text) {
  struct CommandToken {
    const char* token_text;
    SpeechCommand command;
  };

  // todo: map<string, command>
  static const CommandToken TOKENS[] = {
    { "--", SPEECHCOMMAND_GUILD_CHAT },
    { "g-", SPEECHCOMMAND_GUILD_CHAT },
    { "guild-", SPEECHCOMMAND_GUILD_CHAT },
    { "-", SPEECHCOMMAND_PARTY_CHAT },
    { "p-", SPEECHCOMMAND_PARTY_CHAT },
    { "party-", SPEECHCOMMAND_PARTY_CHAT },
    { "g/create", SPEECHCOMMAND_GUILD_CREATE }, { "guild/create", SPEECHCOMMAND_GUILD_CREATE },
    { "g/lead", SPEECHCOMMAND_GUILD_LEAD }, { "guild/lead", SPEECHCOMMAND_GUILD_LEAD },
    { "g/join", SPEECHCOMMAND_GUILD_JOIN }, { "guild/join", SPEECHCOMMAND_GUILD_JOIN },
    { "g/leave", SPEECHCOMMAND_GUILD_LEAVE }, { "guild/leave", SPEECHCOMMAND_GUILD_LEAVE },
    { "g/promote", SPEECHCOMMAND_GUILD_PROMOTE }, { "guild/promote", SPEECHCOMMAND_GUILD_PROMOTE },
    { "g/demote", SPEECHCOMMAND_GUILD_DEMOTE }, { "guild/demote", SPEECHCOMMAND_GUILD_DEMOTE },
    { "g/kick", SPEECHCOMMAND_GUILD_KICK }, { "guild/kick", SPEECHCOMMAND_GUILD_KICK },
    { "g/members", SPEECHCOMMAND_GUILD_MEMBERS }, { "guild/members", SPEECHCOMMAND_GUILD_MEMBERS },
    { "g/geosids", SPEECHCOMMAND_GUILD_GEOSIDS }, { "guild/geosids", SPEECHCOMMAND_GUILD_GEOSIDS },
    { "g/sethome", SPEECHCOMMAND_GUILD_SETHOME }, { "guild/sethome", SPEECHCOMMAND_GUILD_SETHOME },
    { "g/home", SPEECHCOMMAND_GUILD_HOME }, { "guild/home", SPEECHCOMMAND_GUILD_HOME },
    { "g/abandon", SPEECHCOMMAND_GUILD_ABANDON }, { "guild/abandon", SPEECHCOMMAND_GUILD_ABANDON },
    { "p/lead", SPEECHCOMMAND_PARTY_LEAD }, { "party/lead", SPEECHCOMMAND_PARTY_LEAD },
    { "p/join", SPEECHCOMMAND_PARTY_JOIN }, { "party/join", SPEECHCOMMAND_PARTY_JOIN },
    { "p/leave", SPEECHCOMMAND_PARTY_LEAVE }, { "party/leave", SPEECHCOMMAND_PARTY_LEAVE },
    { "p/kick", SPEECHCOMMAND_PARTY_KICK }, { "party/kick", SPEECHCOMMAND_PARTY_KICK },
    { "p/close", SPEECHCOMMAND_PARTY_CLOSE }, { "party/close", SPEECHCOMMAND_PARTY_CLOSE },
    { "/standup", SPEECHCOMMAND_STANDUP },
    { "/respawn", SPEECHCOMMAND_RESPAWN },
    { "/monsters", SPEECHCOMMAND_MONSTERS },
    { "/all", SPEECHCOMMAND_ALL },
    { "/mark", SPEECHCOMMAND_MARK },
  };


  static const size_t NUMBER_OF_COMMAND_TOKENS
    = sizeof(TOKENS) / sizeof(CommandToken);

  int i;
  size_t token_length;
  for (i = 0; i < NUMBER_OF_COMMAND_TOKENS; ++i) {
    token_length = strlen(TOKENS[i].token_text);
    if (0 == strncmp(TOKENS[i].token_text,
                     speech_text,
                     token_length)) {
      break;
    }
  }

  if (i == NUMBER_OF_COMMAND_TOKENS) return false;

  // found a command!
  speech_text += token_length;
  while (*speech_text == ' ') ++speech_text;
  switch (TOKENS[i].command) {
  default: assert(!"Don't know what to do with text token"); break;
  case SPEECHCOMMAND_GUILD_CREATE:
    Client::ActionQueue::add<Action_GuildCreate>()->guild_name = speech_text;
    break;
  case SPEECHCOMMAND_GUILD_LEAD:
    Client::ActionQueue::add<Action_GuildLead>();
    break;
  case SPEECHCOMMAND_GUILD_JOIN:
    Client::ActionQueue::add<Action_GuildJoin>();
    break;
  case SPEECHCOMMAND_GUILD_LEAVE:
    Client::ActionQueue::add<Action_GuildLeave>();
    break;
  case SPEECHCOMMAND_GUILD_PROMOTE:
    Client::ActionQueue::add<Action_GuildPromote>()->name = speech_text;
    break;
  case SPEECHCOMMAND_GUILD_DEMOTE:
    Client::ActionQueue::add<Action_GuildDemote>()->name = speech_text;
    break;
  case SPEECHCOMMAND_GUILD_KICK:
    Client::ActionQueue::add<Action_GuildKick>()->name = speech_text;
    break;
  case SPEECHCOMMAND_GUILD_MEMBERS:
    Client::ActionQueue::add<Action_GuildMembers>();
    break;
  case SPEECHCOMMAND_GUILD_CHAT:
    Client::ActionQueue::add<Action_GuildChat>()->text = speech_text;
    break;
  case SPEECHCOMMAND_GUILD_GEOSIDS:
    Client::ActionQueue::add<Action_GuildGeosids>();
    break;
  case SPEECHCOMMAND_GUILD_SETHOME:
    Client::ActionQueue::add<Action_GuildSetHome>();
    break;
  case SPEECHCOMMAND_GUILD_HOME:
    Client::ActionQueue::add<Action_GuildHome>();
    break;
  case SPEECHCOMMAND_GUILD_ABANDON: {
    Geosid::GeosidIndex geosid;
    if (Client::ClientFacade::singleton()
          ->getGeosidByName(speech_text, &geosid)) {
      Client::ActionQueue::add<Action_GuildAbandon>()->geosid = geosid;
    }
    } break;
  case SPEECHCOMMAND_PARTY_CHAT:
    Client::ActionQueue::add<Action_Party_Chat>()->text = speech_text;
    break;
  case SPEECHCOMMAND_PARTY_LEAD:
    Client::ActionQueue::add<Action_Party_Lead>();
    break;
  case SPEECHCOMMAND_PARTY_JOIN:
    Client::ActionQueue::add<Action_Party_Join>();
    break;
  case SPEECHCOMMAND_PARTY_LEAVE:
    Client::ActionQueue::add<Action_Party_Leave>();
    break;
  case SPEECHCOMMAND_PARTY_KICK:
    Client::ActionQueue::add<Action_Party_Kick>()->name = speech_text;
    break;
  case SPEECHCOMMAND_PARTY_CLOSE:
    Client::ActionQueue::add<Action_Party_Close>();
    break;
  case SPEECHCOMMAND_STANDUP:
    Client::ActionQueue::add<Action_Resurrect_StandUp>();
    break;
  case SPEECHCOMMAND_RESPAWN:
    Client::ActionQueue::add<Action_Resurrect_Respawn>();
    break;
  case SPEECHCOMMAND_MONSTERS:
    Client::ActionQueue::add<Action_AutoTarget_Monsters>();
    break;
  case SPEECHCOMMAND_ALL:
    Client::ActionQueue::add<Action_AutoTarget_All>();
    break;
  case SPEECHCOMMAND_MARK:
    Client::ActionQueue::add<Action_Mark>()->mark_index = atoi(speech_text);
    break;

  }
  return true;
}

//----[  update  ]-------------------------------------------------------------
void ChatTextLine::update() {
  if (send_) {
    send_ = false;

    const char* text = getText();
    if (!ParseSpeechCommand(text)) {
      Client::ActionQueue::add<Action_Speak>()->text = getText();
    }

    setText("");
    exit_ = true;
  }
  if (exit_) {
    exit_ = false;
    Client::ActionQueue::add<Action_CloseDialogs>();
    setText("");
  }
}
