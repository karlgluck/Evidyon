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
#ifndef __GUIEDITABLETEXTLINE_H__
#define __GUIEDITABLETEXTLINE_H__
#pragma once


#include "guicomponent.h"

class GUIEditableTextLine : public GUIComponent
{

  /**
   * Controls rendering the text for this text line
   */
  class TextElement : public GUITextElement
  {
    public:

      /**
       * Initializes the class
       */
      TextElement(GUIEditableTextLine* ownerTextLine);

      /**
       * Changes the font used to render this text
       */
      void setFont(GUIFont* font);

      /**
       * Changes this text's color
       */
      void setTextColor(D3DCOLOR color);

      /**
       * Sets the flag that determines whether or not this text is a password
       */
      void setPasswordFlag(bool isPassword);

      /**
       * Determines whether or not this is a password line
       */
      bool getPasswordFlag() const;


    protected:

      /**
       * Renders this text element.  This method will call GUITextElement::render
       * first, then render the caret for this element (if it is in focus)
       */
      virtual bool render(GUILayer* layer);


    public:

      /**
       * Gets the font to use to render the this label
       */
      GUIFont* getFont();

      /**
       * Returns the text for this label
       */
      bool getNextTextSegment(TextSegmentInfo* nextSegmentInfo);


    protected:

      /// The text line of which this element is a member variable
      GUIEditableTextLine* myTextLine;

      /// The font to use to render this label
      GUIFont* myFont;

      /// Whether or not this is password text
      bool myTextIsPassword;

      /// The color of text to render
      D3DCOLOR myTextColor;
  };

  /**
   * This sentinel watches for mouse input.
   */
  class MouseSentinel : public GUIMouseSentinel
  {
    public:

      /**
       * Initializes this class
       */
      MouseSentinel(GUIEditableTextLine* textLine);

    protected:

      /**
       * Called by the mouse input source when this sentinel loses mouse focus.  This
       * method makes sure the text line's keyboard focus is revoked as well.
       */
      virtual void onLostMouseFocus();

      /**
       * Called by the mouse input source when a mouse button alters its state
       * @return 'true' To indicate the event should stop propogating
       */
      virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);


    protected:

      /// The text line that owns this element
      GUIEditableTextLine* myOwnerTextLine;
  };

  /**
   * Interprets keyboard input for this class
   */
  class KeyboardSentinel : public GUIKeyboardSentinel
  {
    public:

      /**
       * Initializes this sentinel
       */
      KeyboardSentinel(GUIEditableTextLine* textLine);


    protected:

      /**
       * Called by the keyboard input source when this sentinel loses mouse focus.  This
       * method makes sure the text line's mouse focus is revoked as well.
       */
      virtual void onLostKeyboardFocus();

      /**
       * Called by the keyboard input source when this sentinel obtains keyboard focus.  This
       * method makes sure the text line's mouse focus is obtained as well.
       */
      virtual void onGotKeyboardFocus();

      /**
       * When a key is pressed, this method is invoked by the keyboard input source.  This method
       * watches for the delete, escape and arrow keys
       * @return 'true' To indicate the event should stop propogating
       */
      virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);

      /**
       * Called when the user types a key associated with a printable character.  This method
       * does some initial processing on the character to make sure that it is valid.
       * @return 'true' To indicate the event should stop propogating
       */
      virtual bool onTypedCharacter(char character, GUIKeyboardInputSource* keyboard);


    protected:

      /// The text line that owns this element
      GUIEditableTextLine* myOwnerTextLine;
  };

  public:

    /**
     * Initializes the class
     */
    GUIEditableTextLine();

    /**
     * Ensures the sentinels aren't registered as keyboard/mouse focuses and
     * closes down this class
     */
    ~GUIEditableTextLine();

    /**
     * Changes the font used to render this text
     */
    void setFont(GUIFont* font);

    /**
     * Changes this text's color
     */
    void setTextColor(D3DCOLOR color);

    /**
     * Sets the flag that determines whether or not this text is a passwrd
     */
    void setPasswordFlag(bool isPassword);

    /**
     * Changes the displayed text
     */
    void setText(const std::string& text);

    /**
     * Gets a the current contents of the text buffer
     */
    const char* getText() const;

    /**
     * Gets focus for this text line
     */
    void obtainFocus();

    /**
     * Determines whether or not this element currently has focus
     */
    bool hasFocus() const;

    /**
     * Frees the focus for this text line
     */
    void releaseFocus();

    /**
     * Registers this component's visible elements with the given layer
     */
    void registerElements(GUILayer* layer);

    /**
     * Unregisters this component's visible elements
     */
    void unregisterElements(GUILayer* layer);

    /**
     * Gets this component's mouse sentinel so that it can be added to something which triggers its events
     */
    GUIMouseSentinel* getMouseSentinel();

    /**
     * Gets this component's keyboard sentinel so that it can be registered
     */
    GUIKeyboardSentinel* getKeyboardSentinel();


  protected:

    /**
     * When the user clicks inside, the text line acquires keyboard/mouse focus and positions
     * the caret.  Actions such as mouse down/up, double-click and all alternate buttons are
     * ignored by this method.
     */
    virtual void onMouseButtonActionInside(GUIMouseButtonAction action, GUIPoint point, GUIMouseInputSource* mouse);

    /**
     * Called when this text line gets focus
     */
    virtual void onGotFocus();

    /**
     * Called when this text line loses user focus.  This method resets the horizontal text
     * offset
     */
    virtual void onLostFocus();

    /**
     * Invoked when the user enters a valid character that should be added to the internal buffer
     */
    virtual void onTypedPrintableCharacter(char printableCharacter);

    /**
     * Called when the user enters a backspace character
     */
    virtual void onBackspace();

    /**
     * Called when the user presses 'delete'
     */
    virtual void onDelete();


  protected:

    /// The text in this line
    std::string myText;

    /// The mouse input sentinel for this text line
    MouseSentinel myMouseSentinel;

    /// The keyboard input sentinel for this text line
    KeyboardSentinel myKeyboardSentinel;

    /// The text-rendering element
    TextElement myTextElement;

    /// If this line has focus, this represents the character location of the editing caret
    int myCaretLocation;

    /// The x-offset of the text in the line.  This is adjusted so that the caret is always
    /// visible, or if the element doesn't have focus, is zet to zero.  This is never more
    /// than zero.
    float myHorizontalTextOffset;

};



#endif