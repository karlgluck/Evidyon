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
#include "skinnedmeshattachmentpoint.h"


//------------------------------------------------------------------------------------------------
// Name: SkinnedMeshAttachmentPoint
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
SkinnedMeshAttachmentPoint::SkinnedMeshAttachmentPoint()
{
    member("Source Bone Index", &mySourceBoneIndex);
    member("Scaling", &myScaling);
    member("Translation from Bone", &myTranslation);
    member("Rotation from Bone", &myRotation);

    // Initialize the scaling factor
    myScaling.getX()->setValue(1.0f);
    myScaling.getY()->setValue(1.0f);
    myScaling.getZ()->setValue(1.0f);
}



//------------------------------------------------------------------------------------------------
// Name: getSourceBoneIndexValue
// Desc: Obtains the bone on which this attachment point is based
//------------------------------------------------------------------------------------------------
unsigned int SkinnedMeshAttachmentPoint::getSourceBoneIndexValue() const
{
    return mySourceBoneIndex.getValue();
}



//------------------------------------------------------------------------------------------------
// Name: generateOffsetMatrix
// Desc: Creates the offset matrix to which to attach items
//------------------------------------------------------------------------------------------------
void SkinnedMeshAttachmentPoint::generateOffsetMatrix(D3DXMATRIX* offsetMatrix) const
{
    D3DXMATRIX build;
    D3DXMatrixScaling(&build, myScaling.getX()->getValue(),
                               myScaling.getY()->getValue(),
                               myScaling.getZ()->getValue());
    D3DXMATRIX rotation;
    D3DXMatrixRotationYawPitchRoll(&rotation, D3DXToRadian(myRotation.getY()->getValue()),
                                               D3DXToRadian(myRotation.getX()->getValue()),
                                               D3DXToRadian(myRotation.getZ()->getValue()));
    D3DXMatrixMultiply(&build, &build, &rotation);
    D3DXMATRIX translation;
    D3DXMatrixTranslation(&translation, myTranslation.getX()->getValue(),
                                         myTranslation.getY()->getValue(),
                                         myTranslation.getZ()->getValue());
    D3DXMatrixMultiply(offsetMatrix, &build, &translation);
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Gets the string name of this resource type
//       @return Class name
//------------------------------------------------------------------------------------------------
std::string SkinnedMeshAttachmentPoint::staticTypeName()
{
    return "SkinnedMeshAttachmentPoint";
}

