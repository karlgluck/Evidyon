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
#include <queue>



//------------------------------------------------------------------------------------------------
// Name:  getString
// Desc:  Implements the enum-index-to-string conversion for dc::dcEnum<MapNavigability>
//------------------------------------------------------------------------------------------------
const char* dc::dcEnum<MapNavigability>::getString(MapNavigability value)
{
    switch(value)
    {
        case MAPNAV_WALK: return "Walk";
        case MAPNAV_WADE: return "Wade";
        case MAPNAV_SWIM: return "Swim";
        case MAPNAV_PIT: return "Pit";
        case MAPNAV_LOWWALL: return "Low Wall";
        case MAPNAV_HIGHWALL: return "High Wall";
        case MAPNAV_IMPASSABLE: return "Impassable";
        case MAPNAV_SAFEZONE: return "Safe Zone";
        default: return 0;
    }
}




//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Implements the class name for dc::dcEnum<MapNavigability>
//------------------------------------------------------------------------------------------------
std::string dc::dcEnum<MapNavigability>::staticTypeName()
{
    return "Navigability";
}




//------------------------------------------------------------------------------------------------
// Name:  MapLocationInfo
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
MapLocationInfo::MapLocationInfo()
{
    member("Overwrite Navigability", &myOverwriteNavigability);
    member("Navigability", &myNavigability);
    member("Overwrite Terrain 1", &myOverwriteTerrain1);
    member("Terrain 1", &myTerrainRef1);
    member("Overwrite Height 1", &myOverwriteElevation1);
    member("Height 1", &myElevation1);
    member("Overwrite Terrain 2", &myOverwriteTerrain2);
    member("Terrain 2", &myTerrainRef2);
    member("Overwrite Elevation 2", &myOverwriteElevation2);
    member("Elevation 2", &myElevation2);
    member("Overwrite Scenery", &myOverwriteScenery);
    member("Scenery", &mySceneryRef);

    // Initialize components
    myNavigability.setValue(MAPNAV_WALK);
}




//------------------------------------------------------------------------------------------------
// Name:  loadPreview
// Desc:  Initializes the preview structure for this resource
//------------------------------------------------------------------------------------------------
bool MapLocationInfo::loadPreview(Terrain::Preview* terrainPreviewArray, size_t terrainPreviews,
                                   Scenery::Preview* sceneryPreviewArray, size_t sceneryPreviews,
                                   Preview* preview)
{
    // Check the incoming parameters
    if (APP_ERROR(!terrainPreviewArray || !sceneryPreviewArray || !preview)
            ("Invalid parameter to MapLocationInfo::loadPreview"))
        return false;

    // Initialize the preview structure
    preview->mapLocationInfo = this;
    preview->overwriteNavigability = myOverwriteNavigability.getValue();
    preview->navigability = myNavigability.getValue();
    preview->overwriteScenery = myOverwriteScenery.getValue();
    preview->sceneryPreview = 0;
    preview->overwriteTerrain1 = myOverwriteTerrain1.getValue();
    preview->overwriteTerrain2 = myOverwriteTerrain2.getValue();
    preview->terrainPreview1 = 0;
    preview->terrainPreview2 = 0;
    preview->overwriteElevation1 = myOverwriteElevation1.getValue();
    preview->overwriteElevation2 = myOverwriteElevation2.getValue();
    preview->elevation1 = 0.0f;
    preview->elevation2 = 0.0f;

    // Get the first referenced terrain element
    dc::dcList<Terrain>::Element* terrainElement1 = myTerrainRef1.getReferencedResource();
    if (terrainElement1 != 0) // no warning here because this is valid!
    {
        // Get the index of this texture in the table
        size_t index = terrainElement1->getIndex();

        // Obtain the texture preview from the array
        if (!APP_WARNING(index >= terrainPreviews || terrainPreviewArray[index].terrain != terrainElement1)
                ("MapLocationInfo %s references terrain with a mismatched index; ignoring", getName().c_str()))
        {
            // Save the associated texture preview
            preview->terrainPreview1 = &terrainPreviewArray[index];
        }
    }

    // Get the second referenced terrain element
    dc::dcList<Terrain>::Element* terrainElement2 = myTerrainRef2.getReferencedResource();
    if (terrainElement2 != 0) // no warning here because this is valid!
    {
        // Get the index of this texture in the table
        size_t index = terrainElement2->getIndex();

        // Obtain the texture preview from the array
        if (!APP_WARNING(index >= terrainPreviews || terrainPreviewArray[index].terrain != terrainElement2)
                ("MapLocationInfo %s references terrain with a mismatched index; ignoring", getName().c_str()))
        {
            // Save the associated texture preview
            preview->terrainPreview2 = &terrainPreviewArray[index];
        }
    }

    // Get the referenced scenery element
    dc::dcList<Scenery>::Element* sceneryElement = mySceneryRef.getReferencedResource();
    if (sceneryElement != 0) // no warning here because this is valid!
    {
        // Get the index of this texture in the table
        size_t index = sceneryElement->getIndex();

        // Obtain the texture preview from the array
        if (!APP_WARNING(index >= sceneryPreviews || sceneryPreviewArray[index].scenery != sceneryElement)
                ("MapLocationInfo %s references scenery with a mismatched index; ignoring", getName().c_str()))
        {
            // Save the associated texture preview
            preview->sceneryPreview = &sceneryPreviewArray[index];
        }
    }

    // Get the first referenced height element
    dc::dcList<Elevation>::Element* elevationElement1 = myElevation1.getReferencedResource();
    if (elevationElement1 != 0) // no warning here because this is valid!
        preview->elevation1 = elevationElement1->getValue();

    // Get the second referenced height element
    dc::dcList<Elevation>::Element* elevationElement2 = myElevation2.getReferencedResource();
    if (elevationElement2 != 0) // no warning here because this is valid!
        preview->elevation2 = elevationElement2->getValue();

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this resource types
//------------------------------------------------------------------------------------------------
std::string MapLocationInfo::staticTypeName()
{
    return "Map Location Info";
}



































//------------------------------------------------------------------------------------------------
// Name:  MapLayer
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
MapLayer::MapLayer()
{
    member("Priority", &myPriority);
    member("Source Mask", &myLayerMask);
    member("Location Options", &myLayerOptions);
    member("Rules", &myRules);
}


int CompareTerrainRulePreviews(const VOID* p1, const VOID* p2)
{
    TerrainRule::Preview* rule1 = *((TerrainRule::Preview**)p1);
    TerrainRule::Preview* rule2 = *((TerrainRule::Preview**)p2);

    if (!rule1) return -1;
    if (!rule2) return +1;

    // TODO: could the terrainRule pointer ever be invalid?
    return rule1->terrainRule->compareTo(rule2->terrainRule);
}



//------------------------------------------------------------------------------------------------
// Name:  loadPreview
// Desc:  Sets up the preview for this class
//------------------------------------------------------------------------------------------------
bool MapLayer::loadPreview(LPDIRECT3DDEVICE9 d3dDevice,
                            Terrain::Preview* terrainPreviewArray, size_t terrainPreviews,
                            Scenery::Preview* sceneryPreviewArray, size_t sceneryPreviews,
                            TerrainRule::Preview* terrainRulesArray, size_t terrainRules, Preview* preview)
{
    // Check parameters
    if (!d3dDevice || !preview) return false;

    // Initialize the structure
    preview->mapLayer = this;
    preview->layerMaskArray = 0;
    preview->priority = myPriority.getValue();
    preview->locationInfoOptionArray = 0;
    preview->locationInfoOptions = 0;
    preview->terrainRuleArray = 0;
    preview->terrainRules = 0;

    // Use the helper method to load the mask
    if (APP_ERROR(!this->loadLayerMask(d3dDevice, &preview->layerMaskArray, &preview->layerMaskWidth, &preview->layerMaskHeight))
            ("Couldn't load layer mask")) return false;

    // Get the member list of layer options
    const dc::dcGenericResource::Array& options = myLayerOptions.getMembers();

    // Allocate the list
    if (!options.empty())
    {
        preview->locationInfoOptionArray = new MapLocationInfo::Preview[options.size()];
        if (!APP_WARNING(preview->locationInfoOptionArray == 0)("Out of memory!"))
        {
            // Save the number of options
            preview->locationInfoOptions = options.size();

            // Initialize the layer options by rolling through the list
            dc::dcGenericResource::Array::const_iterator i = options.begin();
            dc::dcGenericResource::Array::const_iterator end = options.end();
            for (size_t index = 0; i != end; ++i, ++index)
            {
                // Cast the iterator into its real type
                dc::dcList<MapLocationInfo>::Element* element = (dcList<MapLocationInfo>::Element*)(*i);

                // Add this element to the array
                // TODO: check the return code and do something with it!
                element->loadPreview(terrainPreviewArray, terrainPreviews,
                                      sceneryPreviewArray, sceneryPreviews, &preview->locationInfoOptionArray[index]);
            }
        }
    }

    // Get the list of terrain rules
    const dc::dcGenericResource::Array& rulesMembers = myRules.getMembers();

    if (!rulesMembers.empty())
    {
        // Allocate the list
        preview->terrainRuleArray = new TerrainRule::Preview*[rulesMembers.size()];

        // Check the memory
        if (!APP_FATAL(preview->terrainRuleArray == 0)("Out of memory!"))
        {
            // Reset the memory
            ZeroMemory(preview->terrainRuleArray, sizeof(TerrainRule::Preview*) * preview->terrainRules);

            // Save the number of rules
            preview->terrainRules = rulesMembers.size();

            // Go through the list of rules and save all of the referenced types
            dc::dcGenericResource::Array::const_iterator i = rulesMembers.begin(),
                                                   end = rulesMembers.end();
            for (size_t index = 0; i != end; ++i, ++index)
            {
                // Get the referenced rule
                dc::dcList<TerrainRule>::Reference* reference = (dcList<TerrainRule>::Reference*)(*i);
                dc::dcList<TerrainRule>::Element* element = reference->getReferencedResource();

                // Check to make sure the rule is valid
                if (!APP_WARNING(element == 0)
                    ("Map Layer %s has a null terrain rule reference", getPathString().c_str()))
                {
                    // Get the index of this element in the table
                    size_t ruleIndex = element->getIndex();

                    // Obtain the mesh preview from the array
                    if (!APP_WARNING(ruleIndex >= terrainRules || terrainRulesArray[ruleIndex].terrainRule != element)
                            ("Map Layer %s references terrain rule with a mismatched index; ignoring", getName().c_str()))
                    {
                        // Save the associated terrain preview
                        preview->terrainRuleArray[index] = &terrainRulesArray[ruleIndex];
                    }
                }
                else
                    preview->terrainRuleArray[index] = 0; // Reset this element
            }

            // Sort the list of rules by priority
            qsort(preview->terrainRuleArray, preview->terrainRules, sizeof(TerrainRule::Preview*),
                   CompareTerrainRulePreviews);
        }
    }

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  compareTo
// Desc:  Sorts map layers by their priority
//------------------------------------------------------------------------------------------------
int MapLayer::compareTo(const dc::dcGenericResource* other) const
{
    if (other == 0 || getTypeID() != other->getTypeID())
        return dc::dcGenericResource::compareTo(other);
    else
        return myPriority.compareTo(&((const MapLayer*)other)->myPriority);
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this resource types
//------------------------------------------------------------------------------------------------
std::string MapLayer::staticTypeName()
{
    return "Map Layer";
}




//------------------------------------------------------------------------------------------------
// Name:  loadLayerMask
// Desc:  
//------------------------------------------------------------------------------------------------
bool MapLayer::loadLayerMask(LPDIRECT3DDEVICE9 d3dDevice, BYTE** maskBits, int* width, int* height)
{
    // Check parameters
    if (APP_ERROR(!d3dDevice || !maskBits || !width || !height)("Invalid parameter to loadLayerMask"))
        return false;

    // Reset output parameters
    *maskBits = 0;
    *width = 0;
    *height = 0;

    // The layer mask texture
    LPDIRECT3DTEXTURE9 maskTexture = NULL;
    BYTE* layerMaskArray = NULL;

    // Load information about the image
    D3DXIMAGE_INFO maskInfo;
    if (APP_WARNING(D3DXGetImageInfoFromFile(myLayerMask.getValue().c_str(), &maskInfo))
        ("Failed to get image info from MapLayer mask file %s", myLayerMask.getValue().c_str()))
        return true; // Succeeds, but layer is useless

    // Load the layer mask as a pure alpha channel in whatever memory pool, making black a the transparent color.  This
    // will discard all of the useless color information, if there is any, and just load what we want: whether or not to
    // place a tile at a location.
    // TODO: should we only load black as transparent if source type doesn't already have transparency?
    if (APP_ERROR(FAILED(D3DXCreateTextureFromFileEx(d3dDevice, myLayerMask.getValue().c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
                                             1, 0, D3DFMT_A8, D3DPOOL_SCRATCH, D3DX_FILTER_NONE,
                                             D3DX_FILTER_NONE, 0xFF000000, NULL, NULL, &maskTexture)))
            ("Couldn't load the source mask %s for map layer %s", myLayerMask.getValue().c_str(), getName().c_str()))
    {
        // This still succeeds, but the layer will be useless
        return true;
    }

    { // Make sure the texture was loaded in the correct format
      D3DSURFACE_DESC desc;
      if (maskTexture->GetLevelDesc(0, &desc)) {
        CONFIRM(desc.Format == D3DFMT_A8) else {
          DEBUG_INFO("Source mask %s wasn't a black and white image", myLayerMask.getValue().c_str());
          maskTexture->Release();
          return false;
        }
      }
    }

    // Figure out how big of a layer mask we need
    size_t maskSize = maskInfo.Width * maskInfo.Height / sizeof(BYTE);

    // Allocate a layer mask
    layerMaskArray = new BYTE[maskSize];
    if (APP_FATAL(!layerMaskArray)("Out of memory"))
    {
        maskTexture->Release();
        return false;
    }

    // Reset the mask
    ZeroMemory(layerMaskArray, sizeof(BYTE) * maskSize);

    // Save the mask size

    // Fill the mask
    D3DLOCKED_RECT lr;
    if (SUCCEEDED(maskTexture->LockRect(0, &lr, NULL, D3DLOCK_READONLY)))
    {
        // Get the pointer to the bit-data of this surface
        BYTE* bits = (BYTE*)lr.pBits;

        for (unsigned int x = 0; x < maskInfo.Width; ++x)
            for (unsigned int y = 0; y < maskInfo.Height; ++y)
            {
                unsigned int index = y * maskInfo.Width + x;

                // If the color here is opaque, assign a '1' to the preview
                if (bits[lr.Pitch * y + x] == 0x00)
                    layerMaskArray[index / 8] |= 0x01 << (index % 8);
            }
    }

    // Release the texture
    maskTexture->UnlockRect(0);
    maskTexture->Release();

    // Assign output values
    *maskBits = layerMaskArray;
    *height = maskInfo.Height;
    *width  = maskInfo.Width;

    // Success
    return true;
}