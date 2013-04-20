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
#ifndef __BEAMEFFECTVISUALIZATION_H__
#define __BEAMEFFECTVISUALIZATION_H__
#pragma once



/**
 * 
 */
class BeamEffectVisualization : public dc::dcResource<BeamEffectVisualization,EffectVisualization>
{
    public:

        /**
         * Initializes this class
         */
        BeamEffectVisualization();

        /**
         * Enters data into the client effect structure
         */
        bool compileForClient(ClientEffectVisualizationDescription* description) const;


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The image for this particle
        dc::dcTable<Image>::Reference mySourceImage;
        dc::dcTable<Image>::Reference myTerminalImage;
        dc::dcTable<Image>::Reference myBeamFillImage;

        /// The width of the beam
        dc::dcFloat myBeamWidth;

        /// The size of the start/end images
        dc::dcFloat myEndsImageSize;

        /// The beam's distance from the ground
        dc::dcFloat myHeight;

        dc::dcBoolean myCastFromHandsFlag;


    protected:

        static dc::dcClass<BeamEffectVisualization>::Implements<EffectVisualization> implementsEffectVisualization;
};




#endif