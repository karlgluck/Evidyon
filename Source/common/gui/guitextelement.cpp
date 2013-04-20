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



//-----------------------------------------------------------------------------
// Name: render
// Desc: Renders this text list into the given layer.
//-----------------------------------------------------------------------------
bool GUITextElement::render(GUILayer* layer)
{
  // Get the font with which to render
  GUIFont* font = getFont();
  if (!font) return true;

  // Initialize the subclass
  getNextTextSegment(0);

  // Grab all of the text segment information
  TextSegmentInfo nextSegmentInfo;
  ZeroMemory(&nextSegmentInfo, sizeof(nextSegmentInfo));
  while(getNextTextSegment(&nextSegmentInfo))
  {
    // Draw this text
    if (!font->renderText(
        layer,
        myClippingArea,
        nextSegmentInfo.x, nextSegmentInfo.y,
        mySourceCanvas ? mySourceCanvas->getDepth() - 0.001f : 0.001f, // TODO: epsilon?
        nextSegmentInfo.text,
        nextSegmentInfo.lineLength,
        nextSegmentInfo.color,
        nextSegmentInfo.obscuringCharacter
       ))
      return false;

    // Move the current y-coordinate
    if (nextSegmentInfo.linesToIncrementAfterward)
    {
      float pixelsToIncrement = font->getLineHeightInPixels() * nextSegmentInfo.linesToIncrementAfterward;
      nextSegmentInfo.y += pixelsToIncrement;
    }

    // Move the X-coordinate
    GUISize lastSegmentSize;
    font->getTextExtent(nextSegmentInfo.text, nextSegmentInfo.lineLength, &lastSegmentSize);
    nextSegmentInfo.x += lastSegmentSize.width;
  }

  // Success
  return true;
}


//-----------------------------------------------------------------------------
int GUITextElement::getTexture()
{
  GUIFont* font = getFont();
  if (font) return font->getTextureID();
  else return 0;
}
