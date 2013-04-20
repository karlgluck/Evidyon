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
#include "guimouse.h"




//-----------------------------------------------------------------------------
// Name: GUIMouseSentinel
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIMouseSentinel::GUIMouseSentinel()
{
  myEnabledFlag = true;
  myMouseSentinelParent = 0;
  myMouseSentinelFocusChild = 0;
  myMouseSentinelFirstChild = 0;
  myMouseSentinelChildRingSibling = this;
}



//-----------------------------------------------------------------------------
// Name: ~GUIMouseSentinel
// Desc: Removes this class from its parent and siblings
//-----------------------------------------------------------------------------
GUIMouseSentinel::~GUIMouseSentinel()
{
  removeFromMouseSentinelChain();
}



//-----------------------------------------------------------------------------
// Name: obtainMouseFocus
// Desc: Grabs the mouse focus for this input sentinel from whatever has it right now
//-----------------------------------------------------------------------------
void GUIMouseSentinel::obtainMouseFocus()
{
  // Don't do this while disabled
  if (myEnabledFlag == false) return;

  // We can only obtain focus if the parent has focus
  if (myMouseSentinelParent)
  {
    // Get the parent's sentinel focus
    GUIMouseSentinel* currentParentFocus =
      myMouseSentinelParent->myMouseSentinelFocusChild;

    // Is it different than the current one?
    if (currentParentFocus != this)
    {
      // Let the old focus sentinel know that it's losing focus
      if (currentParentFocus)
      {
        // The old item lost its focus
        currentParentFocus->lostMouseFocus();
      }
      else
      {
        // This parent wasn't part of the focus chain, so we need to make sure the parent
        // itself has focus!
        myMouseSentinelParent->obtainMouseFocus();
      }

      // Set the parent's focus.  This has to happen after calling lostMouseFocus
      // on the old focus, because doing so usually sets this member to 'null'
      myMouseSentinelParent->myMouseSentinelFocusChild = this;

      // This sentinel now has focus
      onGotMouseFocus();
    }
  }
}



//-----------------------------------------------------------------------------
// Name: releaseMouseFocus
// Desc: If this element has mouse focus, this method resets it
//-----------------------------------------------------------------------------
void GUIMouseSentinel::releaseMouseFocus()
{
  if (myMouseSentinelParent)
    if (myMouseSentinelParent->myMouseSentinelFocusChild == this)
      lostMouseFocus();
}



//-----------------------------------------------------------------------------
// Name: hasMouseFocus
// Desc: Determines whether or not this sentinel has mouse focus
//-----------------------------------------------------------------------------
bool GUIMouseSentinel::hasMouseFocus() const
{
  return myMouseSentinelParent && myMouseSentinelParent->myMouseSentinelFocusChild == this;
}



//-----------------------------------------------------------------------------
// Name: setParentMouseSentinel
// Desc: Adds this mouse sentinel to another sentinel's child list.  If this method
//     is called with a null parameter, it will remove the sentinel from its chain.
//-----------------------------------------------------------------------------
void GUIMouseSentinel::setParentMouseSentinel(GUIMouseSentinel* parent)
{
  // Make sure this mouse sentinel is removed from its current list
  removeFromMouseSentinelChain();

  // If the parent exists, set it
  if (parent)
  {
    // Get the parent's first child
    GUIMouseSentinel* parentCurrentFirstChild = parent->myMouseSentinelFirstChild;

    // If the first child doesn't exist, our job is easy!  Just set this as the parent's first child
    if (!parentCurrentFirstChild)
    {
      // We only have to change the parent's child
      parent->myMouseSentinelFirstChild = this;
    }
    else
    {
      // Get the second child, so we can insert between the first and second
      GUIMouseSentinel* parentCurrentSecondChild = parentCurrentFirstChild->myMouseSentinelChildRingSibling;

      // Although the current first child can be self-linked (i.e. second == first), this
      // code doesn't need to change to handle taht condition.
      parentCurrentFirstChild->myMouseSentinelChildRingSibling = this;
      myMouseSentinelChildRingSibling = parentCurrentSecondChild;
    }

    // Set the parent of this sentinel
    myMouseSentinelParent = parent;
  }
}



//-----------------------------------------------------------------------------
// Name: removeFromMouseSentinelChain
// Desc: Extracts this sentinel from its previous sibling's and parents' references
//-----------------------------------------------------------------------------
void GUIMouseSentinel::removeFromMouseSentinelChain()
{
  // Free this sentinel from focus, if it has it
  releaseMouseFocus();

  // Search for the sentinel in the sibling chain that references this sentinel
  GUIMouseSentinel* sentinelThatReferencesThis = myMouseSentinelChildRingSibling;
  while(sentinelThatReferencesThis->myMouseSentinelChildRingSibling != this)
    sentinelThatReferencesThis = sentinelThatReferencesThis->myMouseSentinelChildRingSibling;

  // Extract from the child-ring loop
  sentinelThatReferencesThis->myMouseSentinelChildRingSibling = myMouseSentinelChildRingSibling;
  myMouseSentinelChildRingSibling = this;

  // Extract from the parent list, if necessary
  if (myMouseSentinelParent && (myMouseSentinelParent->myMouseSentinelFirstChild == this))
  {
    // Only swap in the referencing sentinel as the new child if it isn't this class
    myMouseSentinelParent->myMouseSentinelFirstChild = sentinelThatReferencesThis == this ? 0 : sentinelThatReferencesThis;
  }

  // Reset the parent pointer
  myMouseSentinelParent = 0;
}



//-----------------------------------------------------------------------------
// Name: onGotMouseFocus
// Desc: Called internally when this sentinel obtains focus
//-----------------------------------------------------------------------------
void GUIMouseSentinel::onGotMouseFocus()
{
}



//-----------------------------------------------------------------------------
// Name: onLostMouseFocus
// Desc: Called internally when this sentinel loses focus
//-----------------------------------------------------------------------------
void GUIMouseSentinel::onLostMouseFocus()
{
}



//-----------------------------------------------------------------------------
// Name: onMouseMove
// Desc: Called by the mouse input source when the mouse moves
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIMouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)
{
  // First, handle the focus child
  GUIMouseSentinel* focusChild = myMouseSentinelFocusChild;
  if (focusChild && focusChild->onMouseMove(position, oldPosition, mouse)) return true;

  // Now pass the event to all of the children
  GUIMouseSentinel* firstChild = myMouseSentinelFirstChild;
  GUIMouseSentinel* currentChild = firstChild;
  do
  {
    // Exit if the current child becomes invalid.  This is only really supposed
    // to catch the first child, but if something goes wrong and a child's
    // siblings are mis-linked, this will prevent an access fault.
    if (!currentChild) break;
    if (currentChild->myMouseSentinelParent != this) break;

    // Process if this isn't the focus child
    if (currentChild != focusChild &&
        currentChild->isEnabled() &&
        currentChild->onMouseMove(position, oldPosition, mouse))
    {
      // Optimize future calls by reordering the child list.  We have to check to make
      // sure this child is still a member, however!  If the event activates a
      // method that causes the child to remove itself from the input sentinel chain,
      // this causes a very difficult-to-find bug.
      if (currentChild->myMouseSentinelParent == this)
        myMouseSentinelFirstChild = currentChild;

      // Don't propogate this event
      return true;
    }

    // Move to the next child
    currentChild = currentChild->myMouseSentinelChildRingSibling;

  } while(currentChild != firstChild);

  // This event wasn't processed
  return false;
}



//-----------------------------------------------------------------------------
// Name: onMouseButtonAction
// Desc: Called by the mouse input source when a mouse button alters its state
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIMouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)
{
  // First, handle the focus child
  GUIMouseSentinel* focusChild = myMouseSentinelFocusChild;
  if (focusChild && focusChild->onMouseButtonAction(action, position, mouse)) return true;

  // Now pass the event to all of the children
  GUIMouseSentinel* firstChild = myMouseSentinelFirstChild;
  GUIMouseSentinel* currentChild = firstChild;
  do
  {
    // Exit if the current child becomes invalid
    if (!currentChild) break;

    // Process if this isn't the focus child
    if (currentChild != focusChild &&
        currentChild->isEnabled() &&
        currentChild->onMouseButtonAction(action, position, mouse))
    {
      // Optimize future calls by reordering the child list
      if (currentChild->myMouseSentinelParent == this)
        myMouseSentinelFirstChild = currentChild;

      // Don't propogate this event
      return true;
    }

    // Move to the next child
    currentChild = currentChild->myMouseSentinelChildRingSibling;

  } while(currentChild != firstChild);

  // This event wasn't processed
  return false;
}


//-----------------------------------------------------------------------------
// Name: onMouseScroll
// Desc: 
//-----------------------------------------------------------------------------
bool GUIMouseSentinel::onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse)
{
  // First, handle the focus child
  GUIMouseSentinel* focusChild = myMouseSentinelFocusChild;
  if (focusChild && focusChild->onMouseScroll(position, amount, lines, mouse)) return true;

  // Now pass the event to all of the children
  GUIMouseSentinel* firstChild = myMouseSentinelFirstChild;
  GUIMouseSentinel* currentChild = firstChild;
  do
  {
    // Exit if the current child becomes invalid
    if (!currentChild) break;

    // Process if this isn't the focus child
    if (currentChild != focusChild &&
        currentChild->isEnabled() &&
        currentChild->onMouseScroll(position, amount, lines, mouse))
    {
      // Optimize future calls by reordering the child list
      if (currentChild->myMouseSentinelParent == this)
        myMouseSentinelFirstChild = currentChild;

      // Don't propogate this event
      return true;
    }

    // Move to the next child
    currentChild = currentChild->myMouseSentinelChildRingSibling;

  } while(currentChild != firstChild);

  // This event wasn't processed
  return false;
}



//-----------------------------------------------------------------------------
// Name: lostMouseFocus
// Desc: Called internally when this sentinel loses mouse focus
//-----------------------------------------------------------------------------
void GUIMouseSentinel::lostMouseFocus()
{
  // Call this method for all of the children, while they are valid
  if (myMouseSentinelFocusChild) myMouseSentinelFocusChild->lostMouseFocus();

  // Reset the parent's focus, since it should now be invalid
  if (myMouseSentinelParent) myMouseSentinelParent->myMouseSentinelFocusChild = 0;

  // Mouse focus for this element has been lost
  onLostMouseFocus();
}



void GUIMouseSentinel::enable() {
  myEnabledFlag = true;
}

void GUIMouseSentinel::disable() {
  releaseMouseFocus();
  myEnabledFlag = false;
}

bool GUIMouseSentinel::isEnabled() const {
  return myEnabledFlag;
}