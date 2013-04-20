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
#ifndef __LEVELDEFINITIONS_H__
#define __LEVELDEFINITIONS_H__
#pragma once

#include <dc/integer>
#include <dc/float>
#include "../shared/avatar/evidyon_avatar.h"


class LevelAttributes : public dc::dcResource<LevelAttributes> {
public:
  LevelAttributes();
  int getExperienceToNextLevel() const;
  float getMeleeDPS() const;
  float getMagicDPS() const;
  int getAverageHP() const;
  int getAverageMP() const;
  void setAverageHP(int value);
  void setAverageMP(int value);
  float getHoursToGainLevel() const;
  float getSecondsToGainLevel() const;
  float getExpectedExpPerSecond() const;

public:
  static std::string staticTypeName();

private:
  dc::dcInteger experience_to_next_level_;
  dc::dcInteger average_hp_;
  dc::dcInteger average_mp_;
  dc::dcFloat melee_damage_per_second_;
  dc::dcFloat magic_damage_per_second_;
  dc::dcFloat hours_to_gain_level_;
};


/**
 * Defines the amount of experience necessary to achieve a certain level
 */
class LevelDefinitions : public dc::dcResource<LevelDefinitions>
{
public:

  /**
   * Initializes this class
   */
  LevelDefinitions();

  /**
   * Compiles this structure into the output stream for the server
   */
  bool compileForServer(dc::dcStreamOut* stream) const;

  const LevelAttributes* getLevelAttributes(int level) const;
  LevelAttributes* getLevelAttributes(int level);

public:
  static std::string staticTypeName();

protected:
  LevelAttributes level_attributes_[Evidyon::HIGHEST_DEFINED_LEVEL];

};

#endif