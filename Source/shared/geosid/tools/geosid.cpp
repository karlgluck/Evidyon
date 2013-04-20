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
#include "shared/geosid/tools/geosid.h"
#include "shared/world/tools/mapcompilationcontext.h"
#include "shared/geosid/tools/geosidcompiler.h"
#include "shared/geosid/geosidserverdescription.h"
#include "shared/geosid/geosidclientdescription.h"
#include "shared/magic/magicindex.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/world/tools/maplocationreference.h"


namespace Evidyon {
namespace Geosid {
namespace Tools {


  
//----[  Geosid  ]-------------------------------------------------------------
Geosid::Geosid() {
  member("Displayed Name", &displayed_name_);
  member("Type", &type_);
  member("Location", &location_);
  member("Scenery", &scenery_);
  member("Burst Magic", &burst_magic_);
  member("Burst Magic Geonite Cost", &burst_magic_geonite_cost_);
  member("Geonite To Activate", &geonite_to_activate_);
  member("Activation SpecialFX", &activation_special_fx_);
  member("Activation SpecialFX Duration", &activation_special_fx_duration_);
  member("Options for Item to Generate When Activated", &geonite_generated_items_);

  geonite_to_activate_.setValue(1000);
  activation_special_fx_duration_.setValue(4.0f);

  portals_ = new World::Tools::MapLocationReference[Evidyon::Geosid::GeosidServerDescription::MAX_PORTALS];
  for (int i = 0; i < Evidyon::Geosid::GeosidServerDescription::MAX_PORTALS; ++i) {
    char name[128];
    sprintf_s(name, 128, "Portal - #%i", i);
    member(name, &portals_[i]);
  }

  member("Guardian", &guardian_);
}




//----[  Geosid  ]-------------------------------------------------------------
Geosid::~Geosid() {
  delete[] portals_;
}

//----[  compile  ]------------------------------------------------------------
void Geosid::compile(World::Tools::MapCompilationContext* context) const {
  World::Tools::MapLocationData* location = location_.getMapLocationData(context);
  if (location) {
    location->this_layer.visual_data.scenery
      = scenery_.getReferencedResourceIndex();
  }

  { // create the triggers
    Trigger::Trigger trigger;
    trigger.type = Trigger::Trigger::GEOSID_ACTIVATE;
    trigger.geosid_activate.geosid = getIndex();
    Trigger::TriggerIndex trigger_index = context->addTrigger(&trigger);

    float fx, fz;
    location_.get(&fx, &fz);
    int x = int(fx), z = int(fz);
    for (int r = z - 1; r <= z + 1; ++r) {
      for (int c = x - 1; c <= x + 1; ++c) {
        Evidyon::World::Tools::MapLocationData* location
          = context->atOrNull(c, r);
        if (location == NULL) continue;
        location->this_layer.trigger = trigger_index;
      }
    }
  }
}


//----[  compile  ]------------------------------------------------------------
Evidyon::Geosid::GeosidIndex Geosid::compile(
    Evidyon::World::MapIndex map,
    GeosidCompiler* compiler,
    World::Tools::MapCompilationContext* context) {

  GeosidServerDescription server;
  GeosidClientDescription client;

  client.change_owner_event_index = Event::INVALID_EVENT_INDEX;
  client.map = map;
  strncpy_s(client.name,
            client.NAME_LENGTH,
            displayed_name_.getValue().c_str(),
            client.NAME_LENGTH-1);
  client.type = type_.getValue();
  location_.get(&client.x, &client.z);

  // Compile the burst magic
  server.burst_cost = burst_magic_geonite_cost_.getValue();
  Evidyon::Magic::Tools::Magic* magic = burst_magic_.getImplementation();
  server.burst_magic
    = magic ? magic->compile(compiler->magicCompiler()) : Magic::INVALID_MAGIC_INDEX;

  server.hp_bonus = GeosidHPBonus(type_.getValue());
  server.id = dc::dcResourceManager::uniqueIDOfTypeName(getName());
  server.is_white_geosid = type_.getValue() == GEOSID_TOWN_WHITE;
  server.map = map;
  server.mp_bonus = GeosidMPBonus(type_.getValue());
  server.x.set(Number(client.x));
  server.xp_bonus = GeosidXPBonus(type_.getValue());
  server.z.set(Number(client.z));
  server.geonite_to_activate = geonite_to_activate_.getValue();

  Event::Event_TargetedSpecialFX event_description;
  event_description.special_fx
    = activation_special_fx_.getReferencedResourceIndex();
  event_description.duration
    = activation_special_fx_duration_.getValue();
  server.activation_event
    = compiler->magicCompiler()->eventCompiler()->add(&event_description);
  server.activation_event_duration
    = Time::Milliseconds(activation_special_fx_duration_.getValue() * 1000);

  { // fill the items array
    const dc::dcGenericResource::Array& items = geonite_generated_items_.getMembers();
    size_t index = 0;
    for (dc::dcGenericResource::Array::const_iterator i = items.begin();
          i != items.end() && index < server.MAX_ITEMS;
          ++i) {
      dc::dcList<Evidyon::Tools::ItemReference>::Element* element
        = (dc::dcList<Evidyon::Tools::ItemReference>::Element*)(*i);
      Item::ItemIndex item = element->getReferencedResourceIndex();
      server.items[index++] = item;
    }
    server.number_of_items = index;
    while (index < server.MAX_ITEMS) {
      server.items[index++] = Item::INVALID_ITEM_INDEX;
    }
  }

  { // save the portal destinations
    size_t index = 0;
    memset(server.portals, -1, sizeof(server.portals));
    for (int i = 0; i < Evidyon::Geosid::GeosidServerDescription::MAX_PORTALS; ++i) {
      portals_[i].compile(&server.portals[index].map,
                          &server.portals[index].x,
                          &server.portals[index].z);
      if (server.portals[index].map != World::INVALID_MAP_INDEX) {
        ++index;
      }
    }

    server.number_of_portals = index;
  }

  GeosidIndex geosid_index = compiler->add(server, client);

  if (type_.getValue() != GEOSID_TOWN_WHITE) {

    LifeformAI::LifeformAIIndex guardian_lifeform
      = guardian_.compile(displayed_name_.getValue().c_str(),
                          geosid_index,
                          compiler->lifeformCompiler());

    // place in the context so that it gets spawned
    World::Tools::MapLocationData* location = context->atOrNull(client.x, client.z);
    if (location) {
      location->this_layer.number_of_spawns = 1;
      location->this_layer.spawn[0] = guardian_lifeform;
      location->this_layer.spawn_intended_level = -1;
      location->this_layer.spawn_type
        = World::Tools::MapLocationData::SPAWN_INHABITANT;
    }
  }

  return geosid_index;
}



//----[  getLocation  ]--------------------------------------------------------
void Geosid::getLocation(float* x, float* y) {
  location_.get(x, y);
}



//----[  getDisplayedName  ]---------------------------------------------------
const char* Geosid::getDisplayedName() {
  return displayed_name_.getValue().c_str();
}


}
}
}