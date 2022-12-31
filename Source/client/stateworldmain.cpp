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
#include "stdafx.h"
#include <dc/filestream>
#include "shared/evidyon_version.h"
#include "evidyonguidefs.h"
#include "common/gui/guicanvasmatrix.h"
#include "evidyonguibuttonpanel.h"
#include "evidyonguiglobalchatdisplay.h"
#include "shared/evidyon_world.h"
#include "shared/actor/actorsync.h"
#include "shared/evidyon_specialfx.h"
#include "shared/client-server/fastsync.h"
#include "shared/evidyon_network.h"
#include "evidyonguienchantmenticons.h"
#include "shared/magic/enchantmenttype.h"


#include <dcx/bitmask>
#include "common/gui/guiscrollablelist.h"
#include "evidyonguigameplaycontrols.h"


#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"

#include "shared/zone/zone.h"

#include "shared/binding/binding.h"
#include "shared/binding/bindingptr.h"

#include "shared/avatar/avatarlimits.h"
#include "shared/guild/guildlimits.h"

#include "common/kg/kg.h"

#include "evidyonguichatlog.h"
#include "client/actions.h"
#include "client/actionqueue.h"
#include "client/geosid/geosidinstance.h"
#include "shared/geosid/geosidclientdescription.h"
#include "client/clientfacade.h"

struct StateWorldMainContext;

//----[  ActionProcessor  ]----------------------------------------------------
class ActionProcessor {
public:
  ActionProcessor(EvidyonClient* client, StateWorldMainContext* context);
public:
  void process(const Action_CastSpell* action);
  void process(const Action_UseConsumable* action);
  void process(const Action_Move* action);
  void process(const Action_ActorInteraction* action);
  void process(const Action_Attack* action);
  void process(const Action_NPCInteraction* action);
  void process(const Action_LogOutOfWorld* action);
  void process(const Action_ToggleViewAngle* action);
  void process(const Action_ToggleShowNames* action);
  void process(const Action_SayAgain* action);
  void process(const Action_StopAction* action);
  void process(const Action_OpenInventory* action);
  void process(const Action_OpenAttributes* action);
  void process(const Action_OpenKeyBindings* action);
  void process(const Action_OpenChatAndActions* action);
  void process(const Action_OpenStatDisplay* action);
  void process(const Action_OpenChat* action);
  void process(const Action_OpenDrop* action);
  void process(const Action_BazaarOpen* action);
  void process(const Action_ToggleChatLog* action);
  void process(const Action_CloseChatAndActions* action);
  void process(const Action_CloseInventory* action);
  void process(const Action_CloseKeyBindings* action);
  void process(const Action_CloseStatDisplay* action);
  void process(const Action_CloseChat* action);
  void process(const Action_CloseDrop* action);
  void process(const Action_CloseBazaar* action);
  void process(const Action_AttributesAddAbilityPoint* action);
  void process(const Action_Speak* action);
  void process(const Action_DropItem* action);
  void process(const Action_DropCurrency* action);
  void process(const Action_EquipItem* action);
  void process(const Action_UnequipItem* action);
  void process(const Action_BazaarCreateListing* action);
  void process(const Action_BazaarRetractListing* action);
  void process(const Action_BazaarPurchaseListing* action);
  void process(const Action_BazaarRedeemListing* action);
  void process(const Action_BazaarGetListings* action);
  void process(const Action_TakeScreenshot* action);
  void process(const Action_OpenCast* action);
  void process(const Action_OpenUseItem* action);
  void process(const Action_EventEscape* action);
  void process(const Action_StorageOpen* action);
  void process(const Action_StorageWithdrawItem* action);
  void process(const Action_StorageDepositItem* action);
  void process(const Action_Geosid_SacrificeItem* action);
  void process(const Action_Geosid_Burst* action);
  void process(const Action_Geosid_BurstAttackGeosid* action);
  void process(const Action_Geosid_BurstSpell* action);
  void process(const Action_Geosid_BurstUseGeosid* action);
  void process(const Action_Geosid_GetOwner* action);
  void process(const Action_GuildGeosids* action);
  void process(const Action_GuildSetHome* action);
  void process(const Action_GuildHome* action);
  void process(const Action_GuildAbandon* action);
  void process(const Action_TradeAddItem* action);
  void process(const Action_TradeSetCurrency* action);
  void process(const Action_TradeRemoveItem* action);
  void process(const Action_TradeAccept* action);
  void process(const Action_TradeWithdraw* action);
  void process(const Action_TradeCancel* action);
  void process(const Action_SplitStackedItem* action);
  void process(const Action_QuestOfferResponse* action);
  void process(const Action_QuestGiveUp* action);
  void process(const Action_CloseDialogs* action);
  void process(const Action_ToggleNameDisplay* action);
  void process(const Action_GuildCreate* action);
  void process(const Action_GuildLead* action);
  void process(const Action_GuildJoin* action);
  void process(const Action_GuildLeave* action);
  void process(const Action_GuildPromote* action);
  void process(const Action_GuildDemote* action);
  void process(const Action_GuildChat* action);
  void process(const Action_GuildKick* action);
  void process(const Action_GuildMembers* action);
  void process(const Action_Party_Chat* action);
  void process(const Action_Party_Lead* action);
  void process(const Action_Party_Join* action);
  void process(const Action_Party_Leave* action);
  void process(const Action_Party_Kick* action);
  void process(const Action_Party_Close* action);
  void process(const Action_Resurrect_StandUp* action);
  void process(const Action_Resurrect_Respawn* action);
  void process(const Action_Show_Help* action);
  void process(const Action_NPCTrigger* action);
  void process(const Action_Mark* action);
  void process(const Action_AutoTarget_All* action);
  void process(const Action_AutoTarget_Monsters* action);


private:
  EvidyonClient* client_;
  StateWorldMainContext* context_;
};





//----[  MouseOverActorBinding  ]----------------------------------------------
class MouseOverActorBinding : public Evidyon::Binding::Binding {
public:
  MouseOverActorBinding(Evidyon::Binding::BindingPtr actor) {
    actor_ = actor;
  }

  ~MouseOverActorBinding() {
    actor_.reset();
  }

  virtual void update(double time, double time_since_last_update)  {
    if (actor_->hasExpired()) setExpired();
  }
  virtual bool visible() { return actor_->visible(); }
  virtual const D3DXMATRIX* getTransform() const { return actor_->getTransform(); }
  void mouseLeftActor() { setExpired(); }

  Evidyon::Binding::BindingPtr actor_;
};



struct GeosidData {
  std::string description;
  std::string guild;
  Geosid::Geonite geonite;
};

//----[  StateWorldMainContext  ]----------------------------------------------
struct StateWorldMainContext {
  typedef std::map<const Geosid::GeosidClientDescription*,GeosidData> Geosids;
  Geosids geosids;

  EvidyonClient* client_;
  bool take_screenshot;

  double this_frame_time;
  Client::ActionQueue action_queue;
  ActionProcessor action_processor;

  GUIFont font;
  GUIFont speech_font;
  EvidyonGUITooltipBox tooltip_box;
  EvidyonGUIStatusBars status_bars;
  EvidyonGUIEnchantmentIcons enchantment_icons;
  ItemInStorage storage[Evidyon::MAX_ITEMS_IN_STORAGE];
  EvidyonGUIGameplayControls gameplay_controls_;
  Evidyon::Client::EvidyonGUIChatLog chat_log;
  std::string last_spoken_text_line;
  EvidyonGUIGlobalChatDisplay chat_display;
  typedef std::tr1::shared_ptr<MouseOverActorBinding> MouseOverActorBindingPtr;
  MouseOverActorBindingPtr mouse_over_actor_binding;

  GUILayer* gui_primary_layer;

  void updateGeosidVisibility();

  void setGeosidData(const Geosid::GeosidClientDescription* geosid,
                     const char* guild,
                     Geosid::Geonite geonite) {
    StateWorldMainContext::Geosids::iterator i
      = geosids.find(geosid);
    if (i != geosids.end()) {
      kg::CharBuffer<128> description;
      if (guild != NULL && guild[0] != '\0') {
        sprintf_s(description.characters,
                  description.SIZE,
                  "[%s - %i geonite needed%s]",
                  guild,
                  geonite,
                  geosid->type == Geosid::GEOSID_TOWN_WHITE ? " for next event" : " to conjure next item");
      } else {
        sprintf_s(description.characters,
                  description.SIZE,
                  "[%i geonite needed%s]",
                  geonite,
                  geosid->type == Geosid::GEOSID_TOWN_WHITE ? " for next event" : " to conjure next item");
      }
      i->second.description = description.characters;
      i->second.guild = guild;
      i->second.geonite = geonite;
    }
  }

  void setGeosidGeonite(const Geosid::GeosidClientDescription* geosid,
                        Geosid::Geonite geonite) {
    StateWorldMainContext::Geosids::iterator i
      = geosids.find(geosid);
    if (i != geosids.end()) {
      setGeosidData(geosid, i->second.guild.c_str(), geonite);
    }
  }

  bool setMouseOverActor(Evidyon::Binding::BindingPtr actor) {
    if (mouse_over_actor_binding.get() != NULL &&
        mouse_over_actor_binding->actor_ == actor) return false;
    clearMouseOverActor();
    MouseOverActorBindingPtr binding(new MouseOverActorBinding(actor));
    mouse_over_actor_binding = binding;
    return true;
  }
  void clearMouseOverActor() {
    MouseOverActorBinding* binding
      = mouse_over_actor_binding.get();
    if (binding) binding->mouseLeftActor();
    mouse_over_actor_binding.reset();
  }

  // temporary /ignore for annoying people
  std::set<std::string> characters_to_ignore;

  double display_names_time;
  bool display_names;
  bool draw_names; // lags display_names by the fade time

  int last_fast_sync_stat;

  StateWorldMainContext(EvidyonClient* client);
  bool create(dcxIniFileReader* config,
              LPDIRECT3DDEVICE9 d3d_device,
              LPDIRECT3DTEXTURE9 d3d_texture_font,
              GUILayerBasic* primary_layer,
              GUICanvas* mouse_cursor_canvas,
              GUIFont* window_font,
              GUIMouseSentinel* primary_mouse_sentinel);
  void destroy(GUILayerBasic* primary_layer);

  ActiveQuestData active_quests[3];

  Zone::ZoneIndex current_zone;

  const char* zone_text;
  D3DCOLOR zone_text_color;
};





//----[  stateWorldMain  ]-----------------------------------------------------
void EvidyonClient::stateWorldMain() {
using namespace ::Evidyon::Network::Packet;
using namespace ::Evidyon::Network::Packet::Client;


  SetCursor(NULL);
  ShowCursor(FALSE);

  // Clear the screen
  myItemsOnMapTable.clear();

  //if (myUsingMixedDeviceFlag)
  //    CONFIRM(d3d_device_->SetSoftwareVertexProcessing(TRUE));

  StateWorldMainContext* context = new StateWorldMainContext(this);

  // Build the camera
  camera_.rebuildTopdownCamera(d3d_params_.BackBufferWidth,
                               d3d_params_.BackBufferHeight);

  // Set up the context
  CONFIRM(context->create(&config_,
                          d3d_device_,
                          getImage(Evidyon::Client::CLIENT_IMAGE_BLOCK_FONT),
                          &gui_primary_layer_,
                          &mouse_cursor_canvas_,
                          &window_font_,
                          myMouseInputSource.getPrimarySentinel())) else {
    context->destroy(&gui_primary_layer_);
    SAFE_DELETE(context);
  }


  keepConnectionAlive(); // don't lag out during initialization!



  {
    char keymapName[Avatar::MAX_AVATAR_NAME_LENGTH + 16];
    sprintf_s(keymapName,
              sizeof(keymapName),
              "%s.keys",
              myCharacter.name);
    context->gameplay_controls_.create(keymapName,
                                       &context->font,
                                       &gui_primary_layer_,
                                       myMouseInputSource.getPrimarySentinel(),
                                       myKeyboardInputSource.getPrimarySentinel(),
                                       &mouse_cursor_canvas_,
                                       items_,
                                       number_of_items_,
                                       myCharacter.inventory,
                                       spells_,
                                       number_of_spells_,
                                       &myCharacter.spell_availability,
                                       context->status_bars.getSize(),
                                       &camera_,
                                       &actor_manager_,
                                       context->storage,
                                       quest_array_,
                                       number_of_quests_);
  }


  keepConnectionAlive(); // don't lag out during initialization!



  dcxWin32Clock clock;
  double this_frame_time = clock.getAccurateSystemTime();
  context->this_frame_time = this_frame_time;
  double last_frame_time = this_frame_time - 0.1;


  myStateMachine.jumpState(VCS_DISCONNECT);
  myStateMachine.queueState(VCS_SHUTDOWN);



  // Main game loop
  bool runWorldMain = true;
  while(windowsMessagePump() && runWorldMain) {

    // Advance the current frame
    this_frame_time = clock.getAccurateSystemTime();

    // Poll the network connection
    ENetEvent net_event;
    while (enet_host_service(enet_client_, &net_event, 0) > 0) {
      if (net_event.type == ENET_EVENT_TYPE_DISCONNECT) {
        myStateMachine.jumpState(VCS_CONNECT);  // Lost connection; try to reconnect
        runWorldMain = false; // Exit to clean up
        break;
      }
      if (net_event.type != ENET_EVENT_TYPE_RECEIVE) continue;

      using namespace ::Evidyon::Network::Packet;
      Message message;
      decodeMessageType(net_event.packet, &message);

#ifdef ENABLE_ADMIN_CONSOLE
      if (admin_console_.examineNetworkMessage(message, net_event.packet)) {
        enet_packet_destroy(net_event.packet);
        continue;
      }
#endif

      if (message == NETMSG_SLOWSYNC) {
        NetMsg_SlowSync(this_frame_time, net_event.packet);
      } else if (message == ::Evidyon::Network::Packet::NETMSG_FASTSYNC) {
        using namespace ::Evidyon::Network::Packet;
        Evidyon::Actor::ActorFastSync sync;
        NetMsg_FastSync::readPacket(net_event.packet, &sync);
        actor_manager_.fastSync(this_frame_time,
                                sync.time_since_last_sync,
                                sync.data.unpackX(),
                                sync.data.unpackY(),
                                sync.data.flags);
        context->last_fast_sync_stat = sync.stat_type_reset ? 0 : context->last_fast_sync_stat + 1;
        switch (context->last_fast_sync_stat) {
          case 0:
          case 2: myCharacter.currentHPPercent  = 1.0f / 127.0f * sync.stat_value; break;
          case 1:
          case 3: myCharacter.currentMPPercent  = 1.0f / 127.0f * sync.stat_value; break;
          case 4: myCharacter.currentEXPPercent = 1.0f / 127.0f * sync.stat_value; break;
        }
        context->status_bars.set(myCharacter.currentHPPercent,
                                 myCharacter.currentMPPercent,
                                 myCharacter.currentEXPPercent);
      } else if (message == ::Evidyon::Network::Packet::NETMSG_TELLACTORFULLDESCRIPTIONS) {
        using namespace ::Evidyon::Network::Packet;
        static const int BUFFER_SIZE = 512/sizeof(Actor::ActorDescription)+1;
        Actor::ActorDescription descriptions[BUFFER_SIZE];
        size_t number_of_descriptions;
        NetMsg_TellActorFullDescriptions::readPacket(
          net_event.packet,
          &CopiedArray<Actor::ActorDescription>(descriptions,
                                                BUFFER_SIZE,
                                                &number_of_descriptions));
        for (size_t i = 0; i < number_of_descriptions; ++i) {
          actor_manager_.setActorDescription(this_frame_time, &descriptions[i]);
        }
      } else if (message == ::Evidyon::Network::Packet::NETMSG_TELLACTORDESCRIPTION) {
        using namespace ::Evidyon::Network::Packet;
        Actor::ActorDescription description;
        NetMsg_TellActorDescription::readPacket(
          net_event.packet,
          &description);
        actor_manager_.setActorDescription(this_frame_time, &description);
      } else if (message == ::Evidyon::Network::Packet::NETMSG_LEAVEWORLD_CONFIRMED) {
          bool leftWorld = !APP_WARNING(!Decode::leaveWorldConfirmed(net_event.packet))
                                       ("Failed to decode leaveWorldConfirmed");

          if (leftWorld) {
              // Return to main menu
          }
          else
          {
              // Something weird happened; just quit
              myStateMachine.jumpState(VCS_DISCONNECT);
              myStateMachine.queueState(VCS_NO_CONNECTION);
          }

          // Exit the main loop so we can clean up
          runWorldMain = false;

      } else if (message == ::Evidyon::Network::Packet::NETMSG_UPDATEINVENTORY ||
            message == ::Evidyon::Network::Packet::NETMSG_TELLINVENTORY) {
        Decode::updateOrTellInventory(net_event.packet,
                                      myCharacter.inventory,
                                     &myCharacter.highest_inventory_update_index_,
                                      myCharacter.inventory_update_indices_);
        context->gameplay_controls_.syncInventory();

        // force the dialogs to sync with the mouse
        myMouseInputSource.generateMovementUpdate();
      } else if (message == ::Evidyon::Network::Packet::NETMSG_ACTOR_SPEAKS) {
        Actor::ActorID actor_id;
        kg::CharBuffer<256> text;
        size_t text_len;
        if (NetMsg_Actor_Speaks::readPacket(
              net_event.packet,
              &actor_id,
              &CopiedArray<char>(text.characters, text.CAPACITY, &text_len))) {
          text[text_len] = '\0';
          actor_manager_.setActorSpeech(
            actor_id,
            Actor::INVALID_ACTOR_ID,
            text.characters);
        }
      } else if (message == ::Evidyon::Network::Packet::NETMSG_CHANGEMAP) {
        World::MapIndex map_id;
        CONFIRM(Decode::changeMap(net_event.packet, &map_id)) else continue;
        map_renderer_.reset();
        current_map_index_ = map_id;
        current_map_ = &maps_[map_id];
        context->current_zone = Zone::INVALID_ZONE_INDEX;
        actor_manager_.clear(false);
        { // get rid of all of the items on the map
          ItemsOnMapTable::iterator start = myItemsOnMapTable.begin();
          ItemsOnMapTable::iterator end = myItemsOnMapTable.end();
          for (ItemsOnMapTable::iterator i = start; i != end; ++i) {
              scenery_renderer_.release(i->second);
          }
          myItemsOnMapTable.clear();
        }
      } else if (message == ::Evidyon::Network::Packet::NETMSG_TELLITEMSONMAPLOCATION) {
        int x, y;
        unsigned short item_types[MAX_ITEMS_VISIBLE_PER_LOCATION];
        int number_of_item_types;
        CONFIRM(Decode::tellItemsOnMapLocation(net_event.packet,
                                              &x,
                                              &y,
                                               item_types,
                                              &number_of_item_types)) else continue;
        // update the scenery here
        ItemsOnMapTable::key_type locationKey(x, y);
        {
            // Remove all of the scenery at this location
            ItemsOnMapTable::iterator start = myItemsOnMapTable.lower_bound(locationKey);
            ItemsOnMapTable::iterator end = myItemsOnMapTable.upper_bound(locationKey);
            for (ItemsOnMapTable::iterator i = start; i != end; ++i)
                scenery_renderer_.release(i->second);
            myItemsOnMapTable.erase(start, end);
        }

        // Create a transformation matrix for this location
        float height = 0.0f;
        switch (maps_[getCurrentMap()].getNavigability(x, y)) {
          case World::NAVIGABILITY_PIT: height = -5; break;
          case World::NAVIGABILITY_WADE:
          case World::NAVIGABILITY_SWIM: height = -1.0f; break;
        }
        D3DXMATRIXA16 s;
        D3DXMatrixScaling(&s, 2.0f, 2.0f, 2.0f);
        D3DXMATRIXA16 offsetMatrix;
        D3DXMatrixTranslation(&offsetMatrix, 1.0f * x + 0.5f, height+0.01f, 1.0f * y + 0.5f);
        D3DXMatrixMultiply(&offsetMatrix, &s, &offsetMatrix);

        for (size_t i = 0; i < number_of_item_types; ++i) {
          if (item_types[i] == 0xFFFF) continue; // shorthand for invalid item
          Scenery::SceneryInstance* instance
            = scenery_renderer_.acquire(item_types[i]);
          if (instance) {
            instance->setTransform(&offsetMatrix);
            myItemsOnMapTable.insert(
              ItemsOnMapTable::value_type(locationKey, instance));
          }
        }
      } else if (message == ::Evidyon::Network::Packet::NETMSG_UPDATEYOUREFFECTAVAILABILITYMASK) {
        Decode::updateYourEffectAvailabilityMask(net_event.packet,
                                                &myCharacter.spell_availability);
        context->gameplay_controls_.syncSpells();
      }
      else if (message == ::Evidyon::Network::Packet::NETMSG_TELLYOURAVATARSTATS) {
        using namespace ::Evidyon::Network::Packet;
        NetMsg_TellYourAvatarStats_Data data;
        if (NetMsg_TellYourAvatarStats::readPacket(net_event.packet, &data)) {
          Actor::Client::ClientActorInstance* client_actor = actor_manager_.getClientActor();
          Actor::ActorAlignment alignment_class =  client_actor->getAlignment();
          const char* alignment_class_text = Actor::ActorAlignmentString(alignment_class);
          D3DCOLOR alignment_class_color = Actor::ActorAlignmentColor(alignment_class);
            context->gameplay_controls_.syncAvatarStats()->update(
              &data,
              alignment_class_text,
              alignment_class_color,
              myCharacter.money);
        }
      } else if (message == ::Evidyon::Network::Packet::NETMSG_TELLACTORNAME) {
        Evidyon::Actor::ActorID actor_id = 0;
        char actor_name[128];
        size_t name_length;
        NetMsg_TellActorName::readPacket(net_event.packet, &actor_id, &CopiedArray<char>(actor_name, 128, &name_length));
        actor_name[name_length] = '\0';
        actor_manager_.setActorName(actor_id, actor_name);
      } else if (message == ::Evidyon::Network::Packet::NETMSG_UPDATELOCALWORLDREIGON) {
        World::RegionID current_region_id;
        if (Decode::updateLocalWorldRegion(net_event.packet, &current_region_id)) {
          {
            ItemsOnMapTable table_of_items_to_keep;
            for (ItemsOnMapTable::iterator i = myItemsOnMapTable.begin();
                                           i != myItemsOnMapTable.end();
                                           ++i) {
              ItemsOnMapTable::key_type location = i->first;
              short region_x, region_y;
              bool keep_this_item =
                World::regionIDFromPoint(location.first,
                                         location.second,
                                         &region_x,
                                         &region_y) == current_region_id;

              // check neighboring regions if this item isn't in the local region
              if (!keep_this_item) {
                World::RegionID neighbors[8] = {
                  World::regionIDFromRegionCoordinates(region_x - 1, region_y - 1),
                  World::regionIDFromRegionCoordinates(region_x - 1, region_y    ),
                  World::regionIDFromRegionCoordinates(region_x - 1, region_y + 1),
                  World::regionIDFromRegionCoordinates(region_x,     region_y - 1),
                  World::regionIDFromRegionCoordinates(region_x,     region_y + 1),
                  World::regionIDFromRegionCoordinates(region_x + 1, region_y - 1),
                  World::regionIDFromRegionCoordinates(region_x + 1, region_y    ),
                  World::regionIDFromRegionCoordinates(region_x + 1, region_y + 1),
                };
                keep_this_item = keep_this_item || neighbors[0] == current_region_id;
                keep_this_item = keep_this_item || neighbors[1] == current_region_id;
                keep_this_item = keep_this_item || neighbors[2] == current_region_id;
                keep_this_item = keep_this_item || neighbors[3] == current_region_id;
                keep_this_item = keep_this_item || neighbors[4] == current_region_id;
                keep_this_item = keep_this_item || neighbors[5] == current_region_id;
                keep_this_item = keep_this_item || neighbors[6] == current_region_id;
                keep_this_item = keep_this_item || neighbors[7] == current_region_id;
              }

              if (keep_this_item) {
                table_of_items_to_keep.insert(*i);
              } else {
                scenery_renderer_.release(i->second);
              }
            }

            myItemsOnMapTable.swap(table_of_items_to_keep);
          }
        }
      } else if (message == NETMSG_STORAGE_TELL) {
        int number_of_items = 0;
        if (Decode::storageTell(net_event.packet, context->storage, &number_of_items)) {
          context->gameplay_controls_.syncStorageItems();
        }
      } else if (message == NETMSG_TRADE_CANCEL) {
        context->gameplay_controls_.hideDialog(EvidyonGUIGameplayControls::DIALOG_TRADE);
      } else if (message == NETMSG_TRADE_TELLITEMSINTRADE) {

        unsigned char my_items[MAX_ITEMS_PER_TRADE];
        size_t number_of_my_items;
        TradeItemDescription their_items[MAX_ITEMS_PER_TRADE]; // TODO: MAX_ITEMS_PER_TRADE not correct on server, but its bigger so its ok
        size_t number_of_their_items;
        char their_name[Avatar::MAX_AVATAR_NAME_LENGTH];
        size_t their_name_length;
        CurrencyType my_offered_currency, partner_offered_currency;

        bool decoded =
          NetMsg_Trade_TellItemsInTrade::Serializer::readPacket(
            net_event.packet,
           &CopiedArray<char>(their_name, Avatar::MAX_AVATAR_NAME_LENGTH, &their_name_length),
           &my_offered_currency,
           &CopiedArray<unsigned char>(my_items, MAX_ITEMS_PER_TRADE, &number_of_my_items),
           &partner_offered_currency,
           &CopiedArray<TradeItemDescription>(their_items, MAX_ITEMS_PER_TRADE, &number_of_their_items));

        if (decoded && their_name_length < Avatar::MAX_AVATAR_NAME_LENGTH) {
          their_name[their_name_length] = 0;
          context->gameplay_controls_.syncTrade(their_name,
                                       my_offered_currency,
                                       my_items, number_of_my_items,
                                       partner_offered_currency,
                                       their_items, number_of_their_items);
        }
      } else if (message == NETMSG_BAZAAR_TELLITEMS) {
        BazaarCategory category;
        size_t number_of_items;
        ItemInBazaar items[MAX_ITEMS_PER_BAZAAR_CATEGORY];
        bool decoded =
          NetMsg_Bazaar_TellItems::readPacket(net_event.packet,
            &category,
            &CopiedArray<ItemInBazaar>(items, MAX_ITEMS_PER_BAZAAR_CATEGORY, &number_of_items));
        if (decoded) {
          context->gameplay_controls_.syncBazaar(category, items, number_of_items);
        }
      } else if (message == NETMSG_TELLCURRENCY) {
        NetMsg_TellCurrency::readPacket(net_event.packet, &myCharacter.money);
      } else if (message == NETMSG_QUEST_OFFER) {
        unsigned int quest_index;
        Evidyon::Actor::ActorID npc_actor_id;
        int npc_quest_index;
        unsigned int validation_number;
        if (NetMsg_Quest_Offer::readPacket(net_event.packet,
                                           &quest_index,
                                           &npc_actor_id,
                                           &npc_quest_index,
                                           &validation_number)) {
          context->gameplay_controls_.promptQuestProposal(quest_index,
                                                 npc_actor_id,
                                                 npc_quest_index,
                                                 validation_number);
        }
      } else if (message == NETMSG_ACTOR_SPEAKS_TO_ACTOR) {
        char speech[MAX_PACKET_SIZE];
        size_t speech_length;
        Evidyon::Actor::ActorID source_actor_id, target_actor_id;
        if (NetMsg_Actor_Speaks_To_Actor::readPacket(
              net_event.packet,
              &source_actor_id,
              &target_actor_id,
              &CopiedArray<char>(speech, sizeof(speech), &speech_length))) {
          speech[speech_length] = '\0';
          actor_manager_.setActorSpeech(source_actor_id,
                                        target_actor_id,
                                        speech);
        }
      } else if (message == NETMSG_QUEST_TELLCURRENTQUESTS) {
        unsigned int quest[3];
        int minutes_left[3];
        if (NetMsg_Quest_TellCurrentQuests::readPacket(
              net_event.packet,
              &quest[0], &quest[1], &quest[2],
              &minutes_left[0], &minutes_left[1], &minutes_left[2])) {
          for (int i = 0; i < 3; ++i) {
            if (quest[i] < number_of_quests_) {
              context->active_quests[i].name = quest_array_[quest[i]].name;
              context->active_quests[i].description = quest_array_[quest[i]].description;
              context->active_quests[i].expiration_time = this_frame_time + minutes_left[i] * 60.0;
              context->active_quests[i].quest_index = quest[i];
            } else {
              context->active_quests[i].name.clear();
              context->active_quests[i].description.clear();
              context->active_quests[i].expiration_time = 0.0;
              context->active_quests[i].quest_index = 0xFFFFFFFF;
            }
          }
          context->gameplay_controls_.syncActiveQuestDisplay(context->active_quests);
        }
      } else if (message == NETMSG_EVENT_GENERATEBASICPROJECTILE) {
        NetMsg_Event_GenerateBasicProjectile(this_frame_time, net_event.packet);
      } else if (message == NETMSG_EVENT_TERMINATE) {
        NetMsg_Event_Terminate(this_frame_time, net_event.packet);
      } else if (message == NETMSG_EVENT_GENERATETARGETEDSPECIALFX_SRCTOTGT) {
        NetMsg_Event_GenerateTargetedSpecialFX_SrcToTgt(this_frame_time, net_event.packet);
      } else if (message == NETMSG_EVENT_GENERATETARGETEDSPECIALFX_TGTXZ) {
        NetMsg_Event_GenerateTargetedSpecialFX_TgtXZ(this_frame_time, net_event.packet);
      } else if (message == NETMSG_EVENT_GENERATETARGETEDSPECIALFX_SRCXZTGTXZ) {
        NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZ(this_frame_time, net_event.packet);
      } else if (message == NETMSG_EVENT_GENERATETARGETEDSPECIALFX_TGTXZAOE) {
        NetMsg_Event_GenerateTargetedSpecialFX_TgtXZAOE(this_frame_time, net_event.packet);
      } else if (message == NETMSG_EVENT_GENERATETARGETEDSPECIALFX_SRCXZTGTXZAOE) {
        NetMsg_Event_GenerateTargetedSpecialFX_SrcXZTgtXZAOE(this_frame_time, net_event.packet);
      } else if (message == NETMSG_TELLYOURBUFFICONS) {
        Magic::BuffMask buff_icons;
        NetMsg_TellYourBuffIcons::readPacket(net_event.packet, &buff_icons);
        context->enchantment_icons.update(&buff_icons);
      } else if (message == NETMSG_GUILD_CHAT_DISTRIBUTE) {
        kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
        kg::CharBuffer<256> chat;
        size_t name_chars, chat_chars;
        if (NetMsg_Guild_Chat_Distribute::readPacket(net_event.packet,
              &CopiedArray<char>(name.characters, name.CAPACITY, &name_chars),
              &CopiedArray<char>(chat.characters, chat.CAPACITY, &chat_chars))) {
          name[name.CAPACITY] = '\0';
          name[name_chars] = '\0';
          chat[chat.CAPACITY] = '\0';
          chat[chat_chars] = '\0';
          context->chat_display.addGuildTextLine(false, name.characters, chat.characters);
          context->chat_log.addGuildText(name.characters, chat.characters);
        }
      } else if (message == NETMSG_GUILD_MESSAGE) {
        kg::CharBuffer<Guild::MAX_GUILD_MESSAGE_LENGTH> text;
        size_t text_chars;
        if (NetMsg_Guild_Message::readPacket(net_event.packet,
              &CopiedArray<char>(text.characters, text.CAPACITY, &text_chars))) {
          text[text.CAPACITY] = '\0';
          text[text_chars] = '\0';
          context->chat_display.addGuildTextLine(false, "<Guild>", text.characters);
          context->chat_log.addGuildText("<Guild>", text.characters);
        }
      } else if (message == NETMSG_GUILD_TELLFOUNDERACTORNAME) {
        Actor::ActorID actor_id;
        kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
        kg::CharBuffer<Guild::MAX_NAME_LENGTH> guild;
        size_t name_chars, guild_chars;
        if (NetMsg_Guild_TellFounderActorName::readPacket(
              net_event.packet,
              &actor_id,
              &CopiedArray<char>(name.characters, name.CAPACITY, &name_chars),
              &CopiedArray<char>(guild.characters, guild.CAPACITY, &guild_chars))) {
          name[name_chars] = '\0';
          guild[guild_chars] = '\0';
          actor_manager_.setActorGuildFounderName(actor_id, name.characters, guild.characters);
        }
      } else if (message == NETMSG_GUILD_TELLLEADERACTORNAME) {
        Actor::ActorID actor_id;
        kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
        kg::CharBuffer<Guild::MAX_NAME_LENGTH> guild;
        size_t name_chars, guild_chars;
        if (NetMsg_Guild_TellLeaderActorName::readPacket(
              net_event.packet,
              &actor_id,
              &CopiedArray<char>(name.characters, name.CAPACITY, &name_chars),
              &CopiedArray<char>(guild.characters, guild.CAPACITY, &guild_chars))) {
          name[name_chars] = '\0';
          guild[guild_chars] = '\0';
          actor_manager_.setActorGuildLeaderName(actor_id, name.characters, guild.characters);
        }
      } else if (message == NETMSG_GUILD_TELLMEMBERACTORNAME) {
        Actor::ActorID actor_id;
        kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
        kg::CharBuffer<Guild::MAX_NAME_LENGTH> guild;
        size_t name_chars, guild_chars;
        if (NetMsg_Guild_TellMemberActorName::readPacket(
              net_event.packet,
              &actor_id,
              &CopiedArray<char>(name.characters, name.CAPACITY, &name_chars),
              &CopiedArray<char>(guild.characters, guild.CAPACITY, &guild_chars))) {
          name[name_chars] = '\0';
          guild[guild_chars] = '\0';
          actor_manager_.setActorGuildMemberName(actor_id, name.characters, guild.characters);
        }
      } else if (message == NETMSG_GUILD_MESSAGE_TELLGEOSIDS) {
        kg::Array<Geosid::GeosidIndex, Guild::MAX_GEOSIDS_OWNED> geosids;
        if (NetMsg_Guild_Message_TellGeosids::readPacket(net_event.packet,
          &CopiedArray<Geosid::GeosidIndex>(geosids.elements, geosids.SIZE, &geosids.count))) {
          std::string text;
          if (geosids.count == 0) {
            text = "No geosids owned";
          } else {
            text = "Geosids owned:  ";
            for (size_t i = 0; i < geosids.count; ++i) {
              const Geosid::GeosidClientDescription* geosid;
              confirm(geosids_.get(geosids[i], &geosid)) else continue;
              text += std::string(i == 0 ? "" : ", ") + geosid->name;
            }
          }
          context->chat_display.addGuildTextLine(false, "<Guild>", text);
          context->chat_log.addGuildText("<Guild>", text);
        }
      } else if (message == NETMSG_GUILD_MESSAGE_TELLATTACK) {
        bool is_takeover_attack;
        Geosid::GeosidIndex geosid_index;
        kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
        size_t name_chars;
        if (NetMsg_Guild_Message_TellAttack::readPacket(
            net_event.packet,
            &geosid_index,
            &is_takeover_attack,
            &CopiedArray<char>(name.characters, name.CAPACITY, &name_chars))) {
          name.characters[name.CAPACITY] = '\0';
          name.characters[name_chars] = '\0';
          const Geosid::GeosidClientDescription* geosid;
          if (geosids_.get(geosid_index, &geosid)) {
            std::string message = std::string("Your guild is now ");
            message += (is_takeover_attack ? "attempting to take over" : "attacking");
            message += std::string("the ") + geosid->name + std::string(" geosid");
            context->chat_display.addGuildTextLine(false, "<Guild>", message);
            context->chat_log.addGuildText("<Guild>", message);
            message = "It will be ";
            message += is_takeover_attack ? "under your control" : "freed from the owners' control";
            message += " in 30 seconds as long as ";
            message += name.characters;
            message += " stays alive and nearby";
            context->chat_display.addGuildTextLine(false, "<Guild>", message);
            context->chat_log.addGuildText("<Guild>", message);
          }
        }
      } else if (message == NETMSG_GUILD_MESSAGE_GEOSIDGAINED) {
        Geosid::GeosidIndex geosid_index;
        if (NetMsg_Guild_Message_GeosidGained::readPacket(
          net_event.packet,
          &geosid_index)) {
          const Geosid::GeosidClientDescription* geosid;
          if (geosids_.get(geosid_index, &geosid)) {
            std::string message = std::string("Gained the ");
            message += geosid->name;
            message += " geosid!";
            context->chat_display.addGuildTextLine(false, "<Guild>", message);
            context->chat_log.addGuildText("<Guild>", message);
          }
        }
      } else if (message == NETMSG_GUILD_MESSAGE_GEOSIDLOST) {
        Geosid::GeosidIndex geosid_index;
        if (NetMsg_Guild_Message_GeosidLost::readPacket(
          net_event.packet,
          &geosid_index)) {
          const Geosid::GeosidClientDescription* geosid;
          if (geosids_.get(geosid_index, &geosid)) {
            std::string message = std::string("Lost the ");
            message += geosid->name;
            message += " geosid!";
            context->chat_display.addGuildTextLine(false, "<Guild>", message);
            context->chat_log.addGuildText("<Guild>", message);
          }
        }
      } else if (message == NETMSG_GUILD_MESSAGE_GEOSIDUNDERATTACK) {
        kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
        size_t name_chars;
        Geosid::GeosidIndex geosid_index;
        if (NetMsg_Guild_Message_GeosidUnderAttack::readPacket(
              net_event.packet,
              &geosid_index,
              &CopiedArray<char>(name.characters, name.CAPACITY, &name_chars))) {
          name.characters[name.CAPACITY] = '\0';
          name.characters[name_chars] = '\0';
          const Geosid::GeosidClientDescription* geosid;
          if (geosids_.get(geosid_index, &geosid)) {
            std::string message = std::string("The ");
            message += geosid->name;
            message += " geosid is under attack by ";
            message += name.characters;
            message += "!";
            context->chat_display.addGuildTextLine(false, "<Guild>", message);
            context->chat_log.addGuildText("<Guild>", message);
          }
        }
      } else if (message == NETMSG_GEOSID_TELLOWNERDATA) {
        kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
        size_t name_chars;
        Geosid::GeosidIndex geosid_index;
        Geosid::Geonite geonite;
        if (NetMsg_Geosid_TellOwnerData::readPacket(
              net_event.packet,
              &geosid_index,
              &CopiedArray<char>(name.characters, name.CAPACITY, &name_chars),
              &geonite)) {
          name.characters[name.CAPACITY] = '\0';
          name.characters[name_chars] = '\0';
          const Geosid::GeosidClientDescription* geosid;
          if (geosids_.get(geosid_index, &geosid)) {
            context->setGeosidData(geosid, name.characters, geonite);
          }
        }
      } else if (message == NETMSG_GEOSID_CHANGEOWNER) {
        kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
        size_t name_chars;
        Geosid::GeosidIndex geosid_index;
        Geosid::Geonite geonite;
        if (NetMsg_Geosid_ChangeOwner::readPacket(
              net_event.packet,
              &geosid_index,
              &CopiedArray<char>(name.characters, name.CAPACITY, &name_chars)),
              &geonite) {
          name.characters[name.CAPACITY] = '\0';
          name.characters[name_chars] = '\0';
          const Geosid::GeosidClientDescription* geosid;
          if (geosids_.get(geosid_index, &geosid)) {
            context->setGeosidData(geosid, name.characters, geonite);

            // Play the owner-change effect
            Binding::BindingPtr null_ptr;
            special_fx_manager_.generate(
              client_data_.special_fx[Evidyon::Client::CLIENT_SPECIALFX_GEOSID_CHANGEOWNER],
              this_frame_time,
              1.0f,
              geosid->x,
              geosid->z,
              null_ptr,
              geosid->x,
              geosid->z,
              NULL,
              0,
              1.0,
              NULL,
              false);
          }
        }
      } else if (message == NETMSG_GEOSID_TELLGEONITE) {
        Geosid::GeosidIndex geosid_index;
        Geosid::Geonite geonite;
        if (NetMsg_Geosid_TellGeonite::readPacket(net_event.packet, &geosid_index, &geonite)) {
          const Geosid::GeosidClientDescription* geosid;
          if (geosids_.get(geosid_index, &geosid)) {
            context->setGeosidGeonite(geosid, geonite);
          }
        }
      } else if (message == NETMSG_GEOSID_BURST) {
        assert(false);
        //float x, z;
        //if (NetMsg_Geosid_Burst::readPacket(
        //      net_event.packet,
        //      &x,
        //      &z)) {

        //  // Play the effect
        //  Binding::BindingPtr null_ptr;
        //  special_fx_manager_.generate(
        //    client_data_.special_fx[Evidyon::Client::CLIENT_SPECIALFX_GEONITE_BURST],
        //    this_frame_time,
        //    1.0f,
        //    x,
        //    z,
        //    null_ptr,
        //    x,
        //    z,
        //    NULL,
        //    0,
        //    1.0,
        //    NULL,
        //    false);
        //}
      } else if (message == NETMSG_GEOSID_BURSTATTACKGEOSID) {
        Geosid::GeosidIndex geosid_index;
        const Geosid::GeosidClientDescription* geosid;
        if (NetMsg_Geosid_BurstAttackGeosid::readPacket(net_event.packet, &geosid_index) &&
            geosids_.get(geosid_index, &geosid)) {

          // Play the effect
          Binding::BindingPtr null_ptr;
          special_fx_manager_.generate(
            client_data_.special_fx[Evidyon::Client::CLIENT_SPECIALFX_GEOSID_CHANGEOWNER],
            this_frame_time,
            1.0f,
            geosid->x,
            geosid->z,
            null_ptr,
            geosid->x,
            geosid->z,
            NULL,
            0,
            1.0,
            NULL,
            false);
        }
      } else if (message == NETMSG_YOUHAVEBEENRESURRECTED) {
        //dont have to decode; no parameters
        Evidyon::Actor::Client::ClientActorInstance*
          client_actor = actor_manager_.getClientActor();
        assert(client_actor);
        client_actor->setSpeech(true, "<resurrected: say '/respawn' or '/standup'>");
      } else if (message == NETMSG_GLOBAL_CHAT_DISTRIBUTE) {
        kg::CharBuffer<Avatar::MAX_AVATAR_NAME_LENGTH> name;
        kg::CharBuffer<256> chat;
        size_t name_chars, chat_chars;
        if (NetMsg_Global_Chat_Distribute::readPacket(net_event.packet,
              &CopiedArray<char>(name.characters, name.CAPACITY, &name_chars),
              &CopiedArray<char>(chat.characters, chat.CAPACITY, &chat_chars))) {
          name[name.CAPACITY] = '\0';
          name[name_chars] = '\0';
          chat[chat.CAPACITY] = '\0';
          chat[chat_chars] = '\0';
          context->chat_display.addGlobalTextLine(false, name.characters, chat.characters);
          context->chat_log.addGlobalText(name.characters, chat.characters);
        }
      }


      // Free packet memory
      enet_packet_destroy(net_event.packet);
    }

    Actor::Client::ClientActorInstance* client_actor
      = actor_manager_.getClientActor();

    const ActionInstance* action_instance;
    bool change_cursor_state = false;
    Action::Type type = context->gameplay_controls_.update(this_frame_time, &action_instance, &change_cursor_state);
    switch (type) {
      case Action::TYPE_INVALID: break;
      default: assert(false); break;
    //case Action::TYPE_NPCINTERACTION: {
    //    const OldAction_NPCInteraction::Parameters* params = &action_instance->params.npc_interaction;
    //    switch (params->interaction_type) {
    //    default: assert("Unknown NPC interaction type!");
    //    //case NPCINTERACTION_STORAGE: // open up the storage
    //    //  
    //    //break;
    //    case NPCINTERACTION_GEOSID: // geosid sacrifice items
    //      context->gameplay_controls_.showDialog(EvidyonGUIGameplayControls::DIALOG_GEOSID);
    //      break;
    //    //case NPCINTERACTION_BAZAAR: // open the bazaar
    //    //  context->gameplay_controls_.showDialog(EvidyonGUIGameplayControls::DIALOG_BAZAAR);
    //    //  enet_peer_send(enet_server_, 0, Evidyon::Network::Packet::NetMsg_Bazaar_Open::writePacket(ENET_PACKET_FLAG_RELIABLE, params->npc_actor_id));
    //    //  break;
    //    //case NPCINTERACTION_QUEST: // "trigger" type; TODO: make a unique packet for this...
    //    //  
    //    //  break;
    //    }
    //  } break;
    case Action::TYPE_MOVE: {
        const OldAction_Move::Parameters* params = &action_instance->params.move;
        ENetPacket* packet = NULL;
        using namespace Evidyon::Network::Packet::Client::Encode;
        if (params->streaming) {
          packet = requestMoveStreaming(params->x, params->y);
        } else {
          packet = requestMove(params->x, params->y, Actor::INVALID_ACTOR_ID);
        }

        context->gameplay_controls_.hideDialog(EvidyonGUIGameplayControls::DIALOG_ACTIVE_QUESTS);

        if (packet != NULL) enet_peer_send(enet_server_, 1, packet);
      } break;

    case Action::TYPE_ATTACK: {
        const OldAction_Attack::Parameters* params = &action_instance->params.attack;
        using namespace Evidyon::Network::Packet::Client::Encode;
        ENetPacket* packet = requestMove(params->x, params->y, params->actor_id);
        if (packet != NULL) enet_peer_send(enet_server_, 1, packet);
    } break;

    case Action::TYPE_SPELL: {
      const OldAction_CastSpell::Parameters* params = &action_instance->params.spell;
      int spell_type = action_instance->action.spell.spell_type;
      if (spell_type < Evidyon::Avatar::MAX_SPELL_TYPES &&
          myCharacter.spell_availability.isBitSet(spell_type)) {
        using namespace ::Evidyon::Network::Packet;
        sendPacket(NetMsg_CastSpell::writePacket(
          ENET_PACKET_FLAG_UNSEQUENCED,
          params->target_actor_id,
          spell_type,
          unsigned short(params->x * 32.0f),
          unsigned short(params->y * 32.0f),
          params->slot));
      }

    } break;

    case Action::TYPE_CONSUMABLE: {
      const OldAction_UseConsumable::Parameters* params = &action_instance->params.consumable;
      Evidyon::Actor::ActorID id = 0;
      if (params->needs_target) {
        actor_manager_.getActorClosestToLocation(
          true,  // find client
          false, // don't find dead
          params->x,
          params->y,
          &id);
      }
      sendPacket(NetMsg_UseItem::writePacket(ENET_PACKET_FLAG_UNSEQUENCED,
                                             id,
                                             params->inventory_index_hint,
                                             unsigned short(params->x * 32.0f),
                                             unsigned short(params->y * 32.0f),
                                             params->slot));
    } break;

    case Action::TYPE_ALTERNATEACTION: {
    } break;

    case Action::TYPE_USEREVENT: {
        switch (action_instance->action.user_event) {
        default:assert(!"Unknown user event type"); break;
        case STOP_ACTION: {
          Evidyon::Client::ActionQueue::add<Action_StopAction>();
          } break;
        case TOGGLE_NAME_DISPLAY: {
          Evidyon::Client::ActionQueue::add<Action_ToggleNameDisplay>();
          } break;
        case STORAGE_DEPOSIT_ITEM: {
          using namespace Evidyon::Network::Packet::Client;
          enet_peer_send(enet_server_, 0, Encode::storageDeposit(0, action_instance->params.user_event.data_.item_event_.inventory_index_));
          } break;
        case STORAGE_WITHDRAW_ITEM: {
          using namespace Evidyon::Network::Packet::Client;
          enet_peer_send(enet_server_, 0, Encode::storageWithdraw(0, action_instance->params.user_event.data_.item_event_.inventory_index_));
          } break;
        case EVENT_ESCAPE: {
            if (context->gameplay_controls_.hideDialogs() == false) {
              // log out of world
              myStateMachine.jumpState(VCS_LOGGED_INTO_ACCOUNT);
              enet_peer_send(enet_server_, 0, ::Evidyon::Network::Packet::Client::Encode::leaveWorld());
            }
          } break;
          case TOGGLE_CHAT_LOG: {
            Evidyon::Client::ActionQueue::add<Action_ToggleChatLog>();
          } break;

          case ATTRIBUTES_ADD_ABILITY_POINT: {
            using namespace Evidyon::Network::Packet::Client;
            enet_peer_send(enet_server_,
                           0,
                           Encode::askMyAvatarStats(action_instance->params.user_event.data_.add_ability_point_.ability));
          } break;

          case DROP_ITEM: {
            unsigned char itemIndex = action_instance->params.user_event.data_.item_event_.inventory_index_;
            enet_peer_send(enet_server_, 1, ::Evidyon::Network::Packet::Client::Encode::dropInventoryItems(&itemIndex, 1));
            myCharacter.inventory[itemIndex].quantity = 0;
            context->gameplay_controls_.syncInventory();
          } break;

          case DROP_CURRENCY: {
            using namespace Evidyon::Network::Packet;
            CurrencyType amount = action_instance->params.user_event.data_.drop_currency_.amount_;
            amount = amount > myCharacter.money ? myCharacter.money : amount; // drop most available
            sendPacket(NetMsg_Drop_Currency::writePacket(
              ENET_PACKET_FLAG_RELIABLE,
              amount));
          } break;

          case SAY_AGAIN: {
            if (context->last_spoken_text_line.empty()) break; // don't send nothing
            using namespace ::Evidyon::Network::Packet::Client;
            enet_peer_send(enet_server_,
                           1,
                           Encode::requestSpeak(action_instance->params.user_event.data_.speak_.text_));
          } break;

          case UNEQUIP_ITEM:
          case EQUIP_ITEM: {
            using namespace ::Evidyon::Network::Packet::Client;
            unsigned char itemIndices[128];
            size_t selectedItems = 1;
            itemIndices[0] = action_instance->params.user_event.data_.item_event_.inventory_index_;
            enet_peer_send(enet_server_,
                           1,
                           Encode::changeEquippedState(itemIndices, selectedItems));

            // Predict update
            int highest_index = ++myCharacter.highest_inventory_update_index_;
            for (int i = 0; i < selectedItems; ++i) {
              myCharacter.inventory_update_indices_[itemIndices[i]] = highest_index;
            }
            context->gameplay_controls_.syncInventory();

          } break;

          //case GEOSID_SACRIFICE_ITEM: {
          //  using namespace ::Evidyon::Network::Packet::Client;
          //  enet_peer_send(enet_server_,
          //                 1,
          //                 Encode::geosidSacrifice(action_instance->params.user_event.data_.item_event_.inventory_index_));
          //} break;

          case SPLIT_STACKED_ITEM: {
            using namespace ::Evidyon::Network::Packet;
            sendPacket(NetMsg_SplitStackedItem::writePacket(ENET_PACKET_FLAG_RELIABLE,
                                                            action_instance->params.user_event.data_.item_event_.inventory_index_));
          } break;

          case TRADE_ADDITEM: {
            ENetPacket* packet =
              Evidyon::Network::Packet::NetMsg_Trade_AddItem::writePacket
                <unsigned int>(
                ENET_PACKET_FLAG_RELIABLE,
                action_instance->params.user_event.data_.item_event_.inventory_index_);
            CONFIRM(packet) else break;
            sendPacket(packet);
          } break;

          case TRADE_SETCURRENCY: {
            CurrencyType currency = action_instance->params.user_event.data_.trade_set_currency_.currency_;
            if (currency > myCharacter.money) break; // don't bother if it's too much
            sendPacket(Evidyon::Network::Packet::NetMsg_Trade_SetCurrency::writePacket(
              ENET_PACKET_FLAG_RELIABLE,
              currency));
          } break;

          case TRADE_REMOVEITEM: {
            using namespace Evidyon::Network::Packet;
            ENetPacket* packet =
              NetMsg_Trade_RemoveItem::writePacket
                <unsigned int>(
                ENET_PACKET_FLAG_RELIABLE,
                action_instance->params.user_event.data_.item_event_.inventory_index_);
            CONFIRM(packet) else break;
            sendPacket(packet);
          } break;

          case TRADE_ACCEPT: {
            using namespace Evidyon::Network::Packet;
            ENetPacket* packet =
              NetMsg_Trade_Accept::writePacket(
                ENET_PACKET_FLAG_RELIABLE);
            CONFIRM(packet) else break;
            sendPacket(packet);
          } break;

          case TRADE_WITHDRAW: {
            using namespace Evidyon::Network::Packet;
            ENetPacket* packet =
              NetMsg_Trade_Withdraw::writePacket(
                ENET_PACKET_FLAG_RELIABLE);
            CONFIRM(packet) else break;
            sendPacket(packet);
          } break;

          case TRADE_CANCEL: {
            context->gameplay_controls_.hideDialogs();

            using namespace Evidyon::Network::Packet;
            ENetPacket* packet =
              NetMsg_Trade_Cancel::writePacket(
                ENET_PACKET_FLAG_RELIABLE);
            CONFIRM(packet) else break;
            sendPacket(packet);

          } break;

          case QUEST_OFFER_RESPONSE: {
            const UserEventQuestOfferResponse* response = 
              &action_instance->params.user_event.data_.quest_offer_response_;
            sendPacket(NetMsg_Quest_Offer_Response::writePacket(
              ENET_PACKET_FLAG_RELIABLE,
              response->quest_index,
              response->npc_actor_id,
              response->npc_quest_index,
              response->validation_key,
              response->accepted));
            context->gameplay_controls_.hideDialog(
              EvidyonGUIGameplayControls::DIALOG_QUEST_PROPOSAL);
          } break;

          case QUEST_GIVE_UP: {
            const UserEventQuestGiveUp* user_event =
              &action_instance->params.user_event.data_.quest_give_up_;
            sendPacket(NetMsg_Quest_GiveUp::writePacket(
              ENET_PACKET_FLAG_RELIABLE,
              user_event->client_quest_index
              ));
            context->gameplay_controls_.hideDialog(
              EvidyonGUIGameplayControls::DIALOG_ACTIVE_QUESTS);
          } break;

          case CLOSE_DIALOGS: {
            context->gameplay_controls_.hideDialogs();
          } break;
        }
      } break;
    }

    if (change_cursor_state) { // set by game controls::update()
      if (0 == context->gameplay_controls_.getCursorState()) {
        setMouseCursorNormal();
      } else {
        setMouseCursorActive();
      }
    }


    // Handle input from the different menu sources
    context->chat_display.update(this_frame_time);

    // Handle all user events
    context->action_queue.process(&context->action_processor);

    if (allowFrameToBeRendered(this_frame_time)) {
      double time_since_last_rendered_frame = this_frame_time - last_frame_time;
      last_frame_time = this_frame_time;

      {
        float x, z;
        const GUIRect* area = mouse_cursor_canvas_.getArea();
        camera_.getWorldLocation(
          area->left,
          area->top,
          &x,
          &z);
        Actor::ActorID actor_id;
        if (1.0f >= actor_manager_.getActorClosestToLocation(
              false,
              false,
              x,
              z,
              &actor_id)) {

          Actor::Client::ClientActorInstance* instance
            = actor_manager_.getActor(actor_id);
          Binding::BindingPtr actor = instance->getBinding();

          if (context->setMouseOverActor(actor)) {
            Actor::ActorInteraction
              interaction = actor_manager_.getActorInteraction(actor_id);
            Evidyon::Client::ClientSpecialFX client_special_fx;
            if (interaction == Actor::ACTORINTERACTION_ATTACK) {
              if (Actor::ActorAlignmentIsEvil(instance->getAlignment())) {
                client_special_fx
                  = Evidyon::Client::CLIENT_SPECIALFX_MOUSE_OVER_ATTACK_EVIL_ACTOR;
              } else {
                client_special_fx
                  = Evidyon::Client::CLIENT_SPECIALFX_MOUSE_OVER_ATTACK_GOOD_ACTOR;
              }
            } else {
              client_special_fx
                = Evidyon::Client::CLIENT_SPECIALFX_MOUSE_OVER_INTERACTIVE_ACTOR;
            }
            
            actor = context->mouse_over_actor_binding;
            special_fx_manager_.generate(
              client_data_.special_fx[client_special_fx],
              this_frame_time,
              1.0f,
              x,
              z,
              actor,
              x,
              z,
              &actor,
              1,
              -1.0,
              NULL,
              false);
          }
        } else {
          context->clearMouseOverActor();
        }
      }

      context->updateGeosidVisibility();
      

      class ActorSpeaksFunctionoid
        : public Evidyon::Actor::Client::ClientActorManager::ActorSpeaksFunctionoid {
      public:
        virtual void say(const char* name,
                         Actor::ActorAlignment alignment,
                         const std::string& text) {
          context->chat_log.addChatText(
            alignment,
            name,
            text);
        }
        StateWorldMainContext* context;
      } actor_speaks;
      actor_speaks.context = context;

      event_manager_.update(this_frame_time, time_since_last_rendered_frame);
      actor_manager_.update(this_frame_time,
                            time_since_last_rendered_frame,
                            this,
                            &actor_speaks);
      visual_fx_renderer_.update(this_frame_time, time_since_last_rendered_frame);

      float client_x, client_z;
      client_actor->getPosition(&client_x, &client_z);
      map_renderer_.update(current_map_, int(client_x), int(client_z));

      if (current_map_->changeZone(client_x,
                                   client_z,
                                   context->current_zone,
                                   &context->current_zone)) {
        Zone::Zone* current_zone = &zones_[context->current_zone];
        context->zone_text = current_zone->name;
        context->zone_text_color = D3DCOLOR_ARGB(255,255,255,255);
        sound_manager_.loopBackgroundSound(current_zone->background_sound);
      }

      camera_.setPosition(client_x, client_z-0.35);
      camera_.initialize3DRendering(d3d_device_);

      light_manager_.beginScene(client_x, client_z);
      light_manager_.setNightAmbience();
      //light_manager_.setDaylightAmbience();

      sound_manager_.update(client_x, client_z);
      special_fx_manager_.update(this_frame_time, time_since_last_rendered_frame);

      d3d_device_->Clear(0,
                         NULL,
                         D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB(0,0,0),
                         1.0f,
                         0);

      if (beginScene()) {
        const float light_radius = 20.0f;

        // Render all of the objects in the world
        size_t number_of_textures = renderstate_manager_.numberOfTextures();
        for (Texture::TextureIndex texture = 0;
                                   texture < number_of_textures;
                                 ++texture) {
          map_renderer_.render(texture, &renderstate_manager_, client_x, client_z, light_radius);
          visual_fx_renderer_.render(texture);
          scenery_renderer_.render(texture, &renderstate_manager_);
          skinned_mesh_renderer_.render(texture, &renderstate_manager_);
        }

        renderstate_manager_.resetState();

        // Draw the GUI elements
        CONFIRM(SUCCEEDED(d3d_device_->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0)));
        gui_primary_layer_.reinitializeD3DDeviceState();

        // Switch to the block font texture
        renderstate_manager_.setStage0Texture(context->speech_font.getTexture());

        if (context->draw_names) {
          class ActorDrawNameTextFunctionoid
            : public Evidyon::Actor::Client::ClientActorManager::ActorDrawNameTextFunctionoid {
          public:
            virtual void draw(float actor_x,
                              float actor_z,
                              Actor::ActorAlignment alignment,
                              const char* name,
                              const char* guild) {
              float x, y;
              camera->getScreenLocation(
                actor_x,
                0.35f,    // position on feet
                actor_z,
                &x,
                &y);
              GUIRect area = { (short)x, (short)y,
                               (short)x, (short)y };
              GUIPoint pt;
              GUISize name_size
                = name_font->centerTextInArea(name,
                                                -1,
                                                &area,
                                                &pt);
              D3DCOLOR text_color
                = fade_mask | (0x00FFFFFF & Evidyon::Actor::ActorAlignmentColor(alignment));
              name_font->renderText(gui_layer,
                                      NULL,  // no clipping rectangle
                                      pt.x,
                                      pt.y,
                                      1.0f, // depth
                                      name,
                                      -1,   // null-terminated
                                      text_color);
              if (guild && *guild) {
                area.top += name_size.height + 5;
                area.bottom = area.top;
                name_font->centerTextInArea(guild,
                                                -1,
                                                &area,
                                                &pt);
                name_font->renderText(gui_layer,
                                       NULL,
                                       pt.x,
                                       pt.y,
                                       1.0f, // depth
                                       guild,
                                       -1, // null-terminated
                                       text_color);
              }
            }
            IsometricCamera* camera;
            GUIFont* name_font;
            GUILayer* gui_layer;
            D3DCOLOR fade_mask;
          } functionoid;

          static const int TGT_ALPHA = 200;
          int target = context->display_names ? TGT_ALPHA : 0;
          int source = TGT_ALPHA - target;
          double time_until_faded = context->display_names_time - this_frame_time;
          if (time_until_faded >= 0.0) {
            functionoid.fade_mask
              = unsigned int((1.0-time_until_faded) * (target - source) + source) << 24;
          } else {
            functionoid.fade_mask = unsigned int(target) << 24;
            context->draw_names = context->display_names;
          }

          functionoid.gui_layer = &gui_primary_layer_;
          functionoid.name_font = &context->speech_font;
          functionoid.camera = &camera_;
          actor_manager_.drawNames(&functionoid);

          // Draw all of the geosid names
          for (StateWorldMainContext::Geosids::iterator i = context->geosids.begin();
                i != context->geosids.end(); ++i) {
            const Geosid::GeosidClientDescription* description
              = i->first;
            functionoid.draw(description->x,
                             description->z,
                             Actor::ALIGNMENT_ADMIN,
                             description->name,
                             i->second.description.c_str());
          }

          gui_primary_layer_.flush();
        }

        // Draw the text indicating the user needs to type a target
        if (context->gameplay_controls_.isReadingSlot()) {
          context->speech_font.renderText(
            &gui_primary_layer_,
            NULL,
            0.0f,
            0.0f,
            0.1f,
            "Enter a portal destination (0-9)",
            -1,
            D3DCOLOR_XRGB(255,255,255));
        }

        {
          class ActorDrawSpeechTextFunctionoid
            : public Evidyon::Actor::Client::ClientActorManager::ActorDrawSpeechTextFunctionoid {
          public:
            virtual void draw(float actor_x,
                              float actor_z,
                              float actor_height,
                              Actor::ActorAlignment alignment,
                              const std::string& last_line,
                              float last_line_fade,
                              const std::string& current_line,
                              float current_line_fade,
                              float line_offset) {
              float x, y;
              camera->getScreenLocation(
                actor_x,
                actor_height,
                actor_z,
                &x,
                &y);
              GUIRect area = { (short)x, (short)y,
                               (short)x, (short)y };
              GUIPoint pt;
              D3DCOLOR line_color;
              float line_height_pixels = speech_font->getLineHeightInPixels();
              float line_offset_pixels = line_height_pixels * line_offset;

              line_color
                = ((unsigned int)(255.0 * last_line_fade) & 0xFF) << 24;
              line_color |= 0x00FFFFFF & Evidyon::Actor::ActorAlignmentColor(alignment);

              speech_font->centerTextInArea(last_line.c_str(),
                                            last_line.length(),
                                            &area,
                                            &pt);
              pt.y = y;
              speech_font->renderText(gui_layer,
                                      NULL,  // no clipping rectangle
                                      pt.x,
                                      pt.y + line_offset_pixels,
                                      1.0f, // depth
                                      last_line.c_str(),
                                      last_line.length(),
                                      line_color);
              line_color
                = ((unsigned int)(255.0 * current_line_fade) & 0xFF) << 24;
              line_color |= 0x00FFFFFF & Evidyon::Actor::ActorAlignmentColor(alignment);
              speech_font->centerTextInArea(current_line.c_str(),
                                            current_line.length(),
                                            &area,
                                            &pt);
              pt.y = y;
              speech_font->renderText(gui_layer,
                                      NULL,  // no clipping rectangle
                                      pt.x,
                                      pt.y + line_offset_pixels + line_height_pixels,
                                      1.0f, // depth
                                      current_line.c_str(),
                                      current_line.length(),
                                      line_color);
            }
            IsometricCamera* camera;
            GUIFont* speech_font;
            GUILayer* gui_layer;
          } draw_speech;

          draw_speech.gui_layer = &gui_primary_layer_;
          draw_speech.speech_font = &context->speech_font;
          draw_speech.camera = &camera_;
          actor_manager_.drawSpeech(&draw_speech);
          gui_primary_layer_.flush();
        }
        if (context->zone_text) { // Draw the zone name
          GUISize size;
          context->speech_font.getTextExtent(context->zone_text, -1, &size);
          const GUIRect* screen = gui_primary_layer_.getPrimaryCanvas()->getArea();
          context->speech_font.renderText(&gui_primary_layer_,
                                          NULL,
                                          screen->right/2 - size.width/2,
                                          10,
                                          0.9f,
                                          context->zone_text,
                                          -1,
                                          context->zone_text_color);
          D3DCOLOR alpha = context->zone_text_color >> 24;
          if (alpha == 0) {
            context->zone_text = NULL;
          } else {
            context->zone_text_color
              = ((--alpha) << 24) | (context->zone_text_color & 0x00FFFFFF);
          }
        }

        CONFIRM(gui_primary_layer_.render());

        // this is ugly, but it has to be done here because it is the only place the back-buffer has the
        // correct image data in it.
        bool end_scene_succeeded = false;
        if (context->take_screenshot) {
          context->take_screenshot = false;
          char file[MAX_PATH];
          end_scene_succeeded = endSceneScreenshot(VCS_WORLD_MAIN, file);
          std::string message = "Screenshot saved as \"";
          message.append(file);
          message.append("\"");
          context->chat_display.addSystemText(message);
        } else {
          end_scene_succeeded = endScene(VCS_WORLD_MAIN);
        }
        if (!end_scene_succeeded) {
          using namespace Evidyon::Network::Packet::Client::Encode;
          enet_peer_send(enet_server_, 0, leaveWorld());
        }
      }
    }
  }

  SetCursor(LoadCursor(NULL, IDC_ARROW));
  ShowCursor(TRUE);


  // Free class-based information used in this method
  event_manager_.clearEventInstances();
  map_renderer_.reset();
  actor_manager_.clear(true);
  special_fx_manager_.eraseTimedFX();
  scenery_renderer_.clearSceneryInstances();
  visual_fx_renderer_.clearVisualFXInstances();
  renderstate_manager_.resetState();

  context->destroy(&gui_primary_layer_);

  // turn off background sound
  sound_manager_.loopBackgroundSound(Sound::INVALID_SOUND_INDEX);

  // Make sure the mouse goes back to its regular state
  setMouseCursorNormal();

  // get rid of all of the items on the map
  myItemsOnMapTable.clear();

  map_renderer_.reset();
  scenery_renderer_.clearSceneryInstances();

  SAFE_DELETE(context);
}






//----[  StateWorldMainContext  ]----------------------------------------------
StateWorldMainContext::StateWorldMainContext(EvidyonClient* client)
  : font(EvidyonGUIDefs::WINDOW_FONT_TEXTURE_INDEX),
    speech_font(EvidyonGUIDefs::TITLE_FONT_TEXTURE_INDEX),
    action_processor(client, this)
    {
  take_screenshot = false;
  client_ = client;
  draw_names = false;
  display_names = false;
  memset(storage, 0, sizeof(storage));
  last_fast_sync_stat = 0;
  last_spoken_text_line = ""; // for say again
  current_zone = Zone::INVALID_ZONE_INDEX;
  zone_text = NULL;
  zone_text_color = 0;
}



//----[  updateGeosidVisibility  ]---------------------------------------------
void StateWorldMainContext::updateGeosidVisibility() {
  for (Geosids::iterator i = geosids.begin(); i != geosids.end(); ++i) {
    const Geosid::GeosidClientDescription* description = i->first;
    if (!Client::ClientFacade::singleton()->isVisible(
          description->map,
          description->x,
          description->z)) {
      geosids.erase(i);
      break; // any others will be removed next loop
    }
  }

  const ::GeosidArray* client_geosids = client_->getGeosids();
  for (size_t i = 0; i < client_geosids->count; ++i) {
    const Geosid::GeosidClientDescription* description
      = &client_geosids->elements[i];
    if (Client::ClientFacade::singleton()->isVisible(
          description->map,
          description->x,
          description->z) &&
        (geosids.end() == geosids.find(description))) {
      GeosidData data;
      data.description = "";
      data.guild = "";
      data.geonite = 99;
      geosids.insert(Geosids::value_type(description, data));
      Client::ActionQueue::add<Action_Geosid_GetOwner>()->geosid
        = description->index;
    }
  }
}

//----[  create  ]-------------------------------------------------------------
bool StateWorldMainContext::create(
    dcxIniFileReader* config,
    LPDIRECT3DDEVICE9 d3d_device,
    LPDIRECT3DTEXTURE9 d3d_texture_font,
    GUILayerBasic* primary_layer,
    GUICanvas* mouse_cursor_canvas,
    GUIFont* window_font,
    GUIMouseSentinel* primary_mouse_sentinel) {

  CONFIRM(font.create(d3d_device, config->getSetting("Media","FontName","Tahoma"),
                                  config->getSetting("Media","FontSize",10), true)) else return false;
  CONFIRM(speech_font.create(d3d_device, config->getSetting("Media","SpeechFontName","Tahoma"),
                                         config->getSetting("Media","SpeechFontSize",10), true)) else return false;
  gui_primary_layer = primary_layer;
  //{ // Init the speech font from the bitmap
  //  DWORD character_indices[128-32];
  //  character_indices[0] = 8*11+6; // ' '
  //  for (int i = 33; i < 128; ++i) {
  //    character_indices[i-32] = i-33;
  //  }
  //  CONFIRM(speech_font.create(d3d_texture_font,
  //                            16,
  //                            16,
  //                            character_indices,
  //                            1.2f,
  //                            0)) else return false;
  //}
  tooltip_box.create(primary_layer, 40, mouse_cursor_canvas);
  tooltip_box.becomeOwner(this);
  tooltip_box.update(dcxWin32Clock::getAccurateSystemTime());

  primary_layer->setTexture(font.getTextureID(),
                            font.getTexture());
  primary_layer->setTexture(speech_font.getTextureID(),
                            speech_font.getTexture());
  {
    GUISize sz = { 180, 85 };
    tooltip_box.setContentSize(sz);
  }

  GUISize primary_layer_size
    = primary_layer->getPrimaryCanvas()->getArea()->calculateSize();
  status_bars.init(primary_layer_size.width,
                   primary_layer_size.height);
  status_bars.registerElements(primary_layer);

  enchantment_icons.init(primary_layer->getPrimaryCanvas(),
                         status_bars.getSize());
  enchantment_icons.registerElements(primary_layer);

  chat_display.create(&font, primary_layer->getPrimaryCanvas());
  chat_display.show(primary_layer);

  chat_log.create(primary_layer->getPrimaryCanvas(), &font);
  chat_log.setParentSentinels(primary_mouse_sentinel, NULL);

  return true;
}



//----[  destroy  ]------------------------------------------------------------
void StateWorldMainContext::destroy(GUILayerBasic* primary_layer) {
  chat_log.destroy();
  chat_display.destroy();
  gameplay_controls_.destroy();
  status_bars.unregisterElements(primary_layer);
  enchantment_icons.unregisterElements(primary_layer);
  primary_layer->removeTexture(font.getTextureID());
  primary_layer->removeTexture(speech_font.getTextureID());
  speech_font.destroy();
  font.destroy();
}



















ActionProcessor::ActionProcessor(EvidyonClient* client, StateWorldMainContext* context)
  : client_(client),
    context_(context) {
}

void ActionProcessor::process(const Action_CastSpell* action) {
}

void ActionProcessor::process(const Action_UseConsumable* action) {
}

void ActionProcessor::process(const Action_Move* action) {
}

void ActionProcessor::process(const Action_ActorInteraction* action) {
}

void ActionProcessor::process(const Action_Attack* action) {
  using namespace Evidyon::Network::Packet;
  using namespace Evidyon::Network::Packet::Client::Encode;
  float targeted_y
    = this
      ->client_
      ->getClientActor()
      ->renderable_instance
      ->current_movement_state
       .vertical
     + action->y;
  NetworkPacket* packet = requestMove(action->x, targeted_y, action->actor_id);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_NPCInteraction* action) {
}

void ActionProcessor::process(const Action_LogOutOfWorld* action) {
}

void ActionProcessor::process(const Action_ToggleViewAngle* action) {
  client_->getCamera()->enableTopdownView(
    client_->getCamera()->topdownViewEnabled() == false);
}

void ActionProcessor::process(const Action_ToggleShowNames* action) {
}

void ActionProcessor::process(const Action_SayAgain* action) {
}

void ActionProcessor::process(const Action_StopAction* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(
    NetMsg_StopAction::writePacket(ENET_PACKET_FLAG_UNSEQUENCED));
}

void ActionProcessor::process(const Action_OpenInventory* action) {
  context_->gameplay_controls_.toggleDialog(
    EvidyonGUIGameplayControls::DIALOG_INVENTORY);
}

void ActionProcessor::process(const Action_OpenAttributes* action) {
  context_->gameplay_controls_.toggleDialog(
    EvidyonGUIGameplayControls::DIALOG_STAT_DISPLAY);
}

void ActionProcessor::process(const Action_OpenKeyBindings* action) {
  assert(false);
}

void ActionProcessor::process(const Action_OpenChatAndActions* action) {
  assert(false);
}

void ActionProcessor::process(const Action_OpenStatDisplay* action) {
  context_->gameplay_controls_.toggleDialog(
    EvidyonGUIGameplayControls::DIALOG_STAT_DISPLAY);
}

void ActionProcessor::process(const Action_OpenChat* action) {
  context_->gameplay_controls_.toggleDialog(
    EvidyonGUIGameplayControls::DIALOG_CHAT);
}

void ActionProcessor::process(const Action_OpenDrop* action) {
  bool is_white = false;
  if (Client::ClientFacade::singleton()->isClientInsideGeosid(NULL, &is_white) && is_white) {
    context_->gameplay_controls_.toggleDialog(
      EvidyonGUIGameplayControls::DIALOG_DROP_SACRIFICE_ITEMS);
  } else {
    context_->gameplay_controls_.toggleDialog(
      EvidyonGUIGameplayControls::DIALOG_DROP_ITEMS);
  }
}

void ActionProcessor::process(const Action_BazaarOpen* action) {
  using namespace Evidyon::Network::Packet;
  context_->gameplay_controls_.showDialog(EvidyonGUIGameplayControls::DIALOG_BAZAAR);
  client_->sendPacket(NetMsg_Bazaar_Open::writePacket(ENET_PACKET_FLAG_RELIABLE, action->npc_actor_id));
}

void ActionProcessor::process(const Action_ToggleChatLog* action) {
  if (context_->chat_log.isActive()) {
    context_->chat_log.hide();
    context_->chat_display.show(context_->gui_primary_layer);
  } else {
    context_->chat_display.hide();
    context_->chat_log.show(context_->gui_primary_layer);
  }
}

void ActionProcessor::process(const Action_CloseChatAndActions* action) {
}

void ActionProcessor::process(const Action_CloseInventory* action) {
  context_->gameplay_controls_.hideDialog(EvidyonGUIGameplayControls::DIALOG_INVENTORY);
}

void ActionProcessor::process(const Action_CloseKeyBindings* action) {
  assert(false);
}

void ActionProcessor::process(const Action_CloseStatDisplay* action) {
  context_->gameplay_controls_.hideDialog(EvidyonGUIGameplayControls::DIALOG_STAT_DISPLAY);
}

void ActionProcessor::process(const Action_CloseChat* action) {
  context_->gameplay_controls_.hideDialog(EvidyonGUIGameplayControls::DIALOG_CHAT);
}

void ActionProcessor::process(const Action_CloseDrop* action) {
  context_->gameplay_controls_.hideDialog(EvidyonGUIGameplayControls::DIALOG_DROP_ITEMS);
}

void ActionProcessor::process(const Action_CloseBazaar* action) {
  context_->gameplay_controls_.hideDialog(EvidyonGUIGameplayControls::DIALOG_BAZAAR);
}

void ActionProcessor::process(const Action_AttributesAddAbilityPoint* action) {
}

void ActionProcessor::process(const Action_Speak* action) {
  using namespace ::Evidyon::Network::Packet;
  NetworkPacket* packet = 
    NetMsg_Request_Speak::writePacket(
      ENET_PACKET_FLAG_RELIABLE,
      &CopiedArray<char>(action->text.c_str(),
                         action->text.length()));
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_DropItem* action) {
}

void ActionProcessor::process(const Action_DropCurrency* action) {
}

void ActionProcessor::process(const Action_EquipItem* action) {
}

void ActionProcessor::process(const Action_UnequipItem* action) {
}

void ActionProcessor::process(const Action_BazaarCreateListing* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Bazaar_CreateListing::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    action->inventory_index,
    action->listing_price));
}

void ActionProcessor::process(const Action_BazaarRetractListing* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Bazaar_RetractListing::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    action->listing_id));
}

void ActionProcessor::process(const Action_BazaarPurchaseListing* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Bazaar_PurchaseListing::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    action->listing_id));
}

void ActionProcessor::process(const Action_BazaarRedeemListing* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Bazaar_RedeemListing::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    action->listing_id));
}

void ActionProcessor::process(const Action_BazaarGetListings* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Bazaar_GetListings::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    action->category));
}

void ActionProcessor::process(const Action_TakeScreenshot* action) {
  context_->take_screenshot = true;
}

void ActionProcessor::process(const Action_OpenCast* action) {
  context_->gameplay_controls_.toggleDialog(EvidyonGUIGameplayControls::DIALOG_CAST_SPELL);
}

void ActionProcessor::process(const Action_OpenUseItem* action) {
  context_->gameplay_controls_.toggleDialog(EvidyonGUIGameplayControls::DIALOG_USE_ITEM);
}

void ActionProcessor::process(const Action_EventEscape* action) {
}

void ActionProcessor::process(const Action_StorageOpen* action) {
  context_->gameplay_controls_.showDialog(EvidyonGUIGameplayControls::DIALOG_STORAGE);
  using namespace Evidyon::Network::Packet::Client;
  client_->sendPacket(Encode::storageOpen(action->npc_actor_id));
}

void ActionProcessor::process(const Action_StorageWithdrawItem* action) {
}

void ActionProcessor::process(const Action_StorageDepositItem* action) {
}


void ActionProcessor::process(const Action_Geosid_SacrificeItem* action) {
  using namespace Evidyon::Network::Packet;
  Geosid::GeosidIndex geosid;
  bool is_white = false;
  if (!Evidyon::Client::ClientFacade::singleton()->isClientInsideGeosid(&geosid, &is_white)
        || !is_white) {
    return;
  }
  client_->sendPacket(NetMsg_Geosid_DropSacrifice::writePacket(
    ENET_PACKET_FLAG_RELIABLE, geosid, action->inventory_index));
}

void ActionProcessor::process(const Action_Geosid_Burst* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Geosid_Burst::writePacket(
    ENET_PACKET_FLAG_RELIABLE, action->x, action->z));
}

void ActionProcessor::process(const Action_Geosid_BurstAttackGeosid* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Geosid_BurstAttackGeosid::writePacket(
    ENET_PACKET_FLAG_RELIABLE, action->geosid));
}

void ActionProcessor::process(const Action_Geosid_BurstSpell* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Geosid_BurstSpell::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    action->spell_type,
    action->target_actor_id,
    action->x,
    action->y));
}

void ActionProcessor::process(const Action_Geosid_BurstUseGeosid* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Geosid_BurstUseGeosid::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    action->geosid,
    action->actor_id,
    action->x,
    action->z));
}


void ActionProcessor::process(const Action_Geosid_GetOwner* action) {
  using namespace Evidyon::Network::Packet;
  client_->sendPacket(NetMsg_Geosid_GetOwner::writePacket(
    ENET_PACKET_FLAG_RELIABLE,
    action->geosid));
}


void ActionProcessor::process(const Action_TradeAddItem* action) {
}

void ActionProcessor::process(const Action_TradeSetCurrency* action) {
}

void ActionProcessor::process(const Action_TradeRemoveItem* action) {
}

void ActionProcessor::process(const Action_TradeAccept* action) {
}

void ActionProcessor::process(const Action_TradeWithdraw* action) {
}

void ActionProcessor::process(const Action_TradeCancel* action) {
}

void ActionProcessor::process(const Action_SplitStackedItem* action) {
}

void ActionProcessor::process(const Action_QuestOfferResponse* action) {
}

void ActionProcessor::process(const Action_QuestGiveUp* action) {
}

void ActionProcessor::process(const Action_CloseDialogs* action) {
  context_->gameplay_controls_.hideDialogs();
}

void ActionProcessor::process(const Action_ToggleNameDisplay* action) {
  context_->display_names = !context_->display_names;
  context_->display_names_time = context_->this_frame_time + 1.5;
  context_->draw_names = true; // this makes the fader work in either state
}



void ActionProcessor::process(const Action_GuildCreate* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Create::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        &CopiedArray<char>(action->guild_name.c_str(),
                           action->guild_name.length()));
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildLead* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Lead::writePacket(ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildJoin* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Join::writePacket(ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildLeave* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Leave::writePacket(ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildPromote* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Promote::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        &CopiedArray<char>(action->name.c_str(), action->name.length()));
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildDemote* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Demote::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        &CopiedArray<char>(action->name.c_str(), action->name.length()));
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildChat* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Chat::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        &CopiedArray<char>(action->text.c_str(), action->text.length()));
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildKick* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Kick::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        &CopiedArray<char>(action->name.c_str(), action->name.length()));
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildMembers* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Members::writePacket(ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildGeosids* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_GuildAskGeosids::writePacket(ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildSetHome* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_SetHome::writePacket(ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildHome* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Home::writePacket(ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_GuildAbandon* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Guild_Abandon::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        action->geosid);
  client_->sendPacket(packet);
}


void ActionProcessor::process(const Action_Party_Chat* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Party_Chat::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        &CopiedArray<char>(action->text.c_str(), action->text.length()));
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_Party_Lead* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Party_Lead::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_Party_Join* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Party_Join::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_Party_Leave* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Party_Leave::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_Party_Kick* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Party_Kick::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        &CopiedArray<char>(action->name.c_str(), action->name.length()));
  client_->sendPacket(packet);
}


void ActionProcessor::process(const Action_Party_Close* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Party_Close::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}


void ActionProcessor::process(const Action_Resurrect_StandUp* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Resurrect_StandUp::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_Resurrect_Respawn* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Resurrect_Respawn::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}


void ActionProcessor::process(const Action_Show_Help* action) {
  context_->gameplay_controls_.toggleDialog(
    EvidyonGUIGameplayControls::DIALOG_HELP);
}

void ActionProcessor::process(const Action_Mark* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Mark::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED,
        action->mark_index);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_AutoTarget_All* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_AutoTarget_All::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}

void ActionProcessor::process(const Action_AutoTarget_Monsters* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_AutoTarget_Monsters::writePacket(
        ENET_PACKET_FLAG_UNSEQUENCED);
  client_->sendPacket(packet);
}




void ActionProcessor::process(const Action_NPCTrigger* action) {
  using namespace Evidyon::Network::Packet;
  NetworkPacket* packet
    = NetMsg_Quest_Trigger::writePacket(ENET_PACKET_FLAG_RELIABLE,
        action->npc_actor_id,
        0.0f,
        0.0f);
  client_->sendPacket(packet);
}