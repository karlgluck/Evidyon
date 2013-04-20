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

#ifndef __UNSEENKEYFRAMEANIMATION_H__
#define __UNSEENKEYFRAMEANIMATION_H__
#pragma once




/**
 * This class references a .ka file containing a keyframe animation ready to be applied to a mesh
 */
class UnseenKeyframeAnimation : public dc::dcResource<UnseenKeyframeAnimation>
{
    public:

        /**
         * Initializes this resource
         */
        UnseenKeyframeAnimation();


    public:

        /**
         * Loads the referenced animation from the source file
         */
        bool loadAnimation(UnseenSkeletalAnimation* animation) const;

        /**
         * Gets the source file name for this skeletal animation
         * @return Pointer to the internal file name object
         */
        dc::dcFileName* getFileName();


    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The file from which to load the animation
        dc::dcFileName mySourceFile;
};



#endif