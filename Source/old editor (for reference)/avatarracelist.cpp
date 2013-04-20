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
#include "avatarracelist.h"
#include "../avatar/avatarrace.h"
#include "../avatar/avatarclass.h"


AvatarRaceList::AvatarRaceList() {
  for (int i = 0; i < (int)Avatar::NUMBER_OF_AVATAR_RACES; ++i) {
    member(Evidyon::AvatarRaceString((Evidyon::AvatarRace)i),
          &races_[i]);
  }
}

bool AvatarRaceList::compileForServer(dc::dcStreamOut* stream) const {
  { // todo: not needed because # of races is specified
    size_t classes = Avatar::NUMBER_OF_AVATAR_RACES;
    stream->write(&classes, sizeof(classes));
  }
  for (int i = 0; i < (int)Avatar::NUMBER_OF_AVATAR_RACES; ++i) {
    const PlayerCharacterRace* race = &races_[i];

    // Let the log know what we're doing
    DEBUG_INFO("Compiling race %s for server", race->getName().c_str());

    // Write each stat max
    for (int i = 0; i < 5; ++i) {
      unsigned char b;

      switch(i) {
        case 0: b = race->getMaxStrengthValue(); break;
        case 1: b = race->getMaxAgilityValue(); break;
        case 2: b = race->getMaxConstitutionValue(); break;
        case 3: b = race->getMaxIntelligenceValue(); break;
        case 4: b = race->getMaxWisdomValue(); break;
      }

      // Write this element
      if (!stream->write(&b, sizeof(unsigned char))) return false;
    }

    // The gender actor type templates
    for (int i = 0; i < (int)Avatar::NUMBER_OF_AVATAR_GENDERS; ++i) {
      int actor_type_template = race->getActorTypeTemplate((Evidyon::AvatarGender)i);
      if (!stream->write(&actor_type_template, sizeof(actor_type_template))) return false;
    }

    // Write the forms of hair
    size_t last_style = 0;
    for (int i = 0; i < (int)Avatar::NUMBER_OF_AVATAR_GENDERS; ++i) {
      for (int j = 0; j < (int)Evidyon::AVATAR_RACE_HAIR_STYLES_PER_GENDER; ++j) {
        size_t index = race->getHairStyle(i, j);
        if (index == (size_t)-1) index = last_style;
        stream->write(&index, sizeof(index));
        last_style = index;
      }
    }

  }

  return true;
}



bool AvatarRaceList::compileForClient(dc::dcStreamOut* stream) const {
  { // todo: not needed because races are specified
    size_t classes = Avatar::NUMBER_OF_AVATAR_RACES;
    stream->write(&classes, sizeof(classes));
  }
  {
    for (int i = 0; i < (int)Avatar::NUMBER_OF_AVATAR_RACES; ++i) {
      const PlayerCharacterRace* race = &races_[i];

      // Let the log know what we're doing
      DEBUG_INFO("Compiling race %s for client", race->getName().c_str());

      // Save the name of the race
      char race_name[CLIENTGAMEFILE_RACE_STRING_LENGTH];
      strcpy_s(race_name, CLIENTGAMEFILE_RACE_STRING_LENGTH, race->getName().c_str());
      stream->write(race_name, CLIENTGAMEFILE_RACE_STRING_LENGTH * sizeof(char));

      char race_desc[CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH];
      strcpy_s(race_desc, CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH, race->getDescription().c_str());
      stream->write(race_desc, CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH * sizeof(char));

      // Write each stat max
      for (int i = 0; i < 5; ++i)
      {
          unsigned char b;

          switch(i) 
          {
              case 0: b = race->getMaxStrengthValue(); break;
              case 1: b = race->getMaxAgilityValue(); break;
              case 2: b = race->getMaxConstitutionValue(); break;
              case 3: b = race->getMaxIntelligenceValue(); break;
              case 4: b = race->getMaxWisdomValue(); break;
          }

          // Write this element
          if (!stream->write(&b, sizeof(unsigned char))) return false;
      }
    }
  }

  return true;
}

std::string AvatarRaceList::staticTypeName() {
  return "AvatarRaceList";
}