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
#ifndef __ANIMATEDTEXTURE_H__
#define __ANIMATEDTEXTURE_H__
#pragma once

/**
 * This texture iterates through a texture, displaying one frame at a time from it.
 */
class AnimatedTexture : public dc::dcResource<AnimatedTexture,Texture>
{
    public:

        /**
         * Initializes this class
         * @param name The name of this resource
         */
        AnimatedTexture();

    public:

        /**
         * Sets up the texture stage indicated on the provided device for rendering
         * @param pd3dDevice The device on which to set up the textures
         * @param stage The stage to configure
         * @return Error result code from D3D, or S_OK
         */
        HRESULT configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage);

        /**
         * Gets how many frames wide this animated texture is
         */
        int getFramesWideValue() const { return myFramesWide.getValue(); }

        /**
         * Gets how many frames high this animated texture is
         */
        int getFramesHighValue() const { return myFramesHigh.getValue(); }

        /**
         * Gets the speed at which this texture animates
         */
        float getAnimationSpeedValue() const { return myAnimationSpeed.getValue(); }

    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// How many frames wide the texture is
        dc::dcByte myFramesWide;

        /// How many frames high the texture is
        dc::dcByte myFramesHigh;

        /// Defines the size of the circling
        dc::dcFloat myAnimationSpeed;


    protected:

        /// This class implements the Texture interface
        static dc::dcClass<AnimatedTexture>::Implements<Texture> implementsTexture;
};


#endif