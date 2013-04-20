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

#ifndef __RESOURCEPACK_H__
#define __RESOURCEPACK_H__
#pragma once

#include <dc/buffer>


/**
 *
 */
class ResourcePack : public dc::dcResource<ResourcePack>
{
    public:

        /// The number that is always the same in the resource pack header
        static const int MAGIC_NUMBER = 0x7FEED01B;

        /**
         * Structure always at the start of the resource pack.  The pack
         * file is set up like the following:
         *      FileHeader header;
         *      FileResourceIndexElement index[FileHeader.ResourceCount];
         *      (possible junk data here)
         *      << array of resources >>
         *      ProjectVolucris project;
         */
        struct FileHeader
        {
            /// Always the same number (unless the pack changes)
            unsigned long MagicNumber;

            /// The name of this resource pack
            char PackName[32];

            /// Where the project to be imported is located in the file
            size_t ProjectLocation;

            /// How many resources there are in this pack file
            size_t ResourceCount;
        };

        /**
         * Specifies where to find a resource and that resource's byte size.  An
         * array of these comes directly after the header in the resource pack,
         * with one entry for each saved resource.
         */
        struct FileResourceIndexElement
        {
            /// The absolute byte location of the resource in the file
            size_t ResourceLocation;

            /// How large the resource is, in byte
            size_t ResourceSize;
        };


    public:

        /**
         * Initializes the class
         */
        ResourcePack();

        /**
         * Opens a stream at the location of the resource with the given index
         * @param index The index of the item to access
         * @param buffer The destination buffer to be filled with data
         * @return Whether or not the stream will return valid data
         */
        bool openResource(int index, dcBuffer* buffer);

        /**
         * Loads the resources from this pack into the specified project
         * @param packName Destination for the name that this this resource pack
         *                 needs to have in order for its references to work correctly.
         * @param emptyProject A blank project into which to load the resources
         *                     in the pack file.
         */
        bool loadPackResources(std::string* packName, ProjectVolucris* emptyProject);

        /**
         * Gets the source filename for this pack
         */
        dc::dcFileName* getSourceFile() { return &mySourceFile; }


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The resource pack file
        dc::dcFileName mySourceFile;

        /// Implements its own type
        static dc::dcClass<ResourcePack>::Implements<ResourcePack> implementsSelf;
};



#endif