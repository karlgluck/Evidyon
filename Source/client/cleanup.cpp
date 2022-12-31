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
#include <d3dx9.h>
#include "cleanup.h"



//-----------------------------------------------------------------------------
// Name:  Cleanup
// Desc:  
//-----------------------------------------------------------------------------
Cleanup::Cleanup()
{
}



//-----------------------------------------------------------------------------
// Name:  ~Cleanup
// Desc:  
//-----------------------------------------------------------------------------
Cleanup::~Cleanup()
{
  for (Entries::iterator i = myEntries.begin(); i != myEntries.end(); ++i)
  {
    switch(i->first)
    {
      case ARRAY:
      {
        void* pointer = *((void**)(i->second));
        if (pointer) delete[] pointer;
        *((void**)(i->second)) = NULL;
      } break;

      case INTERFACE:
      {
        IUnknown* iface = *((IUnknown**)(i->second));
        if (iface) iface->Release();
        (*(IUnknown**)i->second) = NULL;
      } break; 

      case OBJECT:
      {
        void* pointer = *((void**)(i->second));
        if (pointer) delete pointer;
        *((void**)(i->second)) = NULL;
      } break;

      case WINDOW:
      {
        HWND hWnd = *((HWND*)(i->second));
        if (hWnd) DestroyWindow(hWnd);
        *((HWND*)(i->second)) = NULL;
      } break;

      case STDFILE:
      {
        FILE* pFile = *((FILE**)(i->second));
        if (pFile) fclose(pFile);
        *((FILE**)(i->second)) = NULL;
      } break;
    }
  }
}



//-----------------------------------------------------------------------------
// Name:  addArray
// Desc:  
//-----------------------------------------------------------------------------
void Cleanup::addArray(void** pointer)
{
  myEntries.push_front(Entries::value_type(ARRAY, pointer));
}



//-----------------------------------------------------------------------------
// Name:  addInterface
// Desc:  
//-----------------------------------------------------------------------------
void Cleanup::addInterface(IUnknown** iface)
{
  myEntries.push_front(Entries::value_type(INTERFACE, (void**)iface));
}



//-----------------------------------------------------------------------------
// Name:  addObject
// Desc:  
//-----------------------------------------------------------------------------
void Cleanup::addObject(void** pointer)
{
  myEntries.push_front(Entries::value_type(OBJECT, pointer));
}


//-----------------------------------------------------------------------------
// Name:  addWindow
// Desc:  Adds the given window variable to the cleanup list
//-----------------------------------------------------------------------------
void Cleanup::addWindow(HWND* pointer)
{
  myEntries.push_front(Entries::value_type(WINDOW, (void**)pointer));
}


//-----------------------------------------------------------------------------
// Name:  addStdFile
// Desc:  Adds the given file variable to the cleanup list
//-----------------------------------------------------------------------------
void Cleanup::addStdFile(FILE** pointer)
{
  myEntries.push_front(Entries::value_type(STDFILE, (void**)pointer));
}



//-----------------------------------------------------------------------------
// Name:  clear
// Desc:  
//-----------------------------------------------------------------------------
void Cleanup::clear()
{
  myEntries.clear();
}




