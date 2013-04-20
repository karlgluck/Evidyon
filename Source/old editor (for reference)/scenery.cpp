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

#include "stdafx.h"
#include "image.h"
#include "texture.h"
#include "skin.h"
#include "mesh.h"
#include "scenery.h"


/// Resolve the class implementation linker
dcClass<Scenery>::Implements<Scenery> implementsSelf;



//------------------------------------------------------------------------------------------------
// Name:  Scenery
// Desc:  Initializes this scenery with a name
//------------------------------------------------------------------------------------------------
Scenery::Scenery()
{
    member("Mesh", &myMesh);
    member("Skin", &mySkin);
    member("Rotation", &myRotation);
    member("Dilation", &myScaling);
    member("Translation", &myTranslation);

    // Initialize the scaling with unity
    myScaling.getX()->setValue(1.0f);
    myScaling.getY()->setValue(1.0f);
    myScaling.getZ()->setValue(1.0f);
}


//------------------------------------------------------------------------------------------------
// Name:  loadPreview
// Desc:  Initializes a preview structure for this scenery element
//------------------------------------------------------------------------------------------------
bool Scenery::loadPreview(Mesh::Preview* meshPreviewArray, size_t meshPreviews,
                           Skin::Preview* skinPreviewArray, size_t skinPreviews, Preview* preview)
{
    // Check the parameters
    if (APP_ERROR(!meshPreviewArray || !skinPreviewArray || !preview)
            ("Invalid parameter to Scenery::loadPreview"))
        return false;

    // Set up basic elements of the preview structure
    preview->scenery = this;
    preview->meshPreview = 0;
    preview->skinPreview = 0;

    // Construct the transformation matrix
    {
        D3DXMATRIXA16 w, s, r, t;
        D3DXMatrixScaling(&s, myScaling.getX()->getValue(), myScaling.getY()->getValue(), myScaling.getZ()->getValue());
        D3DXMatrixRotationYawPitchRoll(&r, D3DXToRadian(myRotation.getY()->getValue()),
                                            D3DXToRadian(myRotation.getX()->getValue()),
                                            D3DXToRadian(myRotation.getZ()->getValue()));
        D3DXMatrixTranslation(&t, myTranslation.getX()->getValue(), myTranslation.getY()->getValue(), myTranslation.getZ()->getValue());
        D3DXMatrixMultiply(&w, &s, &r);
        D3DXMatrixMultiply(&preview->transform, &w, &t);
    }

    // Get the referenced mesh
    dc::dcTable<Mesh>::Element* meshElement = myMesh.getReferencedResource();
    if (!APP_WARNING(meshElement == 0)
        ("Scenery %s has a null mesh reference", getName().c_str()))
    {
        // Get the index of this element in the table
        size_t meshIndex = meshElement->getIndex();

        // Obtain the mesh preview from the array
        if (!APP_WARNING(meshIndex >= meshPreviews || meshPreviewArray[meshIndex].mesh != meshElement->getImplementation())
                ("Scenery %s references mesh with a mismatched index; ignoring", getName().c_str()))
        {
            // Save the associated mesh preview
            preview->meshPreview = &meshPreviewArray[meshIndex];
        }
    }

    // Get the referenced skin
    dc::dcList<Skin>::Element* skinElement = mySkin.getReferencedResource();
    if (!APP_WARNING(skinElement == 0)
        ("Scenery %s has a null skin reference", getName().c_str()))
    {
        // Get the index of this element in the table
        size_t skinIndex = skinElement->getIndex();

        // Obtain the mesh preview from the array
        if (!APP_WARNING(skinIndex >= skinPreviews || skinPreviewArray[skinIndex].skin != skinElement)
                ("Scenery %s references skin with a mismatched index; ignoring", getName().c_str()))
        {
            // Save the associated mesh preview
            preview->skinPreview = &skinPreviewArray[skinIndex];
        }
    }

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string Scenery::staticTypeName()
{
    return "Scenery";
}
