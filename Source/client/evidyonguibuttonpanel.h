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
#ifndef __EVIDYONGUIBUTTONPANEL_H__
#define __EVIDYONGUIBUTTONPANEL_H__
#pragma once


#include "common/gui/guicanvasmatrix.h"


template <size_t B> class EvidyonGUIButtonPanel {
public:
  EvidyonGUIButtonPanel();
  bool create(GUIFont* font, LPDIRECT3DTEXTURE9 gui_texture, GUIMouseSentinel* mouse_sentinel);
  void destroy();
  void registerElements(GUILayer* layer);
  void unregisterElements(GUILayer* layer);

  inline void buttonsWidth(int width)
      { button_canvases_.absoluteWidth(width); }
  inline void buttonsHeight(int height)
      { button_canvases_.absoluteHeight(height); }
  inline GUICanvas* getCanvas()
      { return panel_.getCanvas(); }
  inline void setTitleText(const char* title)
      { panel_.setText(title); }
  inline EvidyonGUIButton* getButton(int b)
      { if (b<0||b>=B) return NULL; else return &buttons_[b]; }

private:
  GUICanvas client_canvas_;
  EvidyonGUITitledPanel panel_;
  GUICanvasMatrix<B,1> button_canvases_;
  EvidyonGUIButton buttons_[B];
};





//-----------------------------------------------------------------------------
// Inline Implementation
//-----------------------------------------------------------------------------



template <size_t B> bool EvidyonGUIButtonPanel<B>::create(
    GUIFont* font,
    LPDIRECT3DTEXTURE9 gui_texture,
    GUIMouseSentinel* mouse_sentinel) {
  panel_.setFont(font);
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].setFont(font);
    buttons_[i].getMouseSentinel()->setParentMouseSentinel(mouse_sentinel);
  }
  return true;
}

template <size_t B> void EvidyonGUIButtonPanel<B>::destroy() {
  panel_.setFont(NULL);
  panel_.releaseGUITexture();
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].setFont(NULL);
    buttons_[i].releaseGUITexture();
    buttons_[i].getMouseSentinel()->removeFromMouseSentinelChain();
  }
}


template <size_t B> void EvidyonGUIButtonPanel<B>::registerElements(
    GUILayer* layer) {
  panel_.registerElements(layer);
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].registerElements(layer);
  }
}


template <size_t B> void EvidyonGUIButtonPanel<B>::unregisterElements(
    GUILayer* layer) {
  panel_.unregisterElements(layer);
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].unregisterElements(layer);
  }
}


template <size_t B> EvidyonGUIButtonPanel<B>::EvidyonGUIButtonPanel() {
  client_canvas_.setSourceCanvas(panel_.getCanvas());
  client_canvas_.alignXCenter(0);
  client_canvas_.alignYCenter(0);
  client_canvas_.relativeWidth(0);
  client_canvas_.relativeHeight(0);
  client_canvas_.relativeDepth(-0.01f);
  button_canvases_.setSourceCanvas(&client_canvas_);
  button_canvases_.absoluteWidth(300);
  button_canvases_.absoluteHeight(B*40);
  button_canvases_.alignXCenter(0);
  button_canvases_.alignYCenter(+5);
  button_canvases_.relativeDepth(0.0f);
  for (size_t i = 0; i < B; ++i) {
    buttons_[i].getCanvas()->setSourceCanvas(
        button_canvases_.getCellCanvas(i, 0));
    buttons_[i].getCanvas()->relativeDepth(0.0f);
    buttons_[i].getCanvas()->relativeHeight(-4);
    buttons_[i].getCanvas()->relativeWidth(-12);
    buttons_[i].getCanvas()->alignXCenter(0);
    buttons_[i].getCanvas()->alignYCenter(0);
  }
}

#endif