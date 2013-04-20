#ifndef __GUICOMPONENTSET_H__
#define __GUICOMPONENTSET_H__
#pragma once


#include <set>
class GUIComponent;
class GUILayer;
class GUIMouseSentinel;
class GUIKeyboardSentinel;

// Helps manage registering components with a GUI layer and associating them
// with various input sources.
class GUIComponentSet {
public:
  typedef std::set<GUIComponent*> Components;

public:
  GUIComponentSet();
  ~GUIComponentSet();

  // Puts the component in the set.  Will register with the active layer (if
  // it is valid) and set the parent input sentinels.
  void add(GUIComponent* component);

  // Erases the component from this set.  Unregisters if the the current layer
  // is valid and clears parent input sentinels.
  void remove(GUIComponent* component);

  // Removes all components from this set, but leaves the active layer and
  // sentinels intact.
  void clearComponents();

  // Removes all components and nullifies references to the layer and input
  // sentinels.
  void clear();

  // Adds components in this set to the given layer.  If a layer already is
  // associated with this set, this method will remove elements from that
  // layer before adding them to the new one.
  void registerElements(GUILayer* layer);

  // Gets rid of all components' elements from the current layer, and removes
  // the internal reference to that layer.
  void unregisterElements();

  // Changes the parents of all components in the set.
  void setParentSentinels(GUIMouseSentinel* mouse,
                          GUIKeyboardSentinel* keyboard);

  // Erases all components' parent input sentinels
  void removeFromInputChain();

  // Calls registerElements(layer) and setParentSentinels(mouse, keyboard)
  void associate(GUILayer* layer,
                 GUIMouseSentinel* mouse,
                 GUIKeyboardSentinel* keyboard);

  // Calls unregisterElements and removeFromInputChain
  void disassociate();


private:
  GUILayer* layer_;
  GUIMouseSentinel* parent_mouse_sentinel_;
  GUIKeyboardSentinel* parent_keyboard_sentinel_;
  Components components_;
};

#endif