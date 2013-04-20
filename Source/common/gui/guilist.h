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
#ifndef __GUILIST_H__
#define __GUILIST_H__
#pragma once



#include "guicomponent.h"

/**
 * Manages the basic format of a list of items
 */
class GUIList : public GUIComponent
{
  /**
   * This class is used by the GUIList class to watch for input from the mouse.  This class
   * only passes input to children if the mouse action occurs within the limits of the
   * main canvas of its parent's list.
   */
  class MouseSentinel : public GUIMouseSentinel
  {
    public:

      /**
       * Initializes this list sentinel
       */
      MouseSentinel(GUIList* list);


    protected:

      /**
       * Called by the mouse input source when the mouse moves
       * @return 'true' To indicate the event should stop propogating
       */
      bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);

      /**
       * Called by the mouse input source when a mouse button alters its state
       * @return 'true' To indicate the event should stop propogating
       */
      bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);


    protected:

      /// The list for which this sentinel watches for input
      GUIList* myList;
  };

  /**
   * All lines should contain a sentinel of this type.  If the lines require input
   * notification, they should add this sentinel as a parent (do not derive from
   * this class).
   */
  class LineMouseSentinel : public GUIMouseSentinel
  {
    public:

      /**
       * Initializes this class
       */
      LineMouseSentinel(GUICanvas* sourceCanvas);


    public:

      /**
       * Called by the mouse input source when the mouse moves.  Only propogates messages if
       * the source canvas is valid and the position or old position is within the boundaries
       * of the source canvas.
       * @return 'true' To indicate the event should stop propogating
       */
      bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);

      /**
       * Called by the mouse input source when a mouse button alters its state.  Only
       * propogates messages if the source canvas is valid and the position is within
       * the boundaries of the source canvas.
       * @return 'true' To indicate the event should stop propogating
       */
      bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);


    protected:

      /// The canvas for this line
      GUICanvas* mySourceCanvas;
  };


  public:

    /**
     * Subclasses of GUIList should use this class to manage
     * the rendering of and mouse input to entries in the list.
     */
    class ListEntry : public GUIComponent
    {
      // Allow the list to manage what is inside of this entry
      friend class GUIList;

      public:

        /**
         * Initializes this class
         */
        ListEntry();

        /**
         * Adds a mouse sentinel that is notified of actions inside of this list entry
         */
        void addChildMouseSentinel(GUIMouseSentinel* sentinel);

        /**
         * Adds a canvas that is updated by this line
         */
        void addChildCanvas(GUICanvas* canvas);

        /**
         * Gets this line's canvas coordinates
         */
        const GUIRect* getLineArea();

        /**
         * Gets the area into which elements for this list can render.  If this element
         * is uninitialized, it will return 'null'
         */
        const GUIRect* getClippingArea();

        /**
         * Determines whether or not this list entry is visible
         */
        bool isVisible();

        virtual GUIMouseSentinel* getMouseSentinel();

      public:

        /**
         * Allows derived classes to update themselves when this structure changes.  Specifically,
         * this method is called by GUIList after the clipping canvas is set.
         */
        virtual void refresh();


      private:

        /// The canvas that locates this line
        GUICanvas myLineCanvas;

        /// The canvas to which elements in this list entry should clip themselves.  This
        /// is the target canvas of the owner GUIList.
        GUICanvas* myClippingCanvas;

        /// The sentinel that monitors mouse input for this entry
        LineMouseSentinel myMouseSentinel;
    };


  public:

    /**
     * Initializes this class
     */
    GUIList();

    /** 
     * Sets the height of each line in this list in pixels.  This method refreshes
     * the list so that the canvases are updated.
     */
    void setLineHeight(int pixels);

    /**
     * Updates the list entries to make sure only the valid ones are visible
     */
    void refresh();

    /**
     * Ensures that the given line is visible
     */
    void scrollToLine(unsigned int lineIndex, int first_line_offset);

    /**
     * Sets the list box such that the view begins with the given % of maximum
     */
    void scrollToPercent(float percentIndex);

    /**
     * Calculates the percent index of the list's scroll position
     */
    float getScrollIndexPercent();

    /**
     * Gets the line at which the view currently is positioned
     */
    void getScrollLine(unsigned int* first_line_index, int* first_line_offset);

    virtual void registerElements(GUILayer* layer);
    virtual void unregisterElements(GUILayer* layer);
    virtual GUIMouseSentinel* getMouseSentinel();

  protected:

    /**
     * Invoked by the mouse sentinel when the mouse leaves or enters the
     * list.
     */
    virtual void onMouseCrossedListBorder(bool inside);

    /**
     * Returns the number of lines in this list box
     */
    virtual unsigned int getNumberOfLines() const = 0;

    /**
     * Gets the list entry base class that corresponds to the given list item.  If this
     * method returns 0, the list item will not render correctly.
     */
    virtual ListEntry* getListEntry(unsigned int index) = 0;

    void registerListEntryElements(GUILayer* layer);
    void unregisterListEntryElements(GUILayer* layer);

  protected:

    /// The area into which this list of items renders.  GUI elements should clip themselves
    /// to this canvas space so that they don't overflow its boundaries.
    GUICanvas myTargetCanvas;

  private:

    /// The mouse sentinel watches for input from the mouse and passes it to available children
    MouseSentinel myMouseSentinel;

    /// This canvas holds the location of the list.  The target canvas is used like a window;
    /// this list canvas moves the list items around behind that window.
    GUICanvas myListCanvas;

    /// How much room to give each item in the list
    unsigned int myLineHeight;

    /// This is the index of the first line in the list box that is visible as of the last refresh
    unsigned int myFirstVisibleLineIndex;

    /// The offset between lines
    int myFirstVisibleLineOffset;
};


#endif