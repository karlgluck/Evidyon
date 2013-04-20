//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#include "evidyonquesttool.h"
#include <dc/dcresourcestorage2.h>





//----[  dialogMenuNotification  ]---------------------------------------------
void QuestTool::dialogMenuNotification(Evidyon::Tools::EvidyonToolDialog* dialog,
                                       WORD item_id,
                                       WORD item_index) {
  switch (item_id) {
    case ID_NEW: newProject(); break;
    case ID_OPEN: openProject(); break;
    case ID_SAVEAS: saveProjectAs(); break;
    case ID_SAVE: saveProject(); break;
    case ID_EXIT: dialog->end(0); break;
  }
}


//----[  QuestTool  ]----------------------------------------------------------
QuestTool::QuestTool() {
  member("Quests", &quests_);
  member("Items", &items_);
  member("World Atlases", &atlases_);
  member("Lifeform AI", &lifeform_ai_);
}



//----[  run  ]----------------------------------------------------------------
int QuestTool::run(HINSTANCE instance) {
  return executeRootDialog(this, IDR_MENU) ? S_OK : E_FAIL;
}



//----[  translateQuestMemberToRealPath  ]-------------------------------------
std::string QuestTool::translateQuestMemberToRealPath(const std::string& path) {
  return path.substr(getName().length() + 1);
}



//----[  toolReadFrom  ]-------------------------------------------------------
bool QuestTool::toolReadFrom(const char* file) {
  dc::dcResourceStorage2 storage;
  bool succeeded = false;
  if (storage.openFile(file, "pv")) {
    succeeded = storage.load("/Volucris", &quests_, true);
    succeeded = storage.load("/Volucris", &items_, false);
    succeeded = storage.load("/Volucris", &atlases_, false);
    succeeded = storage.load("/Volucris", &lifeform_ai_, false);
    storage.close();
  }
  return succeeded;
}




//----[  toolWriteTo  ]--------------------------------------------------------
bool QuestTool::toolWriteTo(const char* file) {
  dc::dcResourceStorage2 storage;
  bool succeeded = false;
  if (storage.openFile(file, "pv")) {
    succeeded = storage.save("/Volucris", &quests_);
    storage.close();
  }
  return succeeded;
}




//----[  toolReset  ]----------------------------------------------------------
void QuestTool::toolReset() {
  quests_.clear();
  items_.clear();
  atlases_.clear();
  lifeform_ai_.clear();
}
