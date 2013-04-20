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
#include "guikeyboard.h"


//-----------------------------------------------------------------------------
// Name: GUIKeyboardSentinel
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIKeyboardSentinel::GUIKeyboardSentinel()
{
  myEnabledFlag = true;
  myKeyboardSentinelParent = 0;
  myKeyboardSentinelFocusChild = 0;
  myKeyboardSentinelFirstChild = 0;
  myKeyboardSentinelChildRingSibling = this;
}



//-----------------------------------------------------------------------------
// Name: ~GUIKeyboardSentinel
// Desc: Removes this class from its parent and siblings
//-----------------------------------------------------------------------------
GUIKeyboardSentinel::~GUIKeyboardSentinel()
{
  removeFromKeyboardSentinelChain();
}



//-----------------------------------------------------------------------------
// Name: obtainKeyboardFocus
// Desc: Grabs the keyboard focus for this input sentinel
//-----------------------------------------------------------------------------
void GUIKeyboardSentinel::obtainKeyboardFocus()
{
  // Don't do this while disabled
  if (myEnabledFlag == false) return;

  // We can only obtain focus if the parent has focus
  if (myKeyboardSentinelParent)
  {
    // Get the parent's keyboard focus
    GUIKeyboardSentinel* currentParentFocus =
      myKeyboardSentinelParent->myKeyboardSentinelFocusChild;

    // Is it different than the current one?
    if (currentParentFocus != this)
    {
      // Let the old focus sentinel know that it's losing focus
      if (currentParentFocus)
      {
        // The old item lost its focus
        currentParentFocus->lostKeyboardFocus();
      }
      else
      {
        // This parent wasn't part of the focus chain, so we need to make sure the parent
        // itself has focus!
        myKeyboardSentinelParent->obtainKeyboardFocus();
      }

      // Set the parent's focus.  This has to happen after calling lostKeyboardFocus
      // on the old focus, because doing so usually sets this member to 'null'
      myKeyboardSentinelParent->myKeyboardSentinelFocusChild = this;

      // This sentinel now has keyboard focus
      onGotKeyboardFocus();
    }
  }
}



//-----------------------------------------------------------------------------
// Name: releaseKeyboardFocus
// Desc: If this element has keyboard focus, this method resets it
//-----------------------------------------------------------------------------
void GUIKeyboardSentinel::releaseKeyboardFocus()
{
  if (myKeyboardSentinelParent)
    if (myKeyboardSentinelParent->myKeyboardSentinelFocusChild == this)
      lostKeyboardFocus();
}



//-----------------------------------------------------------------------------
// Name: hasKeyboardFocus
// Desc: Determines whether or not this sentinel has keyboard focus
//-----------------------------------------------------------------------------
bool GUIKeyboardSentinel::hasKeyboardFocus() const
{
  return myKeyboardSentinelParent && myKeyboardSentinelParent->myKeyboardSentinelFocusChild == this;
}



//-----------------------------------------------------------------------------
// Name: setParentKeyboardSentinel
// Desc: Adds this keyboard sentinel to another sentinel's child list.  If this method
//     is called with a null parameter, it will remove the sentinel from its chain.
//-----------------------------------------------------------------------------
void GUIKeyboardSentinel::setParentKeyboardSentinel(GUIKeyboardSentinel* parent)
{
  // Make sure this keyboard sentinel is removed from its current list
  removeFromKeyboardSentinelChain();

  // If the parent exists, set it
  if (parent)
  {
    // Get the parent's first child
    GUIKeyboardSentinel* parentCurrentFirstChild = parent->myKeyboardSentinelFirstChild;

    // If the first child doesn't exist, our job is easy!  Just set this as the parent's first child
    if (!parentCurrentFirstChild)
    {
      // We only have to change the parent's child
      parent->myKeyboardSentinelFirstChild = this;
    }
    else
    {
      // Get the second child, so we can insert between the first and second
      GUIKeyboardSentinel* parentCurrentSecondChild = parentCurrentFirstChild->myKeyboardSentinelChildRingSibling;

      // Although the current first child can be self-linked (i.e. second == first), this
      // code doesn't need to change to handle taht condition.
      parentCurrentFirstChild->myKeyboardSentinelChildRingSibling = this;
      myKeyboardSentinelChildRingSibling = parentCurrentSecondChild;
    }

    // Set the parent of this sentinel
    myKeyboardSentinelParent = parent;
  }
}



//-----------------------------------------------------------------------------
// Name: removeFromKeyboardSentinelChain
// Desc: Extracts this sentinel from its previous sibling's and parents' references
//-----------------------------------------------------------------------------
void GUIKeyboardSentinel::removeFromKeyboardSentinelChain()
{
  // Free this sentinel from focus, if it has it
  releaseKeyboardFocus();

  // Search for the sentinel in the sibling chain that references this sentinel
  GUIKeyboardSentinel* sentinelThatReferencesThis = myKeyboardSentinelChildRingSibling;
  while(sentinelThatReferencesThis->myKeyboardSentinelChildRingSibling != this)
    sentinelThatReferencesThis = sentinelThatReferencesThis->myKeyboardSentinelChildRingSibling;

  // Extract from the child-ring loop
  sentinelThatReferencesThis->myKeyboardSentinelChildRingSibling = myKeyboardSentinelChildRingSibling;
  myKeyboardSentinelChildRingSibling = this;

  // Extract from the parent list, if necessary
  if (myKeyboardSentinelParent && (myKeyboardSentinelParent->myKeyboardSentinelFirstChild == this))
  {
    // Only swap in the referencing sentinel as the new child if it isn't this class
    myKeyboardSentinelParent->myKeyboardSentinelFirstChild = sentinelThatReferencesThis == this ? 0 : sentinelThatReferencesThis;
  }

  // Reset the parent pointer
  myKeyboardSentinelParent = 0;
}



//-----------------------------------------------------------------------------
// Name: onGotKeyboardFocus
// Desc: Called internally when this sentinel obtains focus
//-----------------------------------------------------------------------------
void GUIKeyboardSentinel::onGotKeyboardFocus()
{
}



//-----------------------------------------------------------------------------
// Name: onLostKeyboardFocus
// Desc: Called internally when this sentinel loses focus
//-----------------------------------------------------------------------------
void GUIKeyboardSentinel::onLostKeyboardFocus()
{
}



//-----------------------------------------------------------------------------
// Name: onKeyDown
// Desc: When a key is pressed, this method is invoked by the keyboard input source
//     @return 'true' To indicate the event should stop propogating, 'false' otherwise
//-----------------------------------------------------------------------------
bool GUIKeyboardSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard)
{
  // First, handle the focus child
  GUIKeyboardSentinel* focusChild = myKeyboardSentinelFocusChild;
  if (focusChild && focusChild->onKeyDown(key, keyboard)) return true;

  // Now pass the event to all of the children
  GUIKeyboardSentinel* firstChild = myKeyboardSentinelFirstChild;
  GUIKeyboardSentinel* currentChild = firstChild;
  do
  {
    // Exit if the current child becomes invalid
    if (!currentChild) break;

    // Process if this isn't the focus child
    if (currentChild != focusChild &&
        currentChild->isEnabled() &&
        currentChild->onKeyDown(key, keyboard))
    {
      // Optimize future calls by reordering the child list
      if (currentChild->myKeyboardSentinelParent == this)
        myKeyboardSentinelFirstChild = currentChild;

      // Don't propogate this event
      return true;
    }

    // Move to the next child
    currentChild = currentChild->myKeyboardSentinelChildRingSibling;

  } while(currentChild != firstChild);

  // This event wasn't processed
  return false;
}



//-----------------------------------------------------------------------------
// Name: onKeyUp
// Desc: When a key is released, this method is invoked by the keyboard input source
//     @return 'true' To indicate the event should stop propogating, 'false' otherwise
//-----------------------------------------------------------------------------
bool GUIKeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard)
{
  // First, handle the focus child
  GUIKeyboardSentinel* focusChild = myKeyboardSentinelFocusChild;
  if (focusChild && focusChild->onKeyUp(key, keyboard)) return true;

  // Now pass the event to all of the children
  GUIKeyboardSentinel* firstChild = myKeyboardSentinelFirstChild;
  GUIKeyboardSentinel* currentChild = firstChild;
  do
  {
    // Exit if the current child becomes invalid
    if (!currentChild) break;

    // Process if this isn't the focus child
    if (currentChild != focusChild &&
        currentChild->isEnabled() &&
        currentChild->onKeyUp(key, keyboard))
    {
      // Optimize future calls by reordering the child list
      if (currentChild->myKeyboardSentinelParent == this)
        myKeyboardSentinelFirstChild = currentChild;

      // Don't propogate this event
      return true;
    }

    // Move to the next child
    currentChild = currentChild->myKeyboardSentinelChildRingSibling;

  } while(currentChild != firstChild);

  // This event wasn't processed
  return false;
}



//-----------------------------------------------------------------------------
// Name: onTypedCharacter
// Desc: Called when the user types a key associated with a printable character
//     @return 'true' To indicate the event should stop propogating, 'false' otherwise
//-----------------------------------------------------------------------------
bool GUIKeyboardSentinel::onTypedCharacter(char character, GUIKeyboardInputSource* keyboard)
{
  // First, handle the focus child
  GUIKeyboardSentinel* focusChild = myKeyboardSentinelFocusChild;
  if (focusChild && focusChild->onTypedCharacter(character, keyboard)) return true;

  // Now pass the event to all of the children
  GUIKeyboardSentinel* firstChild = myKeyboardSentinelFirstChild;
  GUIKeyboardSentinel* currentChild = firstChild;
  do
  {
    // Exit if the current child becomes invalid
    if (!currentChild) break;

    // Process if this isn't the focus child
    if (currentChild != focusChild &&
        currentChild->isEnabled() &&
        currentChild->onTypedCharacter(character, keyboard))
    {
      // Optimize future calls by reordering the child list
      if (currentChild->myKeyboardSentinelParent == this)
        myKeyboardSentinelFirstChild = currentChild;

      // Don't propogate this event
      return true;
    }

    // Move to the next child
    currentChild = currentChild->myKeyboardSentinelChildRingSibling;

  } while(currentChild != firstChild);

  // This event wasn't processed
  return false;
}





//-----------------------------------------------------------------------------
// Name: lostKeyboardFocus
// Desc: Called internally when this sentinel loses keyboard focus
//-----------------------------------------------------------------------------
void GUIKeyboardSentinel::lostKeyboardFocus()
{
  // Call this method for all of the children, while they are valid
  if (myKeyboardSentinelFocusChild) myKeyboardSentinelFocusChild->lostKeyboardFocus();

  // Reset the parent's focus, since it should now be invalid
  if (myKeyboardSentinelParent) myKeyboardSentinelParent->myKeyboardSentinelFocusChild = 0;

  // Keyboard focus for this element has been lost
  onLostKeyboardFocus();
}



void GUIKeyboardSentinel::enable() {
  myEnabledFlag = true;
}

void GUIKeyboardSentinel::disable() {
  releaseKeyboardFocus();
  myEnabledFlag = false;
}

bool GUIKeyboardSentinel::isEnabled() const {
  return myEnabledFlag;
}