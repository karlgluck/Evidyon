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
#include "guicomponentset.h"
#include "../null.h"


GUIComponentSet::GUIComponentSet() {
  layer_ = NULL;
  parent_mouse_sentinel_ = NULL;
  parent_keyboard_sentinel_ = NULL;
}


GUIComponentSet::~GUIComponentSet() {
  clear();
}


void GUIComponentSet::add(GUIComponent* component) {
  if (layer_ != NULL) component->registerElements(layer_);
  component->setParentSentinels(parent_mouse_sentinel_,
                parent_keyboard_sentinel_);
  components_.insert(component);
}


void GUIComponentSet::remove(GUIComponent* component) {
  Components::iterator i = components_.find(component);
  if (i == components_.end()) return;
  component->removeFromInputChain();
  if (layer_ != NULL) component->unregisterElements(layer_);
  components_.erase(i);
}


void GUIComponentSet::clearComponents() {
  GUILayer* layer = layer_;
  for (Components::iterator i = components_.begin();
     i != components_.end(); ++i) {
    GUIComponent* component = *i;
    component->removeFromInputChain();
    if (layer != NULL) component->unregisterElements(layer);
  }
  components_.clear();
}


void GUIComponentSet::clear() {
  disassociate();
  components_.clear();
}


void GUIComponentSet::registerElements(GUILayer* layer) {
  unregisterElements();
  layer_ = layer;
  for (Components::iterator i = components_.begin(); i != components_.end(); ++i) {
    GUIComponent* component = *i;
    component->registerElements(layer);
  }
}


void GUIComponentSet::unregisterElements() {
  if (layer_ == NULL) return;
  GUILayer* layer = layer_;
  layer_ = NULL;
  for (Components::iterator i = components_.begin(); i != components_.end(); ++i) {
    GUIComponent* component = *i;
    component->unregisterElements(layer);
  }
}


void GUIComponentSet::setParentSentinels(GUIMouseSentinel* mouse,
                                         GUIKeyboardSentinel* keyboard) {
  for (Components::iterator i = components_.begin(); i != components_.end(); ++i) {
    GUIComponent* component = *i;
    component->setParentSentinels(mouse, keyboard);
  }
  parent_mouse_sentinel_ = mouse;
  parent_keyboard_sentinel_ = keyboard;
}


void GUIComponentSet::removeFromInputChain() {
  if (parent_mouse_sentinel_ == NULL &&
    parent_keyboard_sentinel_ == NULL) return;
}


void GUIComponentSet::associate(GUILayer* layer,
                                GUIMouseSentinel* mouse,
                                GUIKeyboardSentinel* keyboard) {
  registerElements(layer);
  setParentSentinels(mouse, keyboard);
}


void GUIComponentSet::disassociate() {
  unregisterElements();
  removeFromInputChain();
}


