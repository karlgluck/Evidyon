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
#include "guilayer.h"
#include "guipictureelement.h"
#include "guiborderedfillelement.h"
#include "guimouse.h"
#include "guikeyboard.h"
#include "guicomponent.h"
#include "guislider.h"



//-----------------------------------------------------------------------------
// Name: GUISlider
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUISlider::GUISlider() : myHandleElement(this), myHandleMouseSentinel(this)
{
  myHandleCanvas.setSourceCanvas(getCanvas());
  myHandleCanvas.relativeDepth(-0.001f);
  myHandleCanvas.alignYTop(0);
  myHandleCanvas.alignXCenter(0);
  myHandleCanvas.absoluteHeight(0);
  myHandleCanvas.relativeWidth(-2);

  myBackgroundElement.setSourceCanvas(getCanvas());
  myHandleElement.setSourceCanvas(&myHandleCanvas);
}





//-----------------------------------------------------------------------------
// Name: getBackgroundElement
// Desc: Obtains a pointer to the background element for this slider so that its texture region can be set
//-----------------------------------------------------------------------------
GUIBorderedFillElement* GUISlider::getBackgroundElement()
{
  return &myBackgroundElement;
}



//-----------------------------------------------------------------------------
// Name: getHandleElement
// Desc: Gets a pointer to the handle element for this slider so that its texture region can be set
//-----------------------------------------------------------------------------
GUIPictureElement* GUISlider::getHandleElement()
{
  return &myHandleElement;
}



//-----------------------------------------------------------------------------
// Name: setSliderPosition
// Desc: Sets the slider's offset; this method constrains the position so that the slider doesn't try to
//     move past the start or end of the slider.
//-----------------------------------------------------------------------------
void GUISlider::setSliderPosition(int position)
{
  if (position < 0) position = 0;

  int maxPosition = getCanvas()->getArea()->calculateSize().height - myHandleElement.getSliderSize();
  if (position > maxPosition) position = maxPosition;

  // Save the slider's handle position
  myHandleCanvas.alignYTop(position);

  // The handle moved, so update it
  onStateChange(STATECHANGE_HANDLEMOVED);
}



//-----------------------------------------------------------------------------
// Name: setSliderPosition
// Desc: Sets the slider's offset; this method constrains the position so that the slider doesn't try to
//       move past the start or end of the slider.
//-----------------------------------------------------------------------------
void GUISlider::setSliderPosition(float position) {
  syncSliderPosition(position);

  // The handle moved, so update it
  onStateChange(STATECHANGE_HANDLEMOVED);
}



//-----------------------------------------------------------------------------
// Name: getSliderPercentLocation
// Desc: Gets the location of the handle in the slider as a % of the slider's total size
//-----------------------------------------------------------------------------
float GUISlider::calculateSliderPercentLocation()
{
  const GUIRect* backgroundArea = getCanvas()->getArea();
  int sliderHeight = backgroundArea->calculateSize().height;
  sliderHeight -= myHandleElement.getSliderSize();
  int sliderPosition = myHandleCanvas.getArea()->top - backgroundArea->top;

  return sliderPosition / (float)sliderHeight;
}



//-----------------------------------------------------------------------------
// Name: getMouseSentinel
// Desc: Gets this slider's mouse sentinel so it can be added to the input chain
//-----------------------------------------------------------------------------
GUIMouseSentinel* GUISlider::getMouseSentinel() {
  return &myHandleMouseSentinel;
}


//-----------------------------------------------------------------------------
void GUISlider::registerElements(GUILayer* layer) {
  layer->registerElement(&myBackgroundElement);
  layer->registerElement(&myHandleElement);
}

//-----------------------------------------------------------------------------
void GUISlider::unregisterElements(GUILayer* layer) {
  layer->unregisterElement(&myBackgroundElement);
  layer->unregisterElement(&myHandleElement);
}


//-----------------------------------------------------------------------------
void GUISlider::syncSliderPosition(float position) {
  if (position < 0.0f) position = 0.0f;
  if (position > 1.0f) position = 1.0f;
  int maxPosition = getCanvas()->getArea()->calculateSize().height - myHandleElement.getSliderSize();

  // Save the slider's handle position
  myHandleCanvas.alignYTop((int)(maxPosition * position));
}


//-----------------------------------------------------------------------------
void GUISlider::scroll(float amount, int lines) {
  myHandleMouseSentinel.scroll(amount, lines);
}


//-----------------------------------------------------------------------------
// Name: onStateChange
// Desc: Called when the slider's state changes relative to user input
//-----------------------------------------------------------------------------
void GUISlider::onStateChange(StateChange stateChange)
{
}



//-----------------------------------------------------------------------------
// Name: SliderHandleMouseSentinel
// Desc: Initializes this sentinel and attaches it to the given slider
//-----------------------------------------------------------------------------
GUISlider::SliderHandleMouseSentinel::SliderHandleMouseSentinel(GUISlider* slider)
{
  mySlider = slider;
}



//-----------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUISlider::SliderHandleMouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
  GUICanvas* sliderCanvas = mySlider->getCanvas();
  GUICanvas* handleCanvas = &mySlider->myHandleCanvas;

  GUIPoint handleTop = { mySlider->myHandleCanvas.getArea()->left, mySlider->myHandleCanvas.getArea()->top };
  GUISize handleSize = { mySlider->myHandleElement.getPictureWidth(), mySlider->myHandleElement.getPictureHeight() };
  GUIRect handleRect;
  handleRect.set(handleTop, handleSize);

  bool insideOldCanvas = sliderCanvas->pointInside(oldPosition);
  bool insideNewCanvas = sliderCanvas->pointInside(position);
  bool insideOldHandle = handleRect.contains(oldPosition);
  bool insideNewHandle = handleRect.contains(position);

  // Handle context
  if (mouse->isPrimaryButtonDown()) {
    if (hasMouseFocus()) {
      // Move the slider
      float old_location = mySlider->calculateSliderPercentLocation();
      int sliderPosition = handleRect.top - sliderCanvas->getArea()->top + position.y - oldPosition.y;
      mySlider->setSliderPosition(sliderPosition);
      if (mySlider->calculateSliderPercentLocation() == old_location) {
        //mouse->jumpTo(oldPosition);
      }

      // Capture this input
      return true;
    }
  }

  // Trigger state changes
  if (!insideOldCanvas && insideNewCanvas)
  {
    mySlider->onStateChange(insideNewHandle ? STATECHANGE_MOUSEOVER_HANDLEANDBACKGROUND : STATECHANGE_MOUSEOVER_HANDLE);
  }
  else if (insideOldCanvas && !insideNewCanvas)
  {
    mySlider->onStateChange(insideNewHandle ? STATECHANGE_MOUSEOVER_HANDLE : STATECHANGE_MOUSEOUT);
  }
  else
  {
    if (insideOldHandle && !insideNewHandle) mySlider->onStateChange(STATECHANGE_MOUSEOUT);
    if (!insideOldHandle && insideNewHandle) mySlider->onStateChange(STATECHANGE_MOUSEOVER_HANDLE);
  }

  // Don't capture
  return false;
}



//-----------------------------------------------------------------------------
// Name: onMouseButtonAction
// Desc: Called by the mouse input source when a mouse button alters its state
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUISlider::SliderHandleMouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)
{
  bool trap = false;
  if (action == GUIMOUSE_DOWN)
  {
    GUIPoint handleTop = { mySlider->myHandleCanvas.getArea()->left, mySlider->myHandleCanvas.getArea()->top };
    GUISize handleSize = { mySlider->myHandleElement.getPictureWidth(), mySlider->myHandleElement.getPictureHeight() };
    GUIRect handleRect;
    handleRect.set(handleTop, handleSize);
    if (handleRect.contains(position)) {
      obtainMouseFocus();
      return true;
    }
  } else {
    releaseMouseFocus();
    trap = false;
  }

  if (action == GUIMOUSE_UP) {
    if (hasMouseFocus()) {
      releaseMouseFocus();
    }
  }

  return trap;
}


bool GUISlider::SliderHandleMouseSentinel::onMouseScroll(GUIPoint position,
                                                       float amount,
                                                       int lines, GUIMouseInputSource* mouse) {

  GUICanvas* sliderCanvas = mySlider->getCanvas();
  if (sliderCanvas->getArea()->contains(position)) {
    scroll(amount, lines);
    return true;
  } else {
    return false;
  }
}


void GUISlider::SliderHandleMouseSentinel::scroll(float amount, int lines) {
  GUICanvas* sliderCanvas = mySlider->getCanvas();
  GUICanvas* handleCanvas = &mySlider->myHandleCanvas;

  GUIPoint handleTop = { mySlider->myHandleCanvas.getArea()->left, mySlider->myHandleCanvas.getArea()->top };
  GUISize handleSize = { mySlider->myHandleElement.getPictureWidth(), mySlider->myHandleElement.getPictureHeight() };
  GUIRect handleRect;
  handleRect.set(handleTop, handleSize);
  int sliderPosition = handleRect.top - sliderCanvas->getArea()->top + (int)(amount);
  mySlider->setSliderPosition(sliderPosition);
}




//-----------------------------------------------------------------------------
// Name: SliderHandleElement
// Desc: Initializes this element and attaches it to the given slider
//-----------------------------------------------------------------------------
GUISlider::SliderHandleElement::SliderHandleElement(GUISlider* slider)
{
  mySlider = slider;
}



//-----------------------------------------------------------------------------
// Name: render
// Desc: Renders the picture element centered on the slider's canvas at the current location
//-----------------------------------------------------------------------------
bool GUISlider::SliderHandleElement::render(GUILayer* layer)
{
  const GUIRect* destination = mySlider->myHandleCanvas.getArea();
  return GUIPictureElement::render(layer, destination->left/* - getPictureWidth()/2.0f*/, destination->top, mySourceCanvas->getDepth() - 0.001f);
}



//-----------------------------------------------------------------------------
// Name: getSliderSize
// Desc: Returns the size of the slider in pixels
//-----------------------------------------------------------------------------
int GUISlider::SliderHandleElement::getSliderSize() const
{
  return (int)getPictureHeight();
}

