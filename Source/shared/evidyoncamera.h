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
#ifndef __EVIDYONCAMERA_H__
#define __EVIDYONCAMERA_H__
#pragma once



#include <d3dx9.h>

namespace Evidyon {

/**
* Sets up the transform states for view rendering and manages view-based information
*   @author Karl Gluck
*/
class EvidyonCamera
{
public:

  /**
   * Resets this class
   */
  EvidyonCamera();

  // Sets the location of the camera on the map
  void setPosition(float x, float z, float zoom = 1.0f);

  // Changes whether or not this camera looks down on the world from above
  // or in 3/4 view.
  void enableTopdownView(bool enable);
  bool topdownViewEnabled() const;

  void rebuildTopdownCamera(DWORD screen_width, DWORD screen_height);

  bool initialize3DRendering(LPDIRECT3DDEVICE9 d3dDevice);

  // Ray-casts a screen coordinate into the corresponding location on the map
  void getWorldLocation(int screenX,
                        int screenY,
                        float* worldX,
                        float* worldZ,
                        const D3DXMATRIX* world_matrix);

  // Projects a point in the world to its corresponding location on the screen
  void getScreenLocation(float worldX,
                         float worldY,
                         float worldZ,
                         float* screenX,
                         float* screenY,
                         const D3DXMATRIX* world_matrix);


protected:
  void zero();


protected:

  /// How big the screen is
  DWORD myScreenWidth, myScreenHeight;

  /// Whether or not to look in isometric mode on the world
  bool myTiltWorldFlag;

  /// The constructed view matrix
  D3DXMATRIXA16 view_matrix_;

  /// The constructed perspective matrix
  D3DXMATRIXA16 proj_matrix_;
};

}

#endif
