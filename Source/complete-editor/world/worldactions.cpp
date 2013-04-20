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
#include "shared/world/tools/fluidmaplayer.h"
#include "shared/world/tools/navigabilitymaplayer.h"
#include "shared/world/tools/occlusionmaplayer.h"
#include "shared/world/tools/rockyterrainheightmaplayer.h"
#include "shared/world/tools/scenerymaplayer.h"
#include "shared/world/tools/occlusionmaplayer.h"
#include "shared/world/tools/terrainblendingmaplayer.h"
#include "shared/world/tools/terrainheightmaplayer.h"
#include "shared/world/tools/terrainmaplayer.h"
#include "shared/world/tools/wallmaplayer.h"
#include "shared/world/tools/enableswarmspawnmaplayer.h"
#include "shared/world/tools/swarmspawnmaplayer.h"
#include "shared/world/tools/swarmspawntargetlevelmaplayer.h"
#include "shared/world/tools/configurebordersmaplayer.h"
#include "shared/world/tools/zonemaplayer.h"

#include "shared/evidyontool/directcraftactions.h"
#include "shared/world/tools/mapmask.h"
#include "shared/world/tools/mapareareference.h"
#include "shared/world/tools/maplocationreference.h"
#include "shared/world/tools/map.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"

#include "complete-editor/world/mapmaskcolorselectordialog.h"
#include <assert.h>

#include "complete-editor/world/mapeditingdialog.h"

#include "shared/world/tools/geoniteburst.h"

#include "shared/world/tools/mapmaskcolordata.h"
#include "shared/world/tools/maptrigger.h"



BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::GeoniteBurst)
END_DEFINING_ACTIONS()

//----[  dc::dcList<GeoniteBurst>  ]------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::GeoniteBurst>)
  COLLECTION_ACTIONS,
  { "Add Geonite Burst",
    &DirectCraftActions::GenerateCollectionElement<dc::dcList<Evidyon::World::Tools::GeoniteBurst>> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::GeoniteBurst>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::GeoniteBurst>::Reference)
  LIST_REFERENCE_ACTIONS(Evidyon::World::Tools::GeoniteBurst),
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::MapTrigger)
END_DEFINING_ACTIONS()

//----[  dc::dcList<MapTrigger>  ]------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapTrigger>)
  COLLECTION_ACTIONS,
  { "Add Trigger",
    &DirectCraftActions::GenerateCollectionElement<dc::dcList<Evidyon::World::Tools::MapTrigger>> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapTrigger>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapTrigger>::Reference)
  LIST_REFERENCE_ACTIONS(Evidyon::World::Tools::MapTrigger),
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::MapMaskColorData)
END_DEFINING_ACTIONS()

//----[  dc::dcList<MapMaskColorData>  ]---------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapMaskColorData>)
  COLLECTION_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapMaskColorData>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapMaskColorData>::Reference)
END_DEFINING_ACTIONS()



BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::FluidMapLayer)
END_DEFINING_ACTIONS()





BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::WallMapLayer)
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::TerrainMapLayer)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::TerrainHeightMapLayer)
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::TerrainBlendingMapLayer)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::SceneryMapLayer)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::RockyTerrainHeightMapLayer)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::EnableSwarmSpawnMapLayer)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::OcclusionMapLayer)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::NavigabilityMapLayer)
END_DEFINING_ACTIONS()


BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::SwarmSpawnTargetLevelMapLayer)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::SwarmSpawn)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::SwarmSpawnMapLayer)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::ConfigureBordersMapLayer)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::ZoneMapLayer)
END_DEFINING_ACTIONS()





//----[  Location  ]-----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::Location)
END_DEFINING_ACTIONS()


//----[  MapLocationReference  ]-----------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::MapLocationReference)
END_DEFINING_ACTIONS()


//----[  dc::dcList<MapLocationReference>  ]-----------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapLocationReference>)
  COLLECTION_ACTIONS,
{ "Add...", &DirectCraftActions::GenerateCollectionElement<dc::dcList<Evidyon::World::Tools::MapLocationReference>> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapLocationReference>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapLocationReference>::Reference)
  LIST_REFERENCE_ACTIONS(Evidyon::World::Tools::MapLocationReference),
END_DEFINING_ACTIONS()


//----[  MapMask  ]------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::MapMask)
END_DEFINING_ACTIONS()



//----[  CreateMapMask  ]------------------------------------------------------
int CreateMapMask(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  typedef dc::dcList<Evidyon::World::Tools::MapMask> List;
  CAST_ACTION_RESOURCE(List, list);

  std::string path;
  std::list<std::string> files;
  if (!tool->promptUserForImageFiles("Select Map Mask Images", &path, &files)) {
    return 0;
  }

  bool encountered_error = false;

  for (std::list<std::string>::iterator fileIter = files.begin();
       fileIter != files.end(); ++fileIter) {
    const std::string file_name = *fileIter;
    std::string element_name = file_name.substr(0,file_name.find_last_of("."));
    List::Element* element = (List::Element*)list->generateElement(element_name);
    std::string file_path = path + "\\" + file_name;
    encountered_error |= !element->setSourceFile(file_path.c_str(),
                                                 tool->getD3DDevice());
  }

  if (encountered_error) {
    MessageBox(window,
               "Warning: One or more of the layers didn't correctly load",
               "Create Map Masks",
               MB_OK);
  }

  return 1;
}







//----[  CreateMapLayersForMapMaskFunctionoid  ]-------------------------------
class CreateMapLayersForMapMaskFunctionoid
  : public Evidyon::MaskColorSelectorDialogFunctionoid {
public:
  virtual void execute(
      Evidyon::MapMaskColorSelectorDialog* dialog,
      Evidyon::World::Tools::Map* map,
      Evidyon::MapMaskColorSelectorDialog::MapMaskElement* selected_mask,
      D3DCOLOR selected_color,
      bool* close_dialog,
      int texture_mouse_x,
      int texture_mouse_y) {

    std::string caption = "Create Map Layers for " + selected_mask->getName();
    std::list<std::string> layer_type_options;
    layer_type_options.push_back("Terrain Texture");
    layer_type_options.push_back("Wall");
    layer_type_options.push_back("Scenery");
    layer_type_options.push_back("Terrain Height");
    layer_type_options.push_back("Rocky Terrain Height");
    layer_type_options.push_back("Occlusion");
    layer_type_options.push_back("Navigability");
    layer_type_options.push_back("Fluid");
    layer_type_options.push_back("Zone");
    layer_type_options.push_back("(1) Set Swarm Spawn Target Level");
    layer_type_options.push_back("(2) Set Swarm Spawn Lifeforms");
    layer_type_options.push_back("(3) Enable Swarm Spawn");
    layer_type_options.push_back("Configure Borders Map Layer");
    int selection_index = 0;
    if (!dcx::dcxWin32PromptForSelection(dialog->getWindowHandle(),
                                         caption.c_str(),
                                         "What kind of layer would you like to create?",
                                         &layer_type_options,
                                         &selection_index,
                                         true)) {
      *close_dialog = true;
      return;
    }
    std::string name = "";
    if (!dcx::dcxWin32PromptTextInput(dialog->getWindowHandle(), 
                                     "Resource Name?",
                                     "Please enter a name for this resource:",
                                     &name)) {
      *close_dialog = true;
      return;
    }
    Evidyon::World::Tools::MapLayer* layer = NULL;
#define CREATE_LAYER(type) {Evidyon::World::Tools::type* local_layer\
      = map->createLayer<Evidyon::World::Tools::type>(name);\
      local_layer->setArea(selected_mask, selected_color);}
    
    switch (selection_index) {
      default:
      case 0: CREATE_LAYER(TerrainMapLayer); break;
      case 1: CREATE_LAYER(WallMapLayer); break;
      case 2: CREATE_LAYER(SceneryMapLayer); break;
      case 3: CREATE_LAYER(TerrainHeightMapLayer); break;
      case 4: CREATE_LAYER(RockyTerrainHeightMapLayer); break;
      case 5: CREATE_LAYER(OcclusionMapLayer); break;
      case 6: CREATE_LAYER(NavigabilityMapLayer); break;
      case 7: CREATE_LAYER(FluidMapLayer); break;
      case 8: CREATE_LAYER(ZoneMapLayer); break;
      case 9: CREATE_LAYER(SwarmSpawnTargetLevelMapLayer); break;
      case 10: CREATE_LAYER(SwarmSpawnMapLayer); break;
      case 11: CREATE_LAYER(EnableSwarmSpawnMapLayer); break;
      case 12: CREATE_LAYER(ConfigureBordersMapLayer); break;
    }
  }
private:
  Evidyon::World::Tools::MapMaskColorReference* reference_;
};


//----[  CreateMapLayersForMapMask  ]------------------------------------------
int CreateMapLayersForMapMask(dc::dcGenericResource* resource, void* param){
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Evidyon::World::Tools::MapMask>::Element , mask);

  Evidyon::World::Tools::Map* map = tool->getMapFromDescendant(mask);
  assert(map);
  
  Evidyon::MapMaskColorSelectorDialog* mask_dialog
    = new Evidyon::MapMaskColorSelectorDialog();
  CreateMapLayersForMapMaskFunctionoid* functionoid
    = new CreateMapLayersForMapMaskFunctionoid;
  if (!mask_dialog->begin(tool->topWindow(),
                          tool,
                          map,
                          mask,
                          functionoid)) {
    delete functionoid;
    delete mask_dialog;
    return 0;
  }

  // functionoid does the work
  return 1;
}


//----[  SyncMaskColors  ]-----------------------------------------------------
int SyncMaskColors(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  typedef dc::dcList<Evidyon::World::Tools::MapMask>::Element Element;
  CAST_ACTION_RESOURCE(Element, element);

  element->load(tool->getD3DDevice(), true);
  element->updateColorData();

  return 1;
}


//----[  dc::dcList<MapMask>  ]------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapMask>)
  COLLECTION_ACTIONS,
  { "Create Map Mask...", &CreateMapMask },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapMask>::Element)
  COLLECTION_ELEMENT_ACTIONS,
  { "Create Map Layers...", &CreateMapLayersForMapMask },
  { "Sync Colors", &SyncMaskColors },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::MapMask>::Reference)
  LIST_REFERENCE_ACTIONS(Evidyon::World::Tools::MapMask),
END_DEFINING_ACTIONS()




//----[  SelectMapMaskColorReferenceFunctionoid  ]-----------------------------
class SelectMapMaskColorReferenceFunctionoid
  : public Evidyon::MaskColorSelectorDialogFunctionoid {
public:
  SelectMapMaskColorReferenceFunctionoid(
    Evidyon::World::Tools::MapMaskColorReference* reference) {
    reference_ = reference;
  }

  virtual void execute(
      Evidyon::MapMaskColorSelectorDialog* dialog,
      Evidyon::World::Tools::Map* map,
      Evidyon::MapMaskColorSelectorDialog::MapMaskElement* selected_mask,
      D3DCOLOR selected_color,
      bool* close_dialog,
      int texture_mouse_x,
      int texture_mouse_y) {
    reference_->set(selected_mask, selected_color);
    *close_dialog = true;
  }
private:
  Evidyon::World::Tools::MapMaskColorReference* reference_;
};


//----[  SelectMapMaskColorReference  ]----------------------------------------
int SelectMapMaskColorReference(dc::dcGenericResource* resource, void* param){
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(Evidyon::World::Tools::MapMaskColorReference, ref);

  dc::dcList<Evidyon::World::Tools::MapMask>::Element* mask =
    ref->getReferencedMaskListElement();

  Evidyon::World::Tools::Map* map = tool->getMapFromDescendant(ref);
  assert(map);
  
  // Let the user pick the mask
  if (!DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Map Mask",
        "Which map mask contains the color area you want to reference?",
        map->getMapMasks(),
        (dc::dcGenericResource**)&mask)) {
    return 0;
  }

  Evidyon::MapMaskColorSelectorDialog* mask_dialog
    = new Evidyon::MapMaskColorSelectorDialog();
  SelectMapMaskColorReferenceFunctionoid* functionoid
    = new SelectMapMaskColorReferenceFunctionoid(ref);
  if (!mask_dialog->begin(tool->topWindow(),
                          tool,
                          map,
                          mask,
                          functionoid)) {
    delete functionoid;
    delete mask_dialog;
    return 0;
  }

  // functionoid does the work
  return 1;
}

//----[  MapMaskColorReference  ]----------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::MapMaskColorReference)
  { "Pick Visually...", &SelectMapMaskColorReference },
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::World::WallType>)
ENUM_ACTIONS(Evidyon::World::WallType),
END_DEFINING_ACTIONS()





//----[  Map  ]----------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::Map)
END_DEFINING_ACTIONS()




//----[  EditMap  ]------------------------------------------------------------
int EditMap(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  typedef dc::dcList<Evidyon::World::Tools::Map>::Element MapElement;
  CAST_ACTION_RESOURCE(MapElement, map_element);

  Evidyon::MapEditingDialog* map_editing_dialog
    = new Evidyon::MapEditingDialog();
  if (!map_editing_dialog->begin(tool,
                                 map_element)) {
    delete map_editing_dialog;
    return 0;
  }

  // Dialog will be changing this map
  return 1;
}




//----[  SelectMap  ]----------------------------------------------------------
int SelectMap(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Evidyon::World::Tools::Map>::Reference, reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Map",
        "Pick the map to reference",
        tool->getMaps(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}




//----[  dc::dcList<Map>  ]----------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::Map>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Create Map", &DirectCraftActions::GenerateCollectionElement<dc::dcList<Evidyon::World::Tools::Map>> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::Map>::Element)
  COLLECTION_ELEMENT_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Edit", &EditMap },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::Map>::Reference)
  { "Select Map", &SelectMap },
  LIST_REFERENCE_ACTIONS(Evidyon::World::Tools::Map),
END_DEFINING_ACTIONS()


//----[  CreateMapLayer  ]-----------------------------------------------------
template <typename T>
int CreateMapLayer(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Evidyon::World::Tools::MapLayer>, layers);

  std::string name = "";
  if (dcx::dcxWin32PromptTextInput(window, 
                                   "Resource Name?",
                                   "Please enter a name for this resource:",
                                   &name)) {
    T* element = (T*)layers->create<T>(name.c_str());
    element->setPriority(layers->getMembers().size());
    return 1;
  }

  return 0;
}




//----[  SelectMapLayerReference  ]--------------------------------------------
int SelectMapLayerReference(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::World::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<MapLayer>::Reference, reference);

  Map* map = tool->getMapFromDescendant(reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Map Layer",
        "Pick the map layer to reference",
        map->getMapLayers(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}


//----[  dc::dcTable<MapLayer>  ]----------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::World::Tools::MapLayer>)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::World::Tools::MapLayer>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Create Wall Layer", &CreateMapLayer<Evidyon::World::Tools::WallMapLayer> },
  { "Create Terrain Texture Layer", &CreateMapLayer<Evidyon::World::Tools::TerrainMapLayer> },
  { "Create Terrain Height Layer", &CreateMapLayer<Evidyon::World::Tools::TerrainHeightMapLayer> },
  { "Create Scenery Layer", &CreateMapLayer<Evidyon::World::Tools::SceneryMapLayer> },
  { "Create Occlusion Layer", &CreateMapLayer<Evidyon::World::Tools::OcclusionMapLayer> },
  { "Create Navigability Layer", &CreateMapLayer<Evidyon::World::Tools::NavigabilityMapLayer> },
  { "Create Fluid Layer", &CreateMapLayer<Evidyon::World::Tools::FluidMapLayer> },
  ACTION_LIST_SEPARATOR,
  { "Create Terrain Blending Layer", &CreateMapLayer<Evidyon::World::Tools::TerrainBlendingMapLayer> },
  { "Create Rocky Terrain Height Layer", &CreateMapLayer<Evidyon::World::Tools::RockyTerrainHeightMapLayer> },
  ACTION_LIST_SEPARATOR,
  { "(1) Create Set Swarm Spawn Target Level Layer", &CreateMapLayer<Evidyon::World::Tools::SwarmSpawnTargetLevelMapLayer> },
  { "(2) Create Set Swarm Spawn Lifeforms Layer", &CreateMapLayer<Evidyon::World::Tools::SwarmSpawnMapLayer> },
  { "(3) Create Enable Swarm Spawn Layer", &CreateMapLayer<Evidyon::World::Tools::EnableSwarmSpawnMapLayer> },
  ACTION_LIST_SEPARATOR,
  { "Add Zone Layer", &CreateMapLayer<Evidyon::World::Tools::ZoneMapLayer> },
  { "Add Configure Borders Layer", &CreateMapLayer<Evidyon::World::Tools::ConfigureBordersMapLayer> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::World::Tools::MapLayer>::Element)
  COLLECTION_ELEMENT_ACTIONS,
  ACTION_LIST_SEPARATOR,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::World::Tools::MapLayer>::Reference)
  { "Select...", &SelectMapLayerReference },
  { "Clear", &DirectCraftActions::ClearReference },
END_DEFINING_ACTIONS()









//----[  MapAreaReference  ]---------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::MapAreaReference)
END_DEFINING_ACTIONS()



//----[  LifeformAIInhabitantSpawn  ]------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::World::Tools::LifeformAIInhabitantSpawn)
END_DEFINING_ACTIONS()





//----[  dc::dcList<LifeformAIInhabitantSpawn>  ]------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::LifeformAIInhabitantSpawn>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Inhabitant",
    &DirectCraftActions::GenerateCollectionElement<dc::dcList<Evidyon::World::Tools::LifeformAIInhabitantSpawn>> },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::LifeformAIInhabitantSpawn>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::LifeformAIInhabitantSpawn>::Reference)
  LIST_REFERENCE_ACTIONS(Evidyon::World::Tools::LifeformAIInhabitantSpawn),
END_DEFINING_ACTIONS()





//----[  dc::dcEnum<Navigability>  ]-------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::World::Navigability>)
  ENUM_ACTIONS(Evidyon::World::Navigability),
END_DEFINING_ACTIONS()





//----[  AddSwarmSpawn  ]------------------------------------------------------
int AddSwarmSpawn(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::World::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<SwarmSpawn>, list);

  // Let the user pick a set of source animated meshes
  dc::dcGenericResource::Array selection;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Add Swarm Spawn",
        "Pick the lifeforms that will be placed in this spawn",
        tool->getLifeformAIs(),
        &selection,
        false)) {
    return 0;
  }
  std::string name;
  if (!dcx::dcxWin32PromptTextInput(
        tool->topWindow(),
        "Add Sawrm Spawn",
        "Enter a name for this swarm spawn:",
        &name,
        false,
        false,
        false,
        false)) {
    return 0;
  }

  typedef dc::dcList<SwarmSpawn>::Element Element;
  Element* element = (Element*)list->generateElement(name);
  element->addLifeformAIReferences(selection);

  return 0;
}


//----[  dc::dcList<LifeformAIInhabitantSpawn>  ]------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::SwarmSpawn>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add...", &AddSwarmSpawn },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::SwarmSpawn>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::World::Tools::SwarmSpawn>::Reference)
END_DEFINING_ACTIONS()
