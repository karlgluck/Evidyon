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
#include "texture.h"
#include "terrain.h"
#include "terrainrule.h"





//------------------------------------------------------------------------------------------------
// Name:  TerrainRule
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
TerrainRule::TerrainRule()
{
    member("Priority", &myPriority);
    member("Canvas Types", &myCanvasTypes);
    member("Neighbors:  North-West",   &myNeighborTypes[0]);
    member("Neighbors:  North",        &myNeighborTypes[1]);
    member("Neighbors:  North-East",   &myNeighborTypes[2]);
    member("Neighbors:  East",         &myNeighborTypes[3]);
    member("Neighbors:  South-East",   &myNeighborTypes[4]);
    member("Neighbors:  South",        &myNeighborTypes[5]);
    member("Neighbors:  South-West",   &myNeighborTypes[6]);
    member("Neighbors:  West",         &myNeighborTypes[7]);
    member("Fill Types", &myFillTypes);
}



//------------------------------------------------------------------------------------------------
// Name:  loadPreview
// Desc:  Fills in the preview structure for this resource
//------------------------------------------------------------------------------------------------
bool TerrainRule::loadPreview(Terrain::Preview* terrainPreviewArray,
                               size_t terrainPreviews, Preview* preview)
{
    // Check parameters
    if (!terrainPreviewArray || !preview) return false;

    // Initialize the structure
    preview->terrainRule = this;
    preview->canvasTypes.terrainArray = 0;
    preview->canvasTypes.terrainArraySize = 0;
    for (int n = 0; n < 8; ++n)
    {
        preview->neighborOptions[n].terrainArray = 0;
        preview->neighborOptions[n].terrainArraySize = 0;
    }
    preview->fillTypes.terrainArray = 0;
    preview->fillTypes.terrainArraySize = 0;

    // Fill all of the types
    for (int arrayIndex = 0; arrayIndex < 10; ++arrayIndex)
    {
        Preview::TerrainArray* destArray = 0;
        OptionList* sourceList = 0;
        switch(arrayIndex)
        {
            case 8: destArray = &preview->canvasTypes;        sourceList = &myCanvasTypes;      break;
            case 9: destArray = &preview->fillTypes;          sourceList = &myFillTypes;        break;

            default:
                destArray = &preview->neighborOptions[arrayIndex];
                sourceList = &myNeighborTypes[arrayIndex];
                break;
        }

        // Members of the source list
        const dc::dcGenericResource::Array& sourceMembers = sourceList->getMembers();

        // Create this list
        destArray->terrainArray = new Terrain::Preview*[sourceMembers.size()];
        destArray->terrainArraySize = sourceMembers.size();

        // Check the allocation
        if (APP_FATAL(0 == destArray->terrainArray)("Out of memory"))
        {
            // TODO: unwind previous allocations; this would be a good place for the cleanup structure
            return false;
        }

        // Copy entries into the list
        dc::dcGenericResource::Array::const_iterator i = sourceMembers.begin(),
                                               end = sourceMembers.end();
        for (size_t index = 0; i != end; ++i, ++index)
        {
            // Get the referenced rule
            dc::dcList<Terrain>::Reference* reference = (dcList<Terrain>::Reference*)(*i);
            dc::dcList<Terrain>::Element* element = reference->getReferencedResource();

            // Check to make sure the rule is valid
            if (!APP_WARNING(element == 0)
                ("Terrain Rule %s has a null terrain reference", getPathString().c_str()))
            {
                // Get the index of this element in the table
                size_t terrainIndex = element->getIndex();

                // Obtain the mesh preview from the array
                if (!APP_WARNING(terrainIndex >= terrainPreviews || terrainPreviewArray[terrainIndex].terrain != element)
                        ("Terrain Rule %s references terrain with a mismatched index; ignoring", getName().c_str()))
                {
                    // Save the associated terrain preview
                    destArray->terrainArray[index] = &terrainPreviewArray[terrainIndex];
                }
            }
        }

        // TODO: MAJOR SPEED HELP!
        // sort the terrain types by the pointer value so that we can do a binary search
        // instead of a linear one when applying rules
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  compareTo
// Desc:  Compares this terrain rule with another rule (sorts by priority)
//------------------------------------------------------------------------------------------------
int TerrainRule::compareTo(const dc::dcGenericResource* other) const
{
    if (other == 0 || getTypeID() != other->getTypeID())
        return dc::dcGenericResource::compareTo(other);
    else
        return myPriority.compareTo(&((TerrainRule*)(other))->myPriority);
}





//------------------------------------------------------------------------------------------------
// Name:  TypeName
// Desc:  Returns the name of this class type
//------------------------------------------------------------------------------------------------
std::string TerrainRule::staticTypeName()
{
    return "Terrain Rule";
}





