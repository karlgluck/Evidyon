//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#ifndef __EVIDYONQUESTTOOL_H__
#define __EVIDYONQUESTTOOL_H__
#pragma once


#include "../shared/evidyontool/evidyontool.h"
#include "(quest).h"
#include "../shared/evidyontool/directcraftactions.h"
#include "../shared/evidyontool/evidyontooldialog.h"
#include "resource.h"


//----[  QuestTool  ]----------------------------------------------------------
class QuestTool : public Evidyon::Tools::EvidyonTool,
                  public dc::dcResource<QuestTool> {
public:
  QuestTool();
  int run(HINSTANCE instance);
  dc::dcGenericResource* getItems() { return &items_; }
  dc::dcGenericCollection* getQuests() { return &quests_; }
  dc::dcGenericCollection* getAtlases() { return &atlases_; }
  dc::dcGenericCollection* getLifeforms() { return &lifeform_ai_; }
  std::string translateQuestMemberToRealPath(const std::string& path);

public:
  static std::string staticTypeName() { return "QuestTool"; }

private:
  // Invoked by the topmost tool dialog's menu when the user picks something
  virtual void dialogMenuNotification(Evidyon::Tools::EvidyonToolDialog* dialog,
                                      WORD item_id,
                                      WORD item_index);
  virtual bool toolReadFrom(const char* file);
  virtual bool toolWriteTo(const char* file);
  virtual void toolReset();

private:
  dc::dcList<Quest> quests_;
  dc::dcList<Item> items_;
  dc::dcList<Atlas> atlases_;
  dc::dcTable<LifeformAI> lifeform_ai_;
};


#endif