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



#pragma comment(lib, "d3d9.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "dc_d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dc.lib")
#endif



namespace Evidyon {
namespace AnimatedMesh {

struct AnimatedMeshFrameData;
struct AnimatedMeshBoneData;
struct AnimatedMeshTextureGroup;

//----[  AnimatedMeshData  ]---------------------------------------------------
struct AnimatedMeshData {
  unsigned int number_of_texture_groups;
  unsigned int number_of_frames;
  unsigned int number_of_bones;
  AnimatedMeshFrameData* frames;
  AnimatedMeshBoneData* bones;
  AnimatedMeshTextureGroup* texture_groups;
};


static const unsigned int INVALID_FRAME_INDEX = 0xFFFFFFFF;


//----[  AnimatedMeshFrameData  ]----------------------------------------------
struct AnimatedMeshFrameData {

  // This frame's offset from its parent in the hierarchy
  D3DXMATRIX offset;

  // This must be less than this frame's index.  The only exception is bone 0,
  // the root bone, for which this value must be INVALID_FRAME_INDEX.
  DWORD parent_frame_index;
};


//----[  AnimatedMeshBoneData  ]-----------------------------------------------
struct AnimatedMeshBoneData {
  /// Multiply the source frame's adjusted offset by this matrix before setting into the device
  D3DXMATRIXA16 inverse_offset;

  /// The frame index from which this bone's transformation is derived
  DWORD frame_index;
};



//----[  AnimatedMeshVertex  ]-------------------------------------------------
struct AnimatedMeshVertex {
  float x, y, z;
  float bone_weights[4];
  unsigned char bone_matrix_indices[4];
  float nx, ny, nz;
  float u, v;
};
#define D3DFVF_EVIDYON_ANIMATEDMESHVERTEX (D3DFVF_XYZB4|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_LASTBETA_UBYTE4)



//----[  AnimatedMeshIndex  ]--------------------------------------------------
typedef unsigned short AnimatedMeshIndex;
#define D3DFFMT_EVIDYON_ANIMATEDMESHINDEX (D3DFMT_INDEX16)






//----[  AnimatedMeshTextureGroup  ]-------------------------------------------
struct AnimatedMeshTextureGroup {
  AnimatedMeshVertex* vertices;
  unsigned int number_of_vertices;
  AnimatedMeshIndex* indices;
  unsigned int number_of_indices;
};

// loader should bear responsibility of splitting up animated mesh texture
// groups when bringing into memory.
// subsets of animated mesh are (animated mesh, texture group, subset #)
//    like mesh renderer!  initialize (..., ..., ...) then render-render-render...
//      for a subset, the renderer returns array of skeleton indices to set
//      such that ARRAY[...] for each i in ARRAY:  setTransform(WORLD(i), bones[ARRAY[i]])

/*


algo:
convert mesh to standard format
build frame name -> hierarchy index
build bone table
for each bone, get the set of (vertex index, weight) pairs it affects
clean up all vertex influences

*/
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
      Evidyon::TextureDescription td;
      Evidyon::Scenery::EvidyonSceneryRenderer scenery_renderer;
      td.flip_horizontal = false;
      td.flip_vertical = false;
      td.animation_type = Evidyon::TextureDescription::STATIC;
      td.blend_type = Evidyon::TextureDescription::DISABLE_BLENDING;
      renderstate_manager.create(d3d.getDevice());
      D3DXCreateTextureFromFile(d3d.getDevice(), "grass.jpg", &td.d3d_texture);
      renderstate_manager.addTexture(0, td);
      D3DXCreateTextureFromFile(d3d.getDevice(), "dirt.png", &td.d3d_texture);
      renderstate_manager.addTexture(1, td);
      D3DXCreateTextureFromFile(d3d.getDevice(), "water-transparency.png", &td.d3d_texture);
      td.animation_type = Evidyon::TextureDescription::CIRCLING;
      td.animation.circling.radius = 0.5f;
      td.animation.circling.speed = 0.5f;
      td.blend_type = Evidyon::TextureDescription::ALPHABLEND;
      renderstate_manager.addTexture(2, td);

      Evidyon::EvidyonD3DLightManager light_manager;
      light_manager.create(d3d.getDevice());
      Evidyon::EvidyonD3DLightManager::PointLight* light =
        light_manager.allocateSmallLight(D3DCOLOR_XRGB(255,0,0));
      light->h = 0.0f;
      light->v = 0.0f;

      EvidyonCamera camera;

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

              float x = + 3*sin(GetTickCount()/2000.0);
              float y = + 4*cos(GetTickCount()/2000.0);


              map.update(x,y+0.5f);
              light->h = sinf(GetTickCount() / 250.0f);

              light_manager.updateAmbientLight(GetTickCount()/1000.0);
              light_manager.beginScene(x, y);
              camera.setPosition(x, y);

              renderstate_manager.beginScene();
              scenery_renderer.beginScene();

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



