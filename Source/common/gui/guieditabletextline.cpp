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
#include "guieditabletextline.h"



//-----------------------------------------------------------------------------
// Name: GUIEditableTextLine
// Desc: Initializes the class
//-----------------------------------------------------------------------------
GUIEditableTextLine::GUIEditableTextLine()
: myMouseSentinel(this), myKeyboardSentinel(this), myTextElement(this)
{
  myCaretLocation = 0;
  myHorizontalTextOffset = 0.0f;
  myTextElement.setClippingArea(getCanvas()->getArea());
  myTextElement.setSourceCanvas(getCanvas());
}



//-----------------------------------------------------------------------------
// Name: ~GUIEditableTextLine
// Desc: Ensures the sentinels aren't registered as keyboard/mouse focuses and
//     closes down this class
//-----------------------------------------------------------------------------
GUIEditableTextLine::~GUIEditableTextLine()
{
}



//-----------------------------------------------------------------------------
// Name: setFont
// Desc: Changes the font used to render this text
//-----------------------------------------------------------------------------
void GUIEditableTextLine::setFont(GUIFont* font)
{
  myTextElement.setFont(font);
}



//-----------------------------------------------------------------------------
// Name: setTextColor
// Desc: Changes this text's color
//-----------------------------------------------------------------------------
void GUIEditableTextLine::setTextColor(D3DCOLOR color)
{
  myTextElement.setTextColor(color);
}



//-----------------------------------------------------------------------------
// Name: setPasswordFlag
// Desc: Sets the flag that determines whether or not this text is a passwrd
//-----------------------------------------------------------------------------
void GUIEditableTextLine::setPasswordFlag(bool isPassword)
{
  myTextElement.setPasswordFlag(isPassword);
}



//-----------------------------------------------------------------------------
// Name: setText
// Desc: Changes the displayed text
//-----------------------------------------------------------------------------
void GUIEditableTextLine::setText(const std::string& text)
{
  myText = text;
  myCaretLocation = text.length();
}



//-----------------------------------------------------------------------------
// Name: getText
// Desc: Gets a the current contents of the text buffer
//-----------------------------------------------------------------------------
const char* GUIEditableTextLine::getText() const
{
  return myText.c_str();
}



//-----------------------------------------------------------------------------
// Name: obtainFocus
// Desc: Gets focus for this text line
//-----------------------------------------------------------------------------
void GUIEditableTextLine::obtainFocus()
{
  myMouseSentinel.obtainMouseFocus();
  myKeyboardSentinel.obtainKeyboardFocus();
}



//-----------------------------------------------------------------------------
// Name: hasFocus
// Desc: Determines whether or not this element currently has focus
//-----------------------------------------------------------------------------
bool GUIEditableTextLine::hasFocus() const
{
  return myMouseSentinel.hasMouseFocus() && myKeyboardSentinel.hasKeyboardFocus();
}



//-----------------------------------------------------------------------------
// Name: releaseFocus
// Desc: Frees the focus for this text line
//-----------------------------------------------------------------------------
void GUIEditableTextLine::releaseFocus()
{
  myMouseSentinel.releaseMouseFocus();
  myKeyboardSentinel.releaseKeyboardFocus();
}



//-----------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers this component's visible elements with the given layer
//-----------------------------------------------------------------------------
void GUIEditableTextLine::registerElements(GUILayer* layer)
{
  layer->registerElement(&myTextElement);
}



//-----------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Unregisters this component's visible elements
//-----------------------------------------------------------------------------
void GUIEditableTextLine::unregisterElements(GUILayer* layer)
{
  layer->unregisterElement(&myTextElement);
}




//-----------------------------------------------------------------------------
// Name: getMouseSentinel
// Desc: Gets this component's mouse sentinel so that it can be added to something which triggers its events
//-----------------------------------------------------------------------------
GUIMouseSentinel* GUIEditableTextLine::getMouseSentinel()
{
  return &myMouseSentinel;
}



//-----------------------------------------------------------------------------
// Name: getKeyboardSentinel
// Desc: Gets this component's keyboard sentinel so that it can be registered
//-----------------------------------------------------------------------------
GUIKeyboardSentinel* GUIEditableTextLine::getKeyboardSentinel()
{
  return &myKeyboardSentinel;
}



//-----------------------------------------------------------------------------
// Name: onMouseButtonActionInside
// Desc: When the user clicks inside, the text line acquires keyboard/mouse focus and positions
//     the caret.  Actions such as mouse down/up, double-click and all alternate buttons are
//     ignored by this method.
//-----------------------------------------------------------------------------
void GUIEditableTextLine::onMouseButtonActionInside(GUIMouseButtonAction action, GUIPoint point, GUIMouseInputSource* mouse)
{
  if (action == GUIMOUSE_CLICK_SINGLE || action == GUIMOUSE_UP)
  {
    GUIPoint pos = point;
    pos.x -= getCanvas()->getArea()->left + myHorizontalTextOffset;
    pos.y = -1;

    if (myTextElement.getPasswordFlag())
    {
      GUISize size;
      myTextElement.getFont()->getTextExtent("*", 1, &size);
      myCaretLocation = pos.x / size.width;
      if (myCaretLocation > myText.length()) myCaretLocation = myText.length();
    }
    else
      myTextElement.getFont()->getCharacterFromPoint(myText.c_str(), -1, pos, &myCaretLocation);

    // This text line got focus
    onGotFocus();
  }
}



//-----------------------------------------------------------------------------
// Name: onGotFocus
// Desc: Called when this text line gets focus
//-----------------------------------------------------------------------------
void GUIEditableTextLine::onGotFocus()
{
}



//-----------------------------------------------------------------------------
// Name: onLostFocus
// Desc: Called when this text line loses user focus.  This method resets the horizontal text
//     offset
//-----------------------------------------------------------------------------
void GUIEditableTextLine::onLostFocus()
{
  myHorizontalTextOffset = 0.0f;
}



//-----------------------------------------------------------------------------
// Name: onTypedPrintableCharacter
// Desc: Invoked when the user enters a valid character that should be added to the internal buffer
//-----------------------------------------------------------------------------
void GUIEditableTextLine::onTypedPrintableCharacter(char printableCharacter)
{
  char textToInsert[2] = { printableCharacter, '\0' };
  const std::string::size_type length = myText.length();
  if (myCaretLocation > length) myCaretLocation = length;
  myText.insert(myCaretLocation, textToInsert);
  myCaretLocation++;
}



//-----------------------------------------------------------------------------
// Name: onBackspace
// Desc: Called when the user enters a backspace character
//-----------------------------------------------------------------------------
void GUIEditableTextLine::onBackspace()
{
  if (myCaretLocation > 0) { myText.erase(myCaretLocation - 1, 1); --myCaretLocation; }
}



//-----------------------------------------------------------------------------
// Name: onDelete
// Desc: Called when the user presses 'delete'
//-----------------------------------------------------------------------------
void GUIEditableTextLine::onDelete()
{
  if (myCaretLocation < myText.length()) myText.erase(myCaretLocation, 1);
}




//-----------------------------------------------------------------------------
// Name: KeyboardSentinel
// Desc: Initializes this sentinel
//-----------------------------------------------------------------------------
GUIEditableTextLine::KeyboardSentinel::KeyboardSentinel(GUIEditableTextLine* textLine)
{
  myOwnerTextLine = textLine;
}



//-----------------------------------------------------------------------------
// Name: onLostKeyboardFocus
// Desc: Called by the keyboard input source when this sentinel loses mouse focus.  This
//     method makes sure the text line's mouse focus is revoked as well.
//-----------------------------------------------------------------------------
void GUIEditableTextLine::KeyboardSentinel::onLostKeyboardFocus()
{
  if (myOwnerTextLine->myMouseSentinel.hasMouseFocus())
  {
    myOwnerTextLine->onLostFocus();
    myOwnerTextLine->myMouseSentinel.releaseMouseFocus();
  }
}


//-----------------------------------------------------------------------------
// Name: onGotKeyboardFocus
// Desc: Called by the keyboard input source when this sentinel obtains mouse focus.  This
//     method makes sure the text line's mouse focus is obtained as well.
//-----------------------------------------------------------------------------
void GUIEditableTextLine::KeyboardSentinel::onGotKeyboardFocus()
{
  if (!myOwnerTextLine->myMouseSentinel.hasMouseFocus())
  {
    myOwnerTextLine->onGotFocus();
    myOwnerTextLine->myMouseSentinel.obtainMouseFocus();
  }
}



//-----------------------------------------------------------------------------
// Name: onKeyUp
// Desc: When a key is released, this method is invoked by the keyboard input source.  This method
//     watches for the 'delete' and arrow keys
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIEditableTextLine::KeyboardSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard)
{
  if (myOwnerTextLine->hasFocus())
  {
    // Activate children first; then, if nothing happened, do normal processing
    if (!GUIKeyboardSentinel::onKeyDown(key, keyboard))
    {
      switch(key)
      {
        case GUIKEY_ESCAPE: myOwnerTextLine->releaseFocus(); break;
        case GUIKEY_DELETE: myOwnerTextLine->onDelete(); break;
        case GUIKEY_UP:   myOwnerTextLine->myCaretLocation = 0; break;
        case GUIKEY_LEFT:   if (myOwnerTextLine->myCaretLocation > 0) myOwnerTextLine->myCaretLocation--; break;
        case GUIKEY_RIGHT:  if (myOwnerTextLine->myCaretLocation < myOwnerTextLine->myText.length()) myOwnerTextLine->myCaretLocation++; break;
        case GUIKEY_DOWN:   myOwnerTextLine->myCaretLocation = myOwnerTextLine->myText.length(); break;
      }
    }

    // Don't propogate this event if the text line has focus
    return true;
  }
  else
    return GUIKeyboardSentinel::onKeyDown(key, keyboard);
}



//-----------------------------------------------------------------------------
// Name: onTypedCharacter
// Desc: Called when the user types a key associated with a printable character.  This method
//     does some initial processing on the character to make sure that it is valid.
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIEditableTextLine::KeyboardSentinel::onTypedCharacter(char character, GUIKeyboardInputSource* keyboard)
{
  if (myOwnerTextLine->hasFocus())
  {
    if (character < 32 || character > 128)
    {
      if (character == '\b')
        myOwnerTextLine->onBackspace();
    }
    else
      myOwnerTextLine->onTypedPrintableCharacter(character);

    // Don't propogate this character if the text line has focus
    return true;
  }
  else
    return GUIKeyboardSentinel::onTypedCharacter(character, keyboard);
}



//-----------------------------------------------------------------------------
// Name: MouseSentinel
// Desc: Initializes this class
//-----------------------------------------------------------------------------
GUIEditableTextLine::MouseSentinel::MouseSentinel(GUIEditableTextLine* textLine)
{
  myOwnerTextLine = textLine;
}



//-----------------------------------------------------------------------------
// Name: onLostMouseFocus
// Desc: Called by the mouse input source when this sentinel loses mouse focus.  This
//     method makes sure the text line's keyboard focus is revoked as well.
//-----------------------------------------------------------------------------
void GUIEditableTextLine::MouseSentinel::onLostMouseFocus()
{
  if (myOwnerTextLine->myKeyboardSentinel.hasKeyboardFocus())
  {
    myOwnerTextLine->onLostFocus();
    myOwnerTextLine->myKeyboardSentinel.releaseKeyboardFocus();
  }
}



//-----------------------------------------------------------------------------
// Name: onMouseButtonAction
// Desc: Called by the mouse input source when a mouse button alters its state
//     @return 'true' To indicate the event should stop propogating
//-----------------------------------------------------------------------------
bool GUIEditableTextLine::MouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)
{
  // Trap if this point is inside
  if (myOwnerTextLine->getCanvas()->pointInside(position))
  {
    // Make sure this line has focus
    myOwnerTextLine->obtainFocus();

    // Notify of the message
    myOwnerTextLine->onMouseButtonActionInside(action, position, mouse);

    // Trap the call
    return true;
  }
  else
  {
    // Free the text line's focus, if it has it
    myOwnerTextLine->releaseFocus();

    // Don't trap this event
    return GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
  }
}



//-----------------------------------------------------------------------------
// Name: TextElement
// Desc: Initializes the class
//-----------------------------------------------------------------------------
GUIEditableTextLine::TextElement::TextElement(GUIEditableTextLine* ownerTextLine)
{
  myTextLine = ownerTextLine;
  myFont = 0;
  myTextIsPassword = false;
  myTextColor = D3DCOLOR_XRGB(255,255,255);
}



//-----------------------------------------------------------------------------
// Name: setFont
// Desc: Changes the font used to render this text
//-----------------------------------------------------------------------------
void GUIEditableTextLine::TextElement::setFont(GUIFont* font)
{
  myFont = font;
}



//-----------------------------------------------------------------------------
// Name: setTextColor
// Desc: Changes this text's color
//-----------------------------------------------------------------------------
void GUIEditableTextLine::TextElement::setTextColor(D3DCOLOR color)
{
  myTextColor = color;
}



//-----------------------------------------------------------------------------
// Name: setPasswordFlag
// Desc: Sets the flag that determines whether or not this text is a password
//-----------------------------------------------------------------------------
void GUIEditableTextLine::TextElement::setPasswordFlag(bool isPassword)
{
  myTextIsPassword = isPassword;
}



//-----------------------------------------------------------------------------
// Name: getPasswordFlag
// Desc: Determines whether or not this is a password line
//-----------------------------------------------------------------------------
bool GUIEditableTextLine::TextElement::getPasswordFlag() const
{
  return myTextIsPassword;
}



//-----------------------------------------------------------------------------
// Name: render
// Desc: Renders this text element.  This method will call GUITextElement::render
//     first, then render the caret for this element (if it is in focus)
//-----------------------------------------------------------------------------
bool GUIEditableTextLine::TextElement::render(GUILayer* layer)
{
  // Exit if the text line couldn't render 
  if (!GUITextElement::render(layer)) return false;

  // TODO: clean up this caret-rendering code
  if (myTextLine->hasFocus())
  {
    GUISize size;
    size.zero();
    GUIPoint point;
    if (myTextLine->myCaretLocation)
    {
      if (myTextIsPassword)
      {
        myFont->getTextExtent("*", 1, &size);
        size.width *= myTextLine->myCaretLocation;
      }
      else
        myFont->getTextExtent(myTextLine->myText.c_str(), myTextLine->myCaretLocation - 1, &size);
    }
    GUISize caretSize;
    myFont->getTextExtent("|", 1, &caretSize);
    myFont->centerTextInArea("|", 1, mySourceCanvas->getArea(), &point);
    float caretX =
      size.width + mySourceCanvas->getArea()->left + myTextLine->myHorizontalTextOffset;
    if (caretX > mySourceCanvas->getArea()->right)
      myTextLine->myHorizontalTextOffset = -(size.width + caretSize.width - mySourceCanvas->getArea()->right + mySourceCanvas->getArea()->left);
    caretX -= caretSize.width/2.0f;

    if (caretX < mySourceCanvas->getArea()->left)
    {
      myTextLine->myHorizontalTextOffset += 20.0f;
      if (myTextLine->myHorizontalTextOffset > 0.0f)
        myTextLine->myHorizontalTextOffset = 0.0f;
    }
    myFont->renderText(layer, 0,
      caretX, point.y,
      mySourceCanvas->getDepth(), "|", 1, myTextColor);

  }
    

  // Success
  return true;
}



//-----------------------------------------------------------------------------
// Name: getFont
// Desc: Gets the font to use to render the this label
//-----------------------------------------------------------------------------
GUIFont* GUIEditableTextLine::TextElement::getFont()
{
  return myFont;
}



//-----------------------------------------------------------------------------
// Name: getNextTextSegment
// Desc: Returns the text for this label
//-----------------------------------------------------------------------------
bool GUIEditableTextLine::TextElement::getNextTextSegment(TextSegmentInfo* nextSegmentInfo)
{
  if (nextSegmentInfo)
  {
    // Only render one line
    if (nextSegmentInfo->text == myTextLine->myText.c_str()) return false;

    nextSegmentInfo->text = myTextLine->myText.c_str();
    nextSegmentInfo->lineLength = -1;

    if (myFont && mySourceCanvas)
    {
      GUIPoint point;
      myFont->centerTextInArea(nextSegmentInfo->text, -1, mySourceCanvas->getArea(), &point);
      nextSegmentInfo->x = mySourceCanvas->getArea()->left + myTextLine->myHorizontalTextOffset;
      nextSegmentInfo->y = point.y;
    }

    nextSegmentInfo->obscuringCharacter = myTextIsPassword ? '*' : 0;
    nextSegmentInfo->linesToIncrementAfterward = 0;
    nextSegmentInfo->color = myTextColor;

    // Render this segment
    return true;
  }
  else
    return true;
}

