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
#ifndef __EVIDYONGUISCROLLABLELIST_H__
#define __EVIDYONGUISCROLLABLELIST_H__
#pragma once


#include "common/gui/guiscrollablelist.h"
#include "common/gui/guiborderedfillelement.h"
#include "common/gui/guislider.h"
#include "evidyonguislider.h"



// This class takes care of matching a list slider with its
// corresponding GUIScrollableList object.
class EvidyonGUIScrollableListSlider : public EvidyonGUISlider {
public:
  EvidyonGUIScrollableListSlider(GUIScrollableList* list) {
    list_ = list;
    list->setSlider(reinterpret_cast<GUISlider*>(this));
  }

protected:
  virtual void onChangedPosition(float position) {
    list_->sliderScrollToPercent(position);
  }

private:
  GUIScrollableList* list_;
};


namespace Evidyon {

// This class stores an indexed list of GUIComponent objects
// and provides all the necessary setup to make the list appear
// correctly as a part of the Evidyon GUI.
// The list has a background, a slider and a client region where
// the components appear.  Moving the root canvas of the component
// moves the whole object, including the slider.
class EvidyonGUIScrollableList : public GUIComponent {
  class MouseSentinel : public GUIMouseSentinel {
  public:
    MouseSentinel(EvidyonGUIScrollableList* owner);
    virtual void onGotMouseFocus();
    virtual void onLostMouseFocus();
    virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
    virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);
    virtual bool onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse);
  private:
    EvidyonGUIScrollableList* owner_;
  };
public:
  EvidyonGUIScrollableList();
  void setComponent(int id, GUIComponent* component);
  GUIComponent* getComponent(int id);
  void removeComponent(int id);
  void removeAllComponents(GUIScrollableList::ComponentCallback callback);
  void setLineHeight(int pixels);
  void scrollToLine(int line, int pixel_offset);
  //void getLineScrollLocation(int* line, int* pixel_offset);
  void scrollToPercent(float percent);
  float getPercentScrollLocation();
  void scrollToComponent(int id, int pixel_offset);
  void scrollToEnd();

  void setBackgroundColor(D3DCOLOR color);

public:
  virtual GUIMouseSentinel* getMouseSentinel();
  virtual GUIKeyboardSentinel* getKeyboardSentinel();
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

private:
  GUIMouseSentinel mouse_sentinel_;
  GUIBorderedFillElement background_;
  EvidyonGUIScrollableListSlider slider_;
  GUIScrollableList list_;
};

}



#endif