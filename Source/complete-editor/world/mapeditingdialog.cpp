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
#include "mapeditingdialog.h"
#include "shared/world/locationvisualdata.h"
#include "complete-editor/world/mapmaskcolorselectordialog.h"
#include "complete-editor/resource.h"
#include "shared/world/tools/maplocationdata.h"
#include "common/randf.h"

namespace Evidyon {

  
//----[  PenActionString  ]----------------------------------------------------
const char* MapEditingDialog::PenActionString(PenAction action) {
  switch (action) {
    case PENACTION_LAYERBRUSH:  return "Paint w/ Layer Brush";
    case PENACTION_CREATESCENERY:  return "Create Scenery";
    case PENACTION_SETGOODSPAWNPOINT:  return "Set Good Spawn Point";
    case PENACTION_SETEVILSPAWNPOINT:  return "Set Evil Spawn Point";
    default: return NULL;
  }
}
  
//----[  MapEditingDialog  ]---------------------------------------------------
MapEditingDialog::MapEditingDialog() :
  camera_position_(0.0f, 0.0f),
  camera_position_target_(0.0f, 0.0f) {
  dialog_handle_ = NULL;
  editor_ = NULL;
  visibility_.center(-9999, -9999, 1, 1);
  map_ = NULL;
  layer_brush_mask_ = NULL;
  last_update_time_ = 0.0;
}

  
//----[  MapEditingDialog  ]---------------------------------------------------
MapEditingDialog::~MapEditingDialog() {
  if (map_) {
    map_->destroyCompilationContext(&map_compilation_context_);
  }
  map_renderer_.destroy();

  Evidyon::Actor::ActorRenderer* actor_renderer = editor_->getActorRenderer();
  for (SwarmSpawnActors::iterator i = swarm_spawn_actors_.begin();
    i != swarm_spawn_actors_.end(); ++i) {
    actor_renderer->release(i->second);
  }
}

  
//----[  begin  ]--------------------------------------------------------------
bool MapEditingDialog::begin(Evidyon::Tools::CompleteEditor* editor,
                             World::Tools::Map* map) {
  editor_ = editor;

  editor->acquireAllGameObjects();

  if (!map->createCompilationContext(editor->getD3DDevice(),
                                     &map_compilation_context_) ||
      !map_renderer_.create(editor->getD3DDevice(),
                            editor->getSceneryRenderer(),
                            editor->getSkinnedMeshRenderer())) {
    return false;
  }


  map_ = map;
  dcx::dcxWin32DialogTemplate dialog;
  dialog.setStyleAsResizeableAppWindow();
  dialog.setTitle("Map Editor");
  BYTE buffer[256];
  LPCDLGTEMPLATE dlg_template = dialog.compile(buffer, sizeof(buffer));
  dialog_handle_ = createDialog(GetModuleHandle(NULL),
                                dlg_template,
                                editor->topWindow());
  dcx::dcxWin32SetWindowClientSize(dialog_handle_, 1024, 768);
  camera_.rebuildTopdownCamera(1024, 768);
  return editor->addD3DWindow(this);
}


//----[  render  ]-------------------------------------------------------------
bool MapEditingDialog::render(Evidyon::Tools::CompleteEditor* editor,
                              LPDIRECT3DDEVICE9 d3d_device) {

  // Advance all special effects
  double time = dcx::dcxWin32Clock::getAccurateSystemTime();
  double time_since_last_update = time - last_update_time_;

  Evidyon::VisualFX::VisualFXRenderer* visual_fx_renderer
    = editor_->getVisualFXRenderer();
  visual_fx_renderer->update(time, time_since_last_update);
  last_update_time_ = time;

  D3DXVECTOR2 delta;
  D3DXVec2Subtract(&delta,
                   &camera_position_target_,
                   &camera_position_);
  if (D3DXVec2LengthSq(&delta) > 0.01f) {
    //D3DXVec2Add(&camera_position_,
    //            &camera_position_,
    //            D3DXVec2Scale(&delta, D3DXVec2Normalize(&delta, &delta), 8.0f * time_since_last_update));
    D3DXVec2Lerp(&camera_position_,
                 &camera_position_,
                 &camera_position_target_, 0.1f);
  } else {
    camera_position_ = camera_position_target_;
  }


  if (SUCCEEDED(d3d_device->BeginScene())) {
    d3d_device->Clear(0,
                      NULL,
                      D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                      D3DCOLOR_XRGB(0,0,0),
                      1.0f,
                      0);
    camera_.setPosition(camera_position_.x, camera_position_.y);
    camera_.initialize3DRendering(d3d_device);

    { // Update the map
      int tile_x = int(camera_position_.x),
          tile_y = int(camera_position_.y);
      Evidyon::World::MapVisibilityRect::Area r[2];
      visibility_.moveTo(
        tile_x,
        tile_y,
        Evidyon::World::CACHED_HORIZONTAL_EXTENT,
        Evidyon::World::CACHED_VERTICAL_EXTENT,
        &r[0],
        &r[1]);
      RECT rc[2] = {
        // these rectangles have to be grown for two reasons:
        // 1. the returned rectangles have an inclusive right/bottom,
        //    but the context uses exclusive rects
        // 2. the tile area that is updated needs to be larger on all
        //    sides by 1 tile than the actual area needed so that the
        //    map's 1st-order transition elements are calculated correctly
        { r[0].left - 1, r[0].top - 1, r[0].right + 2, r[0].bottom + 2 },
        { r[1].left - 1, r[1].top - 1, r[1].right + 2, r[1].bottom + 2 },
      };
      if (r[0].valid()) {
        map_->buildContextContents(&rc[0], &map_compilation_context_);
      }
      if (r[1].valid()) {
        map_->buildContextContents(&rc[1], &map_compilation_context_);
      }
      map_renderer_.update(this, tile_x, tile_y);

      // Calculate the chaos effect
      if (r[0].valid() || r[1].valid()) {
      //  int number_of_chaos_tiles = 0;
      //  int total_tiles = (visibility_.bottom() - visibility_.top() + 1)
      //                  * (visibility_.right() - visibility_.left() + 1);
      //  for (int y = visibility_.top(); y <= visibility_.bottom(); ++y) {
      //    for (int x = visibility_.left(); x <= visibility_.right(); ++x) {
      //      using namespace Evidyon::World;
      //      number_of_chaos_tiles
      //        += map_compilation_context_.atOrDefault(x, y)
      //            ->this_layer.navigability == NAVIGABILITY_CHAOS ? 1 : 0;
      //    }
      //  }

        // Set the fog factor
        //float f = 1.0f - float(number_of_chaos_tiles) / float(total_tiles);
        float f = 1.0f;
        f *= f;
        float a = 400.0f + 500.0f * f, b = f*100.0f + 300.0f;
        d3d_device->SetRenderState(D3DRS_FOGENABLE, TRUE);
        d3d_device->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(0,0,0));
        d3d_device->SetRenderState(D3DRS_FOGSTART, *(DWORD*)&(f=b));
        d3d_device->SetRenderState(D3DRS_FOGEND, *(DWORD*)&(f=a));
        d3d_device->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
      }
    }

    EvidyonD3DLightManager* light_manager
      = editor_->getLightManager();
    light_manager->setNightAmbience();
    light_manager->beginScene(camera_position_.x, camera_position_.y);

    EvidyonD3DRenderStateManager* renderstate_manager
      = editor_->getRenderStateManager();
    Scenery::SceneryRenderer* scenery_renderer
      = editor_->getSceneryRenderer();
    SkinnedMesh::SkinnedMeshRenderer* skinned_mesh_renderer
      = editor_->getSkinnedMeshRenderer();
    Evidyon::Actor::ActorRenderer* actor_renderer
      = editor_->getActorRenderer();

    for (SwarmSpawnActors::iterator i = swarm_spawn_actors_.begin();
         i != swarm_spawn_actors_.end(); ++i) {
      actor_renderer->updateInstance(i->second,
                                     time,
                                     time_since_last_update,
                                     this);
    }

    // Initialize rendering
    renderstate_manager->beginScene();
    scenery_renderer->beginScene();
    visual_fx_renderer->beginScene();
    skinned_mesh_renderer->beginScene();

    // Render all of the objects in the world
    d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    size_t number_of_textures = editor->getTextureTable()->getMembers().size();
    for (Texture::TextureIndex texture = 0;
                               texture < number_of_textures;
                             ++texture) {
    d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
      map_renderer_.render(texture,
                           renderstate_manager,
                           camera_position_.x,
                           camera_position_.y,
                           40.0f);
      visual_fx_renderer->render(texture);

      if (!layer_brush_affected_areas_.empty()) {
        // render scenery in wireframe when drawing
        d3d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        scenery_renderer->render(texture, renderstate_manager);
        skinned_mesh_renderer->render(texture, renderstate_manager);
        d3d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
      } else {
        scenery_renderer->render(texture, renderstate_manager);
        skinned_mesh_renderer->render(texture, renderstate_manager);
      }
    }

    if (GetAsyncKeyState(VK_TAB)) {
      d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
      d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
      d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
      d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
      d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
      d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
      d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
      d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
      d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
      d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
      d3d_device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
      d3d_device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
      d3d_device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
      renderstate_manager->setWorldMatrix(0, NULL);
      RECT visibility_area = {
        visibility_.left(),
        visibility_.top(),
        visibility_.right(),
        visibility_.bottom()
      };
      for (int y = visibility_area.top; y < visibility_area.bottom; ++y) {
        for (int x = visibility_area.left; x < visibility_area.right; ++x) {
          D3DCOLOR color;
          using namespace Evidyon::World;
          Evidyon::World::Tools::MapLocationData* location
            = map_compilation_context_.atOrDefault(x,y);
          Navigability navigability
            = location->this_layer.navigability;
          if (navigability == NAVIGABILITY_NORMAL) continue;
          switch (navigability) {
            case NAVIGABILITY_NO_TELEPORTING:   color = D3DCOLOR_ARGB(128,255,0,128); break;
            case NAVIGABILITY_WADE:             color = D3DCOLOR_ARGB(128,0,128,255); break;
            case NAVIGABILITY_SWIM:             color = D3DCOLOR_ARGB(128,0,0,255); break;
            case NAVIGABILITY_PIT:              color = D3DCOLOR_ARGB(128,0,0,0); break;
            case NAVIGABILITY_LOWWALL:          color = D3DCOLOR_ARGB(128,255,128,64); break;
            case NAVIGABILITY_HIGHWALL:         color = D3DCOLOR_ARGB(128,128,64,0); break;
            case NAVIGABILITY_SAFEZONE:         color = D3DCOLOR_ARGB(128,255,255,255); break;
            case NAVIGABILITY_IMPASSABLE:       color = D3DCOLOR_ARGB(128,255,0,0); break;
          }
          if (location->this_layer.trigger != Trigger::INVALID_TRIGGER_INDEX) {
            color = D3DCOLOR_ARGB(255,255,255,0);
          }
          struct Vertex {
            FLOAT x, y, z;
            D3DCOLOR diffuse;
          } vertices[4] = {
            { x*1.0f, 0.0f, y*1.0f, color },
            { float(x*1.0f+1.0f), 0.0f, y*1.0f, color },
            { float(x*1.0f+1.0f), 0.0f, (y+1)*1.0f, color },
            { x*1.0f, 0.0f, (y+1)*1.0f, color },
          };
          d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
        }
      }
      d3d_device->SetRenderState(D3DRS_ZENABLE, TRUE);
    }


    d3d_device->EndScene();
  }
  return true;
}



//----[  setPenActionAsLayerBrush  ]-------------------------------------------
void MapEditingDialog::setPenActionAsLayerBrush(World::Tools::MapMask* mask,
                                                D3DCOLOR color) {
  pen_action_ = PENACTION_LAYERBRUSH;
  layer_brush_type_ = LAYERBRUSH_SQUARE;
  layer_brush_mask_ = mask;
  layer_brush_color_ = color;
}



//----[  setCameraLocation  ]--------------------------------------------------
void MapEditingDialog::setCameraLocation(float x, float y) {
  camera_position_.x = x;
  camera_position_.y = y;
  camera_position_target_ = camera_position_;
}


//----[  updateMapTileData  ]--------------------------------------------------
void MapEditingDialog::updateMapTileData(
    int x,
    int y,
    Evidyon::World::LocationVisualData* data) {

  if ((layer_brush_affected_areas_.find(Location(x,y)) != layer_brush_affected_areas_.end()) ||
      ((layer_brush_last_tile_x_ == x) && (layer_brush_last_tile_y_ == y))) {
    // apply painting previews
    memcpy(data,
           &map_compilation_context_.default_location.this_layer.visual_data,
           sizeof(*data));
    map_->buildBrushPreview(layer_brush_mask_, layer_brush_color_, data);
  } else {
    memcpy(data,
          &map_compilation_context_.atOrDefault(x,y)->this_layer.visual_data,
           sizeof(*data));
  }
  updateSwarmSpawnLocation(x, y,
    map_compilation_context_.atOrDefault(x,y));
}



//----[  SetPenActionAsLayerBrushFunctionoid  ]--------------------------------
class SetPenActionAsLayerBrushFunctionoid
  : public MaskColorSelectorDialogFunctionoid {
public:
  SetPenActionAsLayerBrushFunctionoid(MapEditingDialog* dialog) {
    dialog_ = dialog;
  }
  virtual void execute(
      Evidyon::MapMaskColorSelectorDialog* dialog,
      Evidyon::World::Tools::Map* map,
      Evidyon::MapMaskColorSelectorDialog::MapMaskElement* selected_mask,
      D3DCOLOR selected_color,
      bool* close_dialog,
      int texture_mouse_x,
      int texture_mouse_y) {
    dialog_->setPenActionAsLayerBrush(selected_mask, selected_color);
    *close_dialog = true;
  }
  MapEditingDialog* dialog_;
};





//----[  SetCameraMapLocationFunctionoid  ]------------------------------------
class SetCameraMapLocationFunctionoid
  : public MaskColorSelectorDialogFunctionoid {
public:
  SetCameraMapLocationFunctionoid(MapEditingDialog* dialog) {
    dialog_ = dialog;
  }
  virtual void execute(
      Evidyon::MapMaskColorSelectorDialog* dialog,
      Evidyon::World::Tools::Map* map,
      Evidyon::MapMaskColorSelectorDialog::MapMaskElement* selected_mask,
      D3DCOLOR selected_color,
      bool* close_dialog,
      int texture_mouse_x,
      int texture_mouse_y) {
    dialog_->setCameraLocation(texture_mouse_x, texture_mouse_y);
    *close_dialog = true;
  }
  MapEditingDialog* dialog_;
};



//----[  dialogMenuNotification  ]---------------------------------------------
INT_PTR MapEditingDialog::dialogMenuNotification(HWND hDlg,
                                                 HMENU hMenu,
                                                 WORD ItemID,
                                                 WORD ItemIndex) {

  return 0;
}



//----[  dialogEvent  ]------------------------------------------------------
INT_PTR MapEditingDialog::dialogEvent(HWND hDlg,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam) {
  if (uMsg == WM_CLOSE) {
    editor_->removeD3DWindow(this);
    EndDialog(hDlg, 0);
    DestroyWindow(hDlg); 
    SetWindowLong(hDlg, GWL_USERDATA, NULL);
    delete this;
  } else if (uMsg == WM_MBUTTONDOWN ||
             (uMsg == WM_RBUTTONDOWN && (MK_CONTROL==(wParam&MK_CONTROL)))) {
    POINTS pt = MAKEPOINTS(lParam);
    camera_.getWorldLocation(
      pt.x,
      pt.y,
      &camera_position_target_.x,
      &camera_position_target_.y,
      NULL);
  } else if (uMsg == WM_LBUTTONDBLCLK) {
    RECT area_to_rebuild = { 9999, 9999, -9999, -9999 };
    for (LocationSet::iterator i = layer_brush_affected_areas_.begin();
                               i != layer_brush_affected_areas_.end();
                             ++i) {
      int x = i->first, y = i->second;
      layer_brush_mask_->paint(x, y, layer_brush_color_);
      area_to_rebuild.left   = min(x, area_to_rebuild.left);
      area_to_rebuild.top    = min(y, area_to_rebuild.top);
      area_to_rebuild.right  = max(x, area_to_rebuild.right);
      area_to_rebuild.bottom = max(y, area_to_rebuild.bottom);
    }
    layer_brush_affected_areas_.clear();
    area_to_rebuild.left -= 1;
    area_to_rebuild.top -= 1;
    area_to_rebuild.right += 2;
    area_to_rebuild.bottom += 2;
    map_->buildContextContents(&area_to_rebuild, &map_compilation_context_);
    map_renderer_.reset();
  } else if (uMsg == WM_SIZE) {
    camera_.rebuildTopdownCamera(LOWORD(lParam),HIWORD(lParam));
  } else if (uMsg == WM_RBUTTONDBLCLK) { // context menu
    enum {
      DRAW_ON_MASK = 5000,
      CANCEL_CHANGES = 4000,
      GO_TO_LOCATION = 4001,
      CREATE_INHABITANT = 4002,
      ERASE_INHABITANT = 4003,
      WHAT_IS_THIS_COORD = 4004,
    };
    POINTS clicked_at_points = MAKEPOINTS(lParam);
    POINT clicked_at = {clicked_at_points.x, clicked_at_points.y};
    ClientToScreen(hDlg, &clicked_at);
    MENUITEMINFO menu_item;

    HMENU hMenu = CreatePopupMenu();
    HMENU hMasksSubmenu = createMapMasksMenu(DRAW_ON_MASK);


    dcx::dcxWin32InitMenuItem(&menu_item, "Cancel Current Edit", false, CANCEL_CHANGES);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Go to Location", true, GO_TO_LOCATION);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "What is this coordinate?", false, WHAT_IS_THIS_COORD);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Create Inhabitant", false, CREATE_INHABITANT);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItem(&menu_item, "Erase Inhabitant", false, ERASE_INHABITANT);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    dcx::dcxWin32InitMenuItemSeperator(&menu_item);
    InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);

    const dc::dcGenericResource::Array& map_masks = map_->getMapMasks();
    if (map_masks.empty() == false) {
      dcx::dcxWin32InitMenuItem(&menu_item, "Draw on Mask", hMasksSubmenu);
      InsertMenuItem(hMenu, 0xFFFFFFFF, TRUE, &menu_item);
    }

    SetForegroundWindow(hDlg);
    PostMessage(hDlg, WM_NULL, 0, 0);
    int selected = (int)TrackPopupMenu(hMenu,
                                       TPM_RETURNCMD|TPM_LEFTBUTTON,
                                       clicked_at.x,
                                       clicked_at.y,
                                       0,
                                       (HWND)(hDlg),
                                       NULL);
    DestroyMenu(hMasksSubmenu);
    DestroyMenu(hMenu);

    // Check to see whether or not the user picked an item
    if (selected == CANCEL_CHANGES) {
      layer_brush_affected_areas_.clear();
      map_renderer_.reset();
    } else if (selected == GO_TO_LOCATION) {
      Evidyon::MapMaskColorSelectorDialog* mask_dialog
        = new Evidyon::MapMaskColorSelectorDialog();
      SetCameraMapLocationFunctionoid* functionoid
        = new SetCameraMapLocationFunctionoid(this);
      typedef dc::dcList<Evidyon::World::Tools::MapMask>::Element MaskType;
      if (!mask_dialog->begin(hDlg,
                              editor_,
                              map_,
                              layer_brush_mask_ == NULL ? (MaskType*)(map_masks.at(0)) : (MaskType*)layer_brush_mask_,
                              functionoid)) {
        delete functionoid;
        delete mask_dialog;
      } 
    } else if (selected == CREATE_INHABITANT) {
      float x, y;
      POINTS pt = MAKEPOINTS(lParam);
      camera_.getWorldLocation(
        pt.x,
        pt.y,
        &x,
        &y,
        NULL);
      dc::dcGenericResource* lifeform = NULL;
      if (DirectCraftActions::PromptSelectSingleResource(
          editor_->topWindow(),
          "Create Inhabitant",
          "Select the inhabitant to create",
          editor_->getLifeformAIs(),
          &lifeform)) {
        map_->createInhabitant(
          x,
          y,
          (Evidyon::Tools::LifeformAIElement*)lifeform);
      }
    } else if (selected == ERASE_INHABITANT) {
      float x, y;
      POINTS pt = MAKEPOINTS(lParam);
      camera_.getWorldLocation(
        pt.x,
        pt.y,
        &x,
        &y,
        NULL);
      map_->eraseInhabitant(int(x), int(y));
    } else if (selected == WHAT_IS_THIS_COORD) {
      float x, y;
      POINTS pt = MAKEPOINTS(lParam);
      camera_.getWorldLocation(
        pt.x,
        pt.y,
        &x,
        &y,
        NULL);
      char message[128];
      sprintf_s(message, 128, "%i, %i", (int)x, (int)y);
      MessageBox(hDlg, message, "Current Coordinate Is...", MB_OK);
    } else if (selected >= DRAW_ON_MASK && selected < DRAW_ON_MASK + map_masks.size()) {
      Evidyon::MapMaskColorSelectorDialog* mask_dialog
        = new Evidyon::MapMaskColorSelectorDialog();
      SetPenActionAsLayerBrushFunctionoid* functionoid
        = new SetPenActionAsLayerBrushFunctionoid(this);
      typedef dc::dcList<Evidyon::World::Tools::MapMask>::Element MaskType;
      if (!mask_dialog->begin(hDlg,
                              editor_,
                              map_,
                              (MaskType*)(map_masks.at(selected - DRAW_ON_MASK)),
                              functionoid)) {
        delete functionoid;
        delete mask_dialog;
      }
    } else {
      // nothing selected
    }
  } else if (uMsg == WM_PAINT) {
    editor_->renderD3DWindow(this);
  } else if (uMsg == WM_LBUTTONDOWN) {
    if (layer_brush_mask_) {
      layerBrushOverScreenPoint(lParam, true, true, false);
      float x, y;
      POINTS pt = MAKEPOINTS(lParam);
      camera_.getWorldLocation(
        pt.x,
        pt.y,
        &x,
        &y,
        NULL);
      int tile_x = int(x);
      int tile_y = int(y);
      layer_brush_affected_areas_.insert(Location(tile_x, tile_y));
      layer_brush_last_tile_x_ = tile_x;
      layer_brush_last_tile_y_ = tile_y;
      mouse_down_tile_x_ = tile_x;
      mouse_down_tile_y_ = tile_y;
      map_renderer_.reset();
    }
  } else if (uMsg == WM_KEYDOWN) {
    switch (wParam) {
      case VK_ESCAPE: {
        layer_brush_affected_areas_.clear();
        map_renderer_.reset();
      } break;
      case VK_RETURN: {
        RECT area_to_rebuild = { 9999, 9999, -9999, -9999 };
        for (LocationSet::iterator i = layer_brush_affected_areas_.begin();
                                   i != layer_brush_affected_areas_.end();
                                 ++i) {
          int x = i->first, y = i->second;
          layer_brush_mask_->paint(x, y, layer_brush_color_);
          area_to_rebuild.left   = min(x, area_to_rebuild.left);
          area_to_rebuild.top    = min(y, area_to_rebuild.top);
          area_to_rebuild.right  = max(x, area_to_rebuild.right);
          area_to_rebuild.bottom = max(y, area_to_rebuild.bottom);
        }
        layer_brush_affected_areas_.clear();
        area_to_rebuild.left -= 1;
        area_to_rebuild.top -= 1;
        area_to_rebuild.right += 2;
        area_to_rebuild.bottom += 2;
        map_->buildContextContents(&area_to_rebuild, &map_compilation_context_);
        map_renderer_.reset();
      } break;
    }
  } else if (uMsg == WM_MOUSEMOVE) {
    if ((wParam & MK_MBUTTON) ||
        ((wParam &MK_RBUTTON) && (wParam&MK_CONTROL))) {
      POINTS pt = MAKEPOINTS(lParam);
      camera_.getWorldLocation(
        pt.x,
        pt.y,
        &camera_position_target_.x,
        &camera_position_target_.y,
        NULL);
    } else {
      layerBrushOverScreenPoint(lParam,
                                (wParam&MK_LBUTTON) == MK_LBUTTON,
                                false,
                                (wParam & MK_RBUTTON) == MK_RBUTTON);
    }
  }

  return 0;
}





//----[  locationIsVisible  ]--------------------------------------------------
bool MapEditingDialog::locationIsVisible(int x, int z) {
  //map_compilation_context_.atOrDefault(x,z)->this_layer.
  return true;
}



//----[  layerBrushPaintScreenPoint  ]-----------------------------------------
void MapEditingDialog::layerBrushOverScreenPoint(LPARAM lParam,
                                                 bool pen_down,
                                                 bool force_update,
                                                 bool rectangle) {
  if (pen_action_ == PENACTION_LAYERBRUSH && layer_brush_mask_) {
    float x, y;
    POINTS pt = MAKEPOINTS(lParam);
    camera_.getWorldLocation(
      pt.x,
      pt.y,
      &x,
      &y,
      NULL);
    int tile_x = int(x);
    int tile_y = int(y);
    if (force_update || (layer_brush_last_tile_x_ != tile_x ||
                         layer_brush_last_tile_y_ != tile_y)) {
      if (pen_down) {
        if (rectangle) {
          for (int y = tile_y; y <= mouse_down_tile_y_; ++y) {
            for (int x = tile_x; x <= mouse_down_tile_x_; ++x) {
              layer_brush_affected_areas_.insert(Location(x, y));
            }
            for (int x = mouse_down_tile_x_; x <= tile_x; ++x) {
              layer_brush_affected_areas_.insert(Location(x, y));
            }
          }
          for (int y = mouse_down_tile_y_; y <= tile_y; ++y) {
            for (int x = tile_x; x <= mouse_down_tile_x_; ++x) {
              layer_brush_affected_areas_.insert(Location(x, y));
            }
            for (int x = mouse_down_tile_x_; x <= tile_x; ++x) {
              layer_brush_affected_areas_.insert(Location(x, y));
            }
          }
        } else {
          layer_brush_affected_areas_.insert(Location(tile_x, tile_y));
        }
      }
      layer_brush_last_tile_x_ = tile_x;
      layer_brush_last_tile_y_ = tile_y;
      map_renderer_.reset();
    }
  }
}



//----[  createMapMasksMenu  ]-------------------------------------------------
HMENU MapEditingDialog::createMapMasksMenu(int base_id) {
  HMENU hMasksSubmenu = CreateMenu();

  const dc::dcGenericResource::Array& map_masks = map_->getMapMasks();
  for (dc::dcGenericResource::Array::const_iterator i = map_masks.begin();
    i != map_masks.end(); ++i) {
    World::Tools::MapMask* map_mask = (World::Tools::MapMask*)(*i);
    MENUITEMINFO menu_item;
    dcx::dcxWin32InitMenuItem(&menu_item,
                              map_mask->getName().c_str(),
                              false,
                              base_id + map_mask->getIndex());
    InsertMenuItem(hMasksSubmenu, 0xFFFFFFFF, TRUE, &menu_item);
  }

  return hMasksSubmenu;
}




//----[  updateSwarmSpawnLocation  ]-------------------------------------------
void MapEditingDialog::updateSwarmSpawnLocation(
    int x,
    int y,
    const Evidyon::World::Tools::MapLocationData* map_location_data) {
  Location location(x,y);
  SwarmSpawnActors::iterator i = swarm_spawn_actors_.find(location);
  Evidyon::Actor::ActorRenderer* actor_renderer = editor_->getActorRenderer();
  if (i != swarm_spawn_actors_.end()) {
    actor_renderer->release(i->second);
    swarm_spawn_actors_.erase(i);
  }
  Evidyon::Actor::RenderableActorInstance* new_instance = NULL;
  int number_of_spawns = map_location_data->this_layer.number_of_spawns;
  if (number_of_spawns == 0 ||
      map_location_data->this_layer.spawn_type ==
        Evidyon::World::Tools::MapLocationData::INVALID_SPAWN_TYPE) return;
  Evidyon::LifeformAI::LifeformAIIndex lifeform_ai
    = map_location_data->this_layer.spawn[rand()%number_of_spawns];
  Evidyon::Tools::LifeformAIElement* lifeform_ai_element
    = (Evidyon::Tools::LifeformAIElement*)
        editor_->getLifeformAIs().at(lifeform_ai);
  Evidyon::Tools::ActorProfileElement* actor_profile;
  Evidyon::Tools::SkinnedMeshFormElement* form;
  if (!lifeform_ai_element
        ->getImplementation()
        ->preview(&actor_profile, &form)) {
    return;
  }
  Evidyon::Actor::ActorTemplate default_actor_template;
  actor_profile->compileDefaultTemplate(&default_actor_template);
  new_instance = actor_renderer->acquire(
    &default_actor_template,
    actor_profile->getIndex(),
    form->getIndex(),
    true);
  if (new_instance) {
    actor_renderer->updateEquipment(new_instance, NULL, true);
    new_instance->skinned_mesh_instance->hidden = false;
    new_instance->current_movement_state.x = float(x) + 0.5f;
    new_instance->current_movement_state.y = float(y) + 0.5f;
    new_instance->last_sync.facing_angle = randfAngleRadians();
    swarm_spawn_actors_.insert(
      SwarmSpawnActors::value_type(location, new_instance));
  }
}


}