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

