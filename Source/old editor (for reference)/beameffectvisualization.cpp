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

#include "beameffectvisualization.h"

/// This class implements a special effect
dcClass<BeamEffectVisualization>::Implements<EffectVisualization> BeamEffectVisualization::implementsEffectVisualization;



//------------------------------------------------------------------------------------------------
// Name: BeamEffectVisualization
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
BeamEffectVisualization::BeamEffectVisualization()
{
    member("Source Image", &mySourceImage);
    member("Terminal Image", &myTerminalImage);
    member("Beam Fill Image", &myBeamFillImage);
    member("Beam Width", &myBeamWidth);
    member("Ends Image Size", &myEndsImageSize);
    member("Beam Height", &myHeight);
    member("Cast From Hands", &myCastFromHandsFlag);
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool BeamEffectVisualization::compileForClient(ClientEffectVisualizationDescription* description) const
{
    description->soundEffectIndex = getSoundEffectIndex();

    description->duration = myDuration.getValue();
    description->type = CEVISUALIZATION_BEAM;
    description->Beam.beamWidth = myBeamWidth.getValue();
    description->Beam.height = myHeight.getValue();
    description->Beam.sourceTerminalImageSize = myEndsImageSize.getValue();
    description->Beam.cast_from_hands = myCastFromHandsFlag.getValue();

    // Get the particle's image index
    {
        const dc::dcTable<Image>::Element* imageElement = mySourceImage.getReferencedResource();
        description->Beam.sourceImageIndex = imageElement ? imageElement->getIndex() : 0xFFFFFFFF;
    }

    // Get the particle's image index
    {
        const dc::dcTable<Image>::Element* imageElement = myTerminalImage.getReferencedResource();
        description->Beam.terminalImageIndex = imageElement ? imageElement->getIndex() : 0xFFFFFFFF;
    }

    // Get the particle's image index
    {
        const dc::dcTable<Image>::Element* imageElement = myBeamFillImage.getReferencedResource();
        description->Beam.beamImageIndex = imageElement ? imageElement->getIndex() : 0xFFFFFFFF;
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string BeamEffectVisualization::staticTypeName()
{
    return "BeamEffectVisualization";
}

