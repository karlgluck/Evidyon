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
#include "evidyonguimodule_quantityprompt.h"




namespace Evidyon {


void EvidyonGUIModule_QuantityPrompt::create(
    GUICanvas* source_canvas,
    GUIFont* prompt_font) {

  addComponent(&prompt_text_);
  addComponent(&ok_);
  addComponent(&cancel_);
  addComponent(&panel_);
  addComponent(&input_);

  panel_.setFont(prompt_font);
  panel_.setText(""); // unused for now
  {
    GUICanvas* canvas = panel_.getCanvas();
    canvas->setSourceCanvas(source_canvas);
    canvas->alignXCenter(0);
    canvas->alignYCenter(0);
    canvas->absoluteWidth(400);
    canvas->absoluteHeight(300);
    canvas->absoluteDepth(0.2f);
  }

  prompt_text_.textLabel()->setFont(prompt_font);
  {
    GUICanvas* canvas = prompt_text_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->alignXLeft(5);
    canvas->alignYTop(5);
    canvas->relativeWidth(-10);
    canvas->absoluteHeight(50);
    canvas->relativeDepth(-0.01f);
  }

  
  input_.configure(prompt_font,
    false, // not a password
    true,  // restrict to numbers
    0xFFFFFFFF); // color text white
  {
    GUICanvas* canvas = input_.getCanvas();
    canvas->setSourceCanvas(prompt_text_.getCanvas());
    canvas->alignXCenter(0);
    canvas->alignYBottomOutside(5);
    canvas->relativeWidth(-10);
    canvas->absoluteHeight(35);
    canvas->relativeDepth(0.0f);
  }
  
  ok_.setFont(prompt_font);
  ok_.setText("Create");
  ok_.enable();
  {
    GUICanvas* canvas = ok_.getCanvas();
    canvas->setSourceCanvas(cancel_.getCanvas());
    canvas->alignXCenter(0);
    canvas->alignYTopOutside(-10);
    canvas->relativeWidth(0);
    canvas->relativeHeight(0);
    canvas->relativeDepth(0.0f);
  }

  cancel_.setFont(prompt_font);
  cancel_.setText("Cancel");
  cancel_.enable();
  {
    GUICanvas* canvas = cancel_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->alignXCenter(0);
    canvas->relativeWidth(-30);
    canvas->absoluteHeight(35);
    canvas->alignYBottom(-10);
    canvas->relativeDepth(-0.1f);
  }
}




//----[  destroy  ]------------------------------------------------------------
void EvidyonGUIModule_QuantityPrompt::destroy() {
  hide();
  clearComponents();
}




//----[  initialize  ]---------------------------------------------------------
void EvidyonGUIModule_QuantityPrompt::initialize(const char* prompt_text,
                const char* ok_text,
                const char* cancel_text) {
  input_.setText("");
  prompt_text_.textLabel()->setText(prompt_text);
  cancel_.setText(cancel_text);
  ok_.setText(ok_text);
  obtainFocus();
}




//----[  setOKText  ]----------------------------------------------------------
void EvidyonGUIModule_QuantityPrompt::setOKText(const char* ok_text) {
  ok_.setText(ok_text);
}



//----[  enableOK  ]-----------------------------------------------------------
void EvidyonGUIModule_QuantityPrompt::enableOK(bool enable) {
  if (enable) {
    ok_.enable();
  } else {
    ok_.disable();
  }
}



//----[  pollOK  ]-------------------------------------------------------------
bool EvidyonGUIModule_QuantityPrompt::pollOK() { return ok_.pollPushed(); }



//----[  pollCancel  ]---------------------------------------------------------
bool EvidyonGUIModule_QuantityPrompt::pollCancel() { return cancel_.pollPushed(); }



//----[  getValue  ]-----------------------------------------------------------
unsigned int EvidyonGUIModule_QuantityPrompt::getValue() {
  return atoi(input_.getText());
}



//----[  onMouseMove  ]--------------------------------------------------------
bool EvidyonGUIModule_QuantityPrompt::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  return true; // catch all messages
}



//----[  onMouseButtonAction  ]------------------------------------------------
bool EvidyonGUIModule_QuantityPrompt::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse) {
  return true;
}




//----[  onKeyDown  ]----------------------------------------------------------
bool EvidyonGUIModule_QuantityPrompt::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) return true; // we're going to catch it on the way up
  return false;
}



//----[  onKeyUp  ]------------------------------------------------------------
bool EvidyonGUIModule_QuantityPrompt::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) {
    cancel_.push();
    return true;
  } else {
    return false;
  }
}


}
