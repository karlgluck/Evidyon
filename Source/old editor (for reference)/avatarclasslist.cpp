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
#include "avatarclasslist.h"
#include "../shared/server-editor/server_gamefile.h"
#include "../shared/client-editor/client_gamefile.h"

AvatarClassList::AvatarClassList() {
  for (int i = 0; i < (int)Avatar::NUMBER_OF_AVATAR_CLASSES; ++i) {
    member(Evidyon::AvatarClassString((Evidyon::AvatarClass)i),
          &classes_[i]);
  }
}

bool AvatarClassList::compileForServer(dc::dcStreamOut* stream) const {
  { // todo: not needed because classes are specified
    size_t classes = Avatar::NUMBER_OF_AVATAR_CLASSES;
    stream->write(&classes, sizeof(classes));
  }
  {
    for (int i = 0; i < (int)Avatar::NUMBER_OF_AVATAR_CLASSES; ++i) {
      Evidyon::AvatarClassDescription description;
      classes_[i].fillServerDescription(&description);
      CONFIRM(stream->write(&description,
                            sizeof(description))) else return false;

    }
  }

  return true;
}



bool AvatarClassList::compileForClient(dc::dcStreamOut* stream) const {
  { // todo: not needed because classes are specified
    size_t classes = Avatar::NUMBER_OF_AVATAR_CLASSES;
    stream->write(&classes, sizeof(classes));
  }
  {
    for (int i = 0; i < (int)Avatar::NUMBER_OF_AVATAR_CLASSES; ++i) {
    char name[CLIENTGAMEFILE_CLASS_STRING_LENGTH];
    strcpy_s(name,
             CLIENTGAMEFILE_CLASS_STRING_LENGTH,
             classes_[i].getName().c_str());
      CONFIRM(stream->write(name,
                            CLIENTGAMEFILE_CLASS_STRING_LENGTH * sizeof(char))) else return false;
      char desc[CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH];
      strcpy_s(desc,
               CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH,
               classes_[i].getDescription().c_str());
      CONFIRM(stream->write(desc,
                            CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH * sizeof(char))) else return false;
    }
  }

  return true;
}

std::string AvatarClassList::staticTypeName() {
  return "AvatarClassList";
}