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
#ifndef __PLAYERCHARACTERRACE_H__
#define __PLAYERCHARACTERRACE_H__
#pragma once

namespace Evidyon {
struct ClientActorDescriptionTemplate;
}

class ActorDescriptionTemplateCompiler;

#include <dc/byte>
#include <dc/list>
#include "../shared/avatar/evidyon_avatar.h"
#include "actortype.h"
#include "scenery.h"


/**
 * Holds a character race (human, elf, gnome...) and its related attributes 
 */
class PlayerCharacterRace : public dc::dcResource<PlayerCharacterRace>
{
public:

  /**
   * Initializes this class
   */
  PlayerCharacterRace();

  /**
   * Gets the string representation of this resource
   */
  std::string toString();

  dc::dcByte::BasicType getMaxStrengthValue() const      { return myMaxStrength.getValue(); }
  dc::dcByte::BasicType getMaxAgilityValue() const       { return myMaxAgility.getValue(); }
  dc::dcByte::BasicType getMaxConstitutionValue() const  { return myMaxConstitution.getValue(); }
  dc::dcByte::BasicType getMaxIntelligenceValue() const  { return myMaxIntelligence.getValue(); }
  dc::dcByte::BasicType getMaxWisdomValue() const        { return myMaxWisdom.getValue(); }

  //const dc::dcList<ActorType>::Element* getActorType(Evidyon::AvatarGender gender) const { return myActorTypes[gender].getReferencedResource(); }
  int getActorTypeTemplate(Evidyon::AvatarGender gender) const { return actor_type_templates_[gender]; }

  // Writes both genders' actor types into the stream
  bool compileActorTemplates(ActorDescriptionTemplateCompiler* compiler);

  const std::string& getDescription() const;

  size_t getHairStyle(int gender, int style) const { return hair_styles_[gender][style].getReferencedResourceIndex(); }

public:

  /**
   * The name of this resource
   */
  static std::string staticTypeName();


protected:

  // Description of the race
  dc::dcString myDescription;

  /// The maximum values of the stats for level 1 players of this race
  dc::dcByte myMaxStrength;
  dc::dcByte myMaxAgility;
  dc::dcByte myMaxConstitution;
  dc::dcByte myMaxIntelligence;
  dc::dcByte myMaxWisdom;

  /// Which actor type to use for each gender of this race
  dc::dcList<ActorType>::Reference myActorTypes[Avatar::NUMBER_OF_AVATAR_GENDERS];

  // When the races are compiled for the client, this saves the template
  // index so that it can be copied into the server's description.
  int actor_type_templates_[Avatar::NUMBER_OF_AVATAR_GENDERS];

  // Hair styles for each gender
  dc::dcList<Scenery>::Reference hair_styles_[Avatar::NUMBER_OF_AVATAR_GENDERS][Evidyon::AVATAR_RACE_HAIR_STYLES_PER_GENDER];
};

#endif