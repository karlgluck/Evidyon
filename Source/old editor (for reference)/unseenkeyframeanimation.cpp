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

