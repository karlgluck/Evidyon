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
#include "guitexture.h"
#include "guikeyboard.h"
#include "guimouse.h"
#include "guicomponent.h"
#include "guimodule.h"




//-----------------------------------------------------------------------------
GUIModule::ModuleKeyboardSentinel::ModuleKeyboardSentinel(GUIModule* owner) {
  owner_ = owner;
}

//-----------------------------------------------------------------------------
void GUIModule::ModuleKeyboardSentinel::onGotKeyboardFocus() {
  if (!owner_->isActive()) return;
  owner_->onGotKeyboardFocus();
}

//-----------------------------------------------------------------------------
void GUIModule::ModuleKeyboardSentinel::onLostKeyboardFocus() {
  if (!owner_->isActive()) return;
  owner_->onLostKeyboardFocus();
}

//-----------------------------------------------------------------------------
bool GUIModule::ModuleKeyboardSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!owner_->isActive()) return false;
  if (GUIKeyboardSentinel::onKeyDown(key, keyboard)) return true;
  else return owner_->onKeyDown(key, keyboard);
  //if (owner_->onKeyDown(key, keyboard)) return true;
  //return GUIKeyboardSentinel::onKeyDown(key, keyboard);
}

//-----------------------------------------------------------------------------
bool GUIModule::ModuleKeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!owner_->isActive()) return false;
  if (GUIKeyboardSentinel::onKeyUp(key, keyboard)) return true;
  else return owner_->onKeyUp(key, keyboard);
  //if (owner_->onKeyUp(key, keyboard)) return true;
  //return GUIKeyboardSentinel::onKeyUp(key, keyboard);
}

//-----------------------------------------------------------------------------
bool GUIModule::ModuleKeyboardSentinel::onTypedCharacter(char character, GUIKeyboardInputSource* keyboard) {
  if (!owner_->isActive()) return false;
  if (GUIKeyboardSentinel::onTypedCharacter(character, keyboard)) return true;
  else return owner_->onTypedCharacter(character, keyboard);
  //if (owner_->onTypedCharacter(character, keyboard)) return true;
  //return GUIKeyboardSentinel::onTypedCharacter(character, keyboard);
}








//----[  ModuleMouseSentinel  ]------------------------------------------------
GUIModule::ModuleMouseSentinel::ModuleMouseSentinel(GUIModule* owner) {
  owner_ = owner;
}




//----[  onGotMouseFocus  ]----------------------------------------------------
void GUIModule::ModuleMouseSentinel::onGotMouseFocus() {
  if (!owner_->isActive()) return;
  owner_->onGotMouseFocus();
}




//----[  onLostMouseFocus  ]---------------------------------------------------
void GUIModule::ModuleMouseSentinel::onLostMouseFocus() {
  if (!owner_->isActive()) return;
  owner_->onLostMouseFocus();
}


bool GUIModule::ModuleMouseSentinel::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse) {
  if (!owner_->isActive()) return false;
  if (GUIMouseSentinel::onMouseMove(position, oldPosition, mouse)) return true;
  else return owner_->onMouseMove(position, oldPosition, mouse);
  //if (owner_->onMouseMove(position, oldPosition, mouse)) return true;
  //else return GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
}


bool GUIModule::ModuleMouseSentinel::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse) {
  if (!owner_->isActive()) return false;
  if (GUIMouseSentinel::onMouseButtonAction(action, position, mouse)) return true;
  else return owner_->onMouseButtonAction(action, position, mouse);
  //if (owner_->onMouseButtonAction(action, position, mouse)) return true;
  //else return GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
}


bool GUIModule::ModuleMouseSentinel::onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse) {
  if (!owner_->isActive()) return false;
  if (GUIMouseSentinel::onMouseScroll(position, amount, lines, mouse)) return true;
  else return owner_->onMouseScroll(position, amount, lines, mouse);
  //if (owner_->onMouseScroll(position, amount, lines, mouse)) return true;
  //else return GUIMouseSentinel::onMouseScroll(position, amount, lines, mouse);
}





//-----------------------------------------------------------------------------
GUIModule::GUIModule() : module_mouse_sentinel_(this), module_keyboard_sentinel_(this) {
  active_layer_ = NULL;
  module_mouse_sentinel_.disable();
  module_keyboard_sentinel_.disable();
  components_.setParentSentinels(&module_mouse_sentinel_,
                                 &module_keyboard_sentinel_);
}

//-----------------------------------------------------------------------------
void GUIModule::show(GUILayer* layer) {

  // update all submodules
  for (Submodules::iterator i = submodules_.begin(); i != submodules_.end(); ++i) {
    GUIModule* submodule = (*i);
    submodule->show(layer);
  }

  if (layer == NULL) {
    // the layer is invalid, so this module is being hidden
    components_.unregisterElements();
    module_mouse_sentinel_.disable();
    module_keyboard_sentinel_.disable();
  } else {
    // the layer is valid, so this module is active
    components_.registerElements(layer);
    module_mouse_sentinel_.enable();
    module_keyboard_sentinel_.enable();
  }

  active_layer_ = layer;
}


//-----------------------------------------------------------------------------
GUILayer* GUIModule::hide() {
  GUILayer* previous_layer = active_layer_;
  show(NULL);
  return previous_layer;
}


//-----------------------------------------------------------------------------
bool GUIModule::isActive() const {
  return active_layer_ != NULL;
}


//-----------------------------------------------------------------------------
void GUIModule::obtainFocus() {
  module_keyboard_sentinel_.obtainKeyboardFocus();
  module_mouse_sentinel_.obtainMouseFocus();
}

//-----------------------------------------------------------------------------
void GUIModule::releaseFocus() {
  module_keyboard_sentinel_.releaseKeyboardFocus();
  module_mouse_sentinel_.releaseMouseFocus();
}

//-----------------------------------------------------------------------------
bool GUIModule::hasFocus() const {
  return module_keyboard_sentinel_.hasKeyboardFocus() && module_mouse_sentinel_.hasMouseFocus();
}

//-----------------------------------------------------------------------------
void GUIModule::setParentSentinels(GUIMouseSentinel* mouse, GUIKeyboardSentinel* keyboard) {
  module_mouse_sentinel_.setParentMouseSentinel(mouse);
  module_keyboard_sentinel_.setParentKeyboardSentinel(keyboard);
}

//-----------------------------------------------------------------------------
void GUIModule::removeFromInputChain() {
  module_mouse_sentinel_.removeFromMouseSentinelChain();
  module_keyboard_sentinel_.removeFromKeyboardSentinelChain();
}

//-----------------------------------------------------------------------------
void GUIModule::addSubmodule(GUIModule* module) {
  module->setParentSentinels(&module_mouse_sentinel_,
                             &module_keyboard_sentinel_);
  module->show(active_layer_);
  submodules_.insert(module);
}

//-----------------------------------------------------------------------------
void GUIModule::removeSubmodule(GUIModule* module) {
  module->hide();
  module->removeFromInputChain();
  submodules_.erase(module);
}

//-----------------------------------------------------------------------------
void GUIModule::clearSubmodules() {
  for (Submodules::iterator i = submodules_.begin(); i != submodules_.end(); ++i) {
    GUIModule* submodule = (*i);
    submodule->hide();
    submodule->removeFromInputChain();
  }
  submodules_.clear();
}

//-----------------------------------------------------------------------------
void GUIModule::addComponent(GUIComponent* component) {
  components_.add(component);
}


//----[  registerComponentIfActive  ]------------------------------------------
void GUIModule::registerComponentIfActive(GUIComponent* component) {
  if (isActive()) component->registerElements(active_layer_);
}

//-----------------------------------------------------------------------------
void GUIModule::removeComponent(GUIComponent* component) {
  components_.remove(component);
}

//-----------------------------------------------------------------------------
void GUIModule::clearComponents() {
  components_.clear();
}


// Default implementations of the input routines
void GUIModule::onGotMouseFocus() { }
void GUIModule::onLostMouseFocus() { }
bool GUIModule::onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse)  { return false; }
bool GUIModule::onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse)  { return false; }
bool GUIModule::onMouseScroll(GUIPoint position, float amount, int lines, GUIMouseInputSource* mouse)  { return false; }
void GUIModule::onGotKeyboardFocus() { }
void GUIModule::onLostKeyboardFocus() { }
bool GUIModule::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard)  { return false; }
bool GUIModule::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard)  { return false; }
bool GUIModule::onTypedCharacter(char character, GUIKeyboardInputSource* keyboard)  { return false; }
