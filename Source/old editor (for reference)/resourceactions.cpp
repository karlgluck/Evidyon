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
#include "../shared/evidyon_gamefile.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include "rescalemeshesdialog.h"
#include "resource"
#include <dc/bufferstream>
#include <dc/dcresourcetrader.h>
#include "compositeimage.h"


#include "maplink.h"
#include "normalmeleemonsterlifeformai.h"
#include "interactivenpclifeformai.h"
#include "swarmspawner.h"

#include "visualfx_swirlingparticles.h"
#include "visualfx_beam.h"

#include <dc/filestream>


//------------------------------------------------------------------------------------------------
// Name:  PromptSelectSingleResource
// Desc:  Allows the user to pick a single resource from a list and returns the result
//------------------------------------------------------------------------------------------------
bool PromptSelectSingleResource(const char* caption, const char* prompt,
                                 const dc::dcGenericResource::Array& resourceOptions,
                                 dc::dcGenericResource** returnedSelection)
{
    // Make sure the parameters are valid
    if (APP_ERROR(!caption || !prompt || !returnedSelection)("Invalid parameter to PromptSelectSingleResource"))
        return false;

    // Reset the selection
    *returnedSelection = 0;

    // Holds the end of the resource options list
    dc::dcGenericResource::Array::const_iterator end = resourceOptions.end();

    // Add options to the list
    std::list<std::string> selectionOptions;
    for (dcGenericResource::Array::const_iterator i = resourceOptions.begin();
         i != end; ++i)
    {
        selectionOptions.push_back((*i)->getName());
    }

    // Ask the user to pick a resource
    int selection;
    if (!dcxWin32PromptForSelection(globalEditor()->getTopmostWindowHandle(),
                                     caption, prompt, &selectionOptions, &selection))
        return false;

    // Set the returned resource
    *returnedSelection = resourceOptions.at(selection);

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  PromptSelectMultipleResources
// Desc:  Allows the user to pick several resources from a list and returns the result
//------------------------------------------------------------------------------------------------
bool PromptSelectMultipleResources(const char* caption, const char* prompt,
                                    const dc::dcGenericResource::Array& resourceOptions,
                                          dc::dcGenericResource::Array* resourceSelection,
                                    bool initializeSelection)
{
    // Holds the end of the resource options list
    dc::dcGenericResource::Array::const_iterator end = resourceOptions.end();

    // The indices of the entries the user picked
    std::list<int> selection;

    // Add options to the list
    std::list<std::string> selectionOptions;
    int index = 0;
    for (dcGenericResource::Array::const_iterator i = resourceOptions.begin();
         i != end; ++i, ++index)
    {
        // Get the resource from the options list
        dc::dcGenericResource* resource = (*i);

        // Add this resource's name
        selectionOptions.push_back((*i)->getName());

        // If necessary, see if this entry is in the current selection
        if (initializeSelection)
        {
            dc::dcGenericResource::Array::const_iterator selectionEnd = resourceSelection->end();
            for (dcGenericResource::Array::const_iterator j = resourceSelection->begin();
                 j != selectionEnd; ++j)
            {
                 if ((*j) == resource) { selection.push_back(index); break; }
            }
        }
    }

    // Reset the returned selection list
    resourceSelection->clear();

    // Ask the user to pick some resources
    if (!dcxWin32PromptForSelection(globalEditor()->getTopmostWindowHandle(),
                                     caption, prompt, &selectionOptions, &selection, initializeSelection))
        return false;

    // Build a list of the resource pointers
    for (std::list<int>::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Add this entry to the returned selection
        resourceSelection->push_back(resourceOptions.at(*i));
    }

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  EditResourceName
// Desc:  Lets the user change the name of a resource
//------------------------------------------------------------------------------------------------
int EditResourceName(dcGenericResource* resource, void* pointer)
{
    // Get the current name
    std::string name = resource->getName();

    // Prompt the resource's current name
    if (dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Edit Name",
            "Please enter a new name for this resource:", &name))
    {
        // Assign the new name
        if (!resource->setName(name))
        {
            // Display a message to the user
            MessageBox(globalEditor()->getTopmostWindowHandle(),
                        "Edit Name - Failed",
                        "A resource with that name already exists.", MB_OK|MB_ICONEXCLAMATION);

            // Nothing happened
            return 0;
        }
        else
            return 1; // Something changed
    }
    else
        return 0; // Nothing
}

void ApplyBulkRenamePattern(std::string* name, const std::string& nameTemplate, const std::string& oldName, size_t index)
{
    // Create an index string
    char indexString[32];
    sprintf_s(indexString, sizeof(indexString), "%lu", index);
    size_t indexStringLen = strlen(indexString);

    // Initialize the resource name
    name->assign(nameTemplate);

    // Save the index
    for (size_t offset = 0; offset < name->npos;)
    {
        offset = name->find("##", offset);
        if (offset < name->npos)
        {
            // Replace this token
            name->replace(offset, 2, indexString);

            // Advance the offset
            offset += indexStringLen;
        }
    }

    // Save the name
    for (size_t offset = 0; offset < name->npos;)
    {
        offset = name->find("$$", offset);
        if (offset < name->npos)
        {
            // Replace this token with the old name
            name->replace(offset, 2, oldName);

            // Advance the offset
            offset += oldName.length();
        }
    }
}

bool BulkRenameSelection(dcGenericResource::Array& selection, const std::string& nameTemplate, bool updateReferences)
{
    // Repeat for each of the elements
    size_t index = 0;
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i, ++index)
    {
        // Get the resource reference
        dc::dcGenericResource* resource = *i;

        std::string name;
        ApplyBulkRenamePattern(&name, nameTemplate, resource->getName(), index);

        // Change the resource's name
        resource->setName(name, updateReferences);
    }

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  BulkRename
// Desc:  Lets the user change the name of many resources in a collection at once
//------------------------------------------------------------------------------------------------
int BulkRename(dcGenericResource* resource, void* pointer)
{
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Bulk Rename", "Select resources to rename",
            resource->getMembers(), &selection))
        return 0;

// If the user enters a malformed template, we jump back here
LBL_ENTERNAMETEMPLATE:

    // Enter the new name
    std::string nameTemplate;
    if (!dcx::dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Bulk Rename",
            "Enter the name template for the selected resources, using ## to indicate\n"
            "where to place the number, and $$ to notate the resource's old name", &nameTemplate))
        return false;

    // Check to make sure the name template has some sort of key in it
    if (nameTemplate.find("##") >= nameTemplate.npos && nameTemplate.find("$$") >= nameTemplate.npos)
    {
        // Let the user know what's up
        MessageBox(globalEditor()->getTopmostWindowHandle(), "The name template must contain either ## or $$", "Bulk Rename - Error", MB_OK|MB_ICONEXCLAMATION);

        // Yep, it's a goto.  Sorry kids.
        goto LBL_ENTERNAMETEMPLATE;
    }

    // Find out whether or not we should update references
    bool updateReferences = true;
    /*IDYES == MessageBox(globalEditor()->getTopmostWindowHandle(),
        "Do you want to update references to the selected items?", "Bulk Rename", MB_YESNO);*/

    // Execute the renaming
    return BulkRenameSelection(selection, nameTemplate, updateReferences) ? 1 : 0;
}




//------------------------------------------------------------------------------------------------
// Name:  FindReplaceInNames
// Desc:  Lets the user find a string in the names of selected resources and change them.  This
//        method will work for any resource type, but it should ONLY be used for collections as
//        using it for interfaces/structures will result in undefined behaviour.
//------------------------------------------------------------------------------------------------
int FindReplaceInNames(dcGenericResource* resource, void* pointer)
{
    dc::dcGenericResource::Array selection;
    selection.assign(resource->getMembers().begin(), resource->getMembers().end());
    if (!PromptSelectMultipleResources("Find/Replace In Names", "Select resources whose names you want to modify:",
            resource->getMembers(), &selection))
        return 0;

    // Enter the find and replace strings
    std::string findString, replaceString;
    if (!dcx::dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Find/Replace In Names",
        "Enter the string to search for in the names:", &findString) ||
        !dcx::dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Find/Replace In Names",
        "Enter the replacement string for the text, if it's found:", &replaceString))
        return 0;

    // If the find string is empty, don't do anything
    if (findString.empty()) return 0;

    // Find out whether or not we should update references
    bool updateReferences = true;/*IDYES == MessageBox(globalEditor()->getTopmostWindowHandle(),
        "Do you want to update references to the selected items?", "Find/Replace In Names", MB_YESNO);*/

    // Repeat for each of the elements
    size_t index = 0;
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i, ++index)
    {
        // Get the resource reference
        dc::dcGenericResource* resource = *i;

        // Obtain a copy of the resource's name
        std::string name(resource->getName());

        // Look for the string in this name
        std::string::size_type location = name.find(findString);

        // Check to see if the string was found
        if (location != name.npos)
        {
            // Replace the string
            name.replace(location, findString.length(), replaceString);

            // Assign the new name
            resource->setName(name, updateReferences);
        }
    }

    // Success
    return 1;
}




//------------------------------------------------------------------------------------------------
// Name:  getDescendantsOfType
// Desc:  Fills the provided list with all of the descendants of the given type
//------------------------------------------------------------------------------------------------
void getDescendantsOfType(dcGenericResource* resource, unsigned long id, dc::dcGenericResource::Array* descendants)
{
    const dc::dcGenericResource::Array& members = resource->getMembers();

    for (dcGenericResource::Array::const_iterator i = members.begin(); i != members.end(); ++i)
    {
        // Check this member's type
        if ((*i)->getTypeID() == id ) descendants->push_back(*i);

        // Check the member for descendants
        getDescendantsOfType((*i), id, descendants);
    }
}





//------------------------------------------------------------------------------------------------
// Name:  
// Desc:  
//------------------------------------------------------------------------------------------------
int ToggleBooleanValue(dcGenericResource* resource, void* data)
{
    dc::dcBoolean* boolean = (dcBoolean*)resource;
    boolean->toggle();
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  
// Desc:  
//------------------------------------------------------------------------------------------------
template <typename T, typename Y> int AddNewTtoTableY(dcGenericResource* resource, void* data)
{
    // Get the table type
    dc::dcTable<Y>* table = (dcTable<Y>*)resource;

    // Ask the user for a name
    std::string name = "";
    if (dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(),  "Resource Name?", "Please enter a name for this resource:", &name))
    {
        // Create the resource
        dc::dcGenericResource* image = table->create<T>(name);

        // Edit this resource
        EditResourceDialog dialog;
        if (!dialog.execute(image))
        {
            table->releaseElement(image);
            return 0;
        }

        // Sort the table.  TODO: make this not necessary?
        table->sort();

        // Success!
        return 1;
    }

    return 0;
}






//------------------------------------------------------------------------------------------------
// Name:  SelectReference
// Desc:  Lets the user pick which resource a reference points to.  This will display a
//        list dialog with all of the possible options in the project, and select the
//        one that is currently referenced (if any).
//------------------------------------------------------------------------------------------------
template <typename T> int SelectReference(dcGenericResource* resource, void* data)
{
    // Get the reference type
    T* ref = (T*)resource;
    dc::dcGenericResource::Array referencableTypes;
    getDescendantsOfType(globalEditor()->getRootResource(), ref->getReferenceableTypeID(), &referencableTypes);

    // Add options to the list
    std::list<std::string> options;
    int selection = -1;

    // Initialize the selection and the list of options
    {
        // The resource currently referenced
        T::ResourceType* referencedResource = ref->getReferencedResource();

        // Go through the list of possible objects that can be referenced and add their names
        // to the list of string options.  Also, keep track of the index and determine which
        // is currently indexed.
        int index = 0;
        dc::dcGenericResource::Array::const_iterator end = referencableTypes.end();
        for (dcGenericResource::Array::const_iterator i = referencableTypes.begin();
             i != end; ++i, ++index)
        {
            if ((*i) == referencedResource)
                selection = index;
            options.push_back((*i)->getName());
        }
    }

    // Create a prompt
    std::string prompt = "Select a reference for " + resource->getName();

    // Ask the user to pick some resources.  Only initialize the selection if the
    // procedure above identified which resource is currently selected.
    if (dcxWin32PromptForSelection(globalEditor()->getTopmostWindowHandle(),
                                    "Select Resource Reference", prompt.c_str(), &options, &selection, selection > -1))
    {
        ref->setReferencedResource((T::ResourceType*)referencableTypes.at(selection));
        return 1;
    }

    // Nothing changed
    return 0;
}








//------------------------------------------------------------------------------------------------
// Name:  DeleteCollectionEntries
// Desc:  Lets the user pick entries from a collection to be deleted
//------------------------------------------------------------------------------------------------
int DeleteCollectionEntries(dcGenericResource* resource, void* data)
{
    dc::dcGenericCollection* collection = (dcGenericCollection*)resource;

    // Build a prompt
    std::string prompt = "Select entries to delete from " + resource->getName();

    // Add options to the list
    std::list<std::string> options;
    dc::dcGenericResource::Array::const_iterator end = resource->getMembers().end();
    for (dcGenericResource::Array::const_iterator i = resource->getMembers().begin();
         i != end; ++i)
    {
        options.push_back((*i)->getName());
    }

    // Ask the user to pick some resources
    std::list<int> selection;
    dcxWin32PromptForSelection(globalEditor()->getTopmostWindowHandle(),
        "Pick Entries", prompt.c_str(), &options, &selection);

    // Build a list of the resource pointers
    dc::dcGenericResource::Array resourceSelection;
    for (std::list<int>::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        dc::dcGenericResource::Array::const_iterator r = resource->getMembers().begin();

        // Add the entry with the given selection index.  Can't just use a lookup here
        // because this is a list...ugh.
        for (int j = 0; j < *i; ++j, ++r);
        resourceSelection.push_back(*r);
    }

    // Delete the resources.  We can't do this in a single stage (without looking
    // up all of the pointers before) because as soon as the collection is modified,
    // all of the indices change.  We can't delete by index anyway, so it's not a
    // big deal.
    for (dcGenericResource::Array::iterator res = resourceSelection.begin();
         res != resourceSelection.end(); ++res)
    {
        collection->releaseElement(*res);
    }

    return 1;
}



//------------------------------------------------------------------------------------------------
// Name:  EditFileName
// Desc:  Lets the user pick a file (generic, any kind of file)
//------------------------------------------------------------------------------------------------
int EditFileName(dcGenericResource* resource, void* data)
{
    // Get the file-name resource
    dc::dcFileName* fileName = (dcFileName*)resource;

    // Get the file
    char file[512];
    strcpy_s(file, sizeof(file), fileName->getValue().c_str());

    // Run the file dialog
    if (dcx::dcxOpenFileDialog(globalEditor()->getTopmostWindowHandle(),
                                "Select File", "All Files (*.*)\0*.*\0\0", file, sizeof(file)))
    {
        // Copy in the file name
        fileName->setValue(file);

        // Edited the file
        return 1;
    }
    else
        return 0; // Nothing was edited
}



//------------------------------------------------------------------------------------------------
// Name:  EditD3DXImageFile
// Desc:  Edit source file for a D3DXImage
//------------------------------------------------------------------------------------------------
int EditD3DXImageFile(dcGenericResource* resource, void* data)
{
    // Get the file-name resource
    D3DXImage* image = (D3DXImage*)resource;
    dc::dcFileName* fileName = image->getFileName();

    // Get the file
    char file[512];
    strcpy_s(file, sizeof(file), fileName->getValue().c_str());

    // Run the file dialog
    if (dcx::dcxOpenFileDialog(globalEditor()->getTopmostWindowHandle(),
                                "Select Image File",
                                "Image Files (*.JPG; *.JPEG; *.PNG; *.BMP; *.DDS)\0*.JPG;*.JPEG;*.PNG;*.BMP;*.DDS\0\0",
                                file, sizeof(file)))
    {
        // Copy in the file name
        fileName->setValue(file);

        // Edited the file
        return 1;
    }
    else
        return 0; // Nothing was edited
}



//------------------------------------------------------------------------------------------------
// Name:  EditColorKey
// Desc:  Brings up a color-picker to choose colors
//------------------------------------------------------------------------------------------------
int EditColorKey(dcGenericResource* resource, void* data)
{
    ColorKey* colorKey = (ColorKey*)resource;

    // The new color that is picked
    COLORREF chosenColor;

    // Ask the user for a color
    if (dcx::dcxChooseColorDialog(globalEditor()->getTopmostWindowHandle(), colorKey->getRGBColor(), &chosenColor))
    {
        // Set the new color
        colorKey->setRGBColor(chosenColor);

        // Make sure the key is enabled
        colorKey->enable(true);

        // Color key was edited
        return 1;
    }
    else
        return 0; // No editing
}



//------------------------------------------------------------------------------------------------
// Name:  EditD3DColor
// Desc:  Brings up a color-picker to choose colors
//------------------------------------------------------------------------------------------------
int EditD3DColor(dcGenericResource* resource, void* data)
{
    dcx::dcxD3DColor* color = (dcx::dcxD3DColor*)resource;

    // The new color that is picked
    COLORREF chosenColor;

    // Ask the user for a color
    if (dcx::dcxChooseColorDialog(globalEditor()->getTopmostWindowHandle(), 
                                  color->getRGBColor(),
                                 &chosenColor))
    {
        // Set the new color
        color->setRGBColor(chosenColor);

        // Color key was edited
        return 1;
    }
    else
        return 0; // No editing
}







//------------------------------------------------------------------------------------------------
// Name:  DisableColorKey
// Desc:  
//------------------------------------------------------------------------------------------------
int DisableColorKey(dcGenericResource* resource, void* data)
{
    ColorKey* colorKey = (ColorKey*)resource;
    colorKey->enable(false);
    return 1;
}







//------------------------------------------------------------------------------------------------
// Name:  SetD3DXImageColorKey
// Desc:  
//------------------------------------------------------------------------------------------------
int SetD3DXImageColorKey(dcGenericResource* resource, void* data)
{
    D3DXImage* image = (D3DXImage*)resource;

    // Edit the color key object
    return EditColorKey(image->getColorKey(), 0);
}







//------------------------------------------------------------------------------------------------
// Name:  DisableD3DXImageColorKey
// Desc:  
//------------------------------------------------------------------------------------------------
int DisableD3DXImageColorKey(dcGenericResource* resource, void* data)
{
    D3DXImage* image = (D3DXImage*)resource;
    return DisableColorKey(image->getColorKey(), 0);
}






//------------------------------------------------------------------------------------------------
// Name:  EditString
// Desc:  
//------------------------------------------------------------------------------------------------
int EditString(dcGenericResource* resource, void* data)
{
    dc::dcString* string = (dcString*)resource;
    std::string caption = "Edit " + string->getName();
    std::string value = string->getValue();
    if (dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), caption.c_str(), "Type a new value:", &value))
    {
        string->setValue(value);
        return 1;
    }
    else
        return 0;
}








//------------------------------------------------------------------------------------------------
// Name:  EditNumberResource
// Desc:  Edits the value of a dc::dcObject-implementing resource, masking its basic type as V
//------------------------------------------------------------------------------------------------
template <typename T,typename V> int EditNumberResource( dc::dcGenericResource* resource, void* pointer)
{
    T* numberResource = (T*)resource;
    V value = (V)numberResource->getValue();

    // Create a caption and prompt
    std::string caption = "Edit " + resource->getTypeName();
    std::string prompt = "Type a new value for '" + resource->getName() + "'";

    // Get the user input
    if (dcxWin32PromptNumericInput(globalEditor()->getTopmostWindowHandle(), caption.c_str(),
                                    prompt.c_str(), &value))
    {
        // Save the value
        numberResource->setValue((T::BasicType)value);

        // Changed the resource
        return 1;
    }
    else
        // The resource's value wasn't changed
        return 0;
}







//------------------------------------------------------------------------------------------------
// Name:  AddNewEntryToCollection
// Desc:  
//------------------------------------------------------------------------------------------------
template <typename T> int AddNewEntryToCollection(dcGenericResource* resource, void* pointer)
{
    T* collection = (T*)resource;

    // Ask the user for a name
    std::string name = "";
    if (dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(),  "Resource Name?", "Please enter a name for this resource:", &name))
    {
        // Create the resource
        dc::dcGenericResource* element = collection->generateElement(name.c_str());

        // Edit this resource
        EditResourceDialog dialog;
        if (!dialog.execute(element))
        {
            collection->releaseElement(element);
            return 0;
        }

        return 1;
    }

    return 0;
}







//------------------------------------------------------------------------------------------------
// Name:  AddD3DXImagesToTable
// Desc:  
//------------------------------------------------------------------------------------------------
int AddD3DXImagesToTable(dcGenericResource* resource, void* pointer)
{
    dc::dcTable<Image>* table = (dcTable<Image>*)resource;

    std::string path;
    std::list<std::string> files;

    if (dcxOpenMultipleFilesDialog(globalEditor()->getTopmostWindowHandle(), "Select Images", "D3DX-Supported Image Files (*.JPG; *.JPEG; *.PNG; *.BMP; *.DDS)\0*.JPG;*.JPEG;*.PNG;*.BMP;*.DDS\0\0",
                                    &path, &files))
    {
        for (std::list<std::string>::iterator i = files.begin(); i != files.end(); ++i)
        {
            D3DXImage* image = (D3DXImage*)table->create<D3DXImage>(*i);
            image->getFileName()->setValue(path + "\\" + *i);
        }
    }

    return 1;
}







//------------------------------------------------------------------------------------------------
// Name:  AddMultipleTexturesToTable
// Desc:  
//------------------------------------------------------------------------------------------------
template <typename T> int AddMultipleTexturesToTable(dcGenericResource* resource, void* pointer)
{
    dc::dcTable<Texture>* table = (dcTable<Texture>*)resource;


    unsigned long thisID = dc::dcClass<T>::staticResourceManager()->uniqueID();
    const unsigned long STATIC_TEXTURE = dc::dcClass<StaticTexture>::staticResourceManager()->uniqueID();
    const unsigned long SLIDING_TEXTURE = dc::dcClass<SlidingTexture>::staticResourceManager()->uniqueID();
    const unsigned long ANIMATED_TEXTURE = dc::dcClass<AnimatedTexture>::staticResourceManager()->uniqueID();
    const unsigned long CIRCLING_TEXTURE = dc::dcClass<CirclingTexture>::staticResourceManager()->uniqueID();

    // Get the name of this type
    std::string prefix;
    if (thisID == STATIC_TEXTURE) {
        prefix = "static ";
    } else if (thisID == SLIDING_TEXTURE) {
        prefix = "sliding ";
    } else if (thisID == ANIMATED_TEXTURE) {
        prefix = "animated ";
    } else if (thisID == CIRCLING_TEXTURE) {
        prefix = "circling ";
    }
    else {
        prefix = "??? ";
    }

    // Build a prompt
    std::string prompt = "Pick images for which to add " + prefix + "textures";

    // Let the user pick a set of textures
    dc::dcGenericResource::Array selection;
    if (PromptSelectMultipleResources("Add Multiple Textures", prompt.c_str(), globalEditor()->getRootResource()->getImageTable()->getMembers(), &selection))
    {

        // Create a texture for each of these selections
        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
        {
            // Create a texture
            T* texture = (T*)table->create<T>(std::string(prefix) + (*i)->getName());

            // Set the texture's reference...this is safe but not ideal
            texture->getImageRef()->setGenericReferencedResource(*i);
        }
    }

    return 1;
}








//------------------------------------------------------------------------------------------------
// Name:  AddMultipleXMeshesToTable
// Desc:  
//------------------------------------------------------------------------------------------------
int AddMultipleXMeshesToTable(dcGenericResource* resource, void* pointer)
{
    dc::dcTable<Mesh>* table = (dcTable<Mesh>*)resource;

    std::string path;
    std::list<std::string> files;

    if (dcxOpenMultipleFilesDialog(globalEditor()->getTopmostWindowHandle(), "Select Meshes", "DirectX-Supported Mesh Files (*.X)\0*.X\0\0",
                                    &path, &files))
    {
        for (std::list<std::string>::iterator i = files.begin(); i != files.end(); ++i)
        {
            // Add this mesh to the table
            XMesh* mesh = (XMesh*)table->create<XMesh>(*i);
            mesh->getSourceFile()->setValue(path + "\\" + *i);
        }
    }

    return 1;
}



int AddSkinnedMeshFiles(dcGenericResource* resource, void*) {
  dc::dcList<UnseenSkinnedMeshResource>* list = (dcList<UnseenSkinnedMeshResource>*)resource;

  std::string path;
  std::list<std::string> files;

  if (dcxOpenMultipleFilesDialog(globalEditor()->getTopmostWindowHandle(),
                                 "Select Animated Meshes", "Unseen Compiled Mesh Files (*.usm)\0*.usm\0\0",
                                  &path, &files))
  {
    for (std::list<std::string>::iterator i = files.begin(); i != files.end(); ++i) {
      std::string name = *i;
      UnseenSkinnedMeshResource* mesh =
        (UnseenSkinnedMeshResource*)list->generateElement(name.substr(0, name.find_last_of("\\")));
      mesh->getFileName()->setValue(path + "\\" + *i);
    }
  }

  return 1;
}

int AddAnimationFiles(dcGenericResource* resource, void*) {
  dc::dcList<UnseenKeyframeAnimation>* list = (dcList<UnseenKeyframeAnimation>*)resource;

  std::string path;
  std::list<std::string> files;
  std::string prefix;

  if (dcx::dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Add Animation Files",
                                   "Please enter a prefix for the resource names", &prefix) &&
      dcxOpenMultipleFilesDialog(globalEditor()->getTopmostWindowHandle(),
                                   "Select Animations", "Unseen Keyframe Animation Files (*.uka)\0*.uka\0\0",
                                    &path, &files))
  {
    for (std::list<std::string>::iterator i = files.begin(); i != files.end(); ++i) {
      std::string name = *i;
      UnseenKeyframeAnimation* animation =
        (UnseenKeyframeAnimation*)list->generateElement(prefix + name.substr(0, name.find_last_of("\\")));
      animation->getFileName()->setValue(path + "\\" + *i);
    }
  }

  return 1;
}



//------------------------------------------------------------------------------------------------
// Name:  AddMultipleMDJMeshesToTable
// Desc:  
//------------------------------------------------------------------------------------------------
int AddMultipleMDJMeshesToTable(dcGenericResource* resource, void* pointer)
{
    dc::dcTable<Mesh>* table = (dcTable<Mesh>*)resource;

    std::string path;
    std::list<std::string> files;

    if (dcxOpenMultipleFilesDialog(globalEditor()->getTopmostWindowHandle(), "Select Meshes", "MDJ Mesh Files (*.MDJ)\0*.MDJ\0\0",
                                    &path, &files))
    {
        for (std::list<std::string>::iterator i = files.begin(); i != files.end(); ++i)
        {
            // Add this mesh to the table
            MDJMesh* mesh = (MDJMesh*)table->create<MDJMesh>(*i);
            mesh->getSourceFile()->setValue(path + "\\" + *i);
        }
    }

    return 1;
}







//------------------------------------------------------------------------------------------------
// Name:  
// Desc:  Lets the user select meshes, and adds a unique skin to the skin table for each mesh.  The 'resource' parameter is ignored.
//------------------------------------------------------------------------------------------------
int AddUniqueSkinForMeshes(dcGenericResource* resource, void* pointer)
{
    // Let the user pick resources
//    SelectResourceDialog dialog;
    dc::dcGenericResource::Array selection;
    if (PromptSelectMultipleResources("Add Unique Skin For Meshes", "Pick the meshes for which a unique skin should be added", globalEditor()->getRootResource()->getMeshTable()->getMembers(), &selection))
    {
        // Get the skin table
        dc::dcList<Skin>* skinList = globalEditor()->getRootResource()->getSkinList();

        // Create an element for each selected mesh
        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
        {
            dc::dcTable<Mesh>::Element* meshElement = (dcTable<Mesh>::Element*)*i;
            Mesh* mesh = meshElement->getImplementation();
            Skin* skin = (Skin*)skinList->generateElement(std::string("skin for ") + meshElement->getName());

            // Add an entry to this mesh for each of the skin's subsets
            SubsetGeometry geometry;
            if (mesh->obtainGeometry(globalEditor()->getD3DDevice(), &geometry))
            {
                for (SubsetGeometry::iterator i = geometry.begin(); i != geometry.end(); ++i)
                {
                    // Get the index of this subset
                    SubsetIndex index = i->first;

                    // Build a name for this subset
                    char subsetName[16];
                    sprintf_s(subsetName, sizeof(subsetName), "%lu", index);

                    // Add this index to our texture map
                    Skin::Map::Element* element = (Skin::Map::Element*)skin->getMap()->generateElement(subsetName);
                    element->getKey()->setValue(index);
                }
            }

            // Erase the geometry we loaded
            DeallocateGeometry(&geometry);
        }
    }


    return 1;
}








//------------------------------------------------------------------------------------------------
// Name:  AddCommonSkinForMeshes
// Desc:  Lets the user select meshes, and adds a single skin to the skin table that includes entries for every subset in the source meshes.
//------------------------------------------------------------------------------------------------
int AddCommonSkinForMeshes(dcGenericResource* resource, void* pointer)
{
    // Let the user pick resources
//    SelectResourceDialog dialog;
    dc::dcGenericResource::Array selection;
//    if (dialog.execute(&globalEditor()->getRootResource()->getMeshTable()->getMembers(), true, &selection))
    if (PromptSelectMultipleResources("Add Common Skin For Meshes", "Pick the meshes for which a common skin should be added", globalEditor()->getRootResource()->getMeshTable()->getMembers(), &selection))
    {
        // Get the skin table
        dc::dcList<Skin>* skinList = globalEditor()->getRootResource()->getSkinList();

        // Create an element name
        std::string name = "";
        //if (!globalEditor()->promptForText("Resource Name?", "Please enter a name for this resource:", &name))
        if (!dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Resource Name?", "Please enter a name for this resource:", &name))
            return 0;

        // The set of subsets
        std::set<SubsetIndex> subsetIndices;

        // Create an element for each selected mesh
        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
        {
            dc::dcTable<Mesh>::Element* meshElement = (dcTable<Mesh>::Element*)*i;
            Mesh* mesh = meshElement->getImplementation();

            // Add an entry to this mesh for each of the skin's subsets
            SubsetGeometry geometry;
            //globalEditor()->getD3DDevice()
            if (mesh->obtainGeometry(0, &geometry))
            {
                for (SubsetGeometry::iterator i = geometry.begin(); i != geometry.end(); ++i)
                {
                    // Get the index of this subset
                    SubsetIndex index = i->first;
                    subsetIndices.insert(index);
                }
            }

            // Erase the geometry we loaded
            DeallocateGeometry(&geometry);
        }

        // Create the skin
        Skin* skin = (Skin*)skinList->generateElement(name.c_str());

        // Add the indices to our texture map
        for (std::set<SubsetIndex>::iterator j = subsetIndices.begin(); j != subsetIndices.end(); ++j)
        {
            // Build a name for this subset
            char subsetName[16];
            sprintf_s(subsetName, sizeof(subsetName), "%lu", *j);

            Skin::Map::Element* element = (Skin::Map::Element*)skin->getMap()->generateElement(subsetName);
            element->getKey()->setValue(*j);
        }
    }


    return 1;
}







//------------------------------------------------------------------------------------------------
// Name:  AddSceneryForMeshes
// Desc:  
//------------------------------------------------------------------------------------------------
int AddSceneryForMeshes(dcGenericResource* resource, void* pointer)
{
    // Get the tables
    dc::dcTable<Mesh>* meshTable = globalEditor()->getRootResource()->getMeshTable();
    dc::dcList<Scenery>* sceneryList = globalEditor()->getRootResource()->getSceneryList();

    // Let the user pick resources
    dc::dcGenericResource::Array selection;
    if (PromptSelectMultipleResources("Add Scenery For Meshes", "Pick the meshes for which to create scenery", meshTable->getMembers(), &selection))
    {
        // Create an element for each selected mesh
        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
        {
            dc::dcTable<Mesh>::Element* meshElement = (dcTable<Mesh>::Element*)*i;
            Mesh* mesh = meshElement->getImplementation();

            // Create a new scenery item
            Scenery* scenery = (Scenery*)sceneryList->generateElement(meshElement->getName());
            scenery->getMeshRef()->setReferencedResource(meshElement);
        }
    }
    else
        return 0;

    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  SetSkinForMultipleScenery
// Desc:  
//------------------------------------------------------------------------------------------------
int SetSkinForMultipleScenery(dcGenericResource* resource, void* pointer)
{
    // Get the tables
    dc::dcList<Scenery>* sceneryList = globalEditor()->getRootResource()->getSceneryList();
    dc::dcList<Skin>* skinList = globalEditor()->getRootResource()->getSkinList();

    // Let the user pick which resources to assign a skin to
//    SelectResourceDialog dialog;
    dc::dcGenericResource::Array scenerySelection;
    if (PromptSelectMultipleResources("Set Skin for Scenery", "Pick the scenery to set the skin for", sceneryList->getMembers(), &scenerySelection))
    {
        // Let the user select the skin to assign
        dc::dcGenericResource* skinSelection;
        if (PromptSelectSingleResource("Set Skin for Scenery", "Pick the skin for the scenery you selected", skinList->getMembers(), &skinSelection))
        {
            // Set the skin for all of the scenery
            for (dcGenericResource::Array::iterator i = scenerySelection.begin(); i != scenerySelection.end(); ++i)
            {
                dc::dcList<Scenery>::Element* sceneryElement = (dcList<Scenery>::Element*)(*i);
                //Scenery* scenery = sceneryElement->getImplementation();
                sceneryElement->getSkinRef()->setReferencedResource((dcList<Skin>::Element*)skinSelection);
            }
        }
        else
            return 0;
    }
    else
        return 0;

    return 1;
}

/**
 * This method simplifies the adding of .X models to the resource file by automating the most common method
 * of adding meshes as scenery.  It performs the following algorithm:
 *  - Ask the user to select all of the .X meshes to add
 *  - Iterate through and load each of the .X meshes the user selected
 *      - If the mesh already exists, skip this mesh
 *      - Create a mesh with default transformation properties
 *      - Create a skin for this mesh
 *      - Add each of the source mesh's textures as Images and Static Textures.
 *      - Assign entries in the skin for each of the textures
 *      - Create an scenery in the scenery table for the mesh, and assign the skin to it
 */
int AddXModelScenery(dcGenericResource* resource, void* pointer)
{
    dc::dcList<Scenery>* sceneryList = globalEditor()->getRootResource()->getSceneryList();
    dc::dcList<Skin>* skinList = globalEditor()->getRootResource()->getSkinList();
    dc::dcTable<Mesh>* meshTable = globalEditor()->getRootResource()->getMeshTable();
    dc::dcTable<Image>* imageTable = globalEditor()->getRootResource()->getImageTable();
    dc::dcTable<Texture>* textureTable = globalEditor()->getRootResource()->getTextureTable();

    std::string path;
    std::list<std::string> files;

    if (dcxOpenMultipleFilesDialog(globalEditor()->getTopmostWindowHandle(), "Select Meshes", "DirectX-Supported Mesh Files (*.X)\0*.X\0\0",
                                    &path, &files))
    {
        for (std::list<std::string>::iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
        {
            std::string sourceFile = path  + "\\" + *fileIter;

            // Add this mesh to the table
            XMesh* mesh = (XMesh*)meshTable->create<XMesh>(*fileIter);
            mesh->getSourceFile()->setValue(sourceFile);

            // Create a skin for this mesh
            Skin* skin = (Skin*)skinList->generateElement(std::string("skin for ") + *fileIter);

            // Load the mesh information
            LPD3DXMESH              pXMesh         = NULL; // Our mesh object in sysmem
            DWORD                   numMaterials = 0L;   // Number of mesh materials
            LPD3DXBUFFER pD3DXMtrlBuffer;

            // Load the mesh from the specified file
            if (SUCCEEDED(D3DXLoadMeshFromX(sourceFile.c_str(), D3DXMESH_SYSTEMMEM, 
                                           globalEditor()->getD3DDevice(), NULL, 
                                           &pD3DXMtrlBuffer, NULL, &numMaterials, 
                                           &pXMesh)))
            {
                // Get the materials from the loaded buffer
                D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

                // Iterate through each of the materials and add the corresponding images & textures
                for (DWORD i = 0; i < numMaterials; ++i)
                {
                    // Build a string-name for this subset
                    char subsetName[16];
                    sprintf_s(subsetName, sizeof(subsetName), "%lu", i);

                    // Exit
                    if (!d3dxMaterials[i].pTextureFilename) continue;

                    // Get the end of the file name (just the file itself, no path)
                    char* fileName = strrchr(d3dxMaterials[i].pTextureFilename, '\\');
                    if (fileName == NULL)
                        fileName = strrchr(d3dxMaterials[i].pTextureFilename, '/');

                    // Compile the location of this image
                    char imageSourceFile[MAX_PATH];
                    char* imageFileName = (fileName == NULL) ? d3dxMaterials[i].pTextureFilename : (fileName + 1);
                    sprintf_s(imageSourceFile, MAX_PATH, "%s\\%s", path.c_str(), imageFileName);

                    // Add an image
                    D3DXImage* image = (D3DXImage*)imageTable->create<D3DXImage>(std::string(imageFileName) + " - image for " + *fileIter + " [subset " + subsetName + "]");

                    // Set the image's source file
                    image->getFileName()->setValue(imageSourceFile);

                    // Add a texture for this image
                    StaticTexture* texture = (StaticTexture*)textureTable->create<StaticTexture>(std::string("texture of ") + imageFileName + " for " + *fileIter + " [subset " + subsetName + "]");
                    texture->getImageRef()->setGenericReferencedResource(image->getOwner());

                    // Add the entry to the skin
                    Skin::Map::Element* element = (Skin::Map::Element*)skin->getMap()->generateElement(subsetName);
                    element->getKey()->setValue(i);
                    element->getValue()->setGenericReferencedResource(texture->getOwner());
                }

                // Get rid of the allocated mesh parts
                pD3DXMtrlBuffer->Release();
                pXMesh->Release();
            }

            // Add a scenery element for this mesh
            Scenery* scenery = (Scenery*)sceneryList->generateElement(*fileIter);
            scenery->getMeshRef()->setGenericReferencedResource(mesh->getOwner());
            scenery->getSkinRef()->setGenericReferencedResource(skin);
        }
    }

    return 1;
}


int MergeDuplicatesInCollection(dcGenericResource* resource, void* pointer) {
  dc::dcGenericCollection* collection = (dc::dcGenericCollection*)resource;
  ASSERT(collection->isCollection()) else return -1;
  bool restart = true;

  int duplicates_removed = 0;

  // This map stores a list of duplicate elements: the key is the copy, and the
  // value is the original.  By using this map, only one pass through the
  // collection is necessary.
  typedef std::map<const dcGenericResource*, const dcGenericResource*> Duplicates;
  Duplicates duplicates;

  dc::dcGenericResource::Array::const_iterator end = collection->getMembers().end();
  for (dc::dcGenericResource::Array::const_iterator i = collection->getMembers().begin(); i != end; ++i) {
    const dcGenericResource* element = *i;
    if (duplicates.find(element) != duplicates.end()) continue;

    // search from i+1 to the end
    dc::dcGenericResource::Array::const_iterator j = i;
    for (++j; j != end; ++j) {
      const dcGenericResource* other_element = *j;
      if (duplicates.find(other_element) != duplicates.end()) continue;
      if (other_element->compareTo(element) != 0) continue;

      // these elements are duplicates!
      duplicates.insert(Duplicates::value_type(other_element, element));
    }
  }

  // Erase all duplicates and swap out references
  if (duplicates.size() > 0) {

    // Find all references under the root resource
    dc::dcGenericResource::Array all_references;
    dcGenericResource* rootResource = collection;
    while( rootResource->getOwner() != 0 ) rootResource = rootResource->getOwner();
    dcGenericResource::Array resources;
    resources.push_back( rootResource );
    while (!resources.empty()) {
      dcGenericResource* resource = resources.back();
      // here's a tricky bug: avoid parsing resources that
      // are in the duplicates list, since they will be erased
      // and the pointers will not be invalidated!
      if (duplicates.find(resource) == duplicates.end()) {
        const dcGenericResource::Array& members = resource->getMembers();
        if (!members.empty()) resources.insert(resources.begin(), members.begin(), members.end());
        if (resource->isReference()) {
          all_references.push_back(resource);
        }
      }
      resources.pop_back(); 
    }

    // Update all references
    for (Duplicates::iterator i = duplicates.begin(); i != duplicates.end(); ++i) {
      const dcGenericResource* duplicate_element = i->first;
      const dcGenericResource* original_element = i->second;

      dc::dcGenericResource::Path duplicate_path, original_path;
      duplicate_element->getPath(&duplicate_path);
      original_element->getPath(&original_path);

      for (dc::dcGenericResource::Array::iterator j = all_references.begin(); j != all_references.end(); ++j) {
        dcGenericReference* reference = (dcGenericReference*)(*j);
        reference->updateReferencedPath(duplicate_path, original_path);
      }

      collection->releaseElement(const_cast<dc::dcGenericResource*>(duplicate_element));
    }
  }
/*
  // check the elements before each element in the list to see if they are
  // duplicates.  if they are, delete the later one and restart the search
  // since vector-iterators can't be manipulated.
  do {
    if (restart) {
      restart = false;
      i = collection->getMembers().begin();
      end = collection->getMembers().end();
    }
    if (i != end) {
      const dcGenericResource* element = *i;
      for (dc::dcGenericResource::Array::const_iterator j = collection->getMembers().begin(); j != i;) {
        const dcGenericResource* other_element = *j;

        // if these are the same element, delete the second
        if (0 == element->compareTo(other_element)) {
          dc::dcGenericResource::Path element_path, other_path;
          element->getPath(&element_path);
          other_element->getPath(&other_path);

          // Update all references under the root resource
          dcGenericResource* rootResource = collection;
          while( rootResource->getOwner() != 0 ) rootResource = rootResource->getOwner();
          dcGenericResource::Array resources;
          resources.push_back( rootResource );
          while (!resources.empty()) {
            dcGenericResource* resource = resources.back();
            const dcGenericResource::Array& members = resource->getMembers();
            if (!members.empty()) resources.insert(resources.begin(), members.begin(), members.end());
            if( resource->isReference() ) {
              dcGenericReference* reference = (dcGenericReference*)resource;
              reference->updateReferencedPath(other_path, element_path);
            }
            resources.pop_back(); 
          }

          // erase this element
          collection->releaseElement(const_cast<dc::dcGenericResource*>(other_element));

          // exit and try going through the list again
          restart = true;
          ++duplicates_removed;
          break;

        } else {
          ++j;
        }
      }

      ++i;
    }
  } while (i != end);*/

  // indicate how many duplicates were removed
  char message[512];
  sprintf_s(message, 512, "%i duplicates removed", duplicates.size());
  MessageBox(globalEditor()->getTopmostWindowHandle(), message, "Merge Duplicates", MB_OK);

  return 1;
}







//------------------------------------------------------------------------------------------------
// Name:  SetScalingFactorForMultipleMeshes
// Desc:  This method will set a  scaling factor for all of the meshes that the user selects.  This is useful
//        when trying to get the scaling correct when using meshes from different sources.
//------------------------------------------------------------------------------------------------
int SetScalingFactorForMultipleMeshes(dcGenericResource* resource, void* pointer)
{
    // Get the tables
    dc::dcTable<Mesh>* meshTable = globalEditor()->getRootResource()->getMeshTable();
    dc::dcList<Scenery>* sceneryList = globalEditor()->getRootResource()->getSceneryList();

    // Let the user pick resources
//    SelectResourceDialog dialog;
    dc::dcGenericResource::Array selection;
//    if (dialog.execute(&meshTable->getMembers(), true, &selection))
    if (PromptSelectMultipleResources("Set Mesh Scaling Factor", "Pick the meshes for which to set the scaling factor", meshTable->getMembers(), &selection))
    {
        // Ask the user for a scaling factor
        std::string input = "";
        if (dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Scaling factor?", "Please enter a scaling factor for these meshes:", &input))
        {
            // The user's designated scaling factor
            float scalingFactor = (float)atof(input.c_str());

            // Create an element for each selected mesh
            for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
            {
                dc::dcTable<Mesh>::Element* meshElement = (dcTable<Mesh>::Element*)*i;
                Mesh* mesh = meshElement->getImplementation();

                // Set the scaling factor for this mesh
                mesh->getScaling()->getX()->setValue(scalingFactor);
                mesh->getScaling()->getY()->setValue(scalingFactor);
                mesh->getScaling()->getZ()->setValue(scalingFactor);
            }

            // Edited meshes
            return 1;
        }
    }

    // Nothing happened
    return 0;
}

/**
 * This method loads a mesh and displays the floating-point scaling factor necessary to give its largest extent
 * in each of the X/Y/Z directions unit size.  This is useful because, by having a modeler create a 3d object that
 * represents a unit cube of correct size in relation to their other models, all of the models that modelers creates
 * can be re-scaled so that they are correctly sized in the final game.
 */
int CalculateUnityScalingFactorForMesh(dcGenericResource* resource, void* pointer)
{
    // Get the mesh resource
    dc::dcTable<Mesh>::Element* meshElement = (dcTable<Mesh>::Element*)resource;
    Mesh* mesh = meshElement->getImplementation();

    // Holds the scaling factor
    float factor;

    // Get the factor from this resource
    if (mesh->getUnityScalingFactor(globalEditor()->getD3DDevice(), &factor))
    {
        // Print a message
        char message[256];
        sprintf_s(message, sizeof(message), "Scale this mesh by %f", factor);

        // Display the message
        MessageBox(GetDesktopWindow(), message, "Unity Scaling Factor", MB_OK);
    }
    else
        MessageBox(GetDesktopWindow(), "Mesh could not be found", "Error", MB_OK);

    // This method does not modify the resource
    return 0;
}






//------------------------------------------------------------------------------------------------
// Name:  RescaleMeshesScalingRelativeToUnity
// Desc:  
//------------------------------------------------------------------------------------------------
int RescaleMeshesScalingRelativeToUnity(dcGenericResource* resource, void* pointer)
{
    return 1;
}







//------------------------------------------------------------------------------------------------
// Name:  VisuallyRescaleMeshes
// Desc:  
//------------------------------------------------------------------------------------------------
int VisuallyRescaleMeshes(dcGenericResource* resource, void* pointer)
{
    // Get the tables
    dc::dcTable<Mesh>* meshTable = globalEditor()->getRootResource()->getMeshTable();
    dc::dcList<Scenery>* sceneryList = globalEditor()->getRootResource()->getSceneryList();

    // Let the user pick resources
    dc::dcGenericResource::Array selection;
    if (PromptSelectMultipleResources("Visually Rescale Meshes",
                                      "Select Meshes to Scale",
                                      meshTable->getMembers(),
                                      &selection, false)) {
        RescaleMeshesDialog dlg;
        dlg.execute(&selection);
    }

    return 1;
}







//------------------------------------------------------------------------------------------------
// Name:  AddMultipleCollectionEntryReferencesToList
// Desc:  
//------------------------------------------------------------------------------------------------
template <typename Collection> int AddMultipleCollectionEntryReferencesToList(dcGenericResource* resource, void* pointer)
{
    // Look up 
    // Get the reference type
    dc::dcList<Collection::Reference>* list = (dcList<Collection::Reference>*)resource;
    dc::dcGenericResource::Array referencableTypes;
    getDescendantsOfType(globalEditor()->getRootResource(),
                          dc::dcList<Collection::Reference>::Element::staticGetReferenceableTypeID(),
                          &referencableTypes);

    dc::dcGenericResource::Array selection;

    // Display all of the possible entries
    std::string prompt = "Pick some " + dc::dcList<Collection::Reference>::Element::ResourceType::staticTypeName() + " resources";
    if (PromptSelectMultipleResources("Add Multiple References", prompt.c_str(), referencableTypes, &selection))
    {
        // Add these references to the list
        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
        {
            Collection::Reference* ref = (Collection::Reference*)list->generateElement((*i)->getName());
            ref->setReferencedResource((Collection::Element*)*i);
        }

        // Yay! we did something
        return 1;
    }

    // Return code of some sort...
    return 0;
}

/**
 * Edits an enumeration by displaying the string equivalents of the indices in a list
 * and allowing the user to choose among them.  This method makes the assumption that
 * the enumeration begins with a value of zero for the first element, the indices in
 * the enum are contiguous, and that the dc::dcEnum<{enum}> type (the T parameter) will
 * return 'null' from its getString method for the first integer value which does
 * not map to a valid enum type.
 * TODO: it might be better to pass the enum max value as a constant...somehow...?
 */
template <typename T> int EditEnumValue(dcGenericResource* resource, void* pointer)
{
    // Get the enum type
    T* enumResource = (T*)resource;

    // Build a prompt and caption
    std::string caption = "Edit " + resource->getTypeName();
    std::string prompt = "Select '" + resource->getName() + "' value:";

    // Add entries to the options table
    std::list<std::string> options;
    const char* stringValue = T::getString((T::BasicEnumType)0);
    int index = 1;
    for (index = 1; stringValue != 0; ++index)
    {
        options.push_back(stringValue);
        stringValue = T::getString((T::BasicEnumType)index);
    }

    // Set the current selection
    int selection = (int)enumResource->getValue();
    if (selection >= index) selection = -1;

    // Let the user pick the value
    if (dcxWin32PromptForSelection(globalEditor()->getTopmostWindowHandle(), caption.c_str(), prompt.c_str(),
                                    &options, &selection, selection >= 0))
    {
        enumResource->setValue((T::BasicEnumType)selection);
        return 1;
    }
    else
        return 0;
}







//------------------------------------------------------------------------------------------------
// Name:  EditAtlas
// Desc:  Lets the user visually edit the project's main Atlas resource.  This method will
//        most likely take a LONG time to execute, so there needs to be some sort of
//        progress dialog or what not...
//------------------------------------------------------------------------------------------------
int EditAtlas(dcGenericResource* resource, void* pointer)
{
    Atlas* atlas = (Atlas*)resource;
    
    // Sort the texture table to put the alpha-tested textures at the end
    globalEditor()->getRootResource()->getTextureTable()->sort();

    // Run the atlas-edit dialog
    atlas->edit();

    // Success
    return 0;
}







//----[  SortCollectionByName  ]-----------------------------------------------
int SortCollectionByName(dcGenericResource* resource, void* pointer)
{
    dc::dcGenericCollection* collection = (dcGenericCollection*)resource;
    collection->sortByName();
    return 1;
}





//----[  SortCollection  ]-----------------------------------------------------
int SortCollection(dcGenericResource* resource, void* pointer)
{
    dc::dcGenericCollection* collection = (dcGenericCollection*)resource;
    collection->sort();
    return 1;
}




//----[  ClearReference  ]-----------------------------------------------------
// Erases a reference's target
int ClearReference(dcGenericResource* resource, void* pointer)
{
    dc::dcGenericReference* reference = (dcGenericReference*)resource;
    reference->setGenericReferencedResource(NULL);
    return 1;
}




//----[  ImportSubdividedImages  ]---------------------------------------------
// Lets the user pick images that contain multiple sub-images and import them
int ImportSubdividedImages(dcGenericResource* resource, void* pointer)
{
    // Get the image table
    dc::dcTable<Image>* imageTable = (dcTable<Image>*)resource;

    std::string path;
    std::list<std::string> files;
    size_t horizontal, vertical;

    if (!dcxOpenMultipleFilesDialog(globalEditor()->getTopmostWindowHandle(), "Select Image(s)", "Image Files (*.JPG; *.JPEG; *.PNG; *.BMP; *.DDS)\0*.JPG;*.JPEG;*.PNG;*.BMP;*.DDS\0\0",
                                    &path, &files))
        return 0;
    if (!dcx::dcxWin32PromptNumericInput(globalEditor()->getTopmostWindowHandle(), "Import Subdivided Images",
                                          files.size() == 1 ? "How many columns of sub-images does this image contain?" :
                                                              "How many columns of sub-images do these images contain?", &horizontal, false))
        return 0;
    if (!dcx::dcxWin32PromptNumericInput(globalEditor()->getTopmostWindowHandle(), "Import Subdivided Images",
                                          files.size() == 1 ? "How many rows of sub-images does this image contain?" :
                                                              "How many rows of sub-images do these images contain?", &vertical, false))
        return 0;

    // The number of subdivisions
    size_t totalSubImages = horizontal * vertical;

    // For each of the files in the list, import divisions
    for (std::list<std::string>::iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
    {
        // Build the path for this image
        std::string sourceFile = path  + "\\" + *fileIter;

        // Repeat for each sub-image
        for (size_t x = 0; x < horizontal; ++x)
            for (size_t y = 0; y < vertical; ++y)
            {
                // Get the subdivision index and its string equivalent
                int subImage = y * horizontal + x;
                char subImageText[16];
                sprintf_s(subImageText, sizeof(subImageText), " %i/%i", subImage + 1, totalSubImages);


                // Create the image
                D3DXImage* image = (D3DXImage*)imageTable->create<D3DXImage>(*fileIter + subImageText);
                image->getFileName()->setValue(sourceFile.c_str());
                image->getSubImage()->setValue(subImage);
                image->getSubImageColumns()->setValue(horizontal);
                image->getSubImageRows()->setValue(vertical);
            }
    }

    // Changed something
    return 1;
}







//------------------------------------------------------------------------------------------------
// Name:  AddGroundTexturesToTerrain
// Desc:  
//------------------------------------------------------------------------------------------------
int AddGroundTexturesToTerrain(dcGenericResource* resource, void* pointer)
{
    dc::dcList<Terrain>* terrainList = (dcList<Terrain>*)resource;
    dc::dcTable<Texture>* textureTable = globalEditor()->getRootResource()->getTextureTable();

    // Let the user select textures
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Add Ground Textures", "Select ground textures:", textureTable->getMembers(), &selection))
        return 0;

    // Add a ground terrain type for each texture
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Generate a terrain element
        Terrain* terrain = (Terrain*)terrainList->generateElement((*i)->getName());

        // Set up this element
        terrain->getTopTextureRef()->setGenericReferencedResource(*i);
    }

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  GenerateSceneryDialog
// Desc:  
//------------------------------------------------------------------------------------------------
class GenerateSceneryDialog : public dcxWin32DialogController
{
    public:

        struct Parameter
        {
            float min, max, center, centricity;
        };

        struct TransformSettings
        {
            Parameter x, y, z;
        };

        struct GenerateData
        {
            /// How many duplicates of the scenery to generate
            int numberOfCopies;
            bool keepXYZScalingSame;
            bool keepXZScalingSame;
            TransformSettings translation, rotation, scaling;
        };

    public:

        bool execute()
        {
            return TRUE == beginDialog(
                GetModuleHandle(NULL),
                MAKEINTRESOURCE(IDD_GENERATESCENERY),
                globalEditor()->getTopmostWindowHandle());
        }

        const GenerateData* getData()
        {
            return &myData;
        }

    protected:

        HWND myDialog;

        GenerateData myData;

    protected:

        float getFloat(WORD ControlID)
        {
            // Get text from the provided control
            char buffer[64];
            ZeroMemory(buffer, sizeof(buffer));
            Edit_GetText(GetDlgItem(myDialog, ControlID), buffer, sizeof(buffer));

            // Interpret the text
            return (float)atof(buffer);
        }

        void setFloat(WORD ControlID, float value)
        {
            char buffer[64];
            sprintf_s(buffer, sizeof(buffer), "%.02f", value);
            Edit_SetText(GetDlgItem(myDialog, ControlID), buffer);
        }

        unsigned long getNumberOfCopies()
        {
            // Get text from the provided control
            char buffer[64];
            ZeroMemory(buffer, sizeof(buffer));
            Edit_GetText(GetDlgItem(myDialog, IDC_EDIT_NUMCOPIES), buffer, sizeof(buffer));

            // Interpret the text
            return (unsigned long)atol(buffer);
        }

        void setNumberOfCopies(unsigned long value)
        {
            char buffer[64];
            sprintf_s(buffer, sizeof(buffer), "%lu", value);
            Edit_SetText(GetDlgItem(myDialog, IDC_EDIT_NUMCOPIES), buffer);
        }

        INT_PTR initDialog(HWND hDlg)
        {
            // Save the dialog pointer
            myDialog = hDlg;
#define SET_TRANSFORM_HELPER(a, b, c, d) a##b##c##d
#define SET_TRANSFORM(transform, parameter, value) \
    setFloat(SET_TRANSFORM_HELPER(IDC_EDIT_, transform, _MIN, parameter), value); \
    setFloat(SET_TRANSFORM_HELPER(IDC_EDIT_, transform, _MAX, parameter), value); \
    setFloat(SET_TRANSFORM_HELPER(IDC_EDIT_, transform, _CENTER_, parameter), value); \
    setFloat(SET_TRANSFORM_HELPER(IDC_EDIT_, transform, _CENTRICITY_, parameter), 1.0f);

            // Initialize all of the dialog settings
            SET_TRANSFORM(TRA, X, 0.0f);
            SET_TRANSFORM(TRA, Y, 0.0f);
            SET_TRANSFORM(TRA, Z, 0.0f);
            SET_TRANSFORM(ROT, X, 0.0f);
            SET_TRANSFORM(ROT, Y, 0.0f);
            SET_TRANSFORM(ROT, Z, 0.0f);
            SET_TRANSFORM(SCA, X, 1.0f);
            SET_TRANSFORM(SCA, Y, 1.0f);
            SET_TRANSFORM(SCA, Z, 1.0f);

            // Initialize the number of copies
            setNumberOfCopies(1);

            // Success
            return 0;
        }

        void updateScalingFlags()
        {
            BOOL keepXZ  = Button_GetCheck(GetDlgItem(myDialog, IDC_CHECK_KEEPXZSCALINGSAME));
            BOOL keepXYZ = Button_GetCheck(GetDlgItem(myDialog, IDC_CHECK_UNITYSCALING));

            // Only allow one keeper
            Static_Enable(GetDlgItem(myDialog, IDC_CHECK_KEEPXZSCALINGSAME), !keepXYZ);
            Static_Enable(GetDlgItem(myDialog, IDC_CHECK_UNITYSCALING),      !keepXZ);

            BOOL enable;

#define SET_ENABLE_HELPER(a, b, c, d) a##b##c##d
#define SET_ENABLE(transform, parameter)\
            Edit_Enable(GetDlgItem(myDialog, SET_ENABLE_HELPER(IDC_EDIT_, transform, _MIN, parameter)), enable);\
            Edit_Enable(GetDlgItem(myDialog, SET_ENABLE_HELPER(IDC_EDIT_, transform, _MAX, parameter)), enable);\
            Edit_Enable(GetDlgItem(myDialog, SET_ENABLE_HELPER(IDC_EDIT_, transform, _CENTER_, parameter)), enable);\
            Edit_Enable(GetDlgItem(myDialog, SET_ENABLE_HELPER(IDC_EDIT_, transform, _CENTRICITY_, parameter)), enable);

            enable = !keepXYZ;
            SET_ENABLE(SCA, Y);
            enable = !keepXZ && !keepXYZ;
            SET_ENABLE(SCA, Z);
        }

        /**
         * When something on the dialog is activated, this function is invoked
         *   @param hWnd Dialog handle
         *   @param hControl Control that spawned the notification
         *   @param ControlID ID of the control that created the notification
         *   @param Notification The event that occurred
         *   @return Return code
         */
        INT_PTR dialogNotification(HWND hDlg, HWND hControl, UINT_PTR ControlID, UINT Notification)
        {
            // Grab dialog data and exit if the user clicked the OK button
            if (Notification == BN_CLICKED)
            {
                updateScalingFlags();

                // If this is just a check box changing, exit
                if (ControlID == IDC_CHECK_UNITYSCALING ||
                    ControlID == IDC_CHECK_KEEPXZSCALINGSAME)
                    return 0;

#define GET_TRANSFORM_PARAM_HELPER(a, b, c, d) a##b##c##d
#define GET_TRANSFORM_PARAM(transform, parameter, destTransform, destParameter) \
    myData.##destTransform.##destParameter.min = getFloat(GET_TRANSFORM_PARAM_HELPER(IDC_EDIT_, transform, _MIN, parameter)); \
    myData.##destTransform.##destParameter.max = getFloat(GET_TRANSFORM_PARAM_HELPER(IDC_EDIT_, transform, _MAX, parameter)); \
    myData.##destTransform.##destParameter.center = getFloat(GET_TRANSFORM_PARAM_HELPER(IDC_EDIT_, transform, _CENTER_, parameter)); \
    myData.##destTransform.##destParameter.centricity = getFloat(GET_TRANSFORM_PARAM_HELPER(IDC_EDIT_, transform, _CENTRICITY_, parameter));

#define GET_TRANSFORM(transform, destTransform)\
    GET_TRANSFORM_PARAM(transform, X, destTransform, x); \
    GET_TRANSFORM_PARAM(transform, Y, destTransform, y); \
    GET_TRANSFORM_PARAM(transform, Z, destTransform, z);

                // Get all of the user's transformation values
                GET_TRANSFORM(TRA, translation);
                GET_TRANSFORM(SCA, scaling);
                GET_TRANSFORM(ROT, rotation);
/*
#define CONV_DEGREES_PARAM(destTransform, destParameter)\
    myData.##destTransform.##destParameter.min = D3DXToRadian(myData.##destTransform.##destParameter.min);\
    myData.##destTransform.##destParameter.max = D3DXToRadian(myData.##destTransform.##destParameter.max);\
    myData.##destTransform.##destParameter.center = D3DXToRadian(myData.##destTransform.##destParameter.center);
#define CONV_DEGREES(destTransform)\
    CONV_DEGREES_PARAM(destTransform, x);\
    CONV_DEGREES_PARAM(destTransform, y);\
    CONV_DEGREES_PARAM(destTransform, z);

                // Convert rotation values into radians
                CONV_DEGREES(rotation);*/

                // Get misc. values
                myData.numberOfCopies = getNumberOfCopies();
                myData.keepXYZScalingSame = TRUE == Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_UNITYSCALING));
                myData.keepXZScalingSame  = TRUE == Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_KEEPXZSCALINGSAME));

                // Return the result code
                EndDialog(hDlg, ControlID == IDOK);
            }

            return 0;
        }

        /**
         * All messages received by the dialog are passed through here.  The default
         * implementation of this method will call EndDialog with a value of "0"
         * when the window is closed.
         *   @param hWnd Dialog handle
         *   @param uMsg Message
         *   @param wParam Parameter
         *   @param lParam Parameter
         *   @return Return code
         */
        INT_PTR dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            if (uMsg == WM_INITDIALOG) return initDialog(hDlg);
            if (uMsg == WM_CLOSE) EndDialog(hDlg, FALSE);
            return 0;
        }
};







//------------------------------------------------------------------------------------------------
// Name:  GRSD_Param_Helper
// Desc:  Helper for GenerateRandomSceneryDuplicates
//        TODO: make this into a dcx method, because it's super groovy!
//------------------------------------------------------------------------------------------------
float GRSD_Param_Helper(float min, float max, float center, float centricity)
{
    // Make sure max > min and center is in the range
    if (max < min) { float t = max; max = min; min = t; }
    if (center < min) center = min;
    if (center > max) center = max;

    // Generate a linear random value in the range [-1.0,1.0]
    double randomValue = 2.0 * rand() / (double)RAND_MAX - 1.0;

    // Weight this random value toward 0.0
    randomValue = (randomValue < 0.0 ? -1.0 : 1.0) * pow(abs(randomValue), (double)centricity);

    // Pick a value in the right range
    if (randomValue < 0.0)
        return (float)(center - (center - min) * -randomValue);
    else
        return (float)(center + (max - center) * +randomValue);
}






//------------------------------------------------------------------------------------------------
// Name:  GRSD_Param
// Desc:  Helper for GenerateRandomSceneryDuplicates
//------------------------------------------------------------------------------------------------
void GRSD_Param(D3DXVECTOR3* xyz, const GenerateSceneryDialog::TransformSettings* param)
{
    *xyz = D3DXVECTOR3(GRSD_Param_Helper(param->x.min, param->x.max, param->x.center, param->x.centricity),
                        GRSD_Param_Helper(param->y.min, param->y.max, param->y.center, param->y.centricity),
                        GRSD_Param_Helper(param->z.min, param->z.max, param->z.center, param->z.centricity));
}







//------------------------------------------------------------------------------------------------
// Name:  GenerateRandomSceneryDuplicates
// Desc:  
//------------------------------------------------------------------------------------------------
int GenerateRandomSceneryDuplicates(dcGenericResource* resource, void* pointer)
{
    // Get the scenery table being edited
    dc::dcList<Scenery>* sceneryList = (dcList<Scenery>*)resource;

    // Prompt the user to select some scenery
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Generate Random Scenery Duplicates", "Select scenery elements:", sceneryList->getMembers(), &selection))
        return 0;

    // Ask the user for scenery settings
    GenerateSceneryDialog dialog;
    if (dialog.execute())
    {
        const GenerateSceneryDialog::GenerateData* data = dialog.getData();

        // Repeat for each selected terrain element
        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
        {
            // Get the source terrain element
            dc::dcList<Scenery>::Element* sourceSceneryElement = (dcList<Scenery>::Element*)*i;
            //Scenery* sourceScenery = (Scenery*)sourceSceneryElement->getImplementation();

            // Save the source scenery into a buffer
            dc::dcBufferStream buffer;
            sourceSceneryElement->save(&buffer);

            // Repeat for each duplicate
            for (int i = 0; i < data->numberOfCopies; ++i)
            {
                // Reset the buffer
                buffer.seek(0);

                // Create the duplicate element
                char nameSuffix[32];
                sprintf_s(nameSuffix, sizeof(nameSuffix), " - %i", i + 1);
                Scenery* duplicateScenery = (Scenery*)sceneryList->generateElement(sourceSceneryElement->getName() + nameSuffix);

                // Copy the source info
                duplicateScenery->load(&buffer);

                // Randomize the positioning information
                D3DXVECTOR3 s, r, t;
                GRSD_Param(&s, &data->scaling);
                GRSD_Param(&r, &data->rotation);
                GRSD_Param(&t, &data->translation);

                // If we need to keep the mesh from being distored,
                // copy the x-scaling into the other parameters
                if (data->keepXZScalingSame)
                {
                    s.z = s.x;
                }
                if (data->keepXYZScalingSame)
                {
                    s.y = s.x;
                    s.z = s.x;
                }

                // Save the scaling
                duplicateScenery->getScaling()->getX()->setValue(s.x);
                duplicateScenery->getScaling()->getY()->setValue(s.y);
                duplicateScenery->getScaling()->getZ()->setValue(s.z);

                // Save the rotation
                duplicateScenery->getRotation()->getX()->setValue(r.x);
                duplicateScenery->getRotation()->getY()->setValue(r.y);
                duplicateScenery->getRotation()->getZ()->setValue(r.z);

                // Save the translation
                duplicateScenery->getTranslation()->getX()->setValue(t.x);
                duplicateScenery->getTranslation()->getY()->setValue(t.y);
                duplicateScenery->getTranslation()->getZ()->setValue(t.z);
            }
        }

    }
    return 0;
}





//------------------------------------------------------------------------------------------------
// Name:  GenerateRotatedSceneryPermutations
// Desc:  
//------------------------------------------------------------------------------------------------
int GenerateRotatedSceneryPermutations(dcGenericResource* resource, void* pointer)
{
    // Get the scenery table being edited
    dc::dcList<Scenery>* sceneryList = (dcList<Scenery>*)resource;

    // Prompt the user to select some scenery
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Generate Rotated Scenery Permutations", "Select scenery elements:", sceneryList->getMembers(), &selection))
        return 0;

    // Ask the user how many rotations to create
    size_t rotations;
    if (!dcx::dcxWin32PromptNumericInput(globalEditor()->getTopmostWindowHandle(), "Generate Rotated Scenery Permutations", "How many equally spaced rotations within 360 degrees should be created?",
                                          &rotations, false))
        return 0;

    // Repeat for each selected terrain element
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the source terrain element
        dc::dcList<Scenery>::Element* sourceSceneryElement = (dcList<Scenery>::Element*)*i;
//        Scenery* sourceScenery = (Scenery*)sourceSceneryElement->getImplementation();

        // Save the source scenery into a buffer
        dc::dcBufferStream buffer;
        sourceSceneryElement->save(&buffer);

        // Repeat for each duplicate
        for (size_t i = 0; i < rotations; ++i)
        {
            float angle = (360.0f / rotations) * i;

            // Create the duplicate element
            char nameSuffix[32];
            sprintf_s(nameSuffix, sizeof(nameSuffix), " @ %.02f deg", angle);
            Scenery* duplicateScenery = (Scenery*)sceneryList->generateElement(sourceSceneryElement->getName() + nameSuffix);

            // Copy the source info
            buffer.seek(0);
            duplicateScenery->load(&buffer);

            // Set the rotation value
            duplicateScenery->getRotation()->getY()->setValue(angle);
        }

        // Erase the source scenery
        sceneryList->releaseElement(sourceSceneryElement);
    }

    // Success
    return 1;
}







//------------------------------------------------------------------------------------------------
// Name:  GenerateTerrainCardinalPermutations
// Desc:  
//------------------------------------------------------------------------------------------------
int GenerateTerrainCardinalPermutations(dcGenericResource* resource, void* pointer)
{
    dc::dcList<Terrain>* terrainList = (dcList<Terrain>*)resource;

    // Let the user select terrain elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Generate Cardinal Permutations", "Select terrain elements:", terrainList->getMembers(), &selection))
        return 0;

    // Repeat for each selected terrain element
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the source terrain element
        dc::dcList<Terrain>::Element* sourceTerrainElement = (dcList<Terrain>::Element*)*i;
        //Terrain* sourceTerrain = (Terrain*)sourceTerrainElement->getImplementation();

        // Generate cardinal permutations (in all 4 major directions)
        Terrain* north = (Terrain*)terrainList->generateElement(sourceTerrainElement->getName() + " [n]"),
               * east  = (Terrain*)terrainList->generateElement(sourceTerrainElement->getName() + " [e]"),
               * south = (Terrain*)terrainList->generateElement(sourceTerrainElement->getName() + " [s]"),
               * west  = (Terrain*)terrainList->generateElement(sourceTerrainElement->getName() + " [w]");

        // Save the terrain into a buffer
        dc::dcBufferStream buffer;
        sourceTerrainElement->save(&buffer);

        // Copy data into each terrain
        buffer.seek(0); north->load(&buffer);
        buffer.seek(0); east->load(&buffer);
        buffer.seek(0); west->load(&buffer);
        buffer.seek(0); south->load(&buffer);

        // Generate the direction settings
        north->getDirection()->setValue(TERRAINDIRECTION_NORTH);
        east->getDirection()->setValue(TERRAINDIRECTION_EAST);
        south->getDirection()->setValue(TERRAINDIRECTION_SOUTH);
        west->getDirection()->setValue(TERRAINDIRECTION_WEST);

        // Erase the source terrain element
        terrainList->releaseElement(sourceTerrainElement);
    }

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  AddTerrainLayerToMapLayers
// Desc:  
//------------------------------------------------------------------------------------------------
int AddTerrainLayerToMapLayers(dcGenericResource* resource, void* pointer)
{
    // Get the required resources
    dc::dcList<MapLayer>* mapLayerList = (dcList<MapLayer>*)resource;
    dc::dcList<Terrain>* terrainList = globalEditor()->getRootResource()->getTerrainList();

    // Name this layer
    std::string name;

    // Prompt the resource's current name
    if (!dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Add Terrain Layer",
            "Please enter a name for this terrain layer:", &name))
        return 0;

    // Let the user select terrain elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Add Terrain Layer", "Select terrain elements to fill the layer:", terrainList->getMembers(), &selection))
        return 0;

    // Create the layer
    dc::dcList<MapLayer>::Element* mapLayerElement = (dcList<MapLayer>::Element*)mapLayerList->generateElement(name);

    // Obtain the list into which to insert the terrain map locations
    dc::dcList<MapLocationInfo>* mapLocationList = (dcList<MapLocationInfo>*)mapLayerElement->getLayerOptionsList();

    // Add the terrain options to the list
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the source terrain
        dc::dcList<Terrain>::Element* terrainElement = (dcList<Terrain>::Element*)*i;
        Terrain* terrain = (Terrain*)terrainElement;

        // Add an element to the list
        dc::dcList<MapLocationInfo>::Element* mapLocationElement =
            (dcList<MapLocationInfo>::Element*)mapLocationList->generateElement(terrainElement->getName());
        mapLocationElement->getTerrain1Ref()->setGenericReferencedResource(terrainElement);
        mapLocationElement->getOverwriteTerrain1()->setValue(true);
    }

    // Set the layer's priority
    mapLayerElement->getPriority()->setValue(mapLayerList->getMembers().size());

    // Edit this resource
    EditResourceDialog dialog;
    if (!dialog.execute(mapLayerElement))
    {
        mapLayerList->releaseElement(mapLayerElement);
        return 0;
    }

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  AddTerrainToMapLocation
// Desc:  
//------------------------------------------------------------------------------------------------
int AddTerrainToMapLocation(dcGenericResource* resource, void* pointer)
{
    dc::dcList<MapLocationInfo>* mapLocationList = (dcList<MapLocationInfo>*)resource;
    dc::dcList<Terrain>* terrainList = globalEditor()->getRootResource()->getTerrainList();

    // Let the user select terrain elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Add Terrain to Layer", "Select terrain elements to add to the layer:", terrainList->getMembers(), &selection))
        return 0;

    // Add the terrain options to the list
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the source terrain
        dc::dcList<Terrain>::Element* terrainElement = (dcList<Terrain>::Element*)*i;
        Terrain* terrain = (Terrain*)terrainElement;

        // Add an element to the list
        dc::dcList<MapLocationInfo>::Element* mapLocationElement =
            (dcList<MapLocationInfo>::Element*)mapLocationList->generateElement(terrainElement->getName());
        mapLocationElement->getTerrain1Ref()->setGenericReferencedResource(terrainElement);
        mapLocationElement->getOverwriteTerrain1()->setValue(true);
    }

    // Success
    return 1;
}





//------------------------------------------------------------------------------------------------
// Name:  AddSceneryToMapLocation
// Desc:  
//------------------------------------------------------------------------------------------------
int AddSceneryToMapLocation(dcGenericResource* resource, void* pointer)
{
    dc::dcList<MapLocationInfo>* mapLocationList = (dcList<MapLocationInfo>*)resource;
    dc::dcList<Scenery>* sceneryList = globalEditor()->getRootResource()->getSceneryList();

    // Let the user select scenery elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Add Terrain to Layer", "Select scenery elements to add to the layer:", sceneryList->getMembers(), &selection))
        return 0;

    // Add the scenery options to the list
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the source scenery
        dc::dcList<Scenery>::Element* sceneryElement = (dcList<Scenery>::Element*)*i;
//        Scenery* scenery = (Scenery*)sceneryElement->getImplementation();

        // Add an element to the list
        dc::dcList<MapLocationInfo>::Element* mapLocationElement =
            (dcList<MapLocationInfo>::Element*)mapLocationList->generateElement(sceneryElement->getName());
        mapLocationElement->getSceneryRef()->setGenericReferencedResource(sceneryElement);
        mapLocationElement->getOverwriteScenery()->setValue(true);
    }

    // Success
    return 1;
}




//------------------------------------------------------------------------------------------------
// Name:  AddSceneryLayerToMapLayers
// Desc:  
//------------------------------------------------------------------------------------------------
int AddSceneryLayerToMapLayers(dcGenericResource* resource, void* pointer)
{
    // Get the required resources
    dc::dcList<MapLayer>* mapLayerList = (dcList<MapLayer>*)resource;
    dc::dcList<Scenery>* sceneryList = globalEditor()->getRootResource()->getSceneryList();

    // Name this layer
    std::string name;

    // Prompt the resource's current name
    if (!dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Add Scenery Layer",
            "Please enter a name for this scenery layer:", &name))
        return 0;

    // Let the user select terrain elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Add Scenery Layer", "Select scenery elements to fill the layer:", sceneryList->getMembers(), &selection))
        return 0;

    // Create the layer
    dc::dcList<MapLayer>::Element* mapLayerElement = (dcList<MapLayer>::Element*)mapLayerList->generateElement(name);

    // Obtain the list into which to insert the terrain map locations
    dc::dcList<MapLocationInfo>* mapLocationList = (dcList<MapLocationInfo>*)mapLayerElement->getLayerOptionsList();

    // Add the terrain options to the list
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the source terrain
        dc::dcList<Scenery>::Element* sceneryElement = (dcList<Scenery>::Element*)*i;
//        Scenery* scenery = (Scenery*)sceneryElement->getImplementation();

        // Add an element to the list
        dc::dcList<MapLocationInfo>::Element* mapLocationElement =
            (dcList<MapLocationInfo>::Element*)mapLocationList->generateElement(sceneryElement->getName());
        mapLocationElement->getSceneryRef()->setGenericReferencedResource(sceneryElement);
        mapLocationElement->getOverwriteScenery()->setValue(true);
    }

    // Set the layer's priority
    mapLayerElement->getPriority()->setValue(mapLayerList->getMembers().size());

    // Edit this resource
    EditResourceDialog dialog;
    if (!dialog.execute(mapLayerElement))
    {
        mapLayerList->releaseElement(mapLayerElement);
        return 0;
    }

    // Success
    return 1;
}





//------------------------------------------------------------------------------------------------
// Name:  SetMapLocationNavigability
// Desc:  
//------------------------------------------------------------------------------------------------
int SetMapLocationNavigability(dcGenericResource* resource, void* pointer)
{
    // Get the list
    dc::dcList<MapLocationInfo>* mapLocationList = (dcList<MapLocationInfo>*)resource;

    // Let the user select map locations
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Set Navigability", "Select locations:", mapLocationList->getMembers(), &selection))
        return 0;

    // Add entries to the options table
    std::list<std::string> options;
    const char* stringValue = dc::dcEnum<MapNavigability>::getString((MapNavigability)0);
    int index = 1;
    for (index = 1; stringValue != 0; ++index)
    {
        options.push_back(stringValue);
        stringValue = dc::dcEnum<MapNavigability>::getString((MapNavigability)index);
    }

    // Set the current selection
    int enumSelection;

    // Let the user pick the value
    if (!dcxWin32PromptForSelection(globalEditor()->getTopmostWindowHandle(),
                                     "Set Navigability", "Pick a navigability for the selected locations:",
                                     &options, &enumSelection, false))
        return 0;

    // Iterate through the list
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the source scenery
        dc::dcList<MapLocationInfo>::Element* mapLocationElement = (dcList<MapLocationInfo>::Element*)*i;
        mapLocationElement->getNavigability()->setValue((MapNavigability)enumSelection);
        mapLocationElement->getOverwriteNavigability()->setValue(true);
    }

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  SetMapLocationElevation1
// Desc:  
//------------------------------------------------------------------------------------------------
int SetMapLocationElevation1(dcGenericResource* resource, void* pointer)
{
    // Get the list
    dc::dcList<MapLocationInfo>* mapLocationList = (dcList<MapLocationInfo>*)resource;

    // Let the user select map locations
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Set Elevation 1", "Select locations:", mapLocationList->getMembers(), &selection))
        return 0;

    // Get the list of height entries
    const dc::dcGenericResource::Array& heightMembers = globalEditor()->getRootResource()->getElevations()->getMembers();

    // Let the user pick the value
    dc::dcGenericResource* heightSelection = NULL;
    if (!PromptSelectSingleResource("Set Elevation 1", "Pick an elevation of the first terrain layer for the selected "\
                                     "locations.  If you select nothing but press OK, the height entry for the locations "\
                                     "will be cleared and the overwrite flag disabled.",
                                     heightMembers, &heightSelection))
        return 0;

    // Iterate through the list
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the source scenery
        dc::dcList<MapLocationInfo>::Element* mapLocationElement = (dcList<MapLocationInfo>::Element*)*i;
        mapLocationElement->getElevation1()->setGenericReferencedResource(heightSelection);
        mapLocationElement->getOverwriteElevation1()->setValue(heightSelection ? true : false);
    }

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  SetMapLocationElevation2
// Desc:  
//------------------------------------------------------------------------------------------------
int SetMapLocationElevation2(dcGenericResource* resource, void* pointer)
{
    // Get the list
    dc::dcList<MapLocationInfo>* mapLocationList = (dcList<MapLocationInfo>*)resource;

    // Let the user select map locations
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Set Elevation 2", "Select locations:", mapLocationList->getMembers(), &selection))
        return 0;

    // Get the list of height entries
    const dc::dcGenericResource::Array& heightMembers = globalEditor()->getRootResource()->getElevations()->getMembers();

    // Let the user pick the value
    dc::dcGenericResource* heightSelection = NULL;
    if (!PromptSelectSingleResource("Set Elevation 2", "Pick an elevation of the second terrain layer for the selected "\
                                     "locations.  If you select nothing but press OK, the height entry for the locations "\
                                     "will be cleared and the overwrite flag disabled.",
                                     heightMembers, &heightSelection))
        return 0;

    // Iterate through the list
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the source scenery
        dc::dcList<MapLocationInfo>::Element* mapLocationElement = (dcList<MapLocationInfo>::Element*)*i;
        mapLocationElement->getElevation2()->setGenericReferencedResource(heightSelection);
        mapLocationElement->getOverwriteElevation2()->setValue(heightSelection ? true : false);
    }

    // Success
    return 1;
}





//------------------------------------------------------------------------------------------------
// Name:  AddResourcePacks
// Desc:  
//------------------------------------------------------------------------------------------------
int AddResourcePacks(dcGenericResource* resource, void* pointer)
{
    // Get the resource pack table
    dc::dcTable<ResourcePack>* table = (dcTable<ResourcePack>*)resource;

    // Storage location for the files to load
    std::string path;
    std::list<std::string> files;

    // Let the user select one or more pack files
    if (dcxOpenMultipleFilesDialog(globalEditor()->getTopmostWindowHandle(), "Select Pack Files", "Project Volucris Pack Files (*.pvk)\0*.pvk\0\0",
                                    &path, &files))
    {
        // Import each of the pack files selected
        for (std::list<std::string>::iterator i = files.begin(); i != files.end(); ++i)
        {
            // Add this pack to the table
            ResourcePack* pack = table->create<ResourcePack>(*i);

            // Set the pack source file
            pack->getSourceFile()->setValue(path + "\\" + *i);

            // Load up a temporary copy of the project with data from the pack
            std::string packName;
            ProjectVolucris packProject;
            if (APP_ERROR(!pack->loadPackResources(&packName, &packProject))("Unable to load resource pack %s", *i))
                continue;

            // Rename the resource pack.  If this fails, the pack must already exist
            if (APP_ERROR(!pack->getOwner()->setName(packName))
                ("Resource pack %s already exists in project", packName.c_str()))
            {
                // Erase this pack
                table->releaseElement(pack->getOwner());
            }

            // Merge the pack project into the working project
            dc::dcResourceTrader trader;
            trader.merge(globalEditor()->getRootResource(), &packProject, NULL, NULL);
        }
    }

    return 1;
}





//------------------------------------------------------------------------------------------------
// Name:  AddTransitionGroupRules_AddReferences
// Desc:  
//------------------------------------------------------------------------------------------------
void AddTransitionGroupRules_AddReferences(
    const dc::dcGenericResource::Array& referencedObjects,
    dc::dcList<dcList<Terrain>::Reference>* list
   )
{
    for (dcGenericResource::Array::const_iterator i = referencedObjects.begin(); i != referencedObjects.end(); ++i)
    {
        dc::dcGenericReference* reference = (dcGenericReference*)list->generateElement((*i)->getName());
        reference->setGenericReferencedResource(*i);
    }
}





enum Transition
{
    NORTH_WEST_OUTSIDE,
    NORTH_EAST_OUTSIDE,
    SOUTH_EAST_OUTSIDE,
    SOUTH_WEST_OUTSIDE,
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NORTH_WEST_INSIDE,
    NORTH_EAST_INSIDE,
    SOUTH_EAST_INSIDE,
    SOUTH_WEST_INSIDE,
    CENTRAL,
    SURROUNDING,
    NUMBER_OF_TRANSITIONS
};


void AddTransitionRulesForTerrain(const std::string namePrefix,
                                  dc::dcList<TerrainRule>* rulesList,
                                  dc::dcGenericResource::Array transition_terrain[NUMBER_OF_TRANSITIONS]) {

  // Create elements for all of the rules
  dc::dcList<TerrainRule>::Element* rules[12] = {

      // Priority 0:  The outside corners
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[nw-o]"),
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[ne-o]"),
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[se-o]"),
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[sw-o]"),

      // Priority 1:  The edges
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[n]"),
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[e]"),
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[s]"),
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[w]"),

      // Priority 2:  The inside corners
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[nw-i]"),
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[ne-i]"),
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[se-i]"),
      (dcList<TerrainRule>::Element*)rulesList->generateElement(namePrefix + "[sw-i]"),

  };

  // First of all, set the canvas types for all of the rules
  int priority = rulesList->getMembers().size();
  for (int i = 0; i < 12; ++i)
  {
      AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[i]->getCanvasTypes());
      rules[i]->getPriority()->setValue(priority);
  }

  enum NeighborDirection
  {
      NEIGHBOR_NW,
      NEIGHBOR_N,
      NEIGHBOR_NE,
      NEIGHBOR_E,
      NEIGHBOR_SE,
      NEIGHBOR_S,
      NEIGHBOR_SW,
      NEIGHBOR_W
  };

  // North-West Outside Corner
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[0]->getNeighborTypes(NEIGHBOR_N));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[0]->getNeighborTypes(NEIGHBOR_E));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[0]->getNeighborTypes(NEIGHBOR_S));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[0]->getNeighborTypes(NEIGHBOR_W));
  AddTransitionGroupRules_AddReferences(transition_terrain[NORTH_WEST_OUTSIDE], rules[0]->getFillTypes());

  // North-East Outside Corner
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[1]->getNeighborTypes(NEIGHBOR_N));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[1]->getNeighborTypes(NEIGHBOR_E));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[1]->getNeighborTypes(NEIGHBOR_S));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[1]->getNeighborTypes(NEIGHBOR_W));
  AddTransitionGroupRules_AddReferences(transition_terrain[NORTH_EAST_OUTSIDE], rules[1]->getFillTypes());

  // South-East Outside Corner
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[2]->getNeighborTypes(NEIGHBOR_N));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[2]->getNeighborTypes(NEIGHBOR_E));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[2]->getNeighborTypes(NEIGHBOR_S));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[2]->getNeighborTypes(NEIGHBOR_W));
  AddTransitionGroupRules_AddReferences(transition_terrain[SOUTH_EAST_OUTSIDE], rules[2]->getFillTypes());

  // South-West Outside Corner
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[3]->getNeighborTypes(NEIGHBOR_N));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[3]->getNeighborTypes(NEIGHBOR_E));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[3]->getNeighborTypes(NEIGHBOR_S));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[3]->getNeighborTypes(NEIGHBOR_W));
  AddTransitionGroupRules_AddReferences(transition_terrain[SOUTH_WEST_OUTSIDE], rules[3]->getFillTypes());

  // North
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[4]->getNeighborTypes(NEIGHBOR_N));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[4]->getNeighborTypes(NEIGHBOR_S));
  AddTransitionGroupRules_AddReferences(transition_terrain[NORTH], rules[4]->getFillTypes());

  // East
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[5]->getNeighborTypes(NEIGHBOR_E));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[5]->getNeighborTypes(NEIGHBOR_W));
  AddTransitionGroupRules_AddReferences(transition_terrain[EAST], rules[5]->getFillTypes());

  // South
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[6]->getNeighborTypes(NEIGHBOR_N));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[6]->getNeighborTypes(NEIGHBOR_S));
  AddTransitionGroupRules_AddReferences(transition_terrain[SOUTH], rules[6]->getFillTypes());

  // West
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[7]->getNeighborTypes(NEIGHBOR_E));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[7]->getNeighborTypes(NEIGHBOR_W));
  AddTransitionGroupRules_AddReferences(transition_terrain[WEST], rules[7]->getFillTypes());

  // North-West Inside Corner
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[8]->getNeighborTypes(NEIGHBOR_NW));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[8]->getNeighborTypes(NEIGHBOR_N));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[8]->getNeighborTypes(NEIGHBOR_W));
  AddTransitionGroupRules_AddReferences(transition_terrain[NORTH_WEST_INSIDE], rules[8]->getFillTypes());

  // North-East Inside Corner
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[9]->getNeighborTypes(NEIGHBOR_N));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[9]->getNeighborTypes(NEIGHBOR_NE));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[9]->getNeighborTypes(NEIGHBOR_E));
  AddTransitionGroupRules_AddReferences(transition_terrain[NORTH_EAST_INSIDE], rules[9]->getFillTypes());

  // South-East Inside Corner
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[10]->getNeighborTypes(NEIGHBOR_E));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[10]->getNeighborTypes(NEIGHBOR_SE));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[10]->getNeighborTypes(NEIGHBOR_S));
  AddTransitionGroupRules_AddReferences(transition_terrain[SOUTH_EAST_INSIDE], rules[10]->getFillTypes());

  // South-West Inside Corner
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[11]->getNeighborTypes(NEIGHBOR_S));
  AddTransitionGroupRules_AddReferences(transition_terrain[SURROUNDING], rules[11]->getNeighborTypes(NEIGHBOR_SW));
  AddTransitionGroupRules_AddReferences(transition_terrain[CENTRAL], rules[11]->getNeighborTypes(NEIGHBOR_W));
  AddTransitionGroupRules_AddReferences(transition_terrain[SOUTH_WEST_INSIDE], rules[11]->getFillTypes());

  // Name prefix
  // Base priority
  //  0 - the central canvas types
  //  1 - the transition-to types
  //  2 - east
  //  3 - north
  //  4 - northeast inside
  //  5 - northeast outside
  //  6 - northwest inside
  //  7 - northwest outside
  //  8 - south
  //  9 - southeast inside
  // 10 - southeast outside
  // 11 - southwest inside
  // 12 - southwest outside
  // 13 - west
}

//------------------------------------------------------------------------------------------------
// Name:  AddTransitionGroupRules
// Desc:  This method creates multiple transition rules
//------------------------------------------------------------------------------------------------
int AddTransitionGroupRules(dcGenericResource* resource, void* pointer)
{
    // TODO: resource->typecast<dcList<TerrainRule>>
    // (add this to directcraft! requires implementation linking...)
    dc::dcList<TerrainRule>* rulesList = (dcList<TerrainRule>*)resource;

    // The entries in the terrain table
    const dc::dcGenericResource::Array& terrainTableMembers = globalEditor()->getRootResource()->getTerrainList()->getMembers();

    // Ask the user for a name prefix for these rules
    std::string namePrefix = "";
    if (!dcx::dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(),
        "Add Transition Rules", "Please enter a prefix for all of the rules that will be created:", &namePrefix)) return 0;

    // Ask the user for the range of transition resources
    dc::dcGenericResource::Array transitionResources;
    if (!PromptSelectMultipleResources(
            "Add Transition Rules", "Pick transition (non-central, non-surrounding) terrain types from which all of the\n"\
                                    "transitions will be picked.  This is only to speed up this method, so if you're not\n"\
                                    "sure what to do, just select everything.",
            terrainTableMembers, &transitionResources, false))
        return 0;

    // Keep track of the terrain type selections
    dc::dcGenericResource::Array typeSelection[NUMBER_OF_TRANSITIONS];


    // Ask for all of the selections
    for (int i = 0; i < NUMBER_OF_TRANSITIONS; ++i)
    {
        // do the central and surrounding first
        int index = (i+12) % NUMBER_OF_TRANSITIONS;
        const char* terrainTypeName = 0;

        // Get the name of this type
        switch(index)
        {
            case EAST:                  terrainTypeName = "east transition"; break;
            case NORTH:                 terrainTypeName = "north transition"; break;
            case NORTH_EAST_INSIDE:     terrainTypeName = "north-east inside transition"; break;
            case NORTH_EAST_OUTSIDE:    terrainTypeName = "north-east outside transition"; break;
            case NORTH_WEST_INSIDE:     terrainTypeName = "north-west inside transition"; break;
            case NORTH_WEST_OUTSIDE:    terrainTypeName = "north-west outside transition"; break;
            case SOUTH:                 terrainTypeName = "south transition"; break;
            case SOUTH_EAST_INSIDE:     terrainTypeName = "south-east inside transition"; break;
            case SOUTH_EAST_OUTSIDE:    terrainTypeName = "south-east outside transition"; break;
            case SOUTH_WEST_INSIDE:     terrainTypeName = "south-west inside transition"; break;
            case SOUTH_WEST_OUTSIDE:    terrainTypeName = "south-west outside transition"; break;
            case WEST:                  terrainTypeName = "west transition"; break;
            case CENTRAL:               terrainTypeName = "central"; break;
            case SURROUNDING:           terrainTypeName = "surrounding"; break;
        }

        // Construct a prompt
        std::string prompt = std::string("Pick the ") + terrainTypeName + " terrain types";

        // Set the default selection for the transition types
        const dc::dcGenericResource::Array& options = index < 12 ? transitionResources : terrainTableMembers;
        //if (index < 12)
        //    typeSelection[index].assign(transitionResources.begin(), transitionResources.end());

        // Ask the user
        if (!PromptSelectMultipleResources("Add Transition Rules", prompt.c_str(), options, &typeSelection[index], true))
            return 0;
    }

    AddTransitionRulesForTerrain(namePrefix, rulesList, typeSelection);
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  SetSceneryXYZScaling_I
// Desc:  
//------------------------------------------------------------------------------------------------
int SetSceneryXYZScaling_I(dcGenericResource* resource, void* pointer)
{
    // Get the scenery element
    //dcInterface<Scenery>* sceneryInterface = (dcInterface<Scenery>*)resource;
    //Scenery* scenery = sceneryInterface->getImplementation();
    Scenery* scenery = (Scenery*)resource;

    // Ask the user for a scaling value
    float scalingFactor = 0.0f;
    if (!dcx::dcxWin32PromptNumericInput(globalEditor()->getTopmostWindowHandle(),
                                          "Set X/Y/Z Scaling",
                                          "Enter a value for this scenery's X/Y/Z scaling:",
                                          &scalingFactor, false))
        return 0;

    // Set the scaling values
    dcx::dcxVector3<dcFloat>* scaling = scenery->getScaling();
    scaling->getX()->setValue(scalingFactor);
    scaling->getY()->setValue(scalingFactor);
    scaling->getZ()->setValue(scalingFactor);

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  SetSceneryXZScaling_I
// Desc:  
//------------------------------------------------------------------------------------------------
int SetSceneryXZScaling_I(dcGenericResource* resource, void* pointer)
{
    // Get the scenery element
    //dcInterface<Scenery>* sceneryInterface = (dcInterface<Scenery>*)resource;
    //Scenery* scenery = sceneryInterface->getImplementation();
    Scenery* scenery = (Scenery*)resource;

    // Ask the user for a scaling value
    float scalingFactor = 0.0f;
    if (!dcx::dcxWin32PromptNumericInput(globalEditor()->getTopmostWindowHandle(),
                                          "Set X/Z Scaling",
                                          "Enter a value for this scenery's X/Z scaling:",
                                          &scalingFactor, false))
        return 0;

    // Set the scaling values
    dcx::dcxVector3<dcFloat>* scaling = scenery->getScaling();
    scaling->getX()->setValue(scalingFactor);
    scaling->getZ()->setValue(scalingFactor);

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  SetSceneryTransformAsDefaultForMesh_I
// Desc:  
//------------------------------------------------------------------------------------------------
int SetSceneryTransformAsDefaultForMesh_I(dcGenericResource* resource, void* pointer)
{
    // Get the scenery element
    //dcInterface<Scenery>* sceneryInterface = (dcInterface<Scenery>*)resource;
    //Scenery* scenery = sceneryInterface->getImplementation();
    Scenery* scenery = (Scenery*)resource;

    // Ask the user whether or not they want to do this
    int result = MessageBox(globalEditor()->getTopmostWindowHandle(),
                             "This will combine all of the scaling, rotation and translation\n"\
                             "factors assigned to this scenery element with its source mesh, assign\n"
                             "the resulting values to the mesh and reset this scenery's transform.\n\n"
                             "Do you want to continue?",
                             "Set Transform as Mesh Default", MB_YESNO);

    // If the user didn't hit yes (or there was an error) exit 
    if (result != IDYES) return 0;

    // Get the source mesh
    dc::dcTable<Mesh>::Element* mesh = scenery->getMeshRef()->getReferencedResource();

    // If the source mesh is invalid, don't do anything
    if (!mesh || !mesh->getImplementation())
    {
        MessageBox(globalEditor()->getTopmostWindowHandle(),
                    "Source mesh not found", "Set Scaling As Mesh Default", MB_OK);
        return 0;
    }

    // Obtain the transform factors
    dcxVector3<dcFloat>* meshTranslation = mesh->getImplementation()->getTranslation();
    dcxVector3<dcFloat>* sceneryTranslation = scenery->getTranslation();
    dcxVector3<dcFloat>* meshScaling = mesh->getImplementation()->getScaling();
    dcxVector3<dcFloat>* sceneryScaling = scenery->getScaling();
    dcxVector3<dcFloat>* meshRotation = mesh->getImplementation()->getRotation();
    dcxVector3<dcFloat>* sceneryRotation = scenery->getRotation();

    // Multiply the factors and assign scaling values
    for (size_t i = 0; i < 3; ++i)
    {
        // Combine the scaling
        meshScaling->getByIndex(i)->setValue(meshScaling->getByIndex(i)->getValue() *
                                             sceneryScaling->getByIndex(i)->getValue());
        sceneryScaling->getByIndex(i)->setValue(1.0f);

        // Combine the rotation
        meshRotation->getByIndex(i)->setValue(meshRotation->getByIndex(i)->getValue() +
                                              sceneryRotation->getByIndex(i)->getValue());
        meshTranslation->getByIndex(i)->setValue(0.0f);

        // Combine the rotation
        meshTranslation->getByIndex(i)->setValue(meshTranslation->getByIndex(i)->getValue() +
                                                 sceneryTranslation->getByIndex(i)->getValue());
        sceneryTranslation->getByIndex(i)->setValue(0.0f);
    }

    // Success!
    return 1;
}





//------------------------------------------------------------------------------------------------
// Name:  DuplicateCollectionEntries
// Desc:  
//------------------------------------------------------------------------------------------------
int DuplicateCollectionEntries(dcGenericResource* resource, void* pointer)
{
    // Obtain the collection
    dc::dcGenericCollection* collection = (dcGenericCollection*)resource;

    // Ask the user to select elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Duplicate Collection Elements",
        "Pick some elements of this collection to duplicate", collection->getMembers(),
        &selection, false))
        return 0;

    if (selection.size() == 1) {
      std::string name;
      if (!dcx::dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Duplicate Collection Elements",
              "Enter the name for the new element", &name))
          return false;
      dc::dcGenericResource* source = selection.front();
      dc::dcBufferStream buffer;
      buffer.seek(0);
      CONFIRM(source->save(&buffer)) else return -1;
      buffer.seek(0);
      dc::dcGenericResource* duplicate = collection->generateElement(name);
      CONFIRM(duplicate && duplicate->load(&buffer)) else return 1;
      if (duplicate->getName().compare(name) != 0) { // did we have to rename b/c of a conflict?
        char message[512];
        sprintf_s(message, 512, "Name '%s' already exists, so '%s' was used instead", name.c_str(), duplicate->getName().c_str());
        MessageBox(globalEditor()->getTopmostWindowHandle(), message, "Duplicate Collection Elements", MB_OK);
      }

    } else {

  // If the user enters a malformed template, we jump back here
  LBL_ENTERNAMETEMPLATE:

      // Enter the new name
      std::string nameTemplate;
      if (!dcx::dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(), "Duplicate Collection Elements",
              "Enter the name template for the selected resources, using ## to indicate\n"
              "where to place the number, and $$ to notate the resource's old name", &nameTemplate))
          return false;

      // Check to make sure the name template has some sort of key in it
      if (nameTemplate.find("##") >= nameTemplate.npos && nameTemplate.find("$$") >= nameTemplate.npos)
      {
          // Let the user know what's up
          MessageBox(globalEditor()->getTopmostWindowHandle(), "The name template must contain either ## or $$", "Bulk Rename - Error", MB_OK|MB_ICONEXCLAMATION);

          // Yep, it's a goto.  Sorry kids.
          goto LBL_ENTERNAMETEMPLATE;
      }

      // Use this buffer to duplicate resources
      dc::dcBufferStream buffer;

      // Duplicate the resources, and add the created ones to a new list
      dc::dcGenericResource::Array createdResources;
      dc::dcGenericResource::Array::iterator i = selection.begin();
      for (size_t index = 0; i != selection.end(); ++i, ++index)
      {
          // Obtain the original resource
          dc::dcGenericResource* source = (*i);

          // Save the resource to the buffer stream
          buffer.seek(0);
          if (APP_ERROR(!source || !source->save(&buffer))("There was a problem duplicating %s", source ? source->getName().c_str() : "???"))
              continue;

          // Generate the new name for this resource
          std::string name;
          ApplyBulkRenamePattern(&name, nameTemplate, source->getName(), index);

          // Create a new resource
          dc::dcGenericResource* duplicate = collection->generateElement(name);

          // Load this resource's data
          buffer.seek(0);
          if (APP_ERROR(!duplicate || !duplicate->load(&buffer))
              ("There was a problem creating %s from %s", name.c_str(), source->getName().c_str()))
              continue;
      }
    }

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  EnableTextureFlipHorizontal
// Desc:  
//------------------------------------------------------------------------------------------------
int EnableTextureFlipHorizontal(dcGenericResource* resource, void* pointer)
{
    // Get the texture table
    dc::dcTable<Texture>* textureTable = (dcTable<Texture>*)resource;

    // Let the user select elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Enable Flip Horizontal",
        "Pick textures to enable the flip-horizontal flag for:",
        textureTable->getMembers(), &selection, false))
        return 0;

    // Perform the action across the selection
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the texture
        dc::dcTable<Texture>::Element* textureElement = (dcTable<Texture>::Element*)(*i);
        Texture* texture = textureElement->getImplementation();

        // Perform the action
        texture->getFlipHorizontal()->setValue(true);
    }

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  DisableTextureFlipHorizontal
// Desc:  
//------------------------------------------------------------------------------------------------
int DisableTextureFlipHorizontal(dcGenericResource* resource, void* pointer)
{
    // Get the texture table
    dc::dcTable<Texture>* textureTable = (dcTable<Texture>*)resource;

    // Let the user select elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Disable Flip Horizontal",
        "Pick textures to disable the flip-horizontal flag for:",
        textureTable->getMembers(), &selection, false))
        return 0;

    // Perform the action across the selection
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the texture
        dc::dcTable<Texture>::Element* textureElement = (dcTable<Texture>::Element*)(*i);
        Texture* texture = textureElement->getImplementation();

        // Perform the action
        texture->getFlipHorizontal()->setValue(false);
    }

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  EnableTextureFlipVertical
// Desc:  
//------------------------------------------------------------------------------------------------
int EnableTextureFlipVertical(dcGenericResource* resource, void* pointer)
{
    // Get the texture table
    dc::dcTable<Texture>* textureTable = (dcTable<Texture>*)resource;

    // Let the user select elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Enable Flip Vertical",
        "Pick textures to enable the flip-vertical flag for:",
        textureTable->getMembers(), &selection, false))
        return 0;

    // Perform the action across the selection
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the texture
        dc::dcTable<Texture>::Element* textureElement = (dcTable<Texture>::Element*)(*i);
        Texture* texture = textureElement->getImplementation();

        // Perform the action
        texture->getFlipVertical()->setValue(true);
    }

    // Success
    return 1;
}






//------------------------------------------------------------------------------------------------
// Name:  DisableTextureFlipVertical
// Desc:  
//------------------------------------------------------------------------------------------------
int DisableTextureFlipVertical(dcGenericResource* resource, void* pointer)
{
    // Get the texture table
    dc::dcTable<Texture>* textureTable = (dcTable<Texture>*)resource;

    // Let the user select elements
    dc::dcGenericResource::Array selection;
    if (!PromptSelectMultipleResources("Disable Flip Vertical",
        "Pick textures to disable the flip-vertical flag for:",
        textureTable->getMembers(), &selection, false))
        return 0;

    // Perform the action across the selection
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        // Get the texture
        dc::dcTable<Texture>::Element* textureElement = (dcTable<Texture>::Element*)(*i);
        Texture* texture = textureElement->getImplementation();

        // Perform the action
        texture->getFlipVertical()->setValue(false);
    }

    // Success
    return 1;
}



//------------------------------------------------------------------------------------------------
// Name:  EditActorType
// Desc:  
//------------------------------------------------------------------------------------------------
int EditActorType(dcGenericResource* resource, void* pointer)
{
    dc::dcList<ActorType>::Element* element = (dcList<ActorType>::Element*)resource;

    element->runEditDialog();

    return 1;
}


//------------------------------------------------------------------------------------------------
int NormalizeActorActionAnimations(dcGenericResource* resource, void* pointer) {
  dc::dcList<ActorType>::Element* element = (dcList<ActorType>::Element*)resource;

  const ActorAnimation animations_to_normalize[] = { ACTORANIMATION_ATTACK1, ACTORANIMATION_MISC4 };
  const int number_to_normalize = sizeof(animations_to_normalize) / sizeof(ActorAnimation);

  for (int i = 0; i < number_to_normalize; ++i) {
    ActorAnimation animation_to_normalize = animations_to_normalize[i];
    dc::dcList<UnseenKeyframeAnimation>::Element* animation =
        element->getActionAnimation(animation_to_normalize);
    if (!animation) continue; // skip animations that don't exist
    UnseenSkeletalAnimation unseen_animation;
    if (!animation->loadAnimation(&unseen_animation)) {
      std::string text = "Couldn't load animation ";
      text.append(dcEnum<ActorAnimation>::getString(animation_to_normalize));
      MessageBox(globalEditor()->getTopmostWindowHandle(), text.c_str(), "Unable to normalize", MB_OK);
      continue;
    }
    double length_in_seconds = unseen_animation.info.tickLength * unseen_animation.info.secondsPerTick;
    element->getSpeedFactor(animation_to_normalize)->setValue(length_in_seconds);
    element->getSpeedOffset(animation_to_normalize)->setValue(0.0f);
    deallocateUnseenSkeletalAnimation(&unseen_animation);
  }

  return 1;
}

//------------------------------------------------------------------------------------------------
int NormalizeAllActorActionAnimations(dcGenericResource* resource, void* pointer) {
  dc::dcList<ActorType>* list = (dcList<ActorType>*)resource;
  const dc::dcGenericResource::Array& members = list->getMembers();
  for (dcGenericResource::Array::const_iterator i = members.begin(); i != members.end(); ++i) {
    NormalizeActorActionAnimations(*i,NULL);
  }
  return 1;
}




//------------------------------------------------------------------------------------------------
// Name:  SetSkinEntryTextures
// Desc:  Allows the user to set the texture for multiple skin entries at once
//------------------------------------------------------------------------------------------------
int SetSkinEntryTextures(dcGenericResource* resource, void* pointer)
{
    Skin::Map* skinMap = (Skin::Map*)resource;

    // Select entries
    dc::dcGenericResource::Array selection;
    Skin::Map::Value reference;
    if (PromptSelectMultipleResources("Set Textures", "Select entries from the skin map", skinMap->getMembers(),
        &selection, false) &&
        SelectReference<Skin::Map::Value>((dcGenericResource*)&reference, 0))
    {
        Skin::Map::Value::ResourceType* referencedResource = reference.getReferencedResource();

        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
        {
            Skin::Map::Element* element = (Skin::Map::Element*)*i;
            element->getValue()->setReferencedResource(referencedResource);
        }
        return 1;
    }
    else
        return 0;
}






//------------------------------------------------------------------------------------------------
// Name:  ImportMultipleWAVSounds
// Desc:  
//------------------------------------------------------------------------------------------------
int ImportMultipleWAVSounds(dcGenericResource* resource, void* pointer)
{
    dc::dcList<WAVSound>* wavSoundList = (dcList<WAVSound>*)resource;

    std::string path;
    std::list<std::string> files;

    if (dcxOpenMultipleFilesDialog(globalEditor()->getTopmostWindowHandle(),
        "Select Images", "WAV Sound Files (*.WAV)\0*.WAV\0\0",
                                    &path, &files))
    {
        for (std::list<std::string>::iterator i = files.begin(); i != files.end(); ++i)
        {
            WAVSound* wavSound = (WAVSound*)wavSoundList->generateElement(*i);
            wavSound->getSourceFile()->setValue(path + "\\" + *i);
        }

        return 1;
    }
    else
        return 0;
}





int CopyCollectionElementMembers(dcGenericResource* resource, void* pointer)
{
    dc::dcGenericCollection* collection = (dcGenericCollection*)resource->getOwner();
    if (APP_ERROR(!collection->isCollection())("Specified resource is not a member of a collection")) return 0;

    // Select the members of this resource to copy
    dc::dcGenericResource::Array members;
    if (!PromptSelectMultipleResources("Copy Members", "Pick the members to copy", resource->getMembers(), &members, false))
        return 0;

    // Select the target elements to which to copy the members
    dc::dcGenericResource::Array elements;
    if (!PromptSelectMultipleResources("Copy Members", "Pick the elements to which to copy the members", collection->getMembers(), &elements, false))
        return 0;

    // Copy the values over
    for (dcGenericResource::Array::iterator elementIterator = elements.begin(); elementIterator != elements.end(); ++elementIterator)
    {
        dc::dcGenericResource* targetElement = *elementIterator;
        for (dcGenericResource::Array::iterator memberIterator = members.begin(); memberIterator != members.end(); ++memberIterator)
        {
            dc::dcBufferStream bufferStream;
            dc::dcGenericResource* member = (dcGenericResource*)(*memberIterator);
            CONFIRM(member->save(&bufferStream)) else continue;

            // Find the element by name
            for (dcGenericResource::Array::const_iterator tgt = targetElement->getMembers().begin();
                                                          tgt != targetElement->getMembers().end(); ++tgt)
            {
                dc::dcGenericResource* targetMember = *tgt;
                if (targetMember->getName().compare(member->getName()) == 0)
                {
                    bufferStream.seek(0);
                    targetMember->load(&bufferStream);
                }
            }
        }
    }

    return 1;
}





int AddMultiplePlayerClassLevelEffects(dcGenericResource* resource, void* pointer)
{
    PlayerCharacterClassLevelSpells* collection = (PlayerCharacterClassLevelSpells*)resource;

    // Ask for the level and selected effects
    int level;

    // Select entries
    dc::dcGenericResource::Array selection;
    Skin::Map::Value reference;
    if (dcx::dcxWin32PromptNumericInput(globalEditor()->getTopmostWindowHandle(), "Add Multiple Spells", "Level?", &level, false) &&
        PromptSelectMultipleResources("Add Spells", "Select Spells", globalEditor()->getRootResource()->getSpellsList()->getMembers(),
        &selection, false))
    {
        // Create new entries for each of the effects
        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
        {
            char levelStr[16];
            std::string elementName = std::string(_ltoa(level, levelStr, 10)) + " - " + (*i)->getName();
            PlayerCharacterClassLevelSpells::Element* element = (PlayerCharacterClassLevelSpells::Element*)collection->generateElement(elementName);
            element->set(level, (dcList<Evidyon::Spell>::Element*)(*i));
        }

        return 1;
    }
    else
        return 0;
}



int SetItemSceneryRepresentation(dcGenericResource* resource, void* param)
{
    // Select scenery representation
    dc::dcList<Item>* itemList = (dcList<Item>*)resource;

    // Select entries
    dc::dcGenericResource::Array selection;
    if (PromptSelectMultipleResources("Set Item Scenery", "Select entries for which to set the scenery", itemList->getMembers(),
        &selection, false))
    {
        dc::dcList<Item>::Element* element = (dcList<Item>::Element*)selection.front();
        dc::dcList<Scenery>::Reference* reference = element->getSceneryReference();

        if (SelectReference<dcList<Scenery>::Reference>((dcGenericResource*)(reference), 0))
        {
            dc::dcList<Scenery>::Element* referencedResource = reference->getReferencedResource();

            for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
            {
                dc::dcList<Item>::Element* element = (dcList<Item>::Element*)*i;
                element->setSceneryRepresentation(referencedResource);
            }
            return 1;
        }
    }

    return 0;
}
//
//int CopyItemNameToDisplayedName(dcGenericResource* resource, void* param) {
//
//    // Select scenery representation
//    dc::dcList<Item>* itemList = (dcList<Item>*)resource;
//
//    // Select entries
//    dc::dcGenericResource::Array selection;
//    if (PromptSelectMultipleResources("Copy Item Name to Displayed Name",
//                                      "Select entries to process", itemList->getMembers(),
//        &selection, false))
//    {
//        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
//        {
//            dc::dcList<Item>::Element* element = (dcList<Item>::Element*)*i;
//            switch ((EquipmentSlot)(element->getSlot().getValue())) {
//              case EQUIPMENTSLOT_1HWEAPON:
//              case EQUIPMENTSLOT_2HWEAPON:
//                element->getItemAvatarModifier().min_attack_damage_.setFieldStateValue(FIELDSTATE_INNATE_ATTRIBUTE);
//                element->getItemAvatarModifier().max_attack_damage_.setFieldStateValue(FIELDSTATE_INNATE_ATTRIBUTE);
//                break;
//              case EQUIPMENTSLOT_BODYARMOR:
//                element->getItemAvatarModifier().actor_type_.setFieldStateValue(FIELDSTATE_INNATE_ATTRIBUTE);
//              case EQUIPMENTSLOT_BELT:
//              case EQUIPMENTSLOT_BOOTS:
//              case EQUIPMENTSLOT_GLOVES:
//              case EQUIPMENTSLOT_HELMET:
//              case EQUIPMENTSLOT_SHIELD:
//                element->getItemAvatarModifier().physical_defense_.setFieldStateValue(FIELDSTATE_INNATE_ATTRIBUTE);
//                break;
//            }
//        }
//        return 1;
//    }
//
//    return 0;
//}

//
//int AddAICharacterTypesForActors(dcGenericResource* resource, void* param)
//{
//    dc::dcList<AICharacterType>* aiCharacterList = (dcList<AICharacterType>*)resource;
//
//    // Select entries
//    dc::dcGenericResource::Array selection;
//    if (PromptSelectMultipleResources("Set AI Character Types For Actors", "Select actor types", globalEditor()->getRootResource()->getActorTypesList()->getMembers(),
//        &selection, false))
//    {
//        for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
//        {
//            dc::dcList<AICharacterType>::Element* element = (dcList<AICharacterType>::Element*)aiCharacterList->generateElement((*i)->getName());
//            element->setActorType((dcList<ActorType>::Element*)(*i));
//        }
//
//        return 1;
//    }
//
//    return 0;
//}
//
//


void addToArray(dc::dcGenericResource::Array& target, const dc::dcGenericResource::Array& src) {
  for (dc::dcGenericResource::Array::const_iterator i = src.begin(); i != src.end(); ++i) {
    target.push_back(*i);
  }
}

void addTransitionImage(dc::dcGenericResource::Array& target,
                        D3DXImage* image,
                        const char* path,
                        int index) {
  image->getFileName()->setValue(path);
  image->getSubImage()->setValue(index);
  image->getSubImageColumns()->setValue(2);
  image->getSubImageRows()->setValue(2);
  target.push_back(image->getOwner());
}


int AddEvidyon4x4TerrainTransition(dcGenericResource* resource,
                                   void* param) {
  ProjectVolucris* pv = globalEditor()->getRootResource();
  dcTable<Image>* image_table = (dcTable<Image>*)pv->getImageTable();
  dcTable<Texture>* texture_table = pv->getTextureTable();
  dcList<Terrain>* terrain_list = pv->getTerrainList();
  dcList<TerrainRule>* terrain_rules = pv->getTerrainRulesList();


  // Get the image table
  dc::dcTable<Image>* imageTable = (dcTable<Image>*)resource;


  // Pick a set of terrain between which to create a transition
  dc::dcGenericReference::Array a_terrain_selection;
  if (false == PromptSelectMultipleResources("Add Terrain Transition",
                                             "Pick the first group (set A) of terrain types",
                                             terrain_list->getMembers(),
                                            &a_terrain_selection)) {
    return false;
  }
  dc::dcGenericReference::Array b_terrain_selection;
  if (false == PromptSelectMultipleResources("Add Terrain Transition",
                                             "Pick the second group (set B) of terrain types",
                                             terrain_list->getMembers(),
                                            &b_terrain_selection)) {
    return false;
  }

  dc::dcGenericResource::Array a_to_b_horizontal_transition_images;
  dc::dcGenericResource::Array b_to_a_horizontal_transition_images;
  dc::dcGenericResource::Array a_ne_to_b_sw_images;
  dc::dcGenericResource::Array a_se_to_b_nw_images;
  dc::dcGenericResource::Array b_ne_to_a_sw_images;
  dc::dcGenericResource::Array b_se_to_a_nw_images;
  std::string transition_a_b_prefix = "", transition_b_a_prefix = "";


  // Pick transition images
  //if (false == PromptSelectMultipleResources("Add Terrain Transition",
  //                                           "Horizontal Transition: A on left -> B on right",
  //                                           image_table->getMembers(),
  //                                          &a_to_b_horizontal_transition_images)) {
  //  return false;
  //}
  //if (false == PromptSelectMultipleResources("Add Terrain Transition",
  //                                           "Horizontal Transition: B on left -> A on right",
  //                                           image_table->getMembers(),
  //                                          &b_to_a_horizontal_transition_images)) {
  //  return false;
  //}
  //if (false == PromptSelectMultipleResources("Add Terrain Transition",
  //                                           "A's Outside Corner: A on top/right, B on bottom/left",
  //                                           image_table->getMembers(),
  //                                          &a_ne_to_b_sw_images)) {
  //  return false;
  //}
  //if (false == PromptSelectMultipleResources("Add Terrain Transition",
  //                                           "A's Outside Corner: A on bottom/right, B on top/left",
  //                                           image_table->getMembers(),
  //                                          &a_se_to_b_nw_images)) {
  //  return false;
  //}
  //if (false == PromptSelectMultipleResources("Add Terrain Transition",
  //                                           "B's Outside Corner: B on top/right, A on bottom/left",
  //                                           image_table->getMembers(),
  //                                          &b_ne_to_a_sw_images)) {
  //  return false;
  //}
  //if (false == PromptSelectMultipleResources("Add Terrain Transition",
  //                                           "B's Outside Corner: B on bottom/right, A on top/left",
  //                                           image_table->getMembers(),
  //                                          &b_se_to_a_nw_images)) {
  //  return false;
  //}
  if (false == dcx::dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(),
                                            "Add Terrain Transition",
                                            "Terrain Prefix A->B?",
                                            &transition_a_b_prefix)) {
    return false;
  }
  if (false == dcx::dcxWin32PromptTextInput(globalEditor()->getTopmostWindowHandle(),
                                            "Add Terrain Transition",
                                            "Terrain Prefix B->A?",
                                            &transition_b_a_prefix)) {
    return false;
  }



  {
    char path_a_b[MAX_PATH];
    char path_b_a[MAX_PATH];

    if (!dcx::dcxOpenFileDialog(globalEditor()->getTopmostWindowHandle(),
                                "Select A(inner)->B(outer) Transition Image", "Image Files (*.JPG; *.JPEG; *.PNG; *.BMP; *.DDS)\0*.JPG;*.JPEG;*.PNG;*.BMP;*.DDS\0\0",
                                path_a_b, MAX_PATH)) {
        return 0;
    }

    if (!dcx::dcxOpenFileDialog(globalEditor()->getTopmostWindowHandle(),
                                "Select B(inner)->A(outer) Transition Image", "Image Files (*.JPG; *.JPEG; *.PNG; *.BMP; *.DDS)\0*.JPG;*.JPEG;*.PNG;*.BMP;*.DDS\0\0",
                                path_b_a, MAX_PATH)) {
        return 0;
    }


  addTransitionImage(a_to_b_horizontal_transition_images,
                     image_table->create<D3DXImage>(transition_a_b_prefix + "[e-1]"),
                     path_a_b,
                     0);
  addTransitionImage(a_ne_to_b_sw_images,
                     image_table->create<D3DXImage>(transition_a_b_prefix + "[sw]"),
                     path_a_b,
                     1);
  addTransitionImage(a_to_b_horizontal_transition_images,
                     image_table->create<D3DXImage>(transition_a_b_prefix + "[e-2]"),
                     path_a_b,
                     2);
  addTransitionImage(a_se_to_b_nw_images,
                     image_table->create<D3DXImage>(transition_a_b_prefix + "[nw]"),
                     path_a_b,
                     3);

  addTransitionImage(b_to_a_horizontal_transition_images,
                     image_table->create<D3DXImage>(transition_b_a_prefix + "[e-1]"),
                     path_b_a,
                     0);
  addTransitionImage(b_ne_to_a_sw_images,
                     image_table->create<D3DXImage>(transition_b_a_prefix + "[sw]"),
                     path_b_a,
                     1);
  addTransitionImage(b_to_a_horizontal_transition_images,
                     image_table->create<D3DXImage>(transition_b_a_prefix + "[e-2]"),
                     path_b_a,
                     2);
  addTransitionImage(b_se_to_a_nw_images,
                     image_table->create<D3DXImage>(transition_b_a_prefix + "[nw]"),
                     path_b_a,
                     3);

  }










  // Transition terrain (the textures are created but storage isn't needed).  There
  // are 4 of each kind because each is rotated 4 times.
  dc::dcGenericResource::Array a_to_b_horizontal_transition_terrain[4];
  dc::dcGenericResource::Array b_to_a_horizontal_transition_terrain[4];
  dc::dcGenericResource::Array a_ne_to_b_sw_terrain[4];
  dc::dcGenericResource::Array a_se_to_b_nw_terrain[4];
  dc::dcGenericResource::Array b_ne_to_a_sw_terrain[4];
  dc::dcGenericResource::Array b_se_to_a_nw_terrain[4];

  { // Create a texture for each image
    dc::dcGenericResource::Array* all_images[6] = {
      &a_to_b_horizontal_transition_images,
      &a_ne_to_b_sw_images,
      &a_se_to_b_nw_images,
      &b_to_a_horizontal_transition_images,
      &b_ne_to_a_sw_images,
      &b_se_to_a_nw_images,
    };
    dc::dcGenericResource::Array* all_terrain[6] = {
     a_to_b_horizontal_transition_terrain,
     a_ne_to_b_sw_terrain,
     a_se_to_b_nw_terrain,
     b_to_a_horizontal_transition_terrain,
     b_ne_to_a_sw_terrain,
     b_se_to_a_nw_terrain,
    };
    for (int i = 0; i < 6; ++i) {
      dc::dcGenericResource::Array* images = all_images[i];
      dc::dcGenericResource::Array* terrain4 = all_terrain[i];

      dc::dcGenericResource::Array::iterator image_iter = images->begin();
      dc::dcGenericResource::Array::iterator image_end = images->end();
      for (; image_iter != image_end; ++image_iter) {
        dc::dcGenericResource* generic_image = *image_iter;

        // Create the texture for this terrain type
        StaticTexture* texture = texture_table->create<StaticTexture>(generic_image->getName());
        dc::dcTable<Texture>::Element* texture_element =
          (dc::dcTable<Texture>::Element*)texture->getOwner();
        texture->getImageRef()->setGenericReferencedResource(generic_image);

        // Create the 4 rotations of this terrain type
        for (int i = 0; i < 4; ++i) {
          Terrain* terrain = (Terrain*)
            terrain_list->generateElement(generic_image->getName() + " " +
                                          dc::dcEnum<TerrainDirection>::getString((TerrainDirection)i));
          terrain->getTopTextureRef()->setReferencedResource(texture_element);
          terrain->getDirection()->setValue(i);
          terrain4[i].push_back(terrain);
        }
      }
    }
  }


/*
  dc::dcGenericResource::Array a_to_b_horizontal_transition_terrain[4];
  dc::dcGenericResource::Array a_ne_to_b_sw_terrain[4];
  dc::dcGenericResource::Array a_se_to_b_nw_terrain[4];
  dc::dcGenericResource::Array b_to_a_horizontal_transition_terrain[4];
  dc::dcGenericResource::Array b_ne_to_a_sw_terrain[4];
  dc::dcGenericResource::Array b_se_to_a_nw_terrain[4];
*/

  dc::dcGenericResource::Array transition_terrains[2][NUMBER_OF_TRANSITIONS];

  for (int group = 0; group < 2; ++group) {
    dc::dcGenericResource::Array* transition[5] = {
      group == 0 ? a_to_b_horizontal_transition_terrain : b_to_a_horizontal_transition_terrain,
      group == 0 ? a_ne_to_b_sw_terrain : b_ne_to_a_sw_terrain,
      group == 0 ? a_se_to_b_nw_terrain : b_se_to_a_nw_terrain,
      group == 0 ? &a_terrain_selection : &b_terrain_selection,
      group == 0 ? &b_terrain_selection : &a_terrain_selection,
    };

    addToArray(transition_terrains[group][EAST],  transition[0][0]);
    addToArray(transition_terrains[group][SOUTH], transition[0][1]);
    addToArray(transition_terrains[group][WEST],  transition[0][2]);
    addToArray(transition_terrains[group][NORTH], transition[0][3]);

    addToArray(transition_terrains[group][SOUTH_WEST_OUTSIDE], transition[1][0]);
    addToArray(transition_terrains[group][NORTH_WEST_OUTSIDE], transition[1][1]);
    addToArray(transition_terrains[group][NORTH_EAST_OUTSIDE], transition[1][2]);
    addToArray(transition_terrains[group][SOUTH_EAST_OUTSIDE], transition[1][3]);

    addToArray(transition_terrains[group][NORTH_WEST_OUTSIDE], transition[2][0]);
    addToArray(transition_terrains[group][NORTH_EAST_OUTSIDE], transition[2][1]);
    addToArray(transition_terrains[group][SOUTH_EAST_OUTSIDE], transition[2][2]);
    addToArray(transition_terrains[group][SOUTH_WEST_OUTSIDE], transition[2][3]);

    addToArray(transition_terrains[1-group][NORTH_EAST_INSIDE], transition[1][0]);
    addToArray(transition_terrains[1-group][SOUTH_EAST_INSIDE], transition[1][1]);
    addToArray(transition_terrains[1-group][SOUTH_WEST_INSIDE], transition[1][2]);
    addToArray(transition_terrains[1-group][NORTH_WEST_INSIDE], transition[1][3]);

    addToArray(transition_terrains[1-group][SOUTH_EAST_INSIDE], transition[2][0]);
    addToArray(transition_terrains[1-group][SOUTH_WEST_INSIDE], transition[2][1]);
    addToArray(transition_terrains[1-group][NORTH_WEST_INSIDE], transition[2][2]);
    addToArray(transition_terrains[1-group][NORTH_EAST_INSIDE], transition[2][3]);

    addToArray(transition_terrains[group][CENTRAL],     *transition[3]);
    addToArray(transition_terrains[group][SURROUNDING], *transition[4]);
  }

  // Create the transition groups
  AddTransitionRulesForTerrain(transition_a_b_prefix, terrain_rules, transition_terrains[0]);
  AddTransitionRulesForTerrain(transition_b_a_prefix, terrain_rules, transition_terrains[1]);

  return 1;
}











// delta faucet ad music



int ExportItemsToCSV(dcGenericResource* resource, void* param) {
  dc::dcList<Item>* item_list = (dc::dcList<Item>*)resource;


  char buffer[512];
  if (!dcx::dcxSaveFileDialog(globalEditor()->getTopmostWindowHandle(),
                             "Pick File to Export",
                             "*.txt",
                             buffer,
                             sizeof(buffer))) return 0;
  dc::dcFileStream fs;
  if (!fs.open(buffer, STREAM_OVERWRITE|STREAM_TEXT)) {
    MessageBox(globalEditor()->getTopmostWindowHandle(),
               "Unable to open game file",
               "Export Items to CSV",
               MB_OK);
    return 0;
  }

  const dc::dcGenericResource::Array& members = item_list->getMembers();

  dc::dcGenericResource::Array::const_iterator i = members.begin();
  dc::dcGenericResource::Array::const_iterator end = members.end();
  for (; i != end; ++i) {
    dc::dcList<Item>::Element* item_element = (dc::dcList<Item>::Element*)(*i);

    char csv_line[4096];
    item_element->createCSVLine(csv_line, sizeof(csv_line));
    char eol[] = { '\n', '\0' };
    fs.write(csv_line, strlen(csv_line));
    fs.write(eol, strlen(eol));
  }

  fs.close();


  return 1;
}














#define SEPERATOR   { "-", NULL }
#define COLLECTION_ELEMENT_ACTIONS { "Edit Name...", &EditResourceName }, { "Copy Members...", &CopyCollectionElementMembers }
#define COLLECTION_ACTIONS { "Sort", &SortCollection }, { "Sort by Name", &SortCollectionByName }, { "Bulk Rename...", &BulkRename }, { "Find/Replace in Names...", &FindReplaceInNames }, { "Merge Duplicates", &MergeDuplicatesInCollection }, { "Duplicate Entries...", &DuplicateCollectionEntries }, { "Delete Entries...", &DeleteCollectionEntries }
#define REFERENCE_ACTIONS(TYPE)   { "Select...", &SelectReference<dcTable<TYPE>::Reference> }, { "Clear", &ClearReference }
#define LIST_REFERENCE_ACTIONS(TYPE)   { "Select...", &SelectReference<dcList<TYPE>::Reference> }, { "Clear", &ClearReference }

dcAction dc::dcClass<dcByte>::actions[]             = { { "Edit", &EditNumberResource<dcByte,int> }, { 0, NULL } };
dcAction dc::dcClass<dcFileName>::actions[]         = { { "Edit", &EditFileName }, { 0, NULL } };
dcAction dc::dcClass<dcBoolean>::actions[]          = { { "Toggle", &ToggleBooleanValue }, { 0, NULL } };
dcAction dc::dcClass<dcInteger>::actions[]          = { { "Edit", &EditNumberResource<dcInteger,int> }, { 0, NULL } };
dcAction dc::dcClass<dcFloat>::actions[]            = { { "Edit", &EditNumberResource<dcFloat,float> }, { 0, NULL } };
dcAction dc::dcClass<dcIndex16>::actions[]          = { { "Edit", &EditNumberResource<dcIndex16,size_t> }, { 0, NULL } };
dcAction dc::dcClass<dcIndex32>::actions[]          = { { "Edit", &EditNumberResource<dcIndex32,size_t> }, { 0, NULL } };
dcAction dc::dcClass<dcString>::actions[]           = { { "Edit", &EditString }, { 0, NULL } };

dcAction dc::dcClass<dcIntegerWrapper>::actions[]   = { { "Edit", &EditNumberResource<dcIntegerWrapper,int> }, { 0, NULL } };

dcAction dc::dcClass<dcxVector3<dcByte>>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<dcRange<dcInteger>>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<dcInterface<dcGenericResource>>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<dcPair<dcInteger,dcInteger>>::actions[] = { { 0, NULL } };

dcAction dc::dcClass<dcList<Contributor>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };

dcAction dc::dcClass<ProjectVolucris>::actions[] = { { 0, NULL } };

dcAction dc::dcClass<ColorKey>::actions[] = { { "Set...", &EditColorKey }, { "Disable", &DisableColorKey }, { 0, NULL } };

dcAction dc::dcClass<dcx::dcxD3DColor>::actions[] = { { "Set...", &EditD3DColor }, { 0, NULL } };


// Image table actions
dcAction dc::dcClass<dcTable<Image>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR,
                                                    { "Import Subdivided Images...", &ImportSubdividedImages },
                                                    { "Add Image Files...", &AddD3DXImagesToTable },
                                                  //{ "Add Composite Image", &AddNewTtoTableY<CompositeImage,Image> },
                                                    { "Add Evidyon 4x4 Terrain Transition", &AddEvidyon4x4TerrainTransition },
                                                    { 0, NULL } };
dcAction dc::dcClass<dcInterface<Image>>::actions[] = { { 0, NULL } };


// Image derivative actions
dcAction dc::dcClass<D3DXImage>::actions[] = { { "Select Image File...", &EditD3DXImageFile },
                                           { "Set Color Key...", &SetD3DXImageColorKey },
                                           { "Disable Color Key", &DisableD3DXImageColorKey }, { 0, NULL } };

// Texture table actions
dcAction dc::dcClass<dcTable<Texture> >::actions[] = { COLLECTION_ACTIONS, SEPERATOR, 
                                                   { "Add Static Textures...", &AddMultipleTexturesToTable<StaticTexture> },
                                                   { "Add Circling Textures...", &AddMultipleTexturesToTable<CirclingTexture> },
                                                   { "Add Sliding Textures...", &AddMultipleTexturesToTable<SlidingTexture> },
                                                   { "Add Animated Textures...", &AddMultipleTexturesToTable<AnimatedTexture> },
                                                   SEPERATOR,

                                                   // TODO: these should really share code
                                                   { "Enable Flip Vertical...", EnableTextureFlipVertical },
                                                   { "Disable Flip Vertical...", DisableTextureFlipVertical },
                                                   { "Enable Flip Horizontal...", EnableTextureFlipHorizontal },
                                                   { "Disable Flip Horizontal...", DisableTextureFlipHorizontal },

                                                   { 0, NULL } };

dcAction dc::dcClass<dcInterface<Texture> >::actions[] = { { 0, NULL } };


// Texture derivative actions
dcAction dc::dcClass<AnimatedTexture>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<CirclingTexture>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<SlidingTexture>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<StaticTexture>::actions[] = { { 0, NULL } };


// Skin table actions
dcAction dc::dcClass<dcList<Skin>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, 
                                               { "Add Skin...", &AddNewEntryToCollection<dcList<Skin>> },
                                               { "Add Unique Skins for Meshes...", &AddUniqueSkinForMeshes },
                                               { "Add Common Skin for Meshes...", &AddCommonSkinForMeshes },
                                               { 0, NULL } };
dcAction dc::dcClass<dcInterface<Skin> >::actions[] = { { 0, NULL } };



// Skin actions
dcAction dc::dcClass<Skin>::actions[] = { { 0, NULL } };


// Skin map actions
dcAction dc::dcClass<Skin::Map>::actions[] = { { "Add Entry...", &AddNewEntryToCollection<Skin::Map> }, { "Set Entry Textures...", &SetSkinEntryTextures }, { 0, NULL } };



// Stuff for mesh
dcAction dc::dcClass<dcMap<dcIndex32,dcIndex32>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Entry...", &AddNewEntryToCollection<dcMap<dcIndex32,dcIndex32>> }, { 0, NULL } };
dcAction dc::dcClass<dcMap<dcIndex32,dcIndex32>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };
dcAction dc::dcClass<dcxVector3<dcFloat>>::actions[] = { { 0, NULL } };

// Mesh table actions
dcAction dc::dcClass<dcTable<Mesh>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, 
                                               { "Add .X-Format Mesh...", &AddNewTtoTableY<XMesh,Mesh> },
                                               { "Add Multiple .X-Format Meshes...", &AddMultipleXMeshesToTable },
                                               { "Add .MDJ-Format Mesh...", &AddNewTtoTableY<MDJMesh,Mesh> },
                                               { "Add Multiple .MDJ-Format Meshes...", &AddMultipleMDJMeshesToTable },
                                               { "Set Scaling Factor for Meshes...", &SetScalingFactorForMultipleMeshes },
                                               { "Visually Rescale Meshes...", &VisuallyRescaleMeshes }, { 0, NULL } };
dcAction dc::dcClass<dcInterface<Mesh>>::actions[] = { { 0, NULL } };



dcAction dc::dcClass<XMesh>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<MDJMesh>::actions[] = { { 0, NULL } };


dcAction dc::dcClass<Scenery>::actions[] = { { "Set X/Y/Z Scaling...", &SetSceneryXYZScaling_I },
                                         { "Set X/Z Scaling...", &SetSceneryXZScaling_I },
                                         { "Set Transform as Default for Mesh", &SetSceneryTransformAsDefaultForMesh_I }, { 0, NULL } };

// Scenery table actions
dcAction dc::dcClass<dcList<Scenery>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, 
                                                  { "Import .X Model Pack (Scenery, Meshes, Textures, Skins, Images)", &AddXModelScenery },
                                                  SEPERATOR,
                                                  { "Generate Random Duplicates...", &GenerateRandomSceneryDuplicates },
                                                  { "Generate Rotated Permutations...", &GenerateRotatedSceneryPermutations },
                                                  SEPERATOR,
                                                  { "Add Scenery for Meshes...", &AddSceneryForMeshes },
                                                  { "Set Skin for Multiple Scenery", &SetSkinForMultipleScenery },
                                                  { 0, NULL } };


dcAction dc::dcClass<dcInterface<Scenery>>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<dcList<Scenery>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };


dcAction dc::dcClass<Contributor>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<dcList<Contributor>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Contributor...", &AddNewEntryToCollection<dcList<Contributor>> }, { 0, NULL } };

dcAction dc::dcClass<dcTable<Image>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, {  0, NULL } };
dcAction dc::dcClass<dcTable<Texture>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS,{ 0, NULL } };
dcAction dc::dcClass<dcList<Skin>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };
dcAction dc::dcClass<dcTable<Mesh>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, SEPERATOR, { "Calculate unity scaling factor", &CalculateUnityScalingFactorForMesh }, { 0, NULL } };


dcAction dc::dcClass<dcTable<Image>::Reference>::actions[] = { REFERENCE_ACTIONS(Image), { 0, NULL } };
dcAction dc::dcClass<dcTable<Texture>::Reference>::actions[] = { REFERENCE_ACTIONS(Texture), { 0, NULL } };
dcAction dc::dcClass<dcTable<Mesh>::Reference>::actions[] = { REFERENCE_ACTIONS(Mesh), { 0, NULL } };
dcAction dc::dcClass<dcList<Skin>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(Skin), { 0, NULL } };
dcAction dc::dcClass<dcList<Scenery>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(Scenery), { 0, NULL } };


// Element actions
dcAction dc::dcClass<Skin::Map::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };
//dcAction dc::dcClass<

//dcAction dc::dcClass<dcRangeMap<dcInteger,dcList<Scenery>::Reference>>::actions[] = { { "Add Entry...", &AddNewEntryToCollection<dcRangeMap<dcInteger,dcList<Scenery>::Reference>> }, { 0, NULL } };

//dcAction dc::dcClass<dcRangeMap<dcInteger,dcList<Scenery>::Reference>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, SEPERATOR, LIST_REFERENCE_ACTIONS(Scenery), { 0, NULL } };


dcAction dc::dcClass<Terrain>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<dcList<Terrain>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Terrain...", &AddNewEntryToCollection<dcList<Terrain>> }, { "Add Ground Textures...", &AddGroundTexturesToTerrain }, SEPERATOR, { "Generate Cardinal Permutations...", &GenerateTerrainCardinalPermutations },  { 0, NULL } };
dcAction dc::dcClass<dcList<Terrain>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };
dcAction dc::dcClass<dcInterface<Terrain>>::actions[] = { { 0, NULL } };

dcAction dc::dcClass<Location>::actions[] = { { 0, NULL } };

dcAction dc::dcClass<dcList<Terrain>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(Terrain), { 0, NULL } };


dcAction dc::dcClass<dcList<MapLayer>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Empty Layer...", &AddNewEntryToCollection<dcList<MapLayer>> }, { "Add Terrain Layer...", &AddTerrainLayerToMapLayers }, { "Add Scenery Layer...", &AddSceneryLayerToMapLayers }, { 0, NULL } };
dcAction dc::dcClass<MapLayer>::actions[] = { { 0, NULL } };

dcAction dc::dcClass<dcList<MapLayer>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };

dcAction dc::dcClass<dcList<dcList<Terrain>::Reference>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR,  { "Add Terrain...", &AddMultipleCollectionEntryReferencesToList<dcList<Terrain>> }, { 0, NULL } };
dcAction dc::dcClass<dcList<dcList<Scenery>::Reference>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Scenery...", &AddMultipleCollectionEntryReferencesToList<dcList<Scenery>> }, { 0, NULL } };

dcAction dc::dcClass<dcList<dcList<Terrain>::Reference>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, SEPERATOR, LIST_REFERENCE_ACTIONS(Terrain), { 0, NULL } };
dcAction dc::dcClass<dcList<dcList<Scenery>::Reference>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, SEPERATOR, LIST_REFERENCE_ACTIONS(Scenery), { 0, NULL } };


dcAction dc::dcClass<dcEnum<MapNavigability>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<MapNavigability>> }, { 0, NULL } };
dcAction dc::dcClass<dcEnum<TerrainDirection>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<TerrainDirection>> }, { 0, NULL } };

dcAction dc::dcClass<dcMap<WorldLocationType,MapLocationInfo>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Location Info...", &AddNewEntryToCollection<dcMap<WorldLocationType,MapLocationInfo>> }, { 0, NULL } };
dcAction dc::dcClass<dcMap<WorldLocationType,MapLocationInfo>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };
dcAction dc::dcClass<MapLocationInfo>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<Atlas>::actions[] = { { 0, NULL } };


dcAction dc::dcClass<dcList<MapLocationInfo>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR,
                                                        { "Add Terrain...", &AddTerrainToMapLocation },
                                                        { "Add Scenery...", &AddSceneryToMapLocation },
                                                        { "Add Map Location", &AddNewEntryToCollection<dcList<MapLocationInfo>> },
                                                        SEPERATOR,
                                                        { "Set Navigability...", &SetMapLocationNavigability }, 
                                                        { "Set Elevation 1...", &SetMapLocationElevation1 },
                                                        { "Set Elevation 2...", &SetMapLocationElevation2 }, { 0, NULL } };


dcAction dc::dcClass<dcList<MapLocationInfo>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };




// Resource pack related actions
dcAction dc::dcClass<ResourcePack>::actions[] = { 0, NULL };
dcAction dc::dcClass<dcTable<ResourcePack>>::actions[] = { { "Add Resource Packs...", AddResourcePacks }, { 0, NULL } };
dcAction dc::dcClass<dcTable<ResourcePack>::Element>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<dcTable<ResourcePack>::Reference>::actions[] = { 0, NULL };
dcAction dc::dcClass<dcInterface<ResourcePack>>::actions[] = { 0, NULL };

dcAction dc::dcClass<PackImage>::actions[] = { 0, NULL };


// No actions for the pack image's aliased integer
dcAction dc::dcClass<PackImage::Alias<dcInteger>>::actions[] = { 0, NULL };

dcAction dc::dcClass<PackMesh>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<PackMesh::Alias<dcInteger>>::actions[] = { { 0, NULL } };



// Terrain rule list
//=========================
dcAction dc::dcClass<TerrainRule>::actions[] = { { NULL, 0 } };
dcAction dc::dcClass<dcList<TerrainRule>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Rule...", AddNewEntryToCollection<dcList<TerrainRule>> }, { "Add Transition Group...", AddTransitionGroupRules }, { NULL, 0 } };
dcAction dc::dcClass<dcList<TerrainRule>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<TerrainRule>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(TerrainRule), { NULL, 0 } };


// Elevation list
//=========================
dcAction dc::dcClass<Elevation>::actions[] = { { NULL, 0 } };
dcAction dc::dcClass<dcList<Elevation>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Elevation...", &AddNewEntryToCollection<dcList<Elevation>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<Elevation>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, SEPERATOR, { "Edit", EditNumberResource<Elevation,float> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<Elevation>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(Elevation), { NULL, 0 } };




dcAction dc::dcClass<dcList<dcList<TerrainRule>::Reference>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Terrain Rules...", &AddMultipleCollectionEntryReferencesToList<dcList<TerrainRule>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<dcList<TerrainRule>::Reference>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };

dcAction dc::dcClass<PlayerCharacterRace>::actions[] = { { NULL, 0 } };
dcAction dc::dcClass<PlayerCharacterClass>::actions[] = { { NULL, 0 } };

dcAction dc::dcClass<dcList<PlayerCharacterRace>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Race...", &AddNewEntryToCollection<dcList<PlayerCharacterRace>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<PlayerCharacterClass>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Class...", &AddNewEntryToCollection<dcList<PlayerCharacterClass>> }, { NULL, 0 } };

dcAction dc::dcClass<dcList<PlayerCharacterRace>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(PlayerCharacterRace), { NULL, 0 } };
dcAction dc::dcClass<dcList<PlayerCharacterClass>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(PlayerCharacterClass), { NULL, 0 } };

dcAction dc::dcClass<dcList<PlayerCharacterRace>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<PlayerCharacterClass>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };




template <typename I, typename D> int ImplementInterface(dcGenericResource* resource, void* param)
{
    dc::dcInterface<I>* iface = (dcInterface<I>*)resource;
    iface->implement<D>();
    return 1;
}

int ResetInterface(dcGenericResource* resource, void* param)
{
    dc::dcGenericInterface* iface = (dcGenericInterface*)resource;
    iface->release();
    return 1;
}






dcAction dc::dcClass<VisualFXImage>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<VisualFX_OrbitingParticles>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<VisualFX_ParticleTrail>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<VisualFX_SwirlingParticles>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<VisualFX_Beam>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<MagicFX>::actions[] = { { 0, NULL } };


dcAction dc::dcClass<dcList<VisualFXImage>>::actions [] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Image...", &AddNewEntryToCollection<dcList<VisualFXImage>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<VisualFXImage>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<VisualFXImage>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(VisualFXImage), { NULL, 0 } };


dcAction dc::dcClass<dcList<MagicFX>>::actions [] = { COLLECTION_ACTIONS, SEPERATOR, { "Add MagicFX...", &AddNewEntryToCollection<dcList<MagicFX>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<MagicFX>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<MagicFX>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(MagicFX), { NULL, 0 } };


dcAction dc::dcClass<dcTable<VisualFX>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, 
                                                  { "Add Orbiting Particles...", &AddNewTtoTableY<VisualFX_OrbitingParticles,VisualFX> },
                                                  { "Add Particle Trail...", &AddNewTtoTableY<VisualFX_ParticleTrail,VisualFX> },
                                                  { "Add Swirling Particles...", &AddNewTtoTableY<VisualFX_SwirlingParticles,VisualFX> },
                                                  { "Add Beam...", &AddNewTtoTableY<VisualFX_Beam,VisualFX> },
                                                  { 0, NULL } };
dcAction dc::dcClass<dcTable<VisualFX>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcTable<VisualFX>::Reference>::actions[] = { REFERENCE_ACTIONS(VisualFX), { NULL, 0 } };
dcAction dc::dcClass<dcInterface<VisualFX>>::actions[] = { { 0, NULL } };


dcAction dc::dcClass<ItemAvatarModifier>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<ItemAvatarActorTypeModifier>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<ItemAvatarModifierField<dc::dcInteger>>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<ItemAvatarModifierField<dc::dcFloat>>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<ItemAvatarModifierField<ItemAvatarActorTypeModifier>>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<ItemAvatarModifierField<dc::dcList<Spell>::Reference>>::actions[] = { { 0, NULL } };



dcAction dc::dcClass<dcEnum<EquipmentSlot>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<EquipmentSlot>> }, { 0, NULL } };
dcAction dc::dcClass<Item>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<dcList<Item>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR,
                                                  { "Add Item", &AddNewEntryToCollection<dcList<Item>> },
                                                  SEPERATOR,
                                                  { "Set Item Scenery...", &SetItemSceneryRepresentation },
                                                  { "Export Items to CSV...", &ExportItemsToCSV },
                                                  //{ "Copy Item Name -> Displayed Name", &CopyItemNameToDisplayedName },
                                                  { NULL, 0 } };
dcAction dc::dcClass<dcList<Item>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<Item>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(Item), { NULL, 0 } };



dcAction dc::dcClass<UnseenSkinnedMeshResource>::actions[] = { 0, NULL };
dcAction dc::dcClass<UnseenKeyframeAnimation>::actions[] = { 0, NULL };
dcAction dc::dcClass<ActorType>::actions[] = { 0, NULL };


dcAction dc::dcClass<dcList<UnseenSkinnedMeshResource>>::actions [] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Skinned Mesh", &AddNewEntryToCollection<dcList<UnseenSkinnedMeshResource>> }, { "Select Files...", &AddSkinnedMeshFiles }, { NULL, 0 } };
dcAction dc::dcClass<dcList<UnseenSkinnedMeshResource>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<UnseenSkinnedMeshResource>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(UnseenSkinnedMeshResource), { NULL, 0 } };

dcAction dc::dcClass<dcList<UnseenKeyframeAnimation>>::actions [] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Animation", &AddNewEntryToCollection<dcList<UnseenKeyframeAnimation>> }, { "Select Files...", &AddAnimationFiles }, { NULL, 0 } };
dcAction dc::dcClass<dcList<UnseenKeyframeAnimation>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<UnseenKeyframeAnimation>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(UnseenKeyframeAnimation), { NULL, 0 } };

dcAction dc::dcClass<dcList<ActorType>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Actor Type", &AddNewEntryToCollection<dcList<ActorType>> }, { "Normalize All Action Animations", NormalizeAllActorActionAnimations }, { NULL, 0 } };
dcAction dc::dcClass<dcList<ActorType>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, SEPERATOR, { "Edit Dialog...", &EditActorType }, { "Normalize Action Animations", &NormalizeActorActionAnimations }, { NULL, 0 } };
dcAction dc::dcClass<dcList<ActorType>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(ActorType), { NULL, 0 } };



dcAction dc::dcClass<dcEnum<ActorAnimation>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<ActorAnimation>> }, { 0, NULL } };
dcAction dc::dcClass<dcMap<dcEnum<ActorAnimation>,dcList<UnseenKeyframeAnimation>::Reference>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Action Animation", &AddNewEntryToCollection<dcMap<dcEnum<ActorAnimation>,dcList<UnseenKeyframeAnimation>::Reference>> } };
dcAction dc::dcClass<dcMap<dcEnum<ActorAnimation>,dcList<UnseenKeyframeAnimation>::Reference>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };



dcAction dc::dcClass<dcEnum<ActorAction>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<ActorAction>> }, { 0, NULL } };
//dcAction dc::dcClass<dcEnum<EditorActorFaction>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<EditorActorFaction>> }, { 0, NULL } };


//dcAction dc::dcClass<AICharacterType>::actions[] = { { 0, NULL } };
//
//dcAction dc::dcClass<dcList<AICharacterType>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add AI Character Type", &AddNewEntryToCollection<dcList<AICharacterType>> }, { "Add AI Character Types for Actors", &AddAICharacterTypesForActors }, { NULL, 0 } };
//dcAction dc::dcClass<dcList<AICharacterType>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
//dcAction dc::dcClass<dcList<AICharacterType>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(AICharacterType), { NULL, 0 } };

template <typename Enum, const Enum value> int SetEnumValue(dcGenericResource* resource, void* ptr)
{
    dc::dcEnum<Enum>* enumar = (dcEnum<Enum>*)resource;
    enumar->setValue(value);
    return 1;
}

dcAction dc::dcClass<dcEnum<ActorState>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<ActorState>> }, { "Invalidate", &SetEnumValue<ActorState,ACTORSTATE_INVALID> }, { 0, NULL } };

#include "../shared/server-editor/evidyon_gameplay_design.h"

int FillLevelDefnAverageHPMP(dcGenericResource* resource, void* ptr) {
  MessageBox(NULL, "This functionality has been disabled because it's a bad way to assign attributes", "Nothing's gonna happen", MB_OK);
//  LevelDefinitions* level_definitions = (LevelDefinitions*)resource;
//
//  LevelStatIncrement increments[AVATARCLASS_COUNT][HIGHEST_DEFINED_LEVEL];
//  memset(increments, 0, sizeof(increments));
//
//  increments[AVATARCLASS_KNIGHT][0] = 7;
//  increments[AVATARCLASS_KNIGHT][1] = 0;
//  increments[AVATARCLASS_KNIGHT][2] = 11;
//  increments[AVATARCLASS_KNIGHT][3] = 0;
//  increments[AVATARCLASS_KNIGHT][4] = 0;
//
//  increments[AVATARCLASS_SORCERER][0] = 7;
//  increments[AVATARCLASS_SORCERER][1] = 0;
//  increments[AVATARCLASS_SORCERER][2] = 11;
//  increments[AVATARCLASS_SORCERER][3] = 0;
//  increments[AVATARCLASS_SORCERER][4] = 0;
//
//  for (int level = 0; level < HIGHEST_DEFINED_LEVEL; ++level) {
//    int class_hp[AVATARCLASS_COUNT];
//    int class_mp[AVATARCLASS_COUNT];
//    for (int i = 0; i < AVATARCLASS_COUNT; ++i) {
//      dcGenericResource* generic_avatar_class =
//        globalEditor()->getRootResource()->getAvatarClassList()->getMembers().at(i);
//      PlayerCharacterClass* avatar_class = (PlayerCharacterClass*)generic_avatar_class;
//
//      class_hp[i] =
//        Evidyon::GameplayDesign::avatarCoreAttribute(level, 2, increments[i],
//                                                     avatar_class->getHPConBonusIncrementValue(),
//                                                     avatar_class-
//
//      int stats[5];
//      switch (i) {
//      case AVATARCLASS_KNIGHT:
//        stats[0] = 17 + (int)(level / 2.0);
//        stats[2] = 21 + (int)(level / 2.5);
//        stats[3] = 10 + (int)(level / 5.0);
//        stats[4] = 10 + (int)(level / 7.0);
//        break;
//      case AVATARCLASS_SORCERER:
//        stats[0] = 10 + (int)(level / 7.0);
//        stats[2] = 17 + (int)(level / 2.35);
//        stats[3] = 17 + (int)(level / 3.0);
//        stats[4] = 19 + (int)(level / 3.0);
//        break;
//      }
///*
//      class_hp[i] = 
//        avatar_class->getHPModifierValue() * Evidyon::GameplayDesign::avatarCoreHP(level + 1, stats[0], stats[2]);
//      class_mp[i] =
//        avatar_class->getMPModifierValue() * Evidyon::GameplayDesign::avatarCoreMP(level + 1, stats[3], stats[4]);*/
//    }
//
//    int average_hp = 0, average_mp = 0;
//    for (int i = 0; i < AVATARCLASS_COUNT; ++i) {
//      average_hp += class_hp[i];
//      average_mp += class_mp[i];
//    }
//
//    LevelAttributes* attributes = level_definitions->getLevelAttributes(level+1);
//    attributes->setAverageHP(average_hp);
//    attributes->setAverageMP(average_mp);
//  }
//
  return 1;
}


dcAction dc::dcClass<LevelDefinitions>::actions[] = { { "Fill Average HP/MP", &FillLevelDefnAverageHPMP }, { 0, NULL } };


dcAction dc::dcClass<PlayerCharacterClassLevelSpells>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Single Spell", &AddNewEntryToCollection<PlayerCharacterClassLevelSpells> }, { "Add Spells...", &AddMultiplePlayerClassLevelEffects }, { 0, NULL } };
dcAction dc::dcClass<PlayerCharacterClassLevelSpells::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };

//
//dcAction dc::dcClass<MonsterLoot>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Loot", &AddNewEntryToCollection<MonsterLoot> }, { "Add Multiple Items As Loot", &AddMultipleItemsAsLoot }, { 0, NULL } };
//dcAction dc::dcClass<MonsterLoot::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };
//
//dcAction dc::dcClass<MonsterClass>::actions[] = { { 0, NULL } };
//dcAction dc::dcClass<MonsterSpawn>::actions[] = { { 0, NULL } };


//dcAction dc::dcClass<dcList<MonsterClass>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Monster Class", &AddNewEntryToCollection<dcList<MonsterClass>> }, { "Add Class For Monsters...", &AddClassForMonsters }, { NULL, 0 } };
//dcAction dc::dcClass<dcList<MonsterClass>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
//dcAction dc::dcClass<dcList<MonsterClass>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(MonsterClass), { NULL, 0 } };


//dcAction dc::dcClass<dcList<MonsterSpawn>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Monster Spawn Type", &AddNewEntryToCollection<dcList<MonsterSpawn>> }, { NULL, 0 } };
//dcAction dc::dcClass<dcList<MonsterSpawn>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
//dcAction dc::dcClass<dcList<MonsterSpawn>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(MonsterSpawn), { NULL, 0 } };


//dcAction dc::dcClass<dcList<dcList<AICharacterType>::Reference>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Character Type", &AddNewEntryToCollection<dcList<dcList<AICharacterType>::Reference>> }, { NULL, 0 } };
//dcAction dc::dcClass<dcList<dcList<AICharacterType>::Reference>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, SEPERATOR, LIST_REFERENCE_ACTIONS(AICharacterType), { NULL, 0 } };
//dcAction dc::dcClass<dcList<dcList<AICharacterType>::Reference>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(dcList<AICharacterType>::Reference), { NULL, 0 } };


dcAction dc::dcClass<WAVSound>::actions[] = { { 0, NULL } };

dcAction dc::dcClass<dcList<WAVSound>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add WAV Sound", &AddNewEntryToCollection<dcList<WAVSound>> }, { "Import Multiple WAV Sounds...", &ImportMultipleWAVSounds }, { NULL, 0 } };
dcAction dc::dcClass<dcList<WAVSound>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<WAVSound>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(WAVSound), { NULL, 0 } };



dcAction dc::dcClass<SkinnedMeshAttachmentPoint>::actions[] = { { NULL, 0 } };
dcAction dc::dcClass<dcEnum<ActorAttachmentPoint>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<ActorAttachmentPoint>> }, { 0, NULL } };




dcAction dc::dcClass<dcInterface<Magic>>::actions[] = { NULL, 0 };

dcAction dc::dcClass<dcTable<Magic>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, 
                                                { "Add Detailed Magic...", &AddNewTtoTableY<DetailedMagic,Magic> },
                                                  { 0, NULL } };
dcAction dc::dcClass<dcTable<Magic>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcTable<Magic>::Reference>::actions[] = { REFERENCE_ACTIONS(Magic), { NULL, 0 } };






dcAction dc::dcClass<dcTable<LifeformAI>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, 
                                                     { "Add Normal Melee Monster...", &AddNewTtoTableY<NormalMeleeMonsterLifeformAI,LifeformAI> },
                                                     { "Add Interactive NPC...", &AddNewTtoTableY<InteractiveNPCLifeformAI,LifeformAI> },
                                                     { 0, NULL } };
dcAction dc::dcClass<dcTable<LifeformAI>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcTable<LifeformAI>::Reference>::actions[] = { REFERENCE_ACTIONS(LifeformAI), { NULL, 0 } };


dcAction dc::dcClass<dcInterface<LifeformAI>>::actions[] = { 0, NULL };
dcAction dc::dcClass<NormalMeleeMonsterLifeformAI>::actions[] = { 0, NULL };

dcAction dc::dcClass<AIMeleeAttack>::actions[] = { 0, NULL };


dcAction dc::dcClass<dcList<dcReference<Enchantment>>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Enchantment", &AddNewEntryToCollection<dcList<dcReference<Enchantment>>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<dcReference<Enchantment>>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<dcReference<Enchantment>>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(dcReference<Enchantment>), { NULL, 0 } };

dcAction dc::dcClass<dcEnum<Evidyon::ActorAttack>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<Evidyon::ActorAttack>> }, { 0, NULL } };


dcAction dc::dcClass<dcList<NormalMeleeMonsterLifeformAIActorType>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Actor Type", &AddNewEntryToCollection<dcList<NormalMeleeMonsterLifeformAIActorType>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<NormalMeleeMonsterLifeformAIActorType>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<NormalMeleeMonsterLifeformAIActorType>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(NormalMeleeMonsterLifeformAIActorType), { NULL, 0 } };





// orphaned stuff


template<> const char* dc::dcEnum<ActorState>::getString(ActorState value)
{
    switch(value)
    {
        case ACTORSTATE_DEAD:           return "Dead";
        //case ACTORSTATE_IDLE:           return "Idle";
        //case ACTORSTATE_WALKING:        return "Walking";
        case ACTORSTATE_NORMAL:         return "Normal";
        case ACTORSTATE_SPIRITFORM:     return "Spirit Form";
//        case ACTORSTATE_BLOCKING:       return "Blocking";

        default: return 0;
    }
}

std::string dc::dcEnum<ActorState>::staticTypeName()
{
    return "ActorState";
}



template<> const char* dc::dcEnum<ActorAction>::getString(ActorAction value)
{
    switch(value)
    {
        case ACTORACTION_DIE:           return "Die";
        case ACTORACTION_ATTACK1:       return "Attack 1";
        case ACTORACTION_ATTACK2:       return "Attack 2";
        case ACTORACTION_ATTACK3:       return "Attack 3";
        case ACTORACTION_ATTACK4:       return "Attack 4";
        case ACTORACTION_GOTHIT:        return "Got Hit";
        case ACTORACTION_BLOCK1:  return "Block 1";
        case ACTORACTION_BLOCK2:  return "Block 2";
        case ACTORACTION_BLOCK3:  return "Block 3";
        case ACTORACTION_CASTSPELL1: return "Cast Spell 1";
        case ACTORACTION_CASTSPELL2:  return "Cast Spell 2";
        case ACTORACTION_NONE:          return "Force No Action";

        default: return 0;
    }
}

std::string dc::dcEnum<ActorAction>::staticTypeName()
{
    return "ActorAction";
}

dcAction dc::dcClass<dcEnum<Evidyon::MagicDescription::PowerType>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<Evidyon::MagicDescription::PowerType>> }, { 0, NULL } };
dcAction dc::dcClass<dcEnum<Evidyon::MagicDescriptionStageEffect::Type>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<Evidyon::MagicDescriptionStageEffect::Type>> }, { 0, NULL } };
dcAction dc::dcClass<dcEnum<Evidyon::MagicDescriptionStageTargeting::Type>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<Evidyon::MagicDescriptionStageTargeting::Type>> }, { 0, NULL } };

dcAction dc::dcClass<DetailedMagic>::actions[] = { NULL, 0 };
dcAction dc::dcClass<DetailedMagicStage>::actions[] = { NULL, 0 };


dcAction dc::dcClass<Spell>::actions[] = { NULL, 0 };
dcAction dc::dcClass<dcList<Spell>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Spell", &AddNewEntryToCollection<dcList<Spell>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<Spell>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<Spell>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(Spell), { NULL, 0 } };


dcAction dc::dcClass<Projectile>::actions[] = { NULL, 0 };
dcAction dc::dcClass<dcList<Projectile>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Projectile", &AddNewEntryToCollection<dcList<Projectile>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<Projectile>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<Projectile>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(Projectile), { NULL, 0 } };

dcAction dc::dcClass<Enchantment>::actions[] = { NULL, 0 };
dcAction dc::dcClass<EnchantmentClassList>::actions[] = { NULL, 0 };

dcAction dc::dcClass<EnchantmentClassImpl<1>>::actions[] = { NULL, 0 };
dcAction dc::dcClass<EnchantmentClassImpl<2>>::actions[] = { NULL, 0 };
dcAction dc::dcClass<EnchantmentClassImpl<3>>::actions[] = { NULL, 0 };

dcAction dc::dcClass<dcReference<Enchantment>>::actions[] = { {"Select...", &SelectReference<dcReference<Enchantment>> }, { "Clear", &ClearReference }, {NULL,0} };

dcAction dc::dcClass<dcList<Atlas>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Map", &AddNewEntryToCollection<dcList<Atlas>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<Atlas>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { "Preview && Edit", &EditAtlas }, { NULL, 0 } };
dcAction dc::dcClass<dcList<Atlas>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(Atlas), { NULL, 0 } };



dcAction dc::dcClass<dcInterface<MapLocationSpecialFunction>>::actions[] = { { 0, NULL } };
dcAction dc::dcClass<MapLink>::actions[] = { { 0, NULL } };

dcAction dc::dcClass<dcTable<MapLocationSpecialFunction>>::actions[] =
  { COLLECTION_ACTIONS, SEPERATOR, 
    { "Map Link...", &AddNewTtoTableY<MapLink,MapLocationSpecialFunction> },
    { 0, NULL } };
dcAction dc::dcClass<dcTable<MapLocationSpecialFunction>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcTable<MapLocationSpecialFunction>::Reference>::actions[] = { REFERENCE_ACTIONS(MapLocationSpecialFunction), { NULL, 0 } };


dcAction dc::dcClass<SwarmSpawner>::actions[] = { 0, NULL };
dcAction dc::dcClass<SwarmSpawn>::actions[] = { 0, NULL };
dcAction dc::dcClass<SpawnLevel>::actions[] = { 0, NULL };

dcAction dc::dcClass<SpawnDensity>::actions[] = { { "Edit", &EditNumberResource<dcInteger,int> }, { 0, NULL } };



//------------------------------------------------------------------------------------------------
// Name:  EditColorKey
// Desc:  Brings up a color-picker to choose colors
//------------------------------------------------------------------------------------------------
template <typename MappedType> int FillMapColorMaskWithColorsInFile(dcGenericResource* resource, void* data)
{
    MapColorMask<MappedType>* mask = (MapColorMask<MappedType>*)resource;

    unsigned int width, height;
    D3DCOLOR* colors = NULL;
    CONFIRM(AcquireMapMask(globalEditor()->getD3DDevice(),
                           mask->getMaskImageFileName().c_str(),
                          &width, &height, &colors)) else return 0;

    typedef std::set<D3DCOLOR> ColorSet;
    ColorSet color_set;

    unsigned int size = width*height;
    for (unsigned int i = 0; i < size; ++i) {
      color_set.insert(colors[i]);
    }

    ReleaseMapMask(colors);

    // Add all colors that were found to this mask
    for (ColorSet::iterator i = color_set.begin(); i != color_set.end(); ++i) {
      D3DCOLOR color = *i;
      char name[64];
      sprintf_s(name, "Color (%i, %i, %i)", 0xFF&(color>>16), 0xFF&(color>>8), 0xFF&(color>>0));
      mask->addColor(name, color);
    }

    return 1;
}

dcAction dc::dcClass<dcList<MapColorMask<SwarmSpawn>>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Swarm Spawn", &AddNewEntryToCollection<dcList<MapColorMask<SwarmSpawn>>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<MapColorMask<SwarmSpawn>>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, SEPERATOR, { "Add Colors in Mask", &FillMapColorMaskWithColorsInFile<SwarmSpawn> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<MapColorMask<SwarmSpawn>>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(MapColorMask<SwarmSpawn>), { NULL, 0 } };


dcAction dc::dcClass<MapColorMask<SwarmSpawn>>::actions[] = { { "Add Colors in Mask", &FillMapColorMaskWithColorsInFile<SwarmSpawn> }, {0, NULL} };
dcAction dc::dcClass<MapColorMask<SpawnLevel>>::actions[] = { { "Add Colors in Mask", &FillMapColorMaskWithColorsInFile<SpawnLevel> }, {0, NULL} };
dcAction dc::dcClass<MapColorMask<SpawnDensity>>::actions[] = { { "Add Colors in Mask", &FillMapColorMaskWithColorsInFile<SpawnDensity> }, { "Edit", &EditNumberResource<SpawnDensity,int> }, { 0, NULL } };






dcAction dc::dcClass<MapColorMask<SwarmSpawn>::Element>::actions[] = { 0, NULL };
dcAction dc::dcClass<dcList<MapColorMask<SwarmSpawn>::Element>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Swarm Spawn...", &AddNewEntryToCollection<dcList<MapColorMask<SwarmSpawn>::Element>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<MapColorMask<SwarmSpawn>::Element>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<MapColorMask<SwarmSpawn>::Element>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(MapColorMask<SwarmSpawn>::Element), { NULL, 0 } };


dcAction dc::dcClass<MapColorMask<SpawnLevel>::Element>::actions[] = { 0, NULL };
dcAction dc::dcClass<dcList<MapColorMask<SpawnLevel>::Element>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Spawn Level...", &AddNewEntryToCollection<dcList<MapColorMask<SpawnLevel>::Element>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<MapColorMask<SpawnLevel>::Element>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<MapColorMask<SpawnLevel>::Element>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(MapColorMask<SpawnLevel>::Element), { NULL, 0 } };


dcAction dc::dcClass<MapColorMask<SpawnDensity>::Element>::actions[] = { 0, NULL };
dcAction dc::dcClass<dcList<MapColorMask<SpawnDensity>::Element>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Spawn Density...", &AddNewEntryToCollection<dcList<MapColorMask<SpawnDensity>::Element>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<MapColorMask<SpawnDensity>::Element>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<MapColorMask<SpawnDensity>::Element>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(MapColorMask<SpawnDensity>::Element), { NULL, 0 } };



dcAction dc::dcClass<NormalMeleeMonsterLifeformAIActorType>::actions[] = { 0, NULL };

dcAction dc::dcClass<TreasureGeneration>::actions[] = { 0, NULL };
dcAction dc::dcClass<TreasureClass>::actions[] = { 0, NULL };
dcAction dc::dcClass<Treasure>::actions[] = { 0, NULL };

//------------------------------------------------------------------------------------------------
int PickTreasureReference(dcGenericResource* resource, void* data)
{
    dc::dcList<Treasure>::Reference* ref = (dcList<Treasure>::Reference*)resource;

    // the owner must be TreasureClass
    dc::dcGenericResource* generic_owner = ref->getOwner();
    if (generic_owner->getTypeID() != dc::dcClass<dcList<TreasureClass>::Element>::staticResourceManager()->uniqueID())
      return 0;
    TreasureClass* owner = (TreasureClass*)generic_owner;

    // Add options to the list
    dc::dcGenericResource::Array referencableTypes = owner->getTreasureList()->getMembers();
    std::list<std::string> options;
    int selection = -1;

    // Initialize the selection and the list of options
    {
        // The resource currently referenced
        dc::dcList<Treasure>::Reference::ResourceType* referencedResource = ref->getReferencedResource();

        // Go through the list of possible objects that can be referenced and add their names
        // to the list of string options.  Also, keep track of the index and determine which
        // is currently indexed.
        int index = 0;
        dc::dcGenericResource::Array::const_iterator end = referencableTypes.end();
        for (dcGenericResource::Array::const_iterator i = referencableTypes.begin();
             i != end; ++i, ++index)
        {
            if ((*i) == referencedResource)
                selection = index;
            options.push_back((*i)->getName());
        }
    }

    // Create a prompt
    std::string prompt = "Select a reference for " + resource->getName();

    // Ask the user to pick some resources.  Only initialize the selection if the
    // procedure above identified which resource is currently selected.
    if (dcxWin32PromptForSelection(globalEditor()->getTopmostWindowHandle(),
                                    "Select Resource Reference", prompt.c_str(), &options, &selection, selection > -1))
    {
        ref->setReferencedResource((dcList<Treasure>::Reference::ResourceType*)referencableTypes.at(selection));
        return 1;
    }

    // Nothing changed
    return 0;
}



int AddTreasureForItems(dcGenericResource* resource, void* pointer) {
  // Get the lists
  dc::dcList<Treasure>* treasure_list = (dcList<Treasure>*)resource;
  dc::dcList<Item>* item_list = globalEditor()->getRootResource()->getItemsList();

  // Let the user pick resources
  dc::dcGenericResource::Array selection;
  if (PromptSelectMultipleResources("Add Treasure For Items",
                                    "Pick the items for which to create treasure",
                                    item_list->getMembers(),
                                   &selection))
  {
    // Create an element for each selected mesh
    for (dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i)
    {
      dc::dcList<Item>::Element* item_element = (dcList<Item>::Element*)*i;

      // Create a new scenery item
      Treasure* element = (Treasure*)treasure_list->generateElement(item_element->getName());
      element->setItemReference(item_element);
    }
  } else {
    return 0;
  }

  return 1;
}


dcAction dc::dcClass<dcList<Treasure>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Treasure...", &AddNewEntryToCollection<dcList<Treasure>> }, { "Add Treasure For Items...", &AddTreasureForItems }, { NULL, 0 } };
dcAction dc::dcClass<dcList<Treasure>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<Treasure>::Reference>::actions[] = { { "Select...", &PickTreasureReference }, { "Clear", &ClearReference }, { NULL, 0 } };


dcAction dc::dcClass<dcList<TreasureClass>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Treasure Class", &AddNewEntryToCollection<dcList<TreasureClass>> }, { NULL, 0 } };
dcAction dc::dcClass<dcList<TreasureClass>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { NULL, 0 } };
dcAction dc::dcClass<dcList<TreasureClass>::Reference>::actions[] = { LIST_REFERENCE_ACTIONS(TreasureClass), { NULL, 0 } };


dcAction dc::dcClass<AvatarRaceList>::actions[] = { 0, NULL };
dcAction dc::dcClass<AvatarClassList>::actions[] = { 0, NULL };


dcAction dc::dcClass<dcEnum<ItemAvatarModifierFieldState>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<ItemAvatarModifierFieldState>> }, { 0, NULL } };

dcAction dc::dcClass<LevelAttributes>::actions[] = { 0, NULL };


dcAction dc::dcClass<dcMap<WorldLocationType,dcTable<LifeformAI>::Reference>>::actions[] = { COLLECTION_ACTIONS, SEPERATOR, { "Add Inhabitant...", &AddNewEntryToCollection<dcMap<WorldLocationType,dcTable<LifeformAI>::Reference>> }, { 0, NULL } };
dcAction dc::dcClass<dcMap<WorldLocationType,dcTable<LifeformAI>::Reference>::Element>::actions[] = { COLLECTION_ELEMENT_ACTIONS, { 0, NULL } };

dcAction dc::dcClass<dcEnum<Evidyon::NPCInteractionType>>::actions[] = { { "Edit", &EditEnumValue<dcEnum<Evidyon::NPCInteractionType>> }, { 0, NULL } };

dcAction dc::dcClass<InteractiveNPCLifeformAI>::actions[] = { 0, NULL };

dcAction dc::dcClass<dc::dcEnum<Evidyon::ActorSound>>::actions[] = { 0, NULL };


#include "(quest).h"


#define BEGIN_DEFINING_ACTIONS(resource_class) \
  dc::dcAction dc::dcClass<resource_class>::actions[] = {

#define ACTION_LIST_SEPERATOR   { "-", NULL }

#define END_DEFINING_ACTIONS() { 0, NULL } };


BEGIN_DEFINING_ACTIONS(QuestItemReference)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Quest)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(QuestAttemptCost)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(QuestTermination)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(QuestAction)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(QuestAvailability)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(QuestLifeformAIReference)
END_DEFINING_ACTIONS()


//----[  dcList<QuestItemReference>  ]-----------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<QuestItemReference>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPERATOR,
  { "Add Item", &AddNewEntryToCollection<dc::dcList<QuestItemReference>> },
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<QuestItemReference>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<QuestItemReference>::Reference)
  { "Select", &SelectReference<dc::dcList<QuestItemReference>::Reference> },
  { "Clear", &ClearReference },
END_DEFINING_ACTIONS()





//----[  dcList<Quest>  ]------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Quest>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPERATOR,
  { "Add Quest", &AddNewEntryToCollection<dc::dcList<Quest>> },
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<Quest>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(dc::dcList<Quest>::Reference)
  { "Select", &SelectReference<dc::dcList<Quest>::Reference> },
  { "Clear", &ClearReference },
END_DEFINING_ACTIONS()






//----[  NPCQuestInitiation  ]-------------------------------------------------
BEGIN_DEFINING_ACTIONS(NPCQuestInitiation)
END_DEFINING_ACTIONS()

//----[  NPCQuestProgress  ]---------------------------------------------------
BEGIN_DEFINING_ACTIONS(NPCQuestProgress)
END_DEFINING_ACTIONS()

//----[  NPCQuestTermination  ]------------------------------------------------
BEGIN_DEFINING_ACTIONS(NPCQuestTermination)
END_DEFINING_ACTIONS()

