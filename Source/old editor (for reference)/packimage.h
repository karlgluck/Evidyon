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
#ifndef __PACKIMAGE_H__
#define __PACKIMAGE_H__
#pragma once



/**
 * A pack image is an image that is inside of a resource pack
 */
class PackImage : public dc::dcResource<PackImage,Image>
{
    public:

        /**
         * Initializes this resource
         */
        PackImage();


    public:

        /**
         * Loads the image denoted by this class using the specified device.  A device pointer
         * is made a parameter so that even when this class is not being edited, the method can
         * still be used to load the texture.
         *   @param pd3dDevice Device onto which to load the texture
         *   @param options Options used when creating the destination texture
         *   @param ppd3dTexture Return location for texture interface
         *   @return Result code
         */
        bool obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice, DWORD options, LPDIRECT3DTEXTURE9* ppd3dTexture) const;


    public:

        /**
         * Changes the resource index that this packed image uses to access its source data.  This
         * method is required for building resource packs, and really shouldn't be used anywhere
         * other than there.
         */
        void setResourceIndex(int index);

        /**
         * Get the resource pack reference for this image
         */
        dc::dcTable<ResourcePack>::Reference* getResourcePack() { return &myResourcePack; }


    public:

        /**
         * Gets the string name of this resource type
         * @return Class name
         */
        static std::string staticTypeName();


    protected:

        /// The resource pack to which the image is attached
        dc::dcTable<ResourcePack>::Reference myResourcePack;

        /// The index that this image occupies in the pack.  We alias this type
        /// so that it can't be edited like a normal integer.
        Alias<dcInteger> myResourceIndex;

        /// This class implements the Image interface
        static dc::dcClass<PackImage>::Implements<Image> implementsImage;
};



#endif