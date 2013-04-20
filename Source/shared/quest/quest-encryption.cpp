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
#include "quest-encryption.h"
#include <memory.h>
#include <stdlib.h>
#include <string.h>


namespace Evidyon {
namespace Quests {




//----[  encryptQuestDescription  ]--------------------------------------------
// When a quest description is encrypted, the entire output buffer is filled
// with data right up to the output size.  Anything past the input_text's
// null-terminator is filled with random junk so that the pattern is hard to
// pick out by looking at a block of data with a hex editor.
void encryptQuestDescription(const char* input_text,
                             const QuestEncryptionWord* key,
                             char* output,
                             size_t output_size) {
  strcpy_s(output, output_size, input_text);
  return;

  size_t length = strlen(input_text);
  length = length < output_size ? length : output_size;
  memcpy(output, input_text, length);
  output[length] = '\0';
  for (size_t i = 0; i < length; i += 2) {
    output[i] = 256 - output[i];
  }
  for (size_t i = length + 1; i < output_size; ++i) {
    output[i] = rand() % 256;
  }
}



//----[  decryptQuestDescription  ]--------------------------------------------
void decryptQuestDescription(const char* input,
                             const QuestEncryptionWord* key,
                             char* output_text,
                             size_t output_size) {
  strcpy_s(output_text, output_size, input);
  return;

  size_t length = strlen(input); // not encrypted...
  if (length > output_size) length = output_size;
  for (size_t i = 0; i < length; ++i) {
    if ((i % 2) == 0) {
      output_text[i] = input[i];
    } else {
      output_text[i] = 256 - input[i];
    }
  }
  output_text[length] = '\0';
}

}
}