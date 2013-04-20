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
#include <d3dx9.h>
#include <dc/debug>
#include "isometriccamera.h"




//-----------------------------------------------------------------------------
// Name:  IsometricCamera
// Desc:  Resets this class
//-----------------------------------------------------------------------------
IsometricCamera::IsometricCamera()
{
  // Set up default information
  zero();
}




//-----------------------------------------------------------------------------
// Name:  setPosition
// Desc:  Sets the location of the camera on the map
//-----------------------------------------------------------------------------
void IsometricCamera::setPosition(float x, float z, float zoom) {
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
  D3DXMatrixMultiply(&myLocationMatrix, &a, &swizzle);


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

  myViewMatrix = D3DXMATRIX(VIEW_MATRIX);
  myProjMatrix = D3DXMATRIX(PROJECTION_MATRIX);

  D3DXMatrixMultiply(&myViewMatrix, &myLocationMatrix, &myViewMatrix);
  D3DXMatrixIdentity(&myLocationMatrix);
}




//-----------------------------------------------------------------------------
// Name:  enableTopdownView
// Desc:  Changes whether or not this camera looks down on the world from directly above
//-----------------------------------------------------------------------------
void IsometricCamera::enableTopdownView(bool enable)
{
  myTiltWorldFlag = !enable;
}


bool IsometricCamera::topdownViewEnabled() const {
  return !myTiltWorldFlag;
}


void IsometricCamera::rebuildTopdownCamera(DWORD screen_width, DWORD screen_height) {
  myScreenWidth = screen_width;
  myScreenHeight = screen_height;
}


//-----------------------------------------------------------------------------
// Name:  initialize3DRendering
// Desc:  Sets up the destination device for rendering using this camera
//-----------------------------------------------------------------------------
bool IsometricCamera::initialize3DRendering(LPDIRECT3DDEVICE9 d3dDevice)
{
  // If the parameter is invalid, we can't do anything
  if (APP_ERROR(!d3dDevice)("Invalid device provided to IsometricCamera::initialize3DRendering"))
    return false;

  // Set up the matrices
  d3dDevice->SetTransform(D3DTS_VIEW, &myViewMatrix);
  d3dDevice->SetTransform(D3DTS_PROJECTION, &myProjMatrix);

  // Success
  return true;
}





//-----------------------------------------------------------------------------
// Name:  setWorldMatrix
// Desc:  Sets the designated world matrix on the device
//-----------------------------------------------------------------------------
bool IsometricCamera::setWorldMatrix(LPDIRECT3DDEVICE9 d3dDevice, const D3DXMATRIX* worldMatrix, DWORD index)
{
  // Check parmaeters
  if (APP_ERROR(!d3dDevice || !worldMatrix || index > 255)("Invalid parameter provided to IsometricCamera::setWorldMatrix"))
    return false;

  // Combine the matrix
  //D3DXMATRIXA16 mat;
  //D3DXMatrixMultiply(&mat, worldMatrix, &myLocationMatrix);
  //d3dDevice->SetTransform(D3DTS_WORLDMATRIX(index), &mat);
  d3dDevice->SetTransform(D3DTS_WORLDMATRIX(index), worldMatrix);

  // Success
  return true;
}




//-----------------------------------------------------------------------------
// Name:  getWorldLocation
// Desc:  Ray-casts a screen coordinate into the corresponding location on the map
//-----------------------------------------------------------------------------
void IsometricCamera::getWorldLocation(int screenX, int screenY, float* worldX, float* worldZ) {
  D3DXVECTOR3 vPickRayDir;
  D3DXVECTOR3 vPickRayOrig;

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  (((2.0f * screenX) / myScreenWidth) - 1) / myProjMatrix._11;
  v.y = -(((2.0f * screenY) / myScreenHeight) - 1) / myProjMatrix._22;
  v.z =  1.0f;

  // Get the inverse view matrix
  D3DXMATRIX mWorldView = myLocationMatrix * myViewMatrix;
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


//-----------------------------------------------------------------------------
// Name:  zero
// Desc:  Resets the class
//-----------------------------------------------------------------------------
void IsometricCamera::getScreenLocation(float worldX, float worldY, float worldZ, float* screenX, float* screenY)
{
  D3DVIEWPORT9 vpt = { 0, 0, myScreenWidth, myScreenHeight, 0.0f, 0.0f };
  D3DXVECTOR3 world(worldX, worldY, worldZ), screen;
  D3DXVec3Project(&screen, &world, &vpt, &myProjMatrix, &myViewMatrix, &myLocationMatrix);
  if (screenX) *screenX = screen.x;
  if (screenY) *screenY = screen.y;
}





//-----------------------------------------------------------------------------
// Name:  zero
// Desc:  Resets the class
//-----------------------------------------------------------------------------
void IsometricCamera::zero()
{
  myScreenWidth = myScreenHeight = 1; // Prevent divide-by-zero
  myTiltWorldFlag = true;
  D3DXMatrixIdentity(&myViewMatrix);
  D3DXMatrixIdentity(&myProjMatrix);
  D3DXMatrixIdentity(&myLocationMatrix);
}



