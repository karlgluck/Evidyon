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
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include <dcx/classallocator>
#include "resource"
#include <dc/filestream>
#include <dc/dcresourcetrader.h>
#include "compileproject.h"
#include <dc/bufferstream>
#include <dc/dcresourcestorage2.h>


using namespace dc;
using namespace dcx;


// Project constants
const char PV_FILE_FILTER[] = "Evidyon Configuration (*.evidyon)\0*.evidyon\0\0";
const char PV_FILE_EXTENSION[] = ".evidyon";
const char PV_EXPORT_FILE_FILTER[] = "Evidyon Exported Projects (*.evc)\0*.evc\0\0";
const char PV_EXPORT_FILE_EXTENSION[] = ".evc";
const char PV_CLIENT_COMPILE_FILE_FILTER[] = "Evidyon Game File (*.gme)\0*.gme\0\0";
const char PV_CLIENT_COMPILE_FILE_EXTENSION[] = ".gme";
const char PV_SERVER_COMPILE_FILE_EXTENSION[] = ".gsvr";
const char PV_RESOURCEPACK_FILE_FILTER[] = "Evidyon Resource Pack (*.evk)\0*.evk\0\0";
const char PV_RESOURCEPACK_FILE_EXTENSION[] = ".evk";


/// This structure declares the animated mesh vertex format
D3DVERTEXELEMENT9 MeshVertexDeclaration[] =
{
    // stream, offset, type, method, usage, usageindex
    { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 }, // this should actually be D3DCOLOR for compatibility...
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

// reg c0     = view/projection matrix transpose
// reg c[n*4+4] = transposed transformation matrices
// reg v0     = input position
// reg v1.xy  = input texcoord
// reg v2.x   = input bone index
// this could possibly be changed to use m4x3 instead of m4x4 for the bones
const char MDJANIMATEDMESH_VERTEXSHADER[] =
"vs.1.1"\
"dcl_position v0\n"\
"dcl_blendindices v1\n"\
"dcl_texcoord v2\n"\
"mov a0.x, v1.x\n"\
"m4x4 r0, v0, c[a0.x]\n"\
"m4x4 oPos, r0, c0\n"\
"mov oT0, v2.xy";


// The general vertex shader is disabled
/*

D3DVERTEXELEMENT9 GeneralVertexDeclaration[] =
{
    // stream, offset, type, method, usage, usageindex
    { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

// c0-3 is view/proj combo
// c4-7 is world
const char GENERAL_VERTEXSHADER[] = 
"vs.1.1"\
"dcl_position v0\n"\
"dcl_color v1\n"\
"dcl_texcoord v2\n"\
"def c8, 0.707, 0.5, 0.5, 0.5\n"\
"m4x4 r0, v0, c4\n"\
"mul r0.y, r0.y, c8.x\n"\
"m4x4 r1, r0, c0\n"\
"mov oPos, r1\n"
"add oPos.y, r1.y, r0.y\n"\
"mov oT0, v2.xy";
*/





//------------------------------------------------------------------------------------------------
// Name:  globalEditor
// Desc:  Gets the global instance of the PV editor
//------------------------------------------------------------------------------------------------
ProjectVolucrisEditor* globalEditor()
{
    // The static resource editor
    static dcx::dcxClassAllocator<ProjectVolucrisEditor> allocator;

    // Return the editor class
    return allocator.getClass();
}


//------------------------------------------------------------------------------------------------
// Name:  ProjectVolucrisEditor
// Desc:  Constructor
//------------------------------------------------------------------------------------------------
ProjectVolucrisEditor::ProjectVolucrisEditor()
{
    ZeroMemory(myProjectFile, sizeof(myProjectFile));
    myProjectChangedFlag = false;

    // Reset the internal Direct3D variables
    myD3D = NULL;
    myD3DDevice = NULL;
    ZeroMemory(&myD3DParams, sizeof(myD3DParams));
    myMeshVertexShader = NULL;
    myMeshVertexDeclaration = NULL;
    myMaxBonesPerSet = 0;
}


//------------------------------------------------------------------------------------------------
// Name:  Run
// Desc:  Executes the editor
//------------------------------------------------------------------------------------------------
int ProjectVolucrisEditor::Run()
{
    // Initialize the common dialog controls
    dcxInitCommonControls();

    // Create the main dialog for the editor and run it
    myRootDialog.execute(&myConfiguration);

    // Generic return code
    return 0;
}


//------------------------------------------------------------------------------------------------
// Name:  getTopmostWindowHandle
// Desc:  Gets the window handle of the top-most editing window
//------------------------------------------------------------------------------------------------
HWND ProjectVolucrisEditor::getTopmostWindowHandle()
{
    if (myWindows.size() == 0)
        return GetDesktopWindow();
    else
        return myWindows.back();
}


//------------------------------------------------------------------------------------------------
// Name:  pushWindowHandle
// Desc:  Adds a new window to the end of the window handle list
//------------------------------------------------------------------------------------------------
void ProjectVolucrisEditor::pushWindowHandle(HWND hWindow)
{
    myWindows.push_back(hWindow);
}



//------------------------------------------------------------------------------------------------
// Name:  popWindowHandle
// Desc:  Pops the last window off of the end of the internal list
//------------------------------------------------------------------------------------------------
void ProjectVolucrisEditor::popWindowHandle(HWND hConfirmWindow)
{
    if (myWindows.size() == 0 || myWindows.back() != hConfirmWindow)
    {
        DEBUG_ERROR("Logic error in window handling!");
    }
    else
        myWindows.pop_back();
}



//------------------------------------------------------------------------------------------------
// Name:  newProject
// Desc:  Prompts the user to create a new project file
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::newProject()
{
    // Save changes to the current project
    if (false == promptSaveChanges()) return false;

    // The string where the file name will be stored
    char file[512];

    // Ask the user where to save the file
    if (dcxSaveFileDialog(myWindows.back(), "Save File As", PV_FILE_FILTER, file, 512))
    {
        // Get rid of all of the editor dialogs
        closeAllWindows();

        // Clear the configuration
        myConfiguration.clear();

        // Make sure the extension is correct
        int len = (int)strlen(file);
        if ((len < sizeof(PV_FILE_EXTENSION)) ||
            (0 != _stricmp(file + len - sizeof(PV_FILE_EXTENSION), PV_FILE_EXTENSION)))
            strcat_s(file, sizeof(file), PV_FILE_EXTENSION);

        // Set the current project file
        setProjectFile(file);

        // Save the project
        return saveProject();
    }
    else
        return false;
}


//------------------------------------------------------------------------------------------------
// Name:  openProject
// Desc:  Asks the user for a file, and loads it if one is selected
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::openProject()
{
    // Save changes to the current project
    if (false == promptSaveChanges()) return false;

    // The file being opened
    char file[512];

    // Ask the user to pick a file
    if (false == dcx::dcxOpenFileDialog(myWindows.back(), "Open File", PV_FILE_FILTER, file, sizeof(file)))
        return false;


    // Change the path
    dc::dcFileName::setCurrentPathFromFileDirectory(file);

    // Get rid of all of the editor dialogs
    closeAllWindows();

    bool succeeded = false;
    dc::dcResourceStorage2 storage;
    if (storage.openFile(file, "pv")) {
      myConfiguration.clear();
      succeeded = storage.load("", &myConfiguration, true);
      storage.close();
    }

    //// Open the project file
    //dc::dcFileStream source;
    //if (!source.open(file, 0))
    //{
    //    // Error dialog box
    //    MessageBox(myWindows.back(), "The source file couldn't be opened.", "Error", MB_OK);

    //    // Didn't open a file
    //    return false;
    //}

    // Load the file
    //if (!myConfiguration.load(&source))
    if (!succeeded)
    {
        // Give the user an error
        MessageBox(myWindows.back(), "There was a problem loading this project; the information "
                                      "displayed could be corrupted.", "Error", MB_OK);
        setProjectFile(""); // Reset the current project file
        myProjectChangedFlag = true;  // This project has been modified

        // Refresh the root dialog
        myRootDialog.synchronizeDisplay();
        myConfiguration.clear();

        //// Close the file
        //source.close();

        // Exit.  Stuff was changed, so we return true.
        return true;
    }
    else
    {
        // Refresh the root dialog
        myRootDialog.synchronizeDisplay();

        // Set the project file to the one we just opened
        setProjectFile(file);

        //// Close the file
        //source.close();

        // Success
        return true;
    }
}


//------------------------------------------------------------------------------------------------
// Name:  closeProject
// Desc:  Handles the actions that occur when the user desires to close the current project
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::closeProject()
{
    // Save changes to the current project
    if (false == promptSaveChanges()) return false;

    // Get rid of all of the editor dialogs
    closeAllWindows();

    // Clear the configuration
    myConfiguration.clear();

    // Reset the current project file
    setProjectFile("");

    // This project has not yet been modified
    myProjectChangedFlag = false;

    // Refresh the root dialog
    myRootDialog.synchronizeDisplay();

    // Old project was closed
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  saveProject
// Desc:  Saves the current file
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::saveProject()
{
    // If there is a project, save into that file
    if (projectIsOpen())
    {
        //// Open the project file
        //dc::dcFileStream destination;
        //if (!destination.open(myProjectFile, STREAM_OVERWRITE))
        //{
        //    // Problem...
        //    MessageBox(myWindows.back(), "The file you selected couldn't be opened for writing; "
        //                                  "it may be read-only or malformed.  Try picking another "
        //                                  "file", "Error", MB_OK);

        //    // File wasn't saved
        //    return false;
        //}

        // Change the path
        dc::dcFileName::setCurrentPathFromFileDirectory(myProjectFile);

        //// Save all of the data in this project
        //if (!myConfiguration.save(&destination))
        //{
        //    // Let the user know what happened
        //    MessageBox(myWindows.back(), "There was some sort of problem saving this project.  Try picking "
        //                                  "a different destination name.", "Error", MB_OK);
        //}
        //else
        //{
        //    // The project was saved, so reset the change flag
        //    myProjectChangedFlag = false;
        //}



        bool saved = false;
        dc::dcResourceStorage2 storage;
        if (storage.openFile(myProjectFile, "pv")) {
          saved = storage.save(NULL, &myConfiguration);
          storage.close();
        }

        if (saved) {
          myProjectChangedFlag = false;
        } else {
          MessageBox(myWindows.back(), "There was some sort of problem saving this project.  Try picking "
                                        "a different destination name.", "Error", MB_OK);
        }

        // Close the stream
        //destination.close();

        // Project was saved
        return true;
    }
    else
    {
        // Get the user to pick where to save the project
        return saveProjectAs();
    }
}


//------------------------------------------------------------------------------------------------
// Name:  saveProjectAs
// Desc:  Asks the user where to save the current file
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::saveProjectAs()
{
    // The string where the file name will be stored
    char file[512];

    // Ask the user where to save the file
    if (dcxSaveFileDialog(myWindows.back(), "Save File As", PV_FILE_FILTER, file, sizeof(file)))
    {
        // Make sure the extension is correct
        int len = (int)strlen(file);
        if (0 != _stricmp(file + len - 4, PV_FILE_EXTENSION))
            strcat_s(file, sizeof(file), PV_FILE_EXTENSION);

        // Set the current project file
        setProjectFile(file);

        // Save the project
        return saveProject();
    }
    else
        // User canceled the dialog
        return false;
}



//------------------------------------------------------------------------------------------------
// Name:  createResourcePack
// Desc:  Creates a resource pack from the current project
// TODO:  add a pack name in front of each of the resources in the pack to identify them;
//        otherwise, resource conflicts and name changes could occur
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::createResourcePack()
{
    // Save changes to the current project
    if (false == promptSaveChanges()) return false;

    // The file being opened
    char file[512];

    // Ask the user to pick a file
    if (false == dcx::dcxSaveFileDialog(myWindows.back(), "Select Pack File", PV_RESOURCEPACK_FILE_FILTER, file, sizeof(file)))
        return false;

    // Make sure the extension is correct
    int len = (int)strlen(file);
    if (0 != _stricmp(file + len - 4, PV_RESOURCEPACK_FILE_EXTENSION))
        strcat_s(file, sizeof(file), PV_RESOURCEPACK_FILE_EXTENSION);

    // Ask the user to name this resource pack
    std::string packName = "";
    if (false == dcx::dcxWin32PromptTextInput(myWindows.back(), "Create Resource Pack",
        "Enter a name for this resource pack.  This name should be as unique.  Anything beyond 31 characters "\
        "will be truncated",
        &packName, false, false, true, false))
        return false;

    // Change the path so that file names get saved correctly
    dc::dcFileName::setCurrentPathFromFileDirectory(file);

    // The stream with which to export
    dc::dcFileStream packStream;
    if (!packStream.open(file, STREAM_OVERWRITE))
    {
        // Let the user know about the error
        MessageBox(myWindows.back(), "Destination resource pack file could not be opened for writing", "Error", MB_OK);

        // Nothing happened
        return false;
    }

    // Write a header into the stream to hold the place of the
    // real one, when we need to write it
    ResourcePack::FileHeader header;
    ZeroMemory(&header, sizeof(header));
    strncpy_s(header.PackName, sizeof(header.PackName), packName.c_str(), 31);
    packStream.write(&header, sizeof(header));

    // Assign the pack name by reading it back from the header (this ensures the 31 character limit)
    packName = header.PackName;

    // Copy the current project into the pack project
    ProjectVolucris packProject;
    {
        // Generate a temporary file name
        strcat_s(file, sizeof(file), "~tmp");

        // Create a temporary file
        dc::dcFileStream temporaryStream;
        if (!temporaryStream.open(file, STREAM_OVERWRITE))
        {
            // Close the pack stream
            packStream.close();

            // Let the user know about the error
            MessageBox(myWindows.back(), "Temporary resource pack file could not be opened for writing", "Error", MB_OK);

            // Nothing happened
            return false;
        }

        // Save the project into the temporary file
        myConfiguration.save(&temporaryStream);

        // Move back to the beginning of the file
        temporaryStream.seek(0);

        // Load another version of the project into a second project file
        if (!packProject.load(&temporaryStream))
        {
            // Close the streams
            temporaryStream.close();
            packStream.close();

            // Error
            MessageBox(myWindows.back(), "Unable to compile pack file", "Error", MB_OK);

            // Failure
            return false;
        }

        // Clear the types that are going to be included
        // in this file from this project, so they aren't duplicated
        packProject.getImageTable()->clear();
        packProject.getMeshTable()->clear();

        // Close the temp stream
        temporaryStream.close();
    }

    // Create the resource pack in the pack project
    dc::dcTable<ResourcePack>::Element* packProjectResourcePackElement =
        (dcTable<ResourcePack>::Element*)packProject.getResourcePackTable()->create<ResourcePack>(packName)->getOwner();

    // Create the index array by calculating the maximum number of resource indices we'll need
    size_t resourceIndexArraySize = myConfiguration.getImageTable()->getMembers().size() +
                                    myConfiguration.getMeshTable()->getMembers().size();
    ResourcePack::FileResourceIndexElement* resourceIndexArray =
        new ResourcePack::FileResourceIndexElement[resourceIndexArraySize];

    // Save the array into the stream to save a place for it
    if (APP_ERROR(!packStream.write(resourceIndexArray, sizeof(ResourcePack::FileResourceIndexElement) * resourceIndexArraySize))
        ("Unable to compile pack file"))
    {
        packProject.clear();
        packStream.close();
        SAFE_DELETE_ARRAY(resourceIndexArray);
        return false;
    }

    // How many bytes have been written into the pack stream so far
    size_t bytesWritten = packStream.tell();
    int resourceIndex = 0;

    // Load each of the images from the main project and created packed versions
    // in the pack project
    dc::dcTable<Image>* imageTable = myConfiguration.getImageTable();
    dc::dcTable<Image>* packImageTable = packProject.getImageTable();
    const dc::dcGenericResource::Array& imageMembers = imageTable->getMembers();
    dc::dcGenericResource::Array::const_iterator i = imageMembers.begin(), end = imageMembers.end();
    for (; i != end; ++i)
    {
        // Get a pointer to the image element
        dc::dcTable<Image>::Element* element = (dcTable<Image>::Element*)(*i);
        const Image* image = element->getImplementation();

        // Storage location for the loaded texture
        LPDIRECT3DTEXTURE9 d3dTexture = NULL;

        // Get a texture in the managed resource pool without a mipmap chain.  The mipmap chain
        // is unnecessary because we only care about the root image; the system memory pool is used
        // because it is fast and we don't need to ever actually *use* this texture in a D3D call.
        if (image && image->obtainD3DTexture(myD3DDevice, D3DTEX_SYSTEMMEM, &d3dTexture))
        {
            // Variables used while saving the image
            LPD3DXBUFFER pSaveBuffer = NULL;
            bool saveFailed = true, writeFailed = true;
            size_t byteSize = 0;

            // Save the texture into a buffer
            saveFailed = FAILED(D3DXSaveTextureToFileInMemory(&pSaveBuffer, D3DXIFF_PNG, d3dTexture, NULL));

            // Write the contents of the save buffer
            if (!saveFailed)
            {
                // Obtain the byte size of this buffer
                byteSize = pSaveBuffer->GetBufferSize();

                // Write the buffer into the stream
                writeFailed = !packStream.write(pSaveBuffer->GetBufferPointer(), byteSize);
            }

            // Unload any references we obtained
            SAFE_RELEASE(pSaveBuffer);
            SAFE_RELEASE(d3dTexture);

            // If we failed to write the image, exit before saving its resource index
            if (!APP_ERROR(saveFailed || writeFailed)("Unable to save image %s", element->getName().c_str()))
            {
                // Save the byte offset and resource size into the index
                resourceIndexArray[resourceIndex].ResourceLocation = bytesWritten;
                resourceIndexArray[resourceIndex].ResourceSize = byteSize;

                // Create a new entry in the table for this image in the resource pack project
                PackImage* packImage = packImageTable->create<PackImage>(element->getName());
                packImage->setResourceIndex(resourceIndex);
                packImage->getResourcePack()->setReferencedResource(packProjectResourcePackElement);

                // Increment the resource index and byte accumulator
                resourceIndex++;
                bytesWritten += byteSize;
            }
        }
        else
            DEBUG_WARNING("Ignoring image %s; it couldn't be loaded", element->getName().c_str());
    }

    // Load each of the meshes from the main project and create packed
    // versions in the pack project
    dc::dcTable<Mesh>* meshTable = myConfiguration.getMeshTable();
    dc::dcTable<Mesh>* packMeshTable = packProject.getMeshTable();
    const dc::dcGenericResource::Array& meshMembers = meshTable->getMembers();
    i = meshMembers.begin(), end = meshMembers.end();
    for (; i != end; ++i)
    {
        // Get a pointer to the image element
        dc::dcTable<Mesh>::Element* element = (dcTable<Mesh>::Element*)(*i);
        const Mesh* mesh = element->getImplementation();

        // Storage location for the loaded mesh
        SubsetGeometry subsetGeometry;

        // Get the mesh
        if (mesh && mesh->obtainGeometry(myD3DDevice, &subsetGeometry))
        {
            // Where the writing began
            size_t startLocation = packStream.tell();

            // Write the number of subsets
            size_t subsetCount = subsetGeometry.size();
            if (!packStream.write(&subsetCount, sizeof(subsetCount)))
                goto FINISHED_WRITING_MESH;

            // Save each geometry intstance into the buffer
            for (SubsetGeometry::const_iterator i = subsetGeometry.begin(); i != subsetGeometry.end(); ++i)
            {
                // Save the subset index
                if (!packStream.write(&i->first, sizeof(size_t))) goto FINISHED_WRITING_MESH;

                // Save the geometry for this subset
                if (!packStream.write(&i->second->NumVertices, sizeof(DWORD)) ||
                    !packStream.write(&i->second->NumIndices, sizeof(DWORD)) ||
                    ((i->second->NumVertices > 0) && !packStream.write(i->second->pVertices, sizeof(GeometryVertex) * i->second->NumVertices)) ||
                    ((i->second->NumIndices > 0) && !packStream.write(i->second->pIndices, sizeof(GeometryIndex) * i->second->NumIndices)))
                {
                    goto FINISHED_WRITING_MESH;
                }
            }

            // Calculate the size of this entry
            size_t byteSize = packStream.tell() - startLocation;

            // Save the byte offset and resource size into the index
            resourceIndexArray[resourceIndex].ResourceLocation = bytesWritten;
            resourceIndexArray[resourceIndex].ResourceSize = byteSize;

            // Create a new entry in the table for this image in the resource pack project
            PackMesh* packMesh = packMeshTable->create<PackMesh>(element->getName());
            packMesh->setResourceIndex(resourceIndex);
            packMesh->getResourcePack()->setReferencedResource(packProjectResourcePackElement);

            // Increment the resource index and byte accumulator
            resourceIndex++;
            bytesWritten += byteSize;

FINISHED_WRITING_MESH:

            // Erase the geometry that was loaded
            DeallocateGeometry(&subsetGeometry);
        }
        else
            DEBUG_WARNING("Ignoring mesh %s because it couldn't be loaded", mesh->getName().c_str());
    }

    // Remove the resource pack from the pack project, along with any
    // other resource packs.  Those resource packs will be wrapped up
    // into this pack, so it's not neceessary to keep them.
    packProject.getResourcePackTable()->clear();

    // Generate the resource pack header
    header.MagicNumber = ResourcePack::MAGIC_NUMBER;
    header.ProjectLocation = packStream.tell();
    header.ResourceCount = resourceIndex;

    // Save the project using a resource trader so that we can import it later.  This makes sure that
    // the resource packs don't go kablooey between versions.
    dc::dcResourceTrader exporter;
    exporter.exportTo(&packProject, &packStream);

    // Save the header (back at the start of the file)
    packStream.seek(0);
    packStream.write(&header, sizeof(header));

    // Save the pack indices, which are now filled with correct data
    packStream.write(resourceIndexArray, sizeof(ResourcePack::FileResourceIndexElement) * resourceIndexArraySize);

    // Clean up memory
    SAFE_DELETE_ARRAY(resourceIndexArray);
    packStream.close();

    // Success
    MessageBox(myWindows.back(), "Successfully compiled pack file!", "Project Volucris", MB_OK);

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  importProject
// Desc:  Runs the export algorithm to convert this project into the UDT format
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::importProject()
{
    // Save changes to the current project
    if (false == promptSaveChanges()) return false;

    // The file being opened
    char file[512];

    // Ask the user to pick a file
    if (false == dcx::dcxOpenFileDialog(myWindows.back(), "Import File", PV_EXPORT_FILE_FILTER, file, sizeof(file)))
        return false;

    // Open the import file
    dc::dcFileStream stream;
    if (!stream.open(file, 0))
    {
        // Error dialog box
        MessageBox(myWindows.back(), "The source file couldn't be opened.", "Error", MB_OK);

        // Didn't save the file
        return false;
    }

    // Get rid of all of the editor dialogs
    closeAllWindows();

    // Reset the current project file
    setProjectFile("");

    // Clear the configuration
    myConfiguration.clear();

    // Change the path
    dc::dcFileName::setCurrentPathFromFileDirectory(file);

    // Load the file
    dc::dcResourceTrader trader;
    bool importSucceeded = trader.importFrom(&myConfiguration, &stream);

    // Close the file
    stream.close();

    // This project has been modified
    myProjectChangedFlag = true;

    // Let the user know what happened
    if (importSucceeded)
        MessageBox(myWindows.back(), "The project imported successfully!  See the log for more information.", "Import Succeeded", MB_OK);
    else
        MessageBox(myWindows.back(), "The project failed to import correctly.  See the log for more information.", "Import Failed", MB_OK);

    // Create an output log
    std::string logFile;
    trader.generateLogFile(&logFile);

    // Open up Notepad to display the log
    dcxWin32OpenTextEditor(logFile.c_str());

    // The configuration changed
    myProjectChangedFlag = true;

    // Successfully imported
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  exportProject
// Desc:  Runs the export algorithm to convert this project into the UDT format
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::exportProject()
{
    // The string where the file name will be stored
    char file[512];

    // Ask the user where to save the file
    if (false == dcxSaveFileDialog(myWindows.back(), "Export File", PV_EXPORT_FILE_FILTER, file, sizeof(file)))
        return false;

    // Make sure the extension is correct
    int len = (int)strlen(file);
    if (0 != _stricmp(file + len - 4, PV_EXPORT_FILE_EXTENSION))
        strcat_s(file, sizeof(file), PV_EXPORT_FILE_EXTENSION);

    // The stream with which to export
    dc::dcFileStream stream;
    if (!stream.open(file, STREAM_OVERWRITE))
    {
        // Let the user know about the error
        MessageBox(myWindows.back(), "Destination file could not be opened for writing", "Error", MB_OK);

        // Nothing happened
        return false;
    }

    // Change the path so that file names get saved correctly
    dc::dcFileName::setCurrentPathFromFileDirectory(file);

    // Run the exporter
    dc::dcResourceTrader trader;
    bool exportSucceeded = trader.exportTo(&myConfiguration, &stream);

    // Close the stream
    stream.close();

    // Let the user know what happened
    if (exportSucceeded)
        MessageBox(myWindows.back(), "The project exported successfully!  See the log for more information.", "Export Succeeded", MB_OK);
    else
        MessageBox(myWindows.back(), "The project failed to export correctly.  See the log for more information.", "Export Failed", MB_OK);

    // Create an output log
    std::string logFile;
    trader.generateLogFile(&logFile);

    // Open up Notepad to display the log
    dcxWin32OpenTextEditor(logFile.c_str());

    // Something was exported (even if it was unsuccessful)
    return true;

}




//------------------------------------------------------------------------------------------------
// Name:  mergeProject
// Desc:  Opens another project and merges it with this one
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::mergeProject()
{
    // Save changes to the current project
    if (false == promptSaveChanges()) return false;

    // The file being opened
    char file[512];

    // Ask the user to pick a file
    if (false == dcx::dcxOpenFileDialog(myWindows.back(), "Select Merge Source", PV_FILE_FILTER, file, sizeof(file)))
        return false;

    // Get rid of all of the editor dialogs
    closeAllWindows();

    // Open the file
    dc::dcFileStream stream;
    if (!stream.open(file, 0))
    {
        // Error dialog box
        MessageBox(myWindows.back(), "The source merge file couldn't be opened.", "Error", MB_OK);

        // Didn't merge
        return false;
    }

    // Change the path
    dc::dcFileName::setCurrentPathFromFileDirectory(file);

    // Create a temporary instance of the editor's data
    ProjectVolucris temporary;
    if (!temporary.load(&stream))
    {
        // Give the user an error.
        // todo: should we ASK whether or not to merge?
        MessageBox(myWindows.back(), "There was a problem loading this project; it will not be"\
                                      "merged with the current configuration.", "Error", MB_OK);

        // Nothing changed
        return false;
    }

    // Close the stream
    stream.close();

    // Merge using the tool
    dc::dcResourceTrader mergeTool;
    mergeTool.merge(&myConfiguration, &temporary, 0, 0);

    // Change the path back
    dc::dcFileName::setCurrentPathFromFileDirectory(myProjectFile);

    // The configuration changed
    myProjectChangedFlag = true;

    // Success
    return true;
}


#include "../common/lzf/lzf.h"

bool LZFFileRewrite(const char* file_original,
                    const char* file_compressed) {
  FILE* pf_;
  if (0 != fopen_s(&pf_, file_original, "rb")) return false;
  fseek(pf_, 0, SEEK_END);
  size_t length = ftell(pf_);
  char* input_buffer = new char[length];
  char* output_buffer = new char[length];
  fseek(pf_, 0, SEEK_SET);
  fread(input_buffer, sizeof(char), length, pf_);
  fclose(pf_);
  unsigned int output_length = lzf_compress(input_buffer, length, output_buffer, length);
  delete[] input_buffer;
  if (0 != fopen_s(&pf_, file_compressed, "w+b")) {
    delete[] output_buffer;
    return false;
  }
  fwrite(output_buffer, sizeof(char), output_length, pf_);
  fclose(pf_);
  delete[] output_buffer;
  return true;
}









//------------------------------------------------------------------------------------------------
// Name:  compileProject
// Desc:  Lets the user compile this project file and output it
//------------------------------------------------------------------------------------------------
void ProjectVolucrisEditor::compileProject()
{
  // The string where the file name will be stored
  char clientFile[512], serverFile[512];

  // Ask the user where to save the file
  if (false == dcxSaveFileDialog(myWindows.back(), "Compile Project", PV_CLIENT_COMPILE_FILE_EXTENSION, clientFile, sizeof(clientFile)))
      return;

  // Make sure the extension is correct
  int len = (int)strlen(clientFile);
  if (0 != _stricmp(clientFile + len - 4, PV_CLIENT_COMPILE_FILE_EXTENSION))
      strcat_s(clientFile, sizeof(clientFile), PV_CLIENT_COMPILE_FILE_EXTENSION );

  // Copy to the server file and swap the extension
  strcpy_s(serverFile, sizeof(serverFile), clientFile);
  size_t serverFileLength = strlen(serverFile);
  strcpy_s(serverFile + serverFileLength - 4, sizeof(serverFile) - serverFileLength + 4, PV_SERVER_COMPILE_FILE_EXTENSION);

  // The stream with which to export
  dc::dcFileStream clientStream;
  if (!clientStream.open(clientFile, STREAM_OVERWRITE))
  {
      // Let the user know about the error
      MessageBox(myWindows.back(), "Destination client file could not be opened for writing", "Error", MB_OK);

      // Nothing happened
      return;
  }

  // The stream with which to export to the server
  dc::dcFileStream serverStream;
  if (!serverStream.open(serverFile, STREAM_OVERWRITE))
  {
      // Let the user know about the error
      MessageBox(myWindows.back(), "Destination server file could not be opened for writing", "Error", MB_OK);

      // Close the client file
      clientStream.close();

      // Nothing happened
      return;
  }

  bool failedToCompile = false;
  { // Compile the project
    //dc::dcBufferStream client_buffer_stream, server_buffer_stream;
    //failedToCompile = APP_WARNING(!CompileProject(&myConfiguration,
    //                                              &client_buffer_stream,
    //                                              &server_buffer_stream))("Couldn't compile project files");
    failedToCompile = APP_WARNING(!CompileProject(&myConfiguration,
                                                  &clientStream,
                                                  &serverStream))("Couldn't compile project files");

    //// Copy the compiled data to disk in chunks
    //static const int CHUNK_SIZE = 4096;
    //char chunk_buffer[CHUNK_SIZE];
    //{
    //  client_buffer_stream.seek(0);
    //  for (size_t bytes_left = client_buffer_stream.tell();
    //       bytes_left > 0;) {
    //    if (bytes_left < CHUNK_SIZE) {
    //      CONFIRM(client_buffer_stream.read(chunk_buffer, bytes_left)) else break;
    //      CONFIRM(clientStream.write(chunk_buffer, bytes_left)) else break;
    //      bytes_left = 0;
    //    } else {
    //      CONFIRM(client_buffer_stream.read(chunk_buffer, CHUNK_SIZE)) else break;
    //      CONFIRM(clientStream.write(chunk_buffer, CHUNK_SIZE)) else break;
    //      bytes_left -= CHUNK_SIZE;
    //    }
    //  }
    //}
    //{
    //  server_buffer_stream.seek(0);
    //  for (size_t bytes_left = server_buffer_stream.tell();
    //       bytes_left > 0;) {
    //    if (bytes_left < CHUNK_SIZE) {
    //      CONFIRM(server_buffer_stream.read(chunk_buffer, bytes_left)) else break;
    //      CONFIRM(serverStream.write(chunk_buffer, bytes_left)) else break;
    //      bytes_left = 0;
    //    } else {
    //      CONFIRM(server_buffer_stream.read(chunk_buffer, CHUNK_SIZE)) else break;
    //      CONFIRM(serverStream.write(chunk_buffer, CHUNK_SIZE)) else break;
    //      bytes_left -= CHUNK_SIZE;
    //    }
    //  }
    //}
  }

  // Close the streams
  clientStream.close();
  serverStream.close();

  // Compress the output stream
  LZFFileRewrite(clientFile,
                 clientFile);

  // Let the user know about what happened
  MessageBox(myWindows.back(),
      failedToCompile ? "The project failed to compile" : "Successfully compiled project", "Error", MB_OK);
}



//------------------------------------------------------------------------------------------------
// Name:  getRootResource
// Desc:  Gets the root DirectCraft resource
//------------------------------------------------------------------------------------------------
ProjectVolucris* ProjectVolucrisEditor::getRootResource() const
{
    return (ProjectVolucris*)&myConfiguration;
}



//------------------------------------------------------------------------------------------------
// Name:  getD3DDevice
// Desc:  Get the device
//------------------------------------------------------------------------------------------------
LPDIRECT3DDEVICE9 ProjectVolucrisEditor::getD3DDevice()
{
    return myD3DDevice;
}


//------------------------------------------------------------------------------------------------
// Name:  projectChanged
// Desc:  Lets the editor know that something about the project has changed
//------------------------------------------------------------------------------------------------
void ProjectVolucrisEditor::projectChanged()
{
    myProjectChangedFlag = true;
}


//------------------------------------------------------------------------------------------------
// Name:  promptSaveChanges
// Desc:  Asks the user to save changes to the current project if it has been changed
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::promptSaveChanges()
{
    if (myProjectChangedFlag)
    {
        // Ask the user what to do
        int r = dcxYesNoCancelDialog(myWindows.back(), "Volucris Game Editor",
                                      "Save changes to current project?");

        // Handle the user's response
        switch(r)
        {
            case IDYES: return saveProject();
            case IDNO:  return true;
            default:    return false;
        }
    }
    else
        return true;
}


//------------------------------------------------------------------------------------------------
// Name:  setProjectFile
// Desc:  Sets the current project file name
//------------------------------------------------------------------------------------------------
void ProjectVolucrisEditor::closeAllWindows()
{
    // Start with the second window in the list
    std::list<HWND>::reverse_iterator i = myWindows.rbegin();
    std::list<HWND>::reverse_iterator j = i;
    ++j;

    // Get rid of all of the windows until j == end.  This will prevent the root window from
    // being closed.
    for (; j != myWindows.rend(); ++i, ++j)
        EndDialog(*i, 0);
}


//------------------------------------------------------------------------------------------------
// Name:  setProjectFile
// Desc:  Sets the current project file name
//------------------------------------------------------------------------------------------------
void ProjectVolucrisEditor::setProjectFile(const char* file)
{
    dc::dcFileName::setCurrentPathFromFileDirectory(file);
    strcpy_s(myProjectFile, sizeof(myProjectFile), file);
}


//------------------------------------------------------------------------------------------------
// Name:  projectIsOpen
// Desc:  Determines whether or not a project is currently open
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::projectIsOpen()
{
    return myProjectFile[0] != '\0';
}



//------------------------------------------------------------------------------------------------
// Name:  initializeD3DObjects
// Desc:  Initialize the Direct3D objects
//------------------------------------------------------------------------------------------------
bool ProjectVolucrisEditor::initializeD3DObjects()
{
    HRESULT hr;

    // If we've already created, succeed!
    if (myD3D) return true;

    // Create the D3D object.
    if (NULL == (myD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
        MessageBox(GetDesktopWindow(), "Unable to initialize the Direct3D object.  Make sure the latest version "
                                        "of DirectX is installed (microsoft.com/DirectX)", "Error", MB_OK);
		return false;
	}

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    //d3dpp.BackBufferWidth = 800; // We have to make the buffer big enough for the largest window we want to accomodate
    //d3dpp.BackBufferHeight = 600;
    //d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if (/*DX_FAILED(myD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                         D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                        &d3dpp, &myD3DDevice)) &&*/
        FAILED(myD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, myWindows.front(),
                                         D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                        &d3dpp, &myD3DDevice)))
    {
        // Get rid of the main D3D object
        myD3D->Release();

        // Return the result code
        return false;
    }

    // Generate an external copy of the device
//    s_pd3dDevice = myD3DDevice;
#define DX_CHECK(x) if (FAILED(hr = (x))) return false;
    // Save the creation parameters
    memcpy(&myD3DParams, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));

    // Turn on the zbuffer
    DX_CHECK(myD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE));

    // Turn on ambient lighting 
    DX_CHECK(myD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff));

    // Set up the texture blending states.  This isn't mission-critical so
    // if there they fail, we can still move on.  The game might look a bit weird
    // without correct alphatesting, though.
    //DX_CHECK(myD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
    //DX_CHECK(myD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
    //DX_CHECK(myD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));
    DX_CHECK(myD3DDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x08));
    DX_CHECK(myD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL));
    DX_CHECK(myD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
    DX_CHECK(myD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

    // Create the vertex declaration
    if (FAILED(myD3DDevice->CreateVertexDeclaration(MeshVertexDeclaration, &myMeshVertexDeclaration)))
    {
        // Get rid of objects
        myD3DDevice->Release();
        myD3D->Release();

        // Return the error
        return false;// APP_ERROR("Failed to create animated mesh vertex declaration", E_FAIL);
    }

    // Try to compile our VS
    LPD3DXBUFFER pAssembledShader = NULL, pErrors = NULL;
    DX_CHECK(D3DXAssembleShader(MDJANIMATEDMESH_VERTEXSHADER, sizeof(MDJANIMATEDMESH_VERTEXSHADER) - 1,
                                  NULL, NULL, 0, &pAssembledShader, &pErrors));

    // Check to see if anything went wrong
    if (pErrors)
    {
        // Output the errors
        //DEBUG_MSG("Problem compiling vertex shader: %s\n", (LPCSTR)pErrors->GetBufferPointer());

        // Release the errors structure
        pErrors->Release();
    }

    // If we failed to make the vertex shader, return the error
    if (FAILED(hr))
    {
        // Get rid of all of the created objects
        myMeshVertexDeclaration->Release();
        myD3DDevice->Release();
        myD3D->Release();

        // Pass the error
        return false;//APP_ERROR("Failed to compile the vertex shader", E_FAIL);
    }

    // If the shader was assembled, create it
    if (pAssembledShader)
    {
        // Create the vertex shader
        if (FAILED(myD3DDevice->CreateVertexShader((DWORD*)pAssembledShader->GetBufferPointer(), &myMeshVertexShader)))
        {
            // Get rid of all of the created objects
            pAssembledShader->Release();
            myMeshVertexDeclaration->Release();
            myD3DDevice->Release();
            myD3D->Release();

            // Return the error
            return false;//APP_ERROR("Couldn't create animated mesh vertex shader", E_FAIL);
        }

        // Release the assembled shader
        pAssembledShader->Release();
    }

    // Check to find the maximum number of bones that can be drawn simultaneously
    D3DCAPS9 d3dCaps;
    myD3DDevice->GetDeviceCaps(&d3dCaps);
    myMaxBonesPerSet = (d3dCaps.MaxVertexShaderConst - 16) / 4;

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  releaseD3DObjects
// Desc:  Erases the Direct3D objects
//------------------------------------------------------------------------------------------------
void ProjectVolucrisEditor::releaseD3DObjects()
{
    // Only release if there are no more windows
    if (!myWindows.empty()) return;

    if (myMeshVertexShader != NULL)
    {
        myMeshVertexShader->Release();
        myMeshVertexShader = NULL;
    }

    if (myMeshVertexDeclaration != NULL)
    {
        myMeshVertexDeclaration->Release();
        myMeshVertexDeclaration = NULL;
    }

    if (myD3DDevice != NULL)
    {
        myD3DDevice->Release();
        myD3DDevice = NULL;
    }

    if (myD3D != NULL)
    {
        myD3D->Release();
        myD3D = NULL;
    }
}