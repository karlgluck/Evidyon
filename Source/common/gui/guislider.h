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
#ifndef __GUISLIDER_H__
#define __GUISLIDER_H__
#pragma once


#include "guicanvas.h"
#include "guiborderedfillelement.h"
#include "guipictureelement.h"

/**
 * Creates an input element that slides one image along a background used as the texture
 */
class GUISlider : public GUIComponent
{
  /**
   * Handles the rendering of the slider
   */
  class SliderHandleElement : public GUIPictureElement
  {
    public:

      /**
       * Initializes this element and attaches it to the given slider
       */
      SliderHandleElement(GUISlider* slider);

      /**
       * Renders the picture element centered on the slider's canvas at the current location
       */
      bool render(GUILayer* layer);

      /**
       * Returns the size of the slider in pixels
       */
      int getSliderSize() const;


    protected:

      /// The slider of which this element is a part
      GUISlider* mySlider;
  };

  /**
   * Watches for mouse input from the slider
   */
  class SliderHandleMouseSentinel : public GUIMouseSentinel
  {
    public:

      /**
       * Initializes this sentinel and attaches it to the given slider
       */
      SliderHandleMouseSentinel(GUISlider* slider);


    protected:

      /**
       * Called by the mouse input source when the mouse moves
       * @return 'true' To indicate the event should stop propogating
       */
      virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);

      /**
       * Called by the mouse input source when a mouse button alters its state
       * @return 'true' To indicate the event should stop propogating
       */
      virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

      /**
       * Called when the scroll wheel is used over the slider canvas.
       */
      virtual bool onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse);

  public:

      // Scrolls the slider.  Used by other classes to force the slider to move,
      // for example when the mouse is over the object associated with the slider.
      void scroll(float amount, int lines);


    protected:

      /// The slider of which this sentinel is a part
      GUISlider* mySlider;
  };

  protected:

    /**
     * Defines the state changes that the slider can go through
     */
    enum StateChange
    {
      STATECHANGE_MOUSEOVER_BACKGROUND,
      STATECHANGE_MOUSEOVER_HANDLE,
      STATECHANGE_MOUSEOVER_HANDLEANDBACKGROUND,
      STATECHANGE_MOUSEOVERANDDOWN_HANDLE,
      STATECHANGE_MOUSEOUT,
      STATECHANGE_HANDLEMOVED,
    };

  public:

    /**
     * Initializes this class
     */
    GUISlider();

    /**
     * Obtains a pointer to the background element for this slider so that its texture region can be set
     */
    GUIBorderedFillElement* getBackgroundElement();

    /**
     * Gets a pointer to the handle element for this slider so that its texture region can be set
     */
    GUIPictureElement* getHandleElement();

    /**
     * Sets the slider's offset; this method constrains the position so that the slider doesn't try to
     * move past the start or end of the slider.
     */
    void setSliderPosition(int position);

    /**
     * Sets the slider's offset; this method constrains the position so that the slider doesn't try to
     * move past the start or end of the slider.
     */
    void setSliderPosition(float percentPosition);

    /**
     * Gets the location of the handle in the slider as a % of the slider's total size
     */
    float calculateSliderPercentLocation();

    /**
     * Gets this slider's mouse sentinel so it can be added to the input chain
     */
    GUIMouseSentinel* getMouseSentinel();

    virtual void registerElements(GUILayer* layer);
    virtual void unregisterElements(GUILayer* layer);

    /**
     * Performs the same operation as setSliderPosition, but doesn't invoke a handle-
     * moved event.  This is called by lists that are associated with a slider when
     * their scroll methods are called.
     */
    void syncSliderPosition(float percentPosition);

    // Scrolls the slider
    void scroll(float amount, int lines);

  protected:

    /**
     * Called when the slider's state changes relative to user input
     */
    virtual void onStateChange(StateChange stateChange);


  protected:

    /// The rendering element for the background
    GUIBorderedFillElement myBackgroundElement;

    /// Defines the location of the slider handle
    GUICanvas myHandleCanvas;

    /// Draws this slider's handle
    SliderHandleElement myHandleElement;

    /// The input sentinel for this slider
    SliderHandleMouseSentinel myHandleMouseSentinel;
};


#endif