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
#include "evidyonguistatusbars.h"
#include "common/gui/guilayer.h"
#include "evidyonguidefs.h"


namespace Evidyon {



void EvidyonGUIStatusBars::registerElements(GUILayer* layer) {
  layer->registerElement(&barBackground);
  layer->registerElement(&mpBar);
  layer->registerElement(&hpBar);
  layer->registerElement(&expBar);

  barBackgroundCanvas.setSourceCanvas(layer->getPrimaryCanvas());
}


void EvidyonGUIStatusBars::unregisterElements(GUILayer* layer) {
  layer->unregisterElement(&barBackground);
  layer->unregisterElement(&mpBar);
  layer->unregisterElement(&hpBar);
  layer->unregisterElement(&expBar);
}


void EvidyonGUIStatusBars::init(int screen_width, int screen_height) {
  {
    barBackgroundCanvas.absoluteHeight((screen_height/9) + 4);
    barBackgroundCanvas.absoluteWidth((screen_width/25)+8);
    barBackgroundCanvas.alignXLeft(0);
    barBackgroundCanvas.alignYBottom(+2);
    barBackgroundCanvas.absoluteDepth(1.0f);
    barBackground.setSourceCanvas(&barBackgroundCanvas);
    barBackground.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
    barBackground.setSourceRegion(&EvidyonGUIDefs::CONTENT_COMPONENT_BACKGROUND, 8, EvidyonGUIDefs::TEXTURE_SIZE);
  }
  {
    hpBarCanvas.setSourceCanvas(&barBackgroundCanvas);
    hpBarCanvas.absoluteWidth((barBackgroundCanvas.getArea()->calculateSize().width-8)/3);
    hpBarCanvas.relativeHeight(-8);
    hpBarCanvas.alignXLeft(+4);
    hpBarCanvas.alignYBottom(-4);
    hpBarCanvas.relativeDepth(-0.01f);
    hpBar.setSourceCanvas(&hpBarCanvas);
    hpBar.setSourceRegion(&EvidyonGUIDefs::HP_BAR_FILL, EvidyonGUIDefs::TEXTURE_SIZE);
    hpBar.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);

    mpBarCanvas.setSourceCanvas(&barBackgroundCanvas);
    mpBarCanvas.absoluteWidth((barBackgroundCanvas.getArea()->calculateSize().width-8)/3);
    mpBarCanvas.relativeHeight(-8);
    mpBarCanvas.alignXCenter(0);
    mpBarCanvas.alignYBottom(-4);
    mpBarCanvas.relativeDepth(-0.01f);
    mpBar.setSourceCanvas(&mpBarCanvas);
    mpBar.setSourceRegion(&EvidyonGUIDefs::MP_BAR_FILL, EvidyonGUIDefs::TEXTURE_SIZE);
    mpBar.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
  }
  {
    expBarCanvas.setSourceCanvas(&barBackgroundCanvas);
    expBarCanvas.relativeHeight(-8);
    expBarCanvas.absoluteWidth((barBackgroundCanvas.getArea()->calculateSize().width-8)/3);
    expBarCanvas.relativeHeight(-8);
    expBarCanvas.alignXRight(-4);
    expBarCanvas.relativeDepth(-0.01f);
    expBarCanvas.alignYBottom(-4);
    expBar.setSourceCanvas(&expBarCanvas);
    expBar.setSourceRegion(&EvidyonGUIDefs::EXP_BAR_FILL, EvidyonGUIDefs::TEXTURE_SIZE);
    expBar.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
  }

  set(1.0f, 1.0f, 1.0f);
}


void EvidyonGUIStatusBars::set(float hp_percent, float mp_percent, float xp_percent) {
  int barSize = barBackgroundCanvas.getArea()->calculateSize().height - 4*2;
  int hpSize = (int)(barSize * hp_percent);
  int mpSize = (int)(barSize * mp_percent);
  int xpSize = (int)(barSize * xp_percent);
  hpBarCanvas.absoluteHeight(min(hpSize, barSize));
  mpBarCanvas.absoluteHeight(min(mpSize, barSize));
  expBarCanvas.absoluteHeight(min(xpSize, barSize));
}

GUISize EvidyonGUIStatusBars::getSize() {
  return barBackgroundCanvas.getArea()->calculateSize();
}







}

