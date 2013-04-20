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




//-----------------------------------------------------------------------------
// Name: GUITextLabelElement
// Desc: Initializes the class
//-----------------------------------------------------------------------------
GUITextLabelElement::GUITextLabelElement()
{
  myFont = 0;
  myText = "";
  myTextColor = D3DCOLOR_XRGB(255,255,255);
  myAlignment = GUIALIGN_CENTER;
}




//-----------------------------------------------------------------------------
// Name: setFont
// Desc: Changes the font used to render this text
//-----------------------------------------------------------------------------
void GUITextLabelElement::setFont(GUIFont* font)
{
  myFont = font;
}



//-----------------------------------------------------------------------------
// Name: setText
// Desc: Changes the displayed text
//-----------------------------------------------------------------------------
void GUITextLabelElement::setText(const std::string& text)
{
  myText = text;
}



//-----------------------------------------------------------------------------
// Name: setTextColor
// Desc: Changes this text's color
//-----------------------------------------------------------------------------
void GUITextLabelElement::setTextColor(D3DCOLOR color)
{
  myTextColor = color;
}



//-----------------------------------------------------------------------------
void GUITextLabelElement::setAlignment(Alignment alignment)
{
  myAlignment = alignment;
}




//-----------------------------------------------------------------------------
// Name: getFont
// Desc: Gets the font to use to render the this label
//-----------------------------------------------------------------------------
GUIFont* GUITextLabelElement::getFont()
{
  return myFont;
}



//-----------------------------------------------------------------------------
// Name: getNextTextSegment
// Desc: Returns the text for this label
//-----------------------------------------------------------------------------
bool GUITextLabelElement::getNextTextSegment(TextSegmentInfo* nextSegmentInfo)
{
  if (nextSegmentInfo)
  {
    // Only render one line
    if (nextSegmentInfo->text == myText.c_str()) return false;

    nextSegmentInfo->text = myText.c_str();
    nextSegmentInfo->lineLength = -1;

    if (myFont && mySourceCanvas)
    {
      GUIPoint point;
      if (myAlignment == GUIALIGN_CENTER)
        myFont->centerTextInArea(myText.c_str(), -1, mySourceCanvas->getArea(), &point);
      else {
        GUISize size;
        myFont->getTextExtent(myText.c_str(), -1, &size);
        point.y = myAlignment == GUIALIGN_LEFT_TOP ?
              mySourceCanvas->getArea()->top :
              (mySourceCanvas->getArea()->top+mySourceCanvas->getArea()->bottom)/2.0f - size.height / 2.0f;
        point.x = (myAlignment == GUIALIGN_LEFT || myAlignment == GUIALIGN_LEFT_TOP) ?
              mySourceCanvas->getArea()->left :
              mySourceCanvas->getArea()->right - size.width;
      }
      nextSegmentInfo->x = point.x;
      nextSegmentInfo->y = point.y;
    }

    nextSegmentInfo->linesToIncrementAfterward = 0;
    nextSegmentInfo->color = myTextColor;

    // Render this segment
    return true;
  }
  else
    return true;
}

