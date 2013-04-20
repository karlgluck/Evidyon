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
#ifndef __GUIMOUSESENTINEL_H__
#define __GUIMOUSESENTINEL_H__
#pragma once





/**
 * Classes that implement this sentinel are passed messages about mouse events
 */
class GUIMouseSentinel
{
  public:

    /**
     * Initializes this class
     */
    GUIMouseSentinel();

    /**
     * Removes this class from its parent and siblings
     */
    ~GUIMouseSentinel();

    /**
     * Grabs the mouse focus for this input sentinel from whatever has it right now
     */
    void obtainMouseFocus();

    /**
     * If this element has mouse focus, this method resets it
     */
    void releaseMouseFocus();

    /**
     * Determines whether or not this sentinel has mouse focus
     */
    bool hasMouseFocus() const;

    /**
     * Adds this mouse sentinel to another sentinel's child list.  If this method
     * is called with a null parameter, it will remove the sentinel from its chain.
     */
    void setParentMouseSentinel(GUIMouseSentinel* parent);

    /**
     * Extracts this sentinel from its previous sibling's and parents' references
     */
    void removeFromMouseSentinelChain();

    /**
     * Removes each of the children from the sentinel chain.  Use with caution!
     */
    void removeChildrenFromMouseSentinelChain();

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
    virtual void onGotMouseFocus();

    /**
     * Called internally when this sentinel loses focus
     */
    virtual void onLostMouseFocus();

    /**
     * Called by the mouse input source when the mouse moves.  The default
     * implementation propogates the message based on the focus/hierarchy
     * structure established for the sentinels.
     * @return 'true' To indicate the event should stop propogating
     */
    virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);

    /**
     * Called by the mouse input source when a mouse button alters its
     * state.  The default implementation propogates the message based
     * on the focus/hierarchy structure established for the sentinels.
     * @return 'true' To indicate the event should stop propogating
     */
    virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

    /**
     * Called when the mouse scrolls up (-lines) or down (+lines)
     */
    virtual bool onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse);


  private:

    /// Whether or not this sentinel should be active. A sentinel is enabled
    // by default.
    bool myEnabledFlag;

    /// The parent of this sentinel
    GUIMouseSentinel* myMouseSentinelParent;

    /// Messages are always passed to the focus child first; this can be null if none
    /// of the sentinels have focus.
    GUIMouseSentinel* myMouseSentinelFocusChild;

    /// If messages return 'false' for the focus child, they are
    /// then passed to the rest of the children until one returns 'true'
    /// or they have all been notified.
    GUIMouseSentinel* myMouseSentinelFirstChild;

    /// This method points to the next child-of-its-parent in this sentinel's ring.  This
    /// pointer can reference this instance if there is only 1 child.
    GUIMouseSentinel* myMouseSentinelChildRingSibling;


  private:

    /**
     * Called internally when this sentinel loses mouse focus
     */
    void lostMouseFocus();
};





#endif