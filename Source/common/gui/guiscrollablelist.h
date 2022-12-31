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
#ifndef __GUISCROLLABLELIST_H__
#define __GUISCROLLABLELIST_H__
#pragma once



#include "guicomponent.h"
#include "guimouse.h"
#include "guikeyboard.h"
#include <map>
#include "guicanvasrowarray.h"


class GUISlider;




//----[  GUIScrollableList  ]--------------------------------------------------
class GUIScrollableList : public GUIComponent {
  // Passes messages to children when the mouse is interacting with this list.
  // Interaction is defined as when the mouse is inside of the list's primary
  // canvas (the clipping region) or the list has mouse focus.
  class MouseSentinel : public GUIMouseSentinel {
  public:
    MouseSentinel(GUIScrollableList* owner);
    virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
    virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);
    virtual bool onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse);

  private:
    GUIScrollableList* owner_;
  };

public:
  typedef void (*ComponentCallback)(GUIComponent* component);

public:

  GUIScrollableList();
  ~GUIScrollableList();

  // Associates a slider with this list.  This is not necessary, but it
  // causes scrolling messages to be passed correctly.
  void setSlider(GUISlider* slider);

  // Components are ordered in the list based on their ID number.  This method
  // sets the component's clipping area to the list's canvas and adds its mouse
  // and keyboard sentinels (if they exist) to this list's own chain.
  void setComponent(int id, GUIComponent* component);
  GUIComponent* getComponent(int id);

  // When a component is removed, the source canvas of those after it adjust
  // their locations in the list to keep the lines contiguous.  It is the
  // responsibility of the calling method to remove the component from
  // its canvas.
  void removeComponent(int id);

  // If the callback is non-null, this method invokes *callback for each
  // of the internal components before clearing the list.  It is guaranteed
  // that no method will be called on the component after the callback is
  // invoked (so it is safe to use this method to delete dynamically-allocated
  // GUIComponent list entries)
  void removeAllComponents(ComponentCallback callback);

  // Changes how many pixels each line's canvas is given.  This method will
  // sync the associated slider.
  void setLineHeight(int pixels);

  // Makes the list display the given line at the top of its canvas.  This
  // will sync the slider with the current scroll location.
  void scrollToLine(int line, int pixel_offset);
  //void getLineScrollLocation(int* line, int* pixel_offset);

  // Shifts the canvas that moves the rows of this list.  The top of the list
  // is 0.0 (0%).  The bottom of the list, 1.0 (100%), places the last line
  // at the top of the visible area.
  void scrollToPercent(float percent);
  float getPercentScrollLocation();

  // Puts the given component on screen
  void scrollToComponent(int id, int pixel_offset);

  void scrollToEnd();

  // Does the same thing as scrollToPercent, except that it doesn't try to
  // sync the associated slider; thus, this is the method that should be
  // called when a slider is being used to scroll the list.
  void sliderScrollToPercent(float percent);

  virtual GUIMouseSentinel* getMouseSentinel();
  virtual GUIKeyboardSentinel* getKeyboardSentinel();

  // Calls registerElements for all components in the list
  virtual void registerElements(GUILayer* layer);

  // Calls unregisterElements for all components in the list
  virtual void unregisterElements(GUILayer* layer);


private:
  GUICanvasRowArray rows_;

  GUIKeyboardSentinel keyboard_sentinel_;
  MouseSentinel mouse_sentinel_;

  // The slider that is associated with this list.  This is optional.  If
  // the reference is valid, scroll messages received anywhere in the list
  // are passed directly to the slider's mouse sentinel.
  GUISlider* slider_;

  // Maps the component identifier to the line index and the component so that
  // the component can be easily looked up and readjusted when lines are added
  // or removed.
  typedef std::map<int, std::pair<unsigned int,GUIComponent*>> Components;
  Components components_;
};


#endif