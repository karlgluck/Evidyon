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
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include <d3dx9.h>
#include <map>
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guilayer.h"
#include "guifont.h"
#include "guitextelement.h"
#include "guitextlabelelement.h"
#include "guitextboxelement.h"


//-----------------------------------------------------------------------------
// Name: getNextTextSegment
// Desc: Returns the text for this text box
//-----------------------------------------------------------------------------
bool GUITextBoxElement::getNextTextSegment(TextSegmentInfo* nextSegmentInfo)
{/*
  // Build a list of line number to line text pairs
  typedef std::list<std::pair<int,std::string>> LineList;
  LineList lines;
  {
  size_t offset = 0;
  int line = 0;
  while(offset < myText.npos) {
    const char* endline = "\r\n";
    size_t location = myText.find_first_of(endline, offset);
    lines.push_back(std::pair<int,std::string>(line, myText.substr(offset, location - 1)));
    offset = myText.find_first_not_of(endline, location);
    line += offset - location;
  }
  }

  // Wrap each element to the right-left boundary using a binary search for
  // the rightmost word.
  int wrap_width = mySourceCanvas->getArea()->calculateSize().width;
  for (LineList::iterator i = lines.begin(); i != lines.end(); ++i) {
    std::string& lineText = i->second;
    if (
  }*/
  return false;

}

