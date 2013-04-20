#include "stdafx.h"
#include "evidyonguiglobalchatdisplay.h"




//-----------------------------------------------------------------------------
EvidyonGUIGlobalChatDisplay::GlobalChatLine::GlobalChatLine()
    : text_element_(this) {
  zero();
}


//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::GlobalChatLine::initialize(GUIFont* font) {
  text_element_.setFont(font);
  text_element_.setTextColor(0);
  text_element_.setAlignment(GUIALIGN_LEFT);
}


//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::GlobalChatLine::set(D3DCOLOR color, const std::string& text) {
  text_element_.setTextColor(color);
  text_element_.setText(text);
}



//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::GlobalChatLine::registerElements(GUILayer* layer) {
  text_element_.registerElements(layer);
}


//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::GlobalChatLine::unregisterElements(GUILayer* layer) {
  text_element_.unregisterElements(layer);
}


//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::GlobalChatLine::refresh() {
  text_element_.setClippingArea(getClippingArea());
}



//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::GlobalChatLine::zero() {
  text_element_.getCanvas()->alignXLeft(+10);
  text_element_.getCanvas()->alignYCenter(0);
}



//-----------------------------------------------------------------------------
EvidyonGUIGlobalChatDisplay::EvidyonGUIGlobalChatDisplay() {
  zero();
}



//-----------------------------------------------------------------------------
bool EvidyonGUIGlobalChatDisplay::create(GUIFont* text_font,
                                GUICanvas* screen_canvas) {
  destroy();

  background_canvas_.setSourceCanvas(screen_canvas);
  background_canvas_.relativeWidth(0);
  background_canvas_.absoluteHeight(220);
  background_canvas_.absoluteDepth(1.0f);
  background_canvas_.alignXLeft(0);
  background_canvas_.alignYTop(10);

  first_line_index_ = 0;
  num_lines_used_ = 0;

  setLineHeight((int)(text_font->getLineHeightInPixels() * 1.0f));

  for (int i = 0; i < MAXIMUM_LINES; ++i) {
    lines_[i].initialize(text_font);
  }

  myTargetCanvas.setSourceCanvas(&background_canvas_);
  myTargetCanvas.relativeDepth(-0.001f);
  myTargetCanvas.relativeHeight(0);
  myTargetCanvas.relativeWidth(0);
  myTargetCanvas.alignXLeft(0);
  myTargetCanvas.alignYTop(0);

  refresh();

  return true;
}



//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::destroy() {
  hide();
  zero();
}



//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::show(GUILayer* layer) {
  if (layer_ != NULL) { // erase elements from the old layer
    unregisterElements(layer_);
  }
  if (layer != NULL) { // add to the new layer, if it exists
    registerElements(layer);
  }
  layer_ = layer;
}




//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::addSystemText(const std::string& system_text) {
  addLine(system_text, system_text.length()/2, D3DCOLOR_XRGB(255,255,255));
}



//----[  addGlobalTextLine  ]--------------------------------------------------
void EvidyonGUIGlobalChatDisplay::addGlobalTextLine(bool evil, const std::string& speaker_name, const std::string& text) {
  std::string chat_line = speaker_name;
  chat_line.append(" [global]:  ");
  chat_line.append(text);
  addLine(chat_line,
          speaker_name.length() + 1,
          evil ? D3DCOLOR_XRGB(255,124,124) : D3DCOLOR_XRGB(240,232,70));
}

//----[  addGuildTextLine  ]--------------------------------------------------
void EvidyonGUIGlobalChatDisplay::addGuildTextLine(bool evil, const std::string& speaker_name, const std::string& text) {
  std::string chat_line = speaker_name;
  chat_line.append(" [guild]:  ");
  chat_line.append(text);
  addLine(chat_line,
          speaker_name.length() + 1,
          evil ? D3DCOLOR_XRGB(255,124,124) : D3DCOLOR_XRGB(255,253,204));
}

//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::addPartyTextLine(bool evil, const std::string& speaker_name, const std::string& text) {
  std::string chat_line = speaker_name;
  chat_line.append(" [party]:  ");
  chat_line.append(text);
  addLine(chat_line, speaker_name.length() + 1,
          evil ?D3DCOLOR_XRGB(255,124,124) : D3DCOLOR_XRGB(255,253,204));
}

//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::update(double time) {
  bool need_to_refresh = false;
  int old_first_line_index = first_line_index_;
  int old_num_lines_used = num_lines_used_;
  for (int i=0; i<old_num_lines_used; ++i) {
    int index = (old_first_line_index + i)%MAXIMUM_LINES;
    if (line_expiration_times_[index] < time) {
      ASSERT(index == first_line_index_);
      if (layer_) lines_[index].unregisterElements(layer_);
      first_line_index_ = (first_line_index_ + 1) % MAXIMUM_LINES;
      num_lines_used_--;
      need_to_refresh = true;
    }
  }
  last_update_time_ = time;
  if (need_to_refresh) {
    refresh();
  }
}



//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::addLine(const std::string& text_line, int split_character_index, D3DCOLOR color) {
  GUISize text_size;
  double chat_expiration_time = DISPLAY_DURATION_MILLIS / 1000.0 + last_update_time_;
  GUIFont* font = lines_->getFont();
  if (!font || !font->getTextExtent(text_line.c_str(), -1, &text_size)) return;
  GUISize target_size = myTargetCanvas.getArea()->calculateSize();
  if ((text_size.width) < target_size.width) { // add the whole text line
    ensureSpaceForNewLines(1);
    int next_available_line = getNextAvailableLine();
    lines_[next_available_line].set(color, text_line);
    line_expiration_times_[next_available_line] = chat_expiration_time;
    if (layer_) lines_[next_available_line].registerElements(layer_); // make sure the element is visible
    ++num_lines_used_;
  } else { // add the text line and character name on separate lines
    ensureSpaceForNewLines(2);

    int next_available_line = getNextAvailableLine();
    lines_[next_available_line].set(color, text_line.substr(0,split_character_index)); // add the first text section
    if (layer_) lines_[next_available_line].registerElements(layer_);
    line_expiration_times_[next_available_line] = chat_expiration_time;
    ++num_lines_used_;

    next_available_line = getNextAvailableLine();
    lines_[next_available_line].set(color, text_line.substr(split_character_index)); // add the second text section
    if (layer_) lines_[next_available_line].registerElements(layer_);
    line_expiration_times_[next_available_line] = chat_expiration_time;
    ++num_lines_used_;
  }

  scrollToPercent(1.0f); // scroll to the end of the list; will call refresh()
}



//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::ensureSpaceForNewLines(int number) {
  ASSERT(number < MAXIMUM_LINES) else return;
  int overflow = (num_lines_used_ + number) - MAXIMUM_LINES;
  if (overflow > 0) { // too many lines?
    if (layer_) { // erase, if visible
      for (int i = 0; i < overflow; ++i) {
        int index = (first_line_index_ + i) % MAXIMUM_LINES;
        lines_[index].unregisterElements(layer_);
      }
    }
    first_line_index_ = (first_line_index_ + overflow) % MAXIMUM_LINES; // advance the first line
    num_lines_used_ -= overflow;
  }
}



//-----------------------------------------------------------------------------
unsigned int EvidyonGUIGlobalChatDisplay::getNumberOfLines() const {
  return num_lines_used_;
}



//-----------------------------------------------------------------------------
GUIList::ListEntry* EvidyonGUIGlobalChatDisplay::getListEntry(unsigned int index) {
  return (index < num_lines_used_) ? &lines_[(first_line_index_ + index)%MAXIMUM_LINES] : NULL;
}


//-----------------------------------------------------------------------------
void EvidyonGUIGlobalChatDisplay::zero() {
  layer_ = NULL;
  ZeroMemory(line_expiration_times_,sizeof(line_expiration_times_));
  first_line_index_ = 0;
  num_lines_used_ = 0;
  last_update_time_ = 0.0;
}