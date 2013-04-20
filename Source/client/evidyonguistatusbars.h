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
#ifndef __EVIDYONGUISTATUSBARS_H__
#define __EVIDYONGUISTATUSBARS_H__
#pragma once



#include "common/gui/guicomponent.h"
#include "common/gui/guiborderedfillelement.h"
#include "common/gui/guistretchfillelement.h"


namespace Evidyon {


class EvidyonGUIStatusBars : public GUIComponent {
public:
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

  void init(int screen_width, int screen_height);
  void set(float hp_percent, float mp_percent, float xp_percent);
  GUISize getSize();

private:
  GUICanvas hpBarCanvas, mpBarCanvas;
  GUICanvas barBackgroundCanvas;
  GUIBorderedFillElement barBackground;
  GUIStretchFillElement hpBar, mpBar;
  GUICanvas expBarCanvas;
  GUIStretchFillElement expBar;
};

}


#endif