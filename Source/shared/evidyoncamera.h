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
