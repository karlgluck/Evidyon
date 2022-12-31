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
#include "evidyonguidefs.h"




//----[  CharacterLoginInfo  ]-------------------------------------------------
struct CharacterLoginInfo {
  unsigned int id;
  char name[Avatar::MAX_AVATAR_NAME_LENGTH + 50];
  int map_index;
  float world_x, world_y;
  unsigned int actor_type;
  size_t visible_equipment[ACTORATTACHMENTPOINT_COUNT];
};





//----[  EvidyonGUICharacterList  ]--------------------------------------------
class EvidyonGUICharacterList : public EvidyonGUIList {

class EvidyonCharacterListEntry : public EvidyonGUIList::EvidyonListEntryWithMouseInput {

  public:

      EvidyonCharacterListEntry();
      void setOwner(EvidyonGUICharacterList* owner) { owner_ = owner; }
      void setTextFont(GUIFont* font);
      bool getSelected() const;
      void setText(const char* text) { myText.setText(text); myText.setTextColor(EvidyonGUIDefs::CONTENT_COMPONENT_TEXT_COLOR); }
      void setID(unsigned int id) { id_ = id; }
      unsigned int getID() const { return id_; }
      void setSelected(bool selected) {
          selected_ = selected;
          //myBackground.setType(selected ? EvidyonGUIListEntryBackground::DARK : EvidyonGUIListEntryBackground::LIGHT);
          myText.setTextColor(EvidyonGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
      }
      const std::string& getText() { return myText.getText(); }
  public:

      virtual void registerElements(GUILayer* layer);
      virtual void unregisterElements(GUILayer* layer);
      virtual void refresh();


  protected:

      virtual void onMouseAction(MouseAction action);

  protected:

      EvidyonGUICharacterList* owner_;

      /// Whether or not this element is selected
      bool selected_;

      /// This character's ID number
      unsigned int id_;

      /// The background GUI elmeent
      //EvidyonGUIListEntryBackground myBackground;

      /// The text shown in the line
      EvidyonGUIListEntryText myText;
};

public:

  void create(const CharacterLoginInfo character_info[MAX_AVATARS_PER_ACCOUNT], unsigned int characters,
               GUIFont* font, GUILayer* layer, GUIMouseInputSource* input) {
      setCharacters(character_info, characters);
      setTextFont(font);
      registerElements(layer);

      selection_changed_ = false;

      background_canvas_.setSourceCanvas(layer->getPrimaryCanvas());
      background_canvas_.absoluteWidth(400);
      background_canvas_.absoluteHeight(150);
      background_canvas_.alignXCenter(0);
      background_canvas_.alignYCenter(+135);
      background_canvas_.absoluteDepth(0.5f);

      myTargetCanvas.setSourceCanvas(&background_canvas_);
      myTargetCanvas.relativeWidth(-16);
      myTargetCanvas.relativeHeight(-16);
      myTargetCanvas.alignXCenter(0);
      myTargetCanvas.alignYCenter(0);
      myTargetCanvas.relativeDepth(-0.1f);

      background_.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
      background_.setSourceCanvas(&background_canvas_);
      background_.setSourceRegion(&EvidyonGUIDefs::CONTENT_COMPONENT_BACKGROUND,4, EvidyonGUIDefs::TEXTURE_SIZE);
      layer->registerElement(&background_);

      setLineHeight(26);

      GUIList::getMouseSentinel()->setParentMouseSentinel(input->getPrimarySentinel());

      // Refresh this list
      refresh();
  }

  void destroy(GUILayer* layer) {
      unregisterElements(layer);
      layer->unregisterElement(&background_);
  }

  void setTextFont(GUIFont* font) {
      for (unsigned int i = 0; i < MAX_AVATARS_PER_ACCOUNT; ++i) {
          myCharacters[i].setTextFont(font);
      }
  }

  /**
   * Invoked by the mouse sentinel when the mouse leaves or enters the
   * list.
   */
  //virtual void onMouseCrossedListBorder(bool inside);

  void setCharacters(const CharacterLoginInfo* characters, unsigned int number) {
      number_of_characters_ = number;
      for (unsigned int i = 0; i < number; ++i) {
          myCharacters[i].setOwner(this);
          myCharacters[i].setID(characters[i].id);
          myCharacters[i].setText(characters[i].name);
      }
  }

  unsigned int getSelection() {
      for (unsigned int i = 0; i < number_of_characters_; ++i) {
          if (myCharacters[i].getSelected()) return i;
      }
      return 0xFFFFFFFF;
  }

  const std::string getSelectionText() {
      for (unsigned int i = 0; i < number_of_characters_; ++i) {
          if (myCharacters[i].getSelected()) return myCharacters[i].getText();
      }
      return "";
  }

  void setSelection(unsigned int selection) {
      selection_changed_ = true;
      for (unsigned int i = 0; i < number_of_characters_; ++i) {
          myCharacters[i].setSelected(i == selection);
      }
  }

  bool selectionChanged() {
      bool return_value = selection_changed_;
      selection_changed_ = false;
      return return_value;
  }

  GUICanvas* getCanvas() { return &myTargetCanvas; }

protected:

  void setSelection(EvidyonCharacterListEntry* this_pointer) {
      setSelection((this_pointer >= myCharacters &&
                     this_pointer < myCharacters + MAX_AVATARS_PER_ACCOUNT) ?
                      this_pointer - myCharacters : 0xFFFFFFFF);
  }

protected:

  //EvidyonGUIScrollableButtonList buttons_;

  GUICanvas background_canvas_;
  GUIBorderedFillElement background_;
  GUIList::ListEntry* getListEntry(unsigned int index) { return index < number_of_characters_ ? &myCharacters[index] : NULL; }
  unsigned int getNumberOfLines() const                  { return number_of_characters_; }

  bool selection_changed_;
  unsigned int number_of_characters_;
  EvidyonCharacterListEntry myCharacters[MAX_AVATARS_PER_ACCOUNT];
};


//-----------------------------------------------------------------------------
// Name: ItemListEntry
// Desc: Initializes this class
//-----------------------------------------------------------------------------
EvidyonGUICharacterList::EvidyonCharacterListEntry::EvidyonCharacterListEntry()
: /*myBackground(this),*/ myText(this)
{
    myText.setTextColor(EvidyonGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
    selected_ = false;
}



//-----------------------------------------------------------------------------
// Name: setTextFont
// Desc: Sets the font used to display the text in this list entry
//-----------------------------------------------------------------------------
void EvidyonGUICharacterList::EvidyonCharacterListEntry::setTextFont(GUIFont* font)
{
    myText.setFont(font);
}



//-----------------------------------------------------------------------------
bool EvidyonGUICharacterList::EvidyonCharacterListEntry::getSelected() const {
  return selected_;
}


//-----------------------------------------------------------------------------
void EvidyonGUICharacterList::EvidyonCharacterListEntry::registerElements(GUILayer* layer)
{
    myText.registerElements(layer);
    //myBackground.registerElements(layer);
}


//-----------------------------------------------------------------------------
void EvidyonGUICharacterList::EvidyonCharacterListEntry::unregisterElements(GUILayer* layer)
{
    myText.unregisterElements(layer);
    //myBackground.unregisterElements(layer);
}



//-----------------------------------------------------------------------------
// Name: setClippingArea
// Desc: Sets the clipping region of the internal elements
//-----------------------------------------------------------------------------
void EvidyonGUICharacterList::EvidyonCharacterListEntry::refresh()
{
    myText.setClippingArea(getClippingArea());
    //myText.getCanvas()->realign();
    //myBackground.setClippingArea(getClippingArea());
    //myBackground.getCanvas()->realign();
}


void EvidyonGUICharacterList::EvidyonCharacterListEntry::onMouseAction(
    EvidyonGUIList::EvidyonListEntryWithMouseInput::MouseAction action) {
  if (action == CLICKED) {
    owner_->setSelection(this);
  }
}








//-----------------------------------------------------------------------------
// Name:  stateLoggedIntoAccount
// Desc:  
//-----------------------------------------------------------------------------
void EvidyonClient::stateLoggedIntoAccount()
{
using namespace ::Evidyon::Network::Packet;
using namespace ::Evidyon::Network::Packet::Client;
  //global_actor_manager_.reset();

  CharacterLoginInfo character_info[MAX_AVATARS_PER_ACCOUNT];


  // Account information
  float daysLeft;
  unsigned int characterIDs[MAX_AVATARS_PER_ACCOUNT];
  size_t numCharacterIDs;

  // Get a list of characters and other account information from the server
  bool gotAccountInfo = false;
  ENetEvent netEvent;
  enet_peer_send(enet_server_, 0, ::Evidyon::Network::Packet::Client::Encode::askAccountInfo());
  gotAccountInfo = false;
  unsigned long startTime = dcxWin32Clock::getSystemTime();
  while(dcxWin32Clock::getSystemTime() - startTime < 1000 &&
   !(gotAccountInfo = (enet_host_service(enet_client_, &netEvent, 1000) > 0 && netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0)));
  if (gotAccountInfo)
  {
      ::Evidyon::Network::Packet::Message msg = ::Evidyon::Network::Packet::NETMSG_INVALID;
      ::Evidyon::Network::Packet::decodeMessageType(netEvent.packet, &msg);
      gotAccountInfo = msg == ::Evidyon::Network::Packet::NETMSG_TELLACCOUNTINFO;
  }
  if (gotAccountInfo)
  {
      gotAccountInfo = !APP_WARNING(!Decode::tellAccountInfo(netEvent.packet, &daysLeft, characterIDs, &numCharacterIDs))
                          ("Failed to decode tellAccountInfo");
  }
  if (netEvent.packet) enet_packet_destroy(netEvent.packet);
  if (!gotAccountInfo)
  {
      myStateMachine.queueState(VCS_DISCONNECT);
      myStateMachine.queueState(VCS_NO_CONNECTION);
      return;
  }

  // Get each of the characters' names
  // TODO: use an internal database to look up character names! :D
  char characterNames[5][Avatar::MAX_AVATAR_NAME_LENGTH + 50]; // 8-26-08: character name extended...
  for (size_t i = 0; i < numCharacterIDs; ++i)
  {
      enet_peer_send(enet_server_, 0, ::Evidyon::Network::Packet::Client::Encode::askCharacterName(characterIDs[i]));
      unsigned long startTime = dcxWin32Clock::getSystemTime();
      bool gotCharacterName = false;
      while(dcxWin32Clock::getSystemTime() - startTime < 1000 &&
           !(gotCharacterName = (enet_host_service(enet_client_, &netEvent, 1000) > 0 && netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0)));
      if (gotCharacterName)
      {
          ::Evidyon::Network::Packet::Message msg = ::Evidyon::Network::Packet::NETMSG_INVALID;
          ::Evidyon::Network::Packet::decodeMessageType(netEvent.packet, &msg);
          gotAccountInfo = msg == ::Evidyon::Network::Packet::NETMSG_TELLCHARACTERNAME;
      }
      if (gotCharacterName)
      {
          unsigned int id;
          gotCharacterName = !APP_WARNING(!Decode::tellCharacterName(
              netEvent.packet, &id, characterNames[i], Avatar::MAX_AVATAR_NAME_LENGTH,
              &character_info[i].map_index,
              &character_info[i].world_x, &character_info[i].world_y,
              &character_info[i].actor_type, character_info[i].visible_equipment))
                              ("Failed to decode tellCharacterName");
          gotCharacterName = id == characterIDs[i];
      }

      if (netEvent.type == ENET_EVENT_TYPE_RECEIVE &&
          netEvent.packet != NULL) {
        enet_packet_destroy(netEvent.packet);
      }

      if (!gotCharacterName)
      {
          myStateMachine.queueState(VCS_DISCONNECT);
          myStateMachine.queueState(VCS_NO_CONNECTION);
          return;
      }
  }
  for (size_t i = numCharacterIDs; i < MAX_AVATARS_PER_ACCOUNT; ++i) {
    characterNames[i][0] = '\0';
  }

  for (size_t i = 0; i < MAX_AVATARS_PER_ACCOUNT; ++i)
  {
      strcpy_s(character_info[i].name, Avatar::MAX_AVATAR_NAME_LENGTH+50, characterNames[i]);
      character_info[i].id = characterIDs[i];
  }



  const char* names[5] = { characterNames[0], characterNames[1], characterNames[2], characterNames[3], characterNames[4] };
  EvidyonGUICharacterList characterList;
  characterList.create(
    character_info, numCharacterIDs, &window_font_,
    &gui_primary_layer_, &myMouseInputSource);

  EvidyonGUIButton createCharacter, enterWorld, deleteCharacter, logout;
  createCharacter.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
  createCharacter.getCanvas()->alignXCenter(0);
  createCharacter.getCanvas()->alignYCenter(+235);
  createCharacter.getCanvas()->absoluteWidth(320);
  createCharacter.getCanvas()->absoluteHeight(35);
  createCharacter.getCanvas()->absoluteDepth(0.5f);
  createCharacter.setFont(&window_font_);
  createCharacter.registerElements(&gui_primary_layer_);
  createCharacter.setText("Create Character");
  createCharacter.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());

  if (numCharacterIDs >= MAX_AVATARS_PER_ACCOUNT)
      createCharacter.disable();

  logout.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
  logout.getCanvas()->alignXCenter(+0);
  logout.getCanvas()->alignYCenter(270);
  logout.getCanvas()->absoluteWidth(320);
  logout.getCanvas()->absoluteHeight(35);
  logout.getCanvas()->absoluteDepth(0.5f);
  logout.setFont(&window_font_);

  logout.registerElements(&gui_primary_layer_);
  logout.setText("Log Out");
  logout.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());

  enterWorld.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
  enterWorld.getCanvas()->alignXCenter(-200);
  enterWorld.getCanvas()->alignYTop(0);
  enterWorld.getCanvas()->absoluteWidth(250);
  enterWorld.getCanvas()->absoluteHeight(35);
  enterWorld.getCanvas()->absoluteDepth(0.4f);
  enterWorld.setFont(&window_font_);

  enterWorld.setText("Enter World");

  deleteCharacter.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
  deleteCharacter.getCanvas()->alignXCenter(+200);
  deleteCharacter.getCanvas()->alignYTop(0);
  deleteCharacter.getCanvas()->absoluteWidth(250);
  deleteCharacter.getCanvas()->absoluteHeight(35);
  deleteCharacter.getCanvas()->absoluteDepth(0.4f);
  deleteCharacter.setFont(&window_font_);

  deleteCharacter.setText("Delete");

  enterWorld.getCanvas()->alignYCenter(0);
  deleteCharacter.getCanvas()->alignYCenter(0);

  EvidyonGUIButton confirmDeleteCharacter;
  confirmDeleteCharacter.getCanvas()->setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
  confirmDeleteCharacter.getCanvas()->alignXCenter(0);
  confirmDeleteCharacter.getCanvas()->alignYCenter(-100);
  confirmDeleteCharacter.getCanvas()->absoluteWidth(320);
  confirmDeleteCharacter.getCanvas()->absoluteHeight(35);
  confirmDeleteCharacter.getCanvas()->absoluteDepth(0.4f);
  confirmDeleteCharacter.setFont(&window_font_);
  confirmDeleteCharacter.setText("Confirm Delete");
  confirmDeleteCharacter.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
  confirmDeleteCharacter.disable();



  float current_x = 0.0f, current_y = 0.0f;


  // If the window closes, exit
  myStateMachine.jumpState(VCS_DISCONNECT);
  myStateMachine.queueState(VCS_SHUTDOWN);

  double lastRenderedFrameTime = dcxWin32Clock::getAccurateSystemTime();

  bool lastSelectionWasValid = false;


  enum Action {
    ACTION_NONE,
    LOGGING_OUT,
    ENTERING_WORLD,
  };

  Action action = ACTION_NONE;
  double action_time = 0.0;

  // Loop until we get a reply
  bool run_state = true;
  while (run_state && windowsMessagePump()) {
    using namespace Evidyon::Network::Packet;
    using namespace Evidyon::Network::Packet::Client;

    { // handle messages from the network
      ENetEvent netEvent;
      enet_host_service(enet_client_, &netEvent, 0);
      if (netEvent.type == ENET_EVENT_TYPE_DISCONNECT) {
        myStateMachine.jumpState(VCS_NO_CONNECTION);
        run_state = false;
      } else if (netEvent.type == ENET_EVENT_TYPE_RECEIVE) {
        Message msg;
        decodeMessageType(netEvent.packet, &msg);
        switch (msg) {
          case NETMSG_LOGOUTACCOUNT_CONFIRMED:
            CONFIRM(Decode::logoutAccountConfirmed(netEvent.packet));
            myStateMachine.jumpState(VCS_ACCOUNT_LOGIN);
            run_state = false;
            break;

          case NETMSG_ENTERWORLD_SUCCEEDED: {
            using namespace Evidyon::Network::Packet;
            Evidyon::Actor::ActorDescription actor_description;
            float hp, mp, xp;
            bool is_male;
            unsigned int time_of_day;
            NetMsg_EnterWorld_Succeeded::readPacket(
              netEvent.packet,
              &is_male,
              &avatar_.race,
              &avatar_.class_,
              &hp,
              &mp,
              &xp,
              &myCharacter.money,
              &current_map_index_,
              &time_of_day,
              &actor_description,
              &myCharacter.spell_availability);
            avatar_.gender
              = is_male ? Avatar::AVATARGENDER_MALE
                        : Avatar::AVATARGENDER_FEMALE;

            // change maps
            map_renderer_.reset();
            current_map_ = &maps_[current_map_index_];
 
            myCharacter.male = is_male;
            myCharacter.race_index = avatar_.race;
            myCharacter.class_index = avatar_.class_;
            myCharacter.currentHPPercent = hp / 256.0f;
            myCharacter.currentMPPercent = mp / 256.0f;
            myCharacter.currentEXPPercent = xp / 256.0f;

            // Subtract this number of seconds to get the true time
            time_of_day_adjustment_ = dcx::dcxWin32Clock::getAccurateSystemTime() - time_of_day / 1000.0;

            actor_manager_.acquireClientActor(
              myCharacter.name,
              &actor_description);

            special_fx_manager_.eraseTimedFX();

            keepConnectionAlive();

            myStateMachine.jumpState(VCS_WORLD_MAIN);
            run_state = false;

            // this line could possibly fix the logging into world bug
            // where clients taking a while to enter the game
            // get dropped...
            action = ACTION_NONE;

          } break;
        }
      }

      if (netEvent.packet != NULL) {
        enet_packet_destroy(netEvent.packet);
      }
    }

    double currentTime = dcxWin32Clock::getAccurateSystemTime();

    if (action != ACTION_NONE) {
      static const double MAX_ACTION_TIME = 5.0;
      double seconds_left = MAX_ACTION_TIME - (currentTime - action_time);
      if (seconds_left <= 0.0) { // got disconnected
        myStateMachine.jumpState(VCS_DISCONNECT);
        myStateMachine.queueState(VCS_NO_CONNECTION);
        run_state = false;
      }
      char button_text[32];
      int int_seconds_left = (int)(ceil(seconds_left));
      if (int_seconds_left < 4) {
        switch (action) {
        case LOGGING_OUT:
          sprintf_s(button_text, "Logging Out (%i)", int_seconds_left);
          logout.setText(button_text);
          break;
        case ENTERING_WORLD:
          sprintf_s(button_text, "Entering World (%i)", int_seconds_left);
          enterWorld.setText(button_text);
          break;
        }
      }
    }

    if (enterWorld.wasPushed()) {
      enterWorld.resetPushed();

      int character = characterList.getSelection();
      if (character < numCharacterIDs) {
        enterWorld.disable();
        createCharacter.disable();
        deleteCharacter.disable();
        logout.disable();
        enterWorld.setText("Entering World");

        // Log in to the world with this character
        enet_peer_send(enet_server_, 0, Encode::enterWorld(characterIDs[character]));

        action = ENTERING_WORLD;
        action_time = currentTime;

        // Get the player's name
        memcpy(myCharacter.name,
               characterNames[character],
               Avatar::MAX_AVATAR_NAME_LENGTH);
      }
    }

    if (createCharacter.wasPushed()) {
      // Create a new character
      myStateMachine.jumpState(VCS_CREATE_CHARACTER);
      break;
    }

    if (deleteCharacter.wasPushed()) {
      deleteCharacter.resetPushed();
      deleteCharacter.disable();
      enterWorld.disable();
      std::string delete_text = "Delete " + characterList.getSelectionText();
      confirmDeleteCharacter.setText(delete_text.c_str());
      confirmDeleteCharacter.registerElements(&gui_primary_layer_);
      confirmDeleteCharacter.enable();
    }

    //if (deleteCharacter.wasPushed()) {
    if (confirmDeleteCharacter.wasPushed()) {
      unsigned int selection = characterList.getSelection();
      if (selection < numCharacterIDs) {
        enet_peer_send(enet_server_, 0, ::Evidyon::Network::Packet::Client::Encode::deleteCharacter(characterIDs[selection], ""));
        bool deleted = false;
        unsigned long startTime = dcxWin32Clock::getSystemTime();
        ENetEvent netEvent;
        while(!deleted &&
              (enet_host_service(enet_client_, &netEvent, 0) > 0) &&
              (dcxWin32Clock::getSystemTime() - startTime < 1000)) {
            if (netEvent.type == ENET_EVENT_TYPE_DISCONNECT) {
                myStateMachine.jumpState(VCS_SHUTDOWN);
                return;
            }
            ::Evidyon::Network::Packet::Message msg = ::Evidyon::Network::Packet::NETMSG_INVALID;
            ::Evidyon::Network::Packet::decodeMessageType(netEvent.packet, &msg);
            if (netEvent.type == ENET_EVENT_TYPE_RECEIVE && netEvent.channelID == 0) {
              deleted = msg == ::Evidyon::Network::Packet::NETMSG_REQUESTACTIONRESULT;
            }
            // Free packet memory
            if (netEvent.packet) enet_packet_destroy(netEvent.packet);
        }

        myStateMachine.jumpState(VCS_LOGGED_INTO_ACCOUNT); // this is cheating
        break;
      }
    }

    if (logout.pollPushed()) {
      enterWorld.disable();
      createCharacter.disable();
      deleteCharacter.disable();
      logout.disable();

      logout.setText("Logging Out");
      action = LOGGING_OUT;
      action_time = currentTime;

      enet_peer_send(enet_server_, 0, Encode::logoutAccount());
    }

    // If the user presses space + escape, they're trying to exit
    if (myKeyboardInputSource.isKeyDown(GUIKEY_ESCAPE) &&
        myKeyboardInputSource.isKeyDown(GUIKEY_SPACE))
        break;

      if (characterList.selectionChanged()) {
        deleteCharacter.enable();
        enterWorld.enable();
        confirmDeleteCharacter.unregisterElements(&gui_primary_layer_);
        confirmDeleteCharacter.disable();
        unsigned int selection;
        if ((selection = characterList.getSelection()) >= MAX_AVATARS_PER_ACCOUNT) {
          enterWorld.unregisterElements(&gui_primary_layer_);
          enterWorld.getMouseSentinel()->removeFromMouseSentinelChain();
          deleteCharacter.unregisterElements(&gui_primary_layer_);
          deleteCharacter.getMouseSentinel()->removeFromMouseSentinelChain();
          lastSelectionWasValid = false;
        } else {
          current_x = character_info[selection].world_x;
          current_y = character_info[selection].world_y;

          // Add the elements, if necessary
          if (!lastSelectionWasValid) {
              enterWorld.registerElements(&gui_primary_layer_);
              enterWorld.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
              deleteCharacter.registerElements(&gui_primary_layer_);
              deleteCharacter.getMouseSentinel()->setParentMouseSentinel(myMouseInputSource.getPrimarySentinel());
          }

          lastSelectionWasValid = true;

          //unsigned int loop;
          //global_map_manager_.changeMap(character_info[selection].map_index, &loop);
          //global_sound_manager_.loopBackgroundSound(loop);
          //global_map_manager_.update(character_info[selection].world_x, character_info[selection].world_y);

          //global_actor_manager_.reset();
          //internalActor = global_actor_manager_.acquireClientActor(0, current_x, current_y, character_info[selection].name);
          //global_actor_manager_.setActorDescription(0, character_info[selection].actor_type);
        }
      }

      // Draw the scene
      if (allowFrameToBeRendered(currentTime)) {
          float timeSinceLastRenderedFrame = lastRenderedFrameTime - currentTime;
          lastRenderedFrameTime = currentTime;

          // Clear the backbuffer and the zbuffer
          d3d_device_->Clear(0,
                             NULL,
                             D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                             D3DCOLOR_XRGB(0, 0, 0),
                             1.0f,
                             0);

          //global_actor_manager_.update(currentTime, timeSinceLastRenderedFrame);

          // Begin the scene
          if (SUCCEEDED(d3d_device_->BeginScene())) {

            camera_.setPosition(current_x, current_y);
            camera_.initialize3DRendering(d3d_device_);


            //// Clear the zbuffer
            d3d_device_->Clear(0,
                               NULL,
                               D3DCLEAR_ZBUFFER,
                               0,
                               1.0f,
                               0);
            //// Set up the camera
            d3d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);

            d3d_device_->Clear(0,
                               NULL,
                               D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,
                               0,
                               1.0f,
                               0);
            renderLoginBackground();

            // Draw the (new) GUI
            renderstate_manager_.resetState();
            d3d_device_->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
            gui_primary_layer_.render();
            gui_primary_layer_.flush();

            // End the scene
            d3d_device_->EndScene();
          }

          // Present the backbuffer contents to the display
          CONFIRM(SUCCEEDED(d3d_device_->Present(NULL, NULL, NULL, NULL))) else {
            myStateMachine.branchState(VCS_LOST_D3DDEVICE);
            break;
          }
      }
  }

  logout.unregisterElements(&gui_primary_layer_);

  logout.getMouseSentinel()->removeFromMouseSentinelChain();
  enterWorld.unregisterElements(&gui_primary_layer_);

  enterWorld.getMouseSentinel()->removeFromMouseSentinelChain();
  deleteCharacter.unregisterElements(&gui_primary_layer_);

  deleteCharacter.getMouseSentinel()->removeFromMouseSentinelChain();
  createCharacter.unregisterElements(&gui_primary_layer_);

  createCharacter.getMouseSentinel()->removeFromMouseSentinelChain();
  characterList.destroy(&gui_primary_layer_);

  confirmDeleteCharacter.unregisterElements(&gui_primary_layer_);
}