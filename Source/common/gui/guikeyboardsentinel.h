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
#ifndef __GUIKEYBOARDSENTINEL_H__
#define __GUIKEYBOARDSENTINEL_H__
#pragma once



/**
 * Classes that implement this sentinel are passed messages about keyboard events
 */
class GUIKeyboardSentinel
{
  public:

    /**
     * Initializes this class
     */
    GUIKeyboardSentinel();

    /**
     * Removes this class from its parent and siblings
     */
    ~GUIKeyboardSentinel();

    /**
     * Grabs the keyboard focus for this input sentinel
     */
    void obtainKeyboardFocus();

    /**
     * If this element has keyboard focus, this method resets it
     */
    void releaseKeyboardFocus();

    /**
     * Determines whether or not this sentinel has keyboard focus
     */
    bool hasKeyboardFocus() const;

    /**
     * Adds this keyboard sentinel to another sentinel's child list.  If this method
     * is called with a null parameter, it will remove the sentinel from its chain.
     */
    void setParentKeyboardSentinel(GUIKeyboardSentinel* parent);

    /**
     * Extracts this sentinel from its previous sibling's and parents' references
     */
    void removeFromKeyboardSentinelChain();

    /**
     * Disabling this sentinel will also cause it to lose focus.  Once disabled,
     * the sentinel will no longer be passed messages and will not be able to
     * obtain focus.
     * Disabling a sentinel does not break the input chain.
     */
    void disable();
    void enable();
    bool isEnabled() const;

  public:

    /**
     * Called internally when this sentinel obtains focus
     */
    virtual void onGotKeyboardFocus();

    /**
     * Called internally when this sentinel loses focus
     */
    virtual void onLostKeyboardFocus();

    /**
     * When a key is pressed, this method is invoked by the keyboard input source
     * @return 'true' To indicate the event should stop propogating, 'false' otherwise
     */
    virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);

    /**
     * When a key is released, this method is invoked by the keyboard input source
     * @return 'true' To indicate the event should stop propogating, 'false' otherwise
     */
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);

    /**
     * Called when the user types a key associated with a printable character
     * @return 'true' To indicate the event should stop propogating, 'false' otherwise
     */
    virtual bool onTypedCharacter(char character, GUIKeyboardInputSource* keyboard);


  private:

    /// Whether or not this sentinel should be active.  If it is disabled, the
    /// sentinel will not respond to any command and will not be passed messages
    /// until it is re-activated.
    /// A sentinel is enabled by default.
    bool myEnabledFlag;

    /// The parent of this sentinel
    GUIKeyboardSentinel* myKeyboardSentinelParent;

    /// Messages are always passed to the focus child first; this can be null if none
    /// of the sentinels have focus.
    GUIKeyboardSentinel* myKeyboardSentinelFocusChild;

    /// If messages return 'false' for the focus child, they are
    /// then passed to the rest of the children until one returns 'true'
    /// or they have all been notified.
    GUIKeyboardSentinel* myKeyboardSentinelFirstChild;

    /// This method points to the next child-of-its-parent in this sentinel's ring.  This
    /// pointer can reference this instance if there is only 1 child.
    GUIKeyboardSentinel* myKeyboardSentinelChildRingSibling;


  private:

    /**
     * Called internally when this sentinel loses keyboard focus
     */
    void lostKeyboardFocus();
};


#endif