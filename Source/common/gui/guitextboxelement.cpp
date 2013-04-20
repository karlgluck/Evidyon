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

