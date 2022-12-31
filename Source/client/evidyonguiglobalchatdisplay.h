//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#ifndef __EVIDYONGUIGLOBALCHATDISPLAY_H__
#define __EVIDYONGUIGLOBALCHATDISPLAY_H__
#pragma once


#include "evidyonguilist.h"



//----[  EvidyonGUIGlobalChatDisplay  ]----------------------------------------
class EvidyonGUIGlobalChatDisplay : public EvidyonGUIList {
  static const int MAXIMUM_LINES = 8;
  static const int DISPLAY_DURATION_MILLIS = 6000;

  class GlobalChatLine : public EvidyonGUIList::EvidyonListEntry {
  public:
    GlobalChatLine();

    void initialize(GUIFont* font);
    void set(D3DCOLOR color, const std::string& text);
    inline GUIFont* getFont() { return text_element_.getFont(); }

    virtual void registerElements(GUILayer* layer);
    virtual void unregisterElements(GUILayer* layer);
    virtual void refresh();

  private:
    void zero();

  private:
    EvidyonGUIListEntryText text_element_;
  };

public:
  EvidyonGUIGlobalChatDisplay();

  bool create(GUIFont* text_font,
              GUICanvas* screen_canvas);
  void destroy();

  void show(GUILayer* layer);
  inline void hide() { show(NULL); }
  inline bool visible() { return layer_ != NULL; }

  void addSystemText(const std::string& system_text);

  // these methods will split the text into (at most) 2 lines
  void addGlobalTextLine(bool evil, const std::string& speaker_name, const std::string& text);
  void addGuildTextLine(bool evil, const std::string& speaker_name, const std::string& text);
  void addPartyTextLine(bool evil, const std::string& speaker_name, const std::string& text);

  void update(double time);

protected:
  void addLine(const std::string& line_text, int line_split_character_index, D3DCOLOR color);
  void ensureSpaceForNewLines(int number); // make sure to call refresh() after this method
  virtual unsigned int getNumberOfLines() const;
  virtual ListEntry* getListEntry(unsigned int index);
  inline int getNextAvailableLine() { return (first_line_index_ + num_lines_used_)%MAXIMUM_LINES; }

private:
  void zero();

private:
  GUICanvas background_canvas_;
  GUILayer* layer_;
  GlobalChatLine lines_[MAXIMUM_LINES];
  double line_expiration_times_[MAXIMUM_LINES];
  int first_line_index_;
  int num_lines_used_;
  double last_update_time_;
};

#endif