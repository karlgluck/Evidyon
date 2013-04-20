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
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include <d3dx9.h>
#include <map>
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guilayer.h"
#include "guifont.h"
#include "guitextelement.h"
#include "guimouse.h"
#include "guikeyboard.h"
#include "guicomponent.h"
#include "guilist.h"



//-----------------------------------------------------------------------------
// Name: MouseSentinel
// Desc: Initializes this list sentinel
//-----------------------------------------------------------------------------
GUIList::MouseSentinel::MouseSentinel(GUIList* list)
{
  myList = list;
}



//-----------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIList::MouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
  bool newInside = myList->myTargetCanvas.pointInside(position);
  bool oldInside = myList->myTargetCanvas.pointInside(oldPosition);

  if (newInside != oldInside)
    myList->onMouseCrossedListBorder(newInside);

  if (newInside || oldInside)
    return GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
  else
    return false;
}



//-----------------------------------------------------------------------------
// Name: onMouseButtonAction
// Desc: Called by the mouse input source when a mouse button alters its state
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIList::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)
{
  if (myList->myTargetCanvas.pointInside(position))
    return GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
  else
    return false;
}




//-----------------------------------------------------------------------------
// Name: LineMouseSentinel
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIList::LineMouseSentinel::LineMouseSentinel(GUICanvas* sourceCanvas)
{
  mySourceCanvas = sourceCanvas;
}



//-----------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves.  Only propogates messages if
//     the source canvas is valid and the position or old position is within the boundaries
//     of the source canvas.
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIList::LineMouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
  // Ignore all messages unless we have a source canvas
  if (!mySourceCanvas) return false;

  // Pass the message along if it is inside of this canvas
  if (mySourceCanvas->pointInside(position) || mySourceCanvas->pointInside(oldPosition))
    return GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
  else
    return false;
}




//-----------------------------------------------------------------------------
// Name: onMouseButtonAction
// Desc: Called by the mouse input source when a mouse button alters its state.  Only
//     propogates messages if the source canvas is valid and the position is within
//     the boundaries of the source canvas.
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIList::LineMouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)
{
  // Ignore all messages unless we have a source canvas
  if (!mySourceCanvas) return false;

  // Pass the message along if it is inside of this canvas
  if (mySourceCanvas->pointInside(position))
    return GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
  else
    return false;
}



//-----------------------------------------------------------------------------
// Name: ListEntry
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIList::ListEntry::ListEntry() : myMouseSentinel(&myLineCanvas)
{
  myClippingCanvas = 0;
  myLineCanvas.alignXLeft(0);
  myLineCanvas.relativeWidth(0);
  myLineCanvas.relativeDepth(0.0f);
}



//-----------------------------------------------------------------------------
// Name: addChildMouseSentinel
// Desc: Adds a mouse sentinel that is notified of actions inside of this list entry
//-----------------------------------------------------------------------------
void GUIList::ListEntry::addChildMouseSentinel(GUIMouseSentinel* sentinel)
{
  sentinel->setParentMouseSentinel(&myMouseSentinel);
}



//-----------------------------------------------------------------------------
// Name: addChildCanvas
// Desc: Adds a canvas that is updated by this line
//-----------------------------------------------------------------------------
void GUIList::ListEntry::addChildCanvas(GUICanvas* canvas)
{
  canvas->setSourceCanvas(&myLineCanvas);
}



//-----------------------------------------------------------------------------
// Name: getLineArea
// Desc: Gets this line's canvas coordinates
//-----------------------------------------------------------------------------
const GUIRect* GUIList::ListEntry::getLineArea()
{
  return myLineCanvas.getArea();
}



//-----------------------------------------------------------------------------
// Name: getClippingArea
// Desc: Gets the area into which elements for this list can render
//-----------------------------------------------------------------------------
const GUIRect* GUIList::ListEntry::getClippingArea()
{
  return myClippingCanvas ? myClippingCanvas->getArea() : 0;
}



//-----------------------------------------------------------------------------
// Name: isVisible
// Desc: Determines whether or not this list entry is visible
//-----------------------------------------------------------------------------
bool GUIList::ListEntry::isVisible()
{
  return myLineCanvas.getDepth() >= 0.0f && myLineCanvas.getDepth() <= 1.0f;
}



GUIMouseSentinel* GUIList::ListEntry::getMouseSentinel() {
  return &myMouseSentinel;
}



//-----------------------------------------------------------------------------
// Name: refresh
// Desc: Allows derived classes to update themselves when this structure changes
//-----------------------------------------------------------------------------
void GUIList::ListEntry::refresh()
{
}






//-----------------------------------------------------------------------------
// Name: GUIList
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIList::GUIList() : myMouseSentinel(this)
{
  // Realign the list-positioning canvas
  myListCanvas.setSourceCanvas(&myTargetCanvas);
  myListCanvas.relativeWidth(0);
  myListCanvas.absoluteHeight(0);
  myListCanvas.alignXLeft(0);
  myListCanvas.alignYTop(0);
  myListCanvas.relativeDepth(0.0f);
  //myListCanvas.realign();

  // Initialize other internal values
  myLineHeight = 1;
  myFirstVisibleLineIndex = 0;
  myFirstVisibleLineOffset = 0;
}



//-----------------------------------------------------------------------------
// Name: setLineHeight
// Desc: Sets the height of each line in this list in pixels.  This method refreshes
//     the list so that the canvases are updated.
//-----------------------------------------------------------------------------
void GUIList::setLineHeight(int pixels)
{
  myLineHeight = pixels;
  refresh();
}



//-----------------------------------------------------------------------------
// Name: refresh
// Desc: Updates the list entries to make sure only the valid ones are visible
//-----------------------------------------------------------------------------
void GUIList::refresh()
{
  int targetCanvasHeight = myTargetCanvas.getArea()->calculateSize().height;
  int additionalVisiblePixels = targetCanvasHeight % myLineHeight;
  int numberOfVisibleLines = targetCanvasHeight / myLineHeight + (additionalVisiblePixels ? 1 : 0);
  size_t numberOfLines = getNumberOfLines();

  // Make sure the requested line doesn't go beyond the end
  if (myFirstVisibleLineIndex + numberOfVisibleLines > numberOfLines)
  {
    // Calculate where to put the list canvas so that the end of
    // the list is entirely visible without going past it
    if (numberOfLines < numberOfVisibleLines)
    {
      myFirstVisibleLineIndex = 0;
      myFirstVisibleLineOffset = 0;
    }
    else
    {
      myFirstVisibleLineIndex = numberOfLines - numberOfVisibleLines;
      myFirstVisibleLineOffset = -additionalVisiblePixels;
    }
  }

  // Determine the visible range so we can easily 
  unsigned int firstVisibleLine = myFirstVisibleLineIndex;
  unsigned int lastVisibleLine = firstVisibleLine + numberOfVisibleLines;

  // Recalculate the location of the list canvas
  unsigned int lineHeight = myLineHeight;
  myListCanvas.alignYTop(-((int)(firstVisibleLine * lineHeight)) + myFirstVisibleLineOffset);
  myListCanvas.absoluteHeight((int)(numberOfLines * lineHeight));

  // This accumulates the current y-location of the line.  It starts
  // off negative so that the first loop will create a zero offset.
  int lineYLocation = -(int)lineHeight;

  // Iterate through all of the lines
  for (unsigned int i = 0; i < numberOfLines; ++i)
  {
    // Get this line's entry.  If it is invalid, skip and move on.
    ListEntry* entry = getListEntry(i);
    if (!entry) continue;

    // Make sure the clipping canvas is set
    entry->myClippingCanvas = &myTargetCanvas;
    entry->refresh();

    // Get the canvas and sentinel for this line
    GUICanvas* lineCanvas = &entry->myLineCanvas;
    LineMouseSentinel* sentinel = &entry->myMouseSentinel;

    // Add to this line's Y locaiton
    lineYLocation += (int)lineHeight;

    // If the line is invisible, remove it as a canvas child
    // and change its rendering depth so it can't be seen.
    if (i < firstVisibleLine || i > lastVisibleLine)
    {
      // Make the canvas invisible
      lineCanvas->absoluteDepth(-1.0f);

      // Erase it from the list
      lineCanvas->setSourceCanvas(0);

      // Disable the sentinel
      sentinel->removeFromMouseSentinelChain();

      // Go to the next element
      continue;
    }

    // Set this line's location
    lineCanvas->setSourceCanvas(&myListCanvas);
    lineCanvas->alignYTop(lineYLocation);
    lineCanvas->absoluteHeight(lineHeight);
    lineCanvas->relativeDepth(0.0f);

    // Initialize the sentinel
    sentinel->setParentMouseSentinel(&myMouseSentinel);
  }

  // Align the canvas and all of the child list items
  //myListCanvas.realign();
}



//-----------------------------------------------------------------------------
// Name: scrollToLine
// Desc: Ensures that the given line is visible
//-----------------------------------------------------------------------------
void GUIList::scrollToLine(unsigned int lineIndex, int first_line_offset)
{
  // Copy in the request
  myFirstVisibleLineIndex = lineIndex;
  myFirstVisibleLineOffset = first_line_offset;
/*
  // TODO: this code is shared with refresh()
  int targetCanvasHeight = myTargetCanvas.getArea()->calculateSize().height;
  int additionalVisiblePixels = targetCanvasHeight % myLineHeight;
  int numberOfVisibleLines = targetCanvasHeight / myLineHeight + (additionalVisiblePixels ? 1 : 0);
  unsigned int numberOfLines = getNumberOfLines();

  // Make sure the requested line doesn't go beyond the end
  if (myFirstVisibleLineIndex + numberOfVisibleLines > numberOfLines)
  {
    // Calculate where to put the list canvas so that the end of
    // the list is entirely visible without going past it
    if (numberOfLines < numberOfVisibleLines)
    {
      myFirstVisibleLineIndex = 0;
      myFirstVisibleLineOffset = 0;
    }
    else
    {
      myFirstVisibleLineIndex = numberOfLines - numberOfVisibleLines;
      myFirstVisibleLineOffset = -additionalVisiblePixels;
    }
  }*/

  // Refresh this list
  refresh();
}



//-----------------------------------------------------------------------------
// Name: scrollToPercent
// Desc: Sets the list box such that the view begins with the given % of maximum.  If this is
//     not possible, it just makes the bottom entries visible
//-----------------------------------------------------------------------------
void GUIList::scrollToPercent(float percentIndex)
{
  // Calculate the total height of the list
  unsigned int numberOfLines = getNumberOfLines();
  unsigned int totalListHeight = numberOfLines * myLineHeight;

  // Remove the height of the visible area from the list height
  totalListHeight -= myTargetCanvas.getArea()->calculateSize().height;

  // If the height is negative, the list is shorter than the
  // visible are and we don't have to move it
  if (totalListHeight <= 0) return;

  // Make sure that the percent index is valid
  percentIndex = max(percentIndex, 0.0f);
  percentIndex = min(percentIndex, 1.0f);

  // Get the location of the scroll
  unsigned int offset = (unsigned int)(percentIndex * totalListHeight);
  myFirstVisibleLineOffset = -((int)(offset % myLineHeight));
  myFirstVisibleLineIndex = offset / myLineHeight;// - (myFirstVisibleLineOffset != 0 ? 1 : 0);

  // Refresh this list
  refresh();
}


//-----------------------------------------------------------------------------
// Name: getScrollIndexPercent
// Desc: Calculates the percent index of the list's scroll position
//-----------------------------------------------------------------------------
float GUIList::getScrollIndexPercent()
{
  // Calculate the total height of the list
  unsigned int numberOfLines = getNumberOfLines();
  unsigned int totalListHeight = numberOfLines * myLineHeight;

  unsigned int visibleAreaHeight = myTargetCanvas.getArea()->calculateSize().height;
  if (visibleAreaHeight > totalListHeight) return 1.0f;

  totalListHeight -= visibleAreaHeight;

  int position = myFirstVisibleLineIndex * myLineHeight - myFirstVisibleLineOffset;
  float percentIndex = position / (float)totalListHeight;

  // Make sure that the percent index is valid
  percentIndex = max(percentIndex, 0.0f);
  percentIndex = min(percentIndex, 1.0f);

  return percentIndex;
}



//-----------------------------------------------------------------------------
void GUIList::getScrollLine(unsigned int* first_line_index, int* first_line_offset) {
  *first_line_index = myFirstVisibleLineIndex;
  *first_line_offset = myFirstVisibleLineOffset;
}

//-----------------------------------------------------------------------------
void GUIList::registerElements(GUILayer* layer) {
  registerListEntryElements(layer);
}

//-----------------------------------------------------------------------------
void GUIList::unregisterElements(GUILayer* layer) {
  unregisterListEntryElements(layer);
}




//-----------------------------------------------------------------------------
GUIMouseSentinel* GUIList::getMouseSentinel() {
  return &myMouseSentinel;
}


//-----------------------------------------------------------------------------
void GUIList::registerListEntryElements(GUILayer* layer) {
  unsigned int numberOfLines = getNumberOfLines();
  for (unsigned int i = 0; i < numberOfLines; ++i) {
  ListEntry* entry = getListEntry(i);
  if (!entry) continue;
  entry->registerElements(layer);
  }
}


//-----------------------------------------------------------------------------
void GUIList::unregisterListEntryElements(GUILayer* layer) {
  unsigned int numberOfLines = getNumberOfLines();
  for (unsigned int i = 0; i < numberOfLines; ++i) {
  ListEntry* entry = getListEntry(i);
  if (!entry) continue;
  entry->unregisterElements(layer);
  }
}




//-----------------------------------------------------------------------------
// Name: onMouseLeftList
// Desc: Invoked by the mouse sentinel when the mouse has left the list
//-----------------------------------------------------------------------------
void GUIList::onMouseCrossedListBorder(bool inside)
{
}