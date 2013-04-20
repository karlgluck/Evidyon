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

// Desc:    A resource pack puts together a bunch of resources in a flat file so that they can
//          be distributed as a unit
#include "stdafx.h"
#include "allgameresources.h"
#include "resourcepack.h"
#include <dc/filestream>
#include <dc/dcresourcetrader.h>



#include "stdafx.h"











// Resolve self-implementation linker
dcClass<ResourcePack>::Implements<ResourcePack> ResourcePack::implementsSelf;


//------------------------------------------------------------------------------------------------
// Name:  ResourcePack
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
ResourcePack::ResourcePack()
{
    member("Source File", &mySourceFile);
}


//------------------------------------------------------------------------------------------------
// Name:  openResource
// Desc:  Opens a stream at the location of the resource with the given index
//------------------------------------------------------------------------------------------------
bool ResourcePack::openResource(int index, dcBuffer* buffer)
{
    // Fail if the parameters are invalid
    if (APP_ERROR(index < 0 || !buffer)("Invalid parameter to openResource"))
        return false;

    // Open the file
    dc::dcFileStream file;
    if (!file.open(mySourceFile.getValue().c_str(), 0))
        return false;

    // Load the header
    FileHeader header;
    if (!file.read(&header, sizeof(header)))
    {
        file.close();
        return false;
    }

    // Make sure the magic number matches and this resource is valid
    if (APP_WARNING(header.MagicNumber != ResourcePack::MAGIC_NUMBER)("Resource pack invalid") ||
        APP_WARNING(index >= header.ResourceCount)("Invalid resource index requested"))
    {
        file.close();
        return false;
    }

    // Look forward in the file to find this resource's location and size
    size_t resourceLocation, resourceSize;
    if (!file.move(sizeof(size_t) * 2 * index) ||
        !file.read(&resourceLocation, sizeof(resourceLocation)) ||
        !file.read(&resourceSize, sizeof(resourceSize)))
    {
        file.close();
        return false;
    }

    // Scan to wherever this resource is
    file.seek(resourceLocation);

    // Create a temporary buffer
    //unsigned char* buffer = new unsigned char[resourceSize];
    //if (APP_FATAL(!buffer)("Out of memory")) { file.close(); return false; }

    // Read into this buffer
    //if (APP_ERROR(!file.read(buffer, resourceSize)))
    if (APP_ERROR(!buffer->readFrom(&file, resourceSize))("Couldn't read resource from pack file"))
    {
        // Get rid of the input stream
        file.close();

        // Failure
        return false;
    }

    // Close the file
    file.close();

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  loadPackResources
// Desc:  Loads the resources from this pack into the specified project
//------------------------------------------------------------------------------------------------
bool ResourcePack::loadPackResources(std::string* packName, ProjectVolucris* emptyProject)
{
    // Make sure the parameter is valid
    if (APP_ERROR(!emptyProject)("Invalid parameter to loadPackResources"))
        return false;

    // Open the file
    dc::dcFileStream file;
    if (!file.open(mySourceFile.getValue().c_str(), 0))
        return false;

    // Load the header
    FileHeader header;
    if (!file.read(&header, sizeof(header)))
    {
        file.close();
        return false;
    }

    // Make sure the magic number matches
    if (APP_WARNING(header.MagicNumber != ResourcePack::MAGIC_NUMBER)("Resource pack invalid"))
    {
        file.close();
        return false;
    }

    // Save the pack name
    packName->assign(header.PackName);

    // Move to the project location in the file
    file.seek(header.ProjectLocation);

    // Read the project using the importer
    dc::dcResourceTrader importer;
    bool returnValue = importer.importFrom(emptyProject, &file);
    //bool returnValue = emptyProject->load(&file);

    // Close the file
    file.close();

    // Return the result of the loading process
    return returnValue;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string ResourcePack::staticTypeName()
{
    return "Resource Pack";
}

