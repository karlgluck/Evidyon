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
#ifndef __PARTICLETRAILEFFECTVISUALIZATION_H__
#define __PARTICLETRAILEFFECTVISUALIZATION_H__
#pragma once



/**
 * 
 */
class ParticleTrailEffectVisualization : public dc::dcResource<ParticleTrailEffectVisualization,EffectVisualization>
{
    public:

        /**
         * Initializes this class
         */
        ParticleTrailEffectVisualization();

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
        dc::dcTable<Image>::Reference myImage;

        /// The different colors for the particles
        dcxVector3<dcByte> myParticleColor[3];

        /// How many particles to create each period
        dc::dcByte myParticlesPerPeriod;

        /// How quickly new particles are created
        dc::dcFloat mySpawnPeriod;

        /// How often particles are updated
        dc::dcFloat myUpdatePeriod;

        /// How quickly the particles drift upward (negative is downward)
        dc::dcFloat myRisingSpeed;

        /// Distance away in the ring from the source point
        dc::dcFloat myInitialDistanceMin, myInitialDistanceMax;

        /// How fast the particles move away from the center
        dc::dcFloat myRadialVelocity;

        /// How fast the particles move in the effect's linear direction
        dc::dcFloat myDirectedVelocity;

        /// How long each particle lasts
        dc::dcFloat myLifetime;

        /// How long it takes for a particle to decay once its lifetime is over
        dc::dcFloat myDeathTime;

        /// The initial Y-location of the particles
        dc::dcFloat myInitialHeightMin, myInitialHeightMax;

        /// The size of the particles
        dc::dcFloat myParticleSize;

        /// Whether or not to make these particles fall
        dc::dcFloat myGravityFactor;

    protected:

        static dc::dcClass<ParticleTrailEffectVisualization>::Implements<EffectVisualization> implementsEffectVisualization;
};




#endif