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
#ifndef __EVIDYONGUIDEFS_H__
#define __EVIDYONGUIDEFS_H__
#pragma once


#include <d3dx9.h>

// This namespace defines areas of the source texture that is used to render
// the GUI.
namespace EvidyonGUIDefs {

  static const GUISize TEXTURE_SIZE = { 512, 512 };

  static const int BUTTON_BORDER_PIXELS = 8;
  //static const GUIRect BUTTON_BACKGROUND_NEUTRAL   = {0,  0, 31, 31};
  //static const GUIRect BUTTON_BACKGROUND_HOVER     = {0, 32, 31, 63};
  //static const GUIRect BUTTON_BACKGROUND_MOUSEDOWN = {0, 64, 31, 95};
  static const GUIRect BUTTON_BACKGROUND_NEUTRAL   = {8*16, 3*16, 16*16, 5*16};
  static const GUIRect BUTTON_BACKGROUND_HOVER     = {8*16, 5*16, 16*16, 7*16};
  static const GUIRect BUTTON_BACKGROUND_MOUSEDOWN = {8*16, 7*16, 16*16, 9*16};

  static const D3DCOLOR BUTTON_COLOR_NEUTRAL  = D3DCOLOR_XRGB(255, 255, 255);
  static const D3DCOLOR BUTTON_COLOR_DISABLED = D3DCOLOR_XRGB(177, 155, 106);

  static const GUIRect EMPTY = {6*32, 0, 6*32+16, 16};

  //static const GUIRect WINDOW_BACKGROUND = {1*32, 0*32,1*32+32,0*32+32};
  static const GUIRect WINDOW_BACKGROUND = {256, 256,512,512};
  static const int WINDOW_BACKGROUND_BORDER = 8;
  static const D3DCOLOR WINDOW_TITLE_TEXT_COLOR = D3DCOLOR_XRGB(220,220,220);
  //static const GUIRect WINDOW_TITLE_BACKGROUND = {2*32, 0*32,2*32+32,0*32+32};
  static const GUIRect WINDOW_TITLE_BACKGROUND = {256, 256-2*16,512,256};
  static const int WINDOW_TITLE_BACKGROUND_BORDER = 8;
  //static const GUIRect CONTENT_COMPONENT_BACKGROUND = {3*32, 0*32,3*32+32,0*32+32};
  static const GUIRect CONTENT_COMPONENT_BACKGROUND = {0, 256, 256, 512};

  static const D3DCOLOR CONTENT_COMPONENT_TEXT_COLOR = D3DCOLOR_XRGB(200,200,200);

  //static const GUIRect SLIDER_HANDLE = {4*32, 1*32, 4*32+16-1, 1*32+64-1};//old
  static const GUIRect SLIDER_HANDLE = {2*16, 1*32, 2*16+16, 1*32+64};
  static const GUIRect SLIDER_BACKGROUND = {4*32, 0*32, 4*32+16-1, 0*32+32-1};

  static const GUIRect HP_BAR_BACKGROUND = {4*16,3*16,5*16,4*16};
  //static const GUIRect HP_BAR_FILL = {5*16,3*16,6*16,4*16};
  static const GUIRect HP_BAR_FILL = {0,7*16,1*16,16*16};
  static const GUIRect MP_BAR_BACKGROUND = {4*16,4*16,5*16,5*16};
  //static const GUIRect MP_BAR_FILL = {5*16,4*16,6*16,5*16};
  static const GUIRect MP_BAR_FILL = {16,7*16,2*16,16*16};
  static const GUIRect EXP_BAR_FILL = {2*16,7*16,3*16,16*16};

  static const GUIRect LIST_ENTRY_BACKGROUND_MOUSEOVER = {9*16,3*16,15*16,5*16};
  static const GUIRect LIST_ENTRY_BACKGROUND_MOUSEOVERANDDOWN = {9*16,5*16,15*16,7*16};
  static const GUIRect LIST_ENTRY_BACKGROUND_SELECTED = {9*16,7*16,15*16,9*16};
  static const GUIRect LIST_ENTRY_BACKGROUND_NEUTRAL = {6*16+2,3*16+2,7*16-2,4*16-2};
  static const D3DCOLOR LIST_ENTRY_TEXT_COLOR = D3DCOLOR_XRGB(200,200,200);
  static const D3DCOLOR LIST_ENTRY_TEXT_SPECIAL_COLOR = D3DCOLOR_XRGB(200,255,200);

  static const D3DCOLOR TEXT_INPUT_COLOR = D3DCOLOR_XRGB(200,200,200);

  static const GUIRect MOUSE_CURSOR_NORMAL = {32*8, 0,   32*8+64, 64};
  static const GUIRect MOUSE_CURSOR_ACTIVE = {32*10, 0, 32*10+64, 64};

  static const int GUI_TEXTURE_INDEX = 1;
  static const int WINDOW_FONT_TEXTURE_INDEX = 2;
  static const int TITLE_FONT_TEXTURE_INDEX = 3;
};


#endif