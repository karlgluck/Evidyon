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
#include "evidyonguiscrollablebuttonlist.h"
#include "evidyonguidefs.h"
#include "evidyonguisounds.h" // for playing button-click sound

namespace Evidyon {

EvidyonGUIScrollableButtonList::Button::Button(EvidyonGUIScrollableButtonList* owner, int id) {
  id_ = id;
  owner_ = owner;
}

int EvidyonGUIScrollableButtonList::Button::getID() const {
  return id_;
}

void EvidyonGUIScrollableButtonList::Button::onStateChange(GUIButton::StateChange stateChange) {
  if (stateChange == GUIButton::STATECHANGE_PUSHED) {
    Button* old_button = owner_->mouse_clicked_button_;
    owner_->mouse_clicked_button_ = this;
    if (old_button != NULL)  old_button->resetPushed();
  } else if (stateChange == GUIButton::STATECHANGE_MOUSEOVER) {
    owner_->mouse_over_button_ = this;
  } else if (stateChange == GUIButton::STATECHANGE_MOUSEOUT) {
    if (owner_->mouse_over_button_ == this) {
      owner_->mouse_over_button_ = NULL;
    }
  }

  switch (stateChange) {
      case GUIButton::STATECHANGE_RESET: {
        getMouseSentinel()->releaseMouseFocus();
        myText.setTextColor(EvidyonGUIDefs::LIST_ENTRY_TEXT_COLOR);
        myBackground.setSourceRegion(
           &EvidyonGUIDefs::LIST_ENTRY_BACKGROUND_NEUTRAL,
            EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
            EvidyonGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_DISABLED: {
        myText.setTextColor(EvidyonGUIDefs::LIST_ENTRY_TEXT_SPECIAL_COLOR);
        myBackground.setSourceRegion(
           &EvidyonGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
            EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
            EvidyonGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_ENABLED:
        myText.setTextColor(EvidyonGUIDefs::LIST_ENTRY_TEXT_COLOR);

      case GUIButton::STATECHANGE_MOUSEOUT: {
        myBackground.setSourceRegion(
           &EvidyonGUIDefs::LIST_ENTRY_BACKGROUND_NEUTRAL,
            EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
            EvidyonGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_MOUSEOVER: {
        myBackground.setSourceRegion(&EvidyonGUIDefs::LIST_ENTRY_BACKGROUND_MOUSEOVER,
                                       EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
                                       EvidyonGUIDefs::TEXTURE_SIZE);

      } break;

      case GUIButton::STATECHANGE_MOUSEOVERANDDOWN: {
        myBackground.setSourceRegion(&EvidyonGUIDefs::LIST_ENTRY_BACKGROUND_MOUSEOVERANDDOWN,
                                      EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
                                      EvidyonGUIDefs::TEXTURE_SIZE);

      } break;

      case GUIButton::STATECHANGE_PUSHED: {
        Evidyon::EvidyonGUISounds::buttonClick();
        myBackground.setSourceRegion(&EvidyonGUIDefs::LIST_ENTRY_BACKGROUND_SELECTED,
                                      EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
                                      EvidyonGUIDefs::TEXTURE_SIZE);
      } break;
  }

}

EvidyonGUIScrollableButtonList::EvidyonGUIScrollableButtonList() {
  mouse_over_button_ = NULL;
  mouse_clicked_button_ = NULL;

  GUICanvas* list_canvas = list_.getCanvas();
  list_canvas->setSourceCanvas(getCanvas());
  list_canvas->relativeWidth(0);
  list_canvas->relativeHeight(0);
  list_canvas->alignXLeft(0);
  list_canvas->alignYTop(0);
  list_canvas->relativeDepth(0.0f);

  setBackgroundColor(0xA0FFFFFF);
}

EvidyonGUIScrollableButtonList::~EvidyonGUIScrollableButtonList() {
  clear();
}

void EvidyonGUIScrollableButtonList::setLineHeight(int pixels) {
  list_.setLineHeight(pixels);
}

EvidyonGUIButton* EvidyonGUIScrollableButtonList::button(int id) {
  GUIComponent* component = list_.getComponent(id);
  if (!component) {
    component = new Button(this, id);
    GUICanvas* canvas = component->getCanvas();
    canvas->alignXLeft(0);
    canvas->alignYTop(0);
    canvas->relativeWidth(0);
    canvas->relativeHeight(0);
    canvas->relativeDepth(0.0f);
    list_.setComponent(id, component);
  }
  return reinterpret_cast<EvidyonGUIButton*>(component);
}

void EvidyonGUIScrollableList_DeleteComponentCallback(GUIComponent* component) {
  delete component;
}

void EvidyonGUIScrollableButtonList::clear() {
  mouse_over_button_ = NULL;
  mouse_clicked_button_ = NULL;
  list_.removeAllComponents(EvidyonGUIScrollableList_DeleteComponentCallback);
}

bool EvidyonGUIScrollableButtonList::getMouseOverButton(int* id) {
  if (NULL == mouse_over_button_) return false;
  *id = mouse_over_button_->getID();
  return true;
}

bool EvidyonGUIScrollableButtonList::getClickedButton(int* id) {
  if (NULL == mouse_clicked_button_) return false;
  mouse_clicked_button_->resetPushed();
  *id = mouse_clicked_button_->getID();
  mouse_clicked_button_ = NULL;
  return true;
}


void EvidyonGUIScrollableButtonList::setScrollPosition(float position) {
  list_.scrollToPercent(position);
}

float EvidyonGUIScrollableButtonList::getScrollPosition() {
  return list_.getPercentScrollLocation();
}


//----[  getLineScrollLocation  ]----------------------------------------------
//void EvidyonGUIScrollableButtonList::getLineScrollLocation(int* line, int* pixel_offset) {
//  list_.getLineScrollLocation(line, pixel_offset);
//}





//----[  scrollToLine  ]-------------------------------------------------------
void EvidyonGUIScrollableButtonList::scrollToLine(int line, int pixel_offset) {
  list_.scrollToLine(line, pixel_offset);
}



//----[  setBackgroundColor  ]-------------------------------------------------
void EvidyonGUIScrollableButtonList::setBackgroundColor(D3DCOLOR color) {
  list_.setBackgroundColor(color);
}


GUIMouseSentinel* EvidyonGUIScrollableButtonList::getMouseSentinel() {
  return list_.getMouseSentinel();
}

GUIKeyboardSentinel* EvidyonGUIScrollableButtonList::getKeyboardSentinel() {
  return list_.getKeyboardSentinel();
}

void EvidyonGUIScrollableButtonList::registerElements(GUILayer* layer) {
  return list_.registerElements(layer);
}

void EvidyonGUIScrollableButtonList::unregisterElements(GUILayer* layer) {
  return list_.unregisterElements(layer);
}






}



