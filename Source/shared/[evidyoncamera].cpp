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
#include <d3dx9.h>
#include <dc/debug>
#include "evidyoncamera.h"


namespace Evidyon {


  
//----[  EvidyonCamera  ]------------------------------------------------------
EvidyonCamera::EvidyonCamera() {
  zero();
}





//----[  setPosition  ]--------------------------------------------------------
void EvidyonCamera::setPosition(float x, float z, float zoom) {
  static const float SCALING = 0.5f;

  D3DXMATRIX a, b;
  D3DXMatrixTranslation(&a, -x, 0.0f, -z);
  D3DXMatrixScaling(&b, 55.0*SCALING, 33.0*SCALING, 49.0*SCALING);
  D3DXMatrixMultiply(&a, &a, &b);

  const FLOAT SWIZZLE_MATRIX[] = {
    1.0,  0.0,  0.0,  0.0,
    0.0,  myTiltWorldFlag?(-0.67-1.0599993) : 0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  1.0,
  };

  D3DXMATRIX swizzle(SWIZZLE_MATRIX);
  D3DXMatrixMultiply(&swizzle, &a, &swizzle);


  const FLOAT VIEW_MATRIX[] = {
    1.0,  0.0,    0.0,  0.0,
	  0.0, -1.0,    0.0,  0.0,
	  0.0,  0.0,   -1.0,  0.0,
	  0.0,  0.0,  791.0*SCALING,  1.0,
  };


  const FLOAT PROJECTION_MATRIX[] = {
    2.414213, 0.000000,    0.00000,  0.000000,
    0.000000, 2.414213,    0.00000,  0.000000,
    0.000000, 0.000000,    1.14282,  1.000000,
    0.000000, 0.000000,	-571.408*SCALING*zoom,    0.000000,
  };

  view_matrix_ = D3DXMATRIX(VIEW_MATRIX);
  proj_matrix_ = D3DXMATRIX(PROJECTION_MATRIX);

  D3DXMatrixMultiply(&view_matrix_, &swizzle, &view_matrix_);
}





//----[  enableTopdownView  ]--------------------------------------------------
void EvidyonCamera::enableTopdownView(bool enable) {
  myTiltWorldFlag = !enable;
}





//----[  topdownViewEnabled  ]-------------------------------------------------
bool EvidyonCamera::topdownViewEnabled() const {
  return !myTiltWorldFlag;
}




//----[  rebuildTopdownCamera  ]-----------------------------------------------
void EvidyonCamera::rebuildTopdownCamera(DWORD screen_width, DWORD screen_height) {
  myScreenWidth = screen_width;
  myScreenHeight = screen_height;
}




//----[  initialize3DRendering  ]----------------------------------------------
bool EvidyonCamera::initialize3DRendering(LPDIRECT3DDEVICE9 d3dDevice)
{
  // If the parameter is invalid, we can't do anything
  if (APP_ERROR(!d3dDevice)("Invalid device provided to EvidyonCamera::initialize3DRendering"))
    return false;

  // Set up the matrices
  d3dDevice->SetTransform(D3DTS_VIEW, &view_matrix_);
  d3dDevice->SetTransform(D3DTS_PROJECTION, &proj_matrix_);

  // Success
  return true;
}




//----[  getWorldLocation  ]---------------------------------------------------
void EvidyonCamera::getWorldLocation(int screenX,
                                     int screenY,
                                     float* worldX,
                                     float* worldZ,
                                     const D3DXMATRIX* world_matrix) {
  D3DXVECTOR3 vPickRayDir;
  D3DXVECTOR3 vPickRayOrig;

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  (((2.0f * screenX) / myScreenWidth) - 1) / proj_matrix_._11;
  v.y = -(((2.0f * screenY) / myScreenHeight) - 1) / proj_matrix_._22;
  v.z =  1.0f;

  // Get the inverse view matrix
  D3DXMATRIX mWorldView;
  if (world_matrix) {
    D3DXMatrixMultiply(&mWorldView, world_matrix, &view_matrix_);
  } else {
    mWorldView = view_matrix_;
  }
  D3DXMATRIX m;
  D3DXMatrixInverse(&m, NULL, &mWorldView);

  // Transform the screen space Pick ray into 3D space
  vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
  vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
  vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
  vPickRayOrig.x = m._41;
  vPickRayOrig.y = m._42;
  vPickRayOrig.z = m._43;

  // Hooray! We have a pick-ray; now, find the intersection with the x/z plane
  float dist = -vPickRayOrig.y / vPickRayDir.y;
  if (worldX) *worldX = vPickRayOrig.x + dist * vPickRayDir.x;
  if (worldZ) *worldZ = vPickRayOrig.z + dist * vPickRayDir.z;
}




//----[  getScreenLocation  ]--------------------------------------------------
void EvidyonCamera::getScreenLocation(float worldX,
                                      float worldY,
                                      float worldZ,
                                      float* screenX,
                                      float* screenY,
                                      const D3DXMATRIX* world_matrix) {
  D3DVIEWPORT9 vpt = { 0, 0, myScreenWidth, myScreenHeight, 0.0f, 0.0f };
  D3DXVECTOR3 world(worldX, worldY, worldZ), screen;
  D3DXVec3Project(&screen, &world, &vpt, &proj_matrix_, &view_matrix_, world_matrix);
  if (screenX) *screenX = screen.x;
  if (screenY) *screenY = screen.y;
}







//----[  zero  ]---------------------------------------------------------------
void EvidyonCamera::zero() {
  myScreenWidth = myScreenHeight = 1; // Prevent divide-by-zero
  myTiltWorldFlag = true;
  D3DXMatrixIdentity(&view_matrix_);
  D3DXMatrixIdentity(&proj_matrix_);
}


}