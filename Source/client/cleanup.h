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
#ifndef __CLEANUP_H__
#define __CLEANUP_H__
#pragma once



#include <list>



class Cleanup
{
  enum Type
  {
    INTERFACE,
    OBJECT,
    ARRAY,
    WINDOW,
    STDFILE,
  };

  /// The entries that are in this cleanup class
  typedef std::list<std::pair<Type,void**>> Entries;


  public:

    /**
     * Initializes the class
     */
    Cleanup();

    /**
     * Erases all of the allocated memory in the class
     */
    ~Cleanup();

    /**
     * Adds a Direct3D interface to the cleanup list.  This will both
     * call (*iface)->Release and set (*iface) = NULL.
     */
    void addInterface(IUnknown** iface);

    /**
     * Adds the given pointer to the cleanup list.  This will
     * call delete (*pointer) and (*pointer) = NULL;
     */
    void addObject(void** pointer);

    /**
     * Adds the given pointer to the cleanup list.  This will
     * call delete [] (*pointer) and (*pointer) = NULL;
     */
    void addArray(void** pointer);

    /**
     * Adds the given window variable to the cleanup list.  This will
     * call DestroyWindow(*pointer) and *pointer = NULL;
     */
    void addWindow(HWND* pointer);

    /**
     * Adds the given file variable to the cleanup list.  This will
     * call fclose(*pointer) and *pointer = NULL;
     */
    void addStdFile(FILE** pointer);

    /**
     * Erases all of the entries in the list so that, when this
     * class is destructed, it won't erase all of the information
     * that it contains.
     */
    void clear();


  protected:

    /// The entries in the cleanup set
    Entries myEntries;
};





#endif