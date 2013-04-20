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
#ifndef __MAPEDITINGDIALOG_H__
#define __MAPEDITINGDIALOG_H__
#pragma once


#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "shared/world/tools/map.h"
#include "shared/world/tools/mapcompilationcontext.h"
#include "shared/world/maprendererdatasource.h"
#include "shared/evidyoncamera.h"
#include "shared/evidyond3dlightmanager.h"
#include "shared/world/mapvisibilityrect.h"
#include "shared/world/maprenderer.h"
#include "shared/client/clientdata.h"

namespace Evidyon {
namespace World {
namespace Tools {
struct MapLocationData;
}
}
namespace Actor {
struct RenderableActorInstance;
}
}


namespace Evidyon {




//----[  MapEditingDialog  ]---------------------------------------------------
class MapEditingDialog : public dcx::dcxWin32DialogController,
                                Tools::D3DWindow,
                                World::MapRendererDataSource,
                                Client::ClientData {
public:  
  enum PenAction {
    PENACTION_LAYERBRUSH,
    PENACTION_CREATESCENERY,
    PENACTION_SETGOODSPAWNPOINT,
    PENACTION_SETEVILSPAWNPOINT,
  };
  static const char* PenActionString(PenAction action);
  enum LayerBrushType {
    LAYERBRUSH_CIRCLE,
    LAYERBRUSH_SQUARE,
  };

public:
  MapEditingDialog();
  ~MapEditingDialog();

  // The functionoid is automatically deleted when this dialog exits as long as
  // begin() has successfully instantiated the dialog.
  bool begin(Evidyon::Tools::CompleteEditor* editor,
             World::Tools::Map* map);
  HWND getWindowHandle() { return dialog_handle_; }
  bool render(Evidyon::Tools::CompleteEditor* editor,
              LPDIRECT3DDEVICE9 d3d_device);

  // Invoked asynchronously if the user picks a segment of a map color mask
  void setPenActionAsLayerBrush(World::Tools::MapMask* mask, D3DCOLOR color);

  void setCameraLocation(float x, float y);

public:

  // This method is called by the map renderer to fill in the information about
  // a location in the world.
  virtual void updateMapTileData(int x,
                                 int y,
                                 Evidyon::World::LocationVisualData* data);


private:
  virtual INT_PTR dialogMenuNotification(HWND hDlg,
                                         HMENU hMenu,
                                         WORD ItemID,
                                         WORD ItemIndex);
  virtual INT_PTR dialogEvent(HWND hDlg,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam);

public:
  
  //// Determines whether or not the given location is visible on the current
  //// map, based on the user's location.  This interfaces with the map renderer
  //// at some level, but this abstraction allows that to happen somewhere else.
  virtual bool locationIsVisible(int x, int z);

private:
  void layerBrushOverScreenPoint(LPARAM lParam,
                                 bool pen_down,
                                 bool force_update,
                                 bool rectangle);
  HMENU createMapMasksMenu(int base_id);

  void updateSwarmSpawnLocation(
    int x,
    int y,
    const Evidyon::World::Tools::MapLocationData* map_location_data);

private:

  // Main window handle
  HWND dialog_handle_;

  // Root editor object
  Evidyon::Tools::CompleteEditor* editor_;

  // Current position in the map
  D3DXVECTOR2 camera_position_,
              camera_position_target_;

  // Set of visible tiles.  Primary use is so that data is only calculated
  // for areas of the map that can be seen.
  Evidyon::World::MapVisibilityRect visibility_;

  // Map being edited
  World::Tools::Map* map_;

  // Holds compiled data about the map
  Evidyon::World::Tools::MapCompilationContext map_compilation_context_;

  // Handles the view transformation
  Evidyon::EvidyonCamera camera_;

  // Manages ambient light and lighting effects
  Evidyon::EvidyonD3DLightManager light_manager_;

  // Draws the map
  Evidyon::World::MapRenderer map_renderer_;

  // What to do when the user clicks
  PenAction pen_action_;

  // Layer-brush config information
  LayerBrushType layer_brush_type_;
  World::Tools::MapMask* layer_brush_mask_;
  D3DCOLOR layer_brush_color_;
  int layer_brush_last_tile_x_, layer_brush_last_tile_y_;
  int mouse_down_tile_x_, mouse_down_tile_y_;

  typedef std::pair<int,int> Location;
  typedef std::set<Location> LocationSet;
  LocationSet layer_brush_affected_areas_;

  typedef std::map<Location,Actor::RenderableActorInstance*> SwarmSpawnActors;
  SwarmSpawnActors swarm_spawn_actors_;

  // Updated each time render() is called.
  double last_update_time_;
};

}


#endif