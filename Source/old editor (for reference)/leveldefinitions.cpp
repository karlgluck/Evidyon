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
#include "stdafx.h"
#include "leveldefinitions.h"



LevelAttributes::LevelAttributes() {
  member("Exp. to Next Level", &experience_to_next_level_);
  member("Average HP", &average_hp_);
  member("Average MP", &average_mp_);
  member("Average Melee DPS", &melee_damage_per_second_);
  member("Average Magic DPS", &magic_damage_per_second_);
  member("Hours To Level", &hours_to_gain_level_);
}

int LevelAttributes::getExperienceToNextLevel() const {
  return experience_to_next_level_.getValue();
}

float LevelAttributes::getMagicDPS() const {
  return magic_damage_per_second_.getValue();
}

float LevelAttributes::getMeleeDPS() const {
  return melee_damage_per_second_.getValue();
}

int LevelAttributes::getAverageHP() const {
  return average_hp_.getValue();
}

int LevelAttributes::getAverageMP() const {
  return average_mp_.getValue();
}

void LevelAttributes::setAverageHP(int value) {
  average_hp_.setValue(value);
}

void LevelAttributes::setAverageMP(int value) {
  average_mp_.setValue(value);
}

float LevelAttributes::getHoursToGainLevel() const {
  return hours_to_gain_level_.getValue();
}

float LevelAttributes::getSecondsToGainLevel() const {
  return hours_to_gain_level_.getValue() * 60.0f * 60.0f;
}

float LevelAttributes::getExpectedExpPerSecond() const {
  return getExperienceToNextLevel() / getSecondsToGainLevel();
}

std::string LevelAttributes::staticTypeName() {
  return "LevelAttributes";
}






//------------------------------------------------------------------------------------------------
// Name: LevelDefinitions
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
LevelDefinitions::LevelDefinitions()
{
    for (int i = 0; i < HIGHEST_DEFINED_LEVEL; ++i)
    {
        char name[8];
        sprintf_s(name, sizeof(name)/sizeof(char), "%i", i+1);
        member(name, &level_attributes_[i]);
    }
}



//------------------------------------------------------------------------------------------------
// Name: compileForServer
// Desc: Compiles this structure into the output stream for the server
//------------------------------------------------------------------------------------------------
bool LevelDefinitions::compileForServer(dcStreamOut* stream) const
{
    if (APP_ERROR(!stream)("Invalid stream")) return false;

    for (int i = 0; i < HIGHEST_DEFINED_LEVEL; ++i)
    {
        int experience = level_attributes_[i].getExperienceToNextLevel();
        if (!stream->write(&experience, sizeof(experience))) return false;
    }

    // Success
    return true;
}


const LevelAttributes* LevelDefinitions::getLevelAttributes(int level) const {
  if (level <= 0) return &level_attributes_[0];
  if (level >= HIGHEST_DEFINED_LEVEL) return &level_attributes_[HIGHEST_DEFINED_LEVEL - 1];
  return &level_attributes_[level - 1];
}

LevelAttributes* LevelDefinitions::getLevelAttributes(int level) {
  if (level <= 0) return &level_attributes_[0];
  if (level >= HIGHEST_DEFINED_LEVEL) return &level_attributes_[HIGHEST_DEFINED_LEVEL - 1];
  return &level_attributes_[level - 1];
}


//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string LevelDefinitions::staticTypeName()
{
    return "LevelDefinitions";
}

