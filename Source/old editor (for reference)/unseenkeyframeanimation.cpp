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

#include "stdafx.h"
#include "unseenkeyframeanimation.h"
#include <dc/filestream>



//------------------------------------------------------------------------------------------------
// Name: UnseenKeyframeAnimation
// Desc: Initializes this resource
//------------------------------------------------------------------------------------------------
UnseenKeyframeAnimation::UnseenKeyframeAnimation()
{
    member("Source File", &mySourceFile);
}



//------------------------------------------------------------------------------------------------
// Name: loadAnimation
// Desc: Loads the referenced animation from the source file
//------------------------------------------------------------------------------------------------
bool UnseenKeyframeAnimation::loadAnimation(UnseenSkeletalAnimation* animation) const
{
    // Open the source stream
    dc::dcFileStream fs;
    if (APP_ERROR(!fs.open(mySourceFile.getValue().c_str(), 0))("Couldn't open source keyframe animation file %s", mySourceFile.getValue().c_str()))
        return false;

    // Load the mesh
    bool failed = APP_ERROR(!loadUnseenSkeletalAnimation(&fs, animation))("Invalid Unseen keyframe animation file %s", mySourceFile.getValue().c_str());

    // Close the stream
    fs.close();

    // Return the result
    return !failed;
}



//------------------------------------------------------------------------------------------------
// Name: getFileName
// Desc: Gets the source file name for this skeletal animation
//       @return Pointer to the internal file name object
//------------------------------------------------------------------------------------------------
dcFileName* UnseenKeyframeAnimation::getFileName()
{
    return &mySourceFile;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Gets the string name of this resource type
//       @return Class name
//------------------------------------------------------------------------------------------------
std::string UnseenKeyframeAnimation::staticTypeName()
{
    return "UnseenKeyframeAnimation";
}

