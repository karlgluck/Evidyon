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




