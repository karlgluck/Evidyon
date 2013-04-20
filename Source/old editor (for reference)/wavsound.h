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
// Desc:    Controls audio playing and loading
#ifndef __WAVSOUND_H__
#define __WAVSOUND_H__
#pragma once

#include <dc/filename>
//#include <dc/stream>


/**
 * Holds sound information
 */
class WAVSound : public dc::dcResource<WAVSound>
{
    public:

        /**
         * Initializes this class
         */
        WAVSound();

        /**
         * Compiles this sound so that the client can load it
         */
        bool compileForClient(dc::dcStreamOut* clientStream) const;

        dc::dcFileName* getSourceFile() { return &mySourceFile; }

    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The file from which to load the sound
        dc::dcFileName mySourceFile;
};





#endif