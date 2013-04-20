#include "guitextlabelcomponent.h"
#include "guilayer.h"





//----[  setClippingArea  ]----------------------------------------------------
void GUITextLabelComponent::setClippingArea(const GUIRect* area) {
  element_.setClippingArea(area);
}


//----[  registerElements  ]---------------------------------------------------
void GUITextLabelComponent::registerElements(GUILayer* layer) {
  element_.setSourceCanvas(getCanvas());
  layer->registerElement(&element_);
}



//----[  unregisterElements  ]-------------------------------------------------
void GUITextLabelComponent::unregisterElements(GUILayer* layer) {
  layer->unregisterElement(&element_);
  element_.setSourceCanvas(NULL);
}
