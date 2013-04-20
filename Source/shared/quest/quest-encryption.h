//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __QUEST_ENCRYPTION_H__
#define __QUEST_ENCRYPTION_H__
#pragma once


namespace Evidyon {
namespace Quests {

typedef unsigned int QuestEncryptionWord;
static const int QUEST_ENCRYPTION_KEY_WORDS = 4;



//----[  encryptQuestDescription  ]--------------------------------------------
// When a quest description is encrypted, the entire output buffer is filled
// with data right up to the output size.  Anything past the input_text's
// null-terminator is filled with random junk so that the pattern is hard to
// pick out by looking at a block of data with a hex editor.
void encryptQuestDescription(const char* input_text,
                             const QuestEncryptionWord* key,
                             char* output,
                             size_t output_size);



//----[  decryptQuestDescription  ]--------------------------------------------
void decryptQuestDescription(const char* input,
                             const QuestEncryptionWord* key,
                             char* output_text,
                             size_t output_size);

}
}

#endif