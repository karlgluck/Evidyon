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
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )


#include "visibletilesrect.h"
#include "evidyonmap.h"
#include "../common/promotedwarnings.h"
#include "../common/isometriccamera.h"
#include "terrain.h"
#include "map.h"
#include "../shared/evidyond3drenderstatemanager.h"
#include "../shared/evidyond3dlightmanager.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9d.lib")




namespace Evidyon {
namespace Map {




MapManager::MapManager() {
}



//----[  create  ]-------------------------------------------------------------
bool MapManager::create(LPDIRECT3DDEVICE9 d3d_device) {
  if (!map_.create(d3d_device)) {
    destroy();
    return false;
  }
srand(GetTickCount());
  return true;
}



//----[  destroy  ]------------------------------------------------------------
void MapManager::destroy() {
  map_.destroy();
}





//----[  updateMapTileData  ]--------------------------------------------------
void MapManager::updateMapTileData(int x, int y, EvidyonMapTile_Data* data) {
  data->fluid_texture = INVALID_TEXTURE_INDEX;
  data->occluding = false;
  data->wall_texture = INVALID_TEXTURE_INDEX;
  data->wall_type = WALLTYPE_HIGH;
  data->terrain_height = 0.0f;
  data->terrain_texture_rotation = rand()%4;
  data->terrain_texture = 0;
  if ((y >= -3 && y <= 3 && x >= -3 && x <= 3) ||
      (y >= 2 && y <= 7 && x >= 2 && x <= 5)) {
    if ((y > -3 && y < 3 && x > -3 && x < 3) ||
        (y > 2 && y < 7 && x > 2 && x < 5)) {
      data->terrain_height = -2.0f;
      data->terrain_texture = 1;
      //data->wall_type = WALLTYPE_LOW;
      //data->wall_texture = 1;
      //data->terrain_texture = INVALID_TEXTURE_INDEX;
      //data->occluding = true;
      data->fluid_texture = 2;
    } else {
      data->terrain_height = -2.0f;
      data->terrain_texture = 1;
      data->fluid_texture = 2;
      data->wall_type = WALLTYPE_FLOOR;
      //data->wall_texture = 1;
      //data->terrain_height = 0;
      //data->terrain_texture = 1;
      //data->terrain_texture = INVALID_TEXTURE_INDEX;
      //data->occluding = true;
    }
  }
  if (((y == 0) && (x < -5 || x > 5)) ||
      ((x == -6) && (y<0)) ){
    data->wall_type = WALLTYPE_HIGH;
    data->wall_texture = 1;
    data->occluding = true;
  }
}

void MapManager::update(int viewer_x, int viewer_y) {
  map_.update(this, viewer_x, viewer_y);
}

// Draw the map to the screen
bool MapManager::render(LPDIRECT3DDEVICE9 d3d_device,
                        TextureIndex texture,
                        EvidyonD3DRenderStateManager* renderstate_manager) {
  return map_.render(texture, renderstate_manager);                            
}




}
}

class D3DRenderingManager {
public:
  virtual LPDIRECT3DDEVICE9 acquireDevice() = 0;
private:
};

class D3DRenderingManagerSimple : public D3DRenderingManager {
public:
  bool create(HWND hWnd);
  void destroy();
  LPDIRECT3DDEVICE9 getDevice() { return d3d_device_; }
  virtual LPDIRECT3DDEVICE9 acquireDevice() { d3d_device_->AddRef(); return d3d_device_; }
private:
  LPDIRECT3D9 d3d_;
  LPDIRECT3DDEVICE9 d3d_device_;
};

bool D3DRenderingManagerSimple::create(HWND hWnd) {
  // Create the D3D object.
  if( NULL == ( d3d_ = Direct3DCreate9( D3D_SDK_VERSION ) ) )
      return false;

  // Set up the structure used to create the D3DDevice. Since we are now
  // using more complex geometry, we will create a device with a zbuffer.
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory( &d3dpp, sizeof( d3dpp ) );
  d3dpp.Windowed = TRUE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

  // Create the D3DDevice
  if( FAILED( d3d_->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                    &d3dpp, &d3d_device_ ) ) )
  {
      return false;
  }

  // Turn on the zbuffer
  d3d_device_->SetRenderState( D3DRS_ZENABLE, TRUE );

  // Turn on ambient lighting 
  d3d_device_->SetRenderState( D3DRS_LIGHTING, FALSE );

  return true;
}


void D3DRenderingManagerSimple::destroy() {
  if( d3d_device_ != NULL )
      d3d_device_->Release();

  if( d3d_ != NULL )
      d3d_->Release();

  d3d_device_ = NULL;
  d3d_ = NULL;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        "D3D Tutorial", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "D3D Tutorial", "D3D Tutorial 06: Meshes",
                              WS_OVERLAPPEDWINDOW, 10, 10, 1000, 1000,
                              NULL, NULL, wc.hInstance, NULL );

    D3DRenderingManagerSimple d3d;
    

    // Initialize Direct3D
    if( d3d.create( hWnd ) )
    {
      Evidyon::EvidyonD3DRenderStateManager renderstate_manager;
      Evidyon::EvidyonD3DRenderStateManager::TextureDescription td;
      td.flip_horizontal = false;
      td.flip_vertical = false;
      td.animation_type = Evidyon::EvidyonD3DRenderStateManager::TextureDescription::STATIC;
      td.blend_type = Evidyon::EvidyonD3DRenderStateManager::TextureDescription::DISABLE_BLENDING;
      renderstate_manager.create(d3d.getDevice());
      D3DXCreateTextureFromFile(d3d.getDevice(), "grass.jpg", &td.d3d_texture);
      renderstate_manager.addTexture(0, td);
      D3DXCreateTextureFromFile(d3d.getDevice(), "dirt.png", &td.d3d_texture);
      renderstate_manager.addTexture(1, td);
      D3DXCreateTextureFromFile(d3d.getDevice(), "water-transparency.png", &td.d3d_texture);
      td.animation_type = Evidyon::EvidyonD3DRenderStateManager::TextureDescription::CIRCLING;
      td.animation.circling.radius = 0.5f;
      td.animation.circling.speed = 0.5f;
      td.blend_type = Evidyon::EvidyonD3DRenderStateManager::TextureDescription::ALPHABLEND;
      renderstate_manager.addTexture(2, td);

      Evidyon::EvidyonD3DLightManager light_manager;
      light_manager.create(d3d.getDevice());
      Evidyon::EvidyonD3DLightManager::PointLight* light =
        light_manager.allocateSmallLight(D3DCOLOR_XRGB(255,0,0));
      light->h = 0.0f;
      light->v = 0.0f;

      //Evidyon::Map::TerrainQuadBuffer terrain;
      Evidyon::Map::MapManager map;
      map.create(d3d.getDevice());
      IsometricCamera camera;

      // Show the window
      ShowWindow( hWnd, SW_SHOWDEFAULT );
      UpdateWindow( hWnd );

      DWORD start = GetTickCount();
      // Enter the message loop
      MSG msg;
      ZeroMemory( &msg, sizeof( msg ) );
      while( msg.message != WM_QUIT )
      {
          if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
          {
              TranslateMessage( &msg );
              DispatchMessage( &msg );
          }
          else {
            LPDIRECT3DDEVICE9 d3d_device = d3d.getDevice();
            camera.initialize3DRendering(d3d_device);
            if (SUCCEEDED(d3d_device->BeginScene())) {
              d3d_device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

              float x =  + 3*sin(GetTickCount()/2000.0);
              float y =  + 4*cos(GetTickCount()/2000.0);


              map.update(x,y+0.5f);
              light->h = sinf(GetTickCount() / 250.0f);

              light_manager.updateAmbientLight(GetTickCount()/1000.0);
              light_manager.beginScene(x, y);
              camera.setPosition(x, y);


              renderstate_manager.beginScene();
              map.render(d3d_device, 0, &renderstate_manager);
              map.render(d3d_device, 1, &renderstate_manager);
              map.render(d3d_device, 2, &renderstate_manager);
              d3d_device->EndScene();
              d3d_device->Present(NULL, NULL, NULL, NULL);
            }
          }
      }

      light_manager.destroy();
      renderstate_manager.destroy();
      map.destroy();
    }

    d3d.destroy();

    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}



