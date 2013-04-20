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
#include "wavsound.h"

#include "image.h"
#include "texture.h"
#include "skin.h"
#include "mesh.h"
#include "scenery.h"
#include "sceneryeffectvisualization.h"

/// This class implements a special effect
dcClass<SceneryEffectVisualization>::Implements<EffectVisualization> SceneryEffectVisualization::implementsEffectVisualization;



//------------------------------------------------------------------------------------------------
// Name: SceneryEffectVisualization
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
SceneryEffectVisualization::SceneryEffectVisualization()
{
    member("Scenery", &myScenery);
    member("Translation", &myTranslation);
    member("Rotate Flag", &myRotateFlag);
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool SceneryEffectVisualization::compileForClient(ClientEffectVisualizationDescription* description) const
{
    description->soundEffectIndex = getSoundEffectIndex();

    description->duration = myDuration.getValue();
    description->type = CEVISUALIZATION_SCENERY;

    // Get the particle's image index
    {
        const dc::dcList<Scenery>::Element* sceneryElement = myScenery.getReferencedResource();
        description->Scenery.sceneryIndex = sceneryElement ? sceneryElement->getIndex() : 0xFFFFFFFF;
    }

    description->Scenery.x = myTranslation.getX()->getValue();
    description->Scenery.y = myTranslation.getY()->getValue();
    description->Scenery.z = myTranslation.getZ()->getValue();

    description->Scenery.rotate = myRotateFlag.getValue();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string SceneryEffectVisualization::staticTypeName()
{
    return "SceneryEffectVisualization";
}

