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
#include "resource"
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include <dcx/d3d>
#include "../common/isometriccamera.h"  // The common camera for viewing the world
#include <dcx/win32>
#include <dcx/eventtimer>
#include <windowsx.h>
#include <set>
#include "commctrl.h"
#include "atlasterrainmesh.h"
#include <vector>
#include "atlaseditdialog.h"
#include "atlascompilationcontext.h"



void AtlasEditDialog::removeCacheEntry(int x, int y)
{
    MapCacheType::iterator global = myGlobalMapCache.find(MapCacheType::key_type(x, y)),
                            local = myLocalMapCache.find(MapCacheType::key_type(x, y));
    MapLocation* loc = NULL;
    if (local != myLocalMapCache.end())
    {
        loc = local->second;
        myLocalMapCache.erase(local);
    }
    if (global != myGlobalMapCache.end())
    {
        loc = global->second;
        myGlobalMapCache.erase(global);
    }
    SAFE_DELETE(loc);
}



ToolboxTab* AtlasEditDialog::getTab(ToolboxTab::TabIndex index)
{
    switch(index)
    {
        case ToolboxTab::TABINDEX_TERRAIN1: return &myTerrain1Tab;
        case ToolboxTab::TABINDEX_TERRAIN2: return &myTerrain2Tab;
        case ToolboxTab::TABINDEX_SCENERY:  return &mySceneryTab;
        case ToolboxTab::TABINDEX_ELEVATION1:  return &myElevation1Tab;
        case ToolboxTab::TABINDEX_ELEVATION2:  return &myElevation2Tab;
        case ToolboxTab::TABINDEX_NAVIGABILITY: return &myNavigabilityTab;
        default: return 0;
    }
}

void AtlasEditDialog::readTabDataFromToolbox(ToolboxTab::TabIndex tabIndex)
{
    // Get the current tab and its information structure
    ToolboxTab* tab = getTab(tabIndex);

    // Save the checkbox (overwrite) flag
    tab->Overwrite = FALSE != Button_GetCheck(GetDlgItem(myToolbox, IDC_CHECK_OVERWRITE));

    // Save the new selection
    std::list<int> selection;
    if (!tab->Selection.empty())
        tab->Selection.clear();

    // Get the terrain selection
    dcxWin32GetListMultipleSelection(GetDlgItem(myToolbox, IDC_LISTBOX), &selection);
    for (std::list<int>::iterator i = selection.begin(); i != selection.end(); ++i)
        tab->Selection.insert(*i);
}

void AtlasEditDialog::updateCurrentToolboxTab()
{
    int currentTab = TabCtrl_GetCurSel(GetDlgItem(myToolbox, IDC_TABS));
    ToolboxTab* tab = getTab((ToolboxTab::TabIndex)currentTab);

    // Clear the list box
    HWND hListBox = GetDlgItem(myToolbox, IDC_LISTBOX);
    ListBox_ResetContent(hListBox);

    // Get the items that need to be displayed and show them
    dc::dcGenericResource* listEntries = tab->CollectionContents;

    // Toggle the check box
    Button_SetCheck(GetDlgItem(myToolbox, IDC_CHECK_OVERWRITE), tab->Overwrite);

    // Add the members of this tab to the box, and select the ones needed
    ToolboxTab::ListBoxSelection* selection = &tab->Selection;
    if (listEntries)
    {
        dc::dcGenericResource::Array::const_iterator i;
        dc::dcGenericResource::Array::const_iterator end;
        i = listEntries->getMembers().begin();
        end = listEntries->getMembers().end();
        for (size_t index = 0; i != end; ++i, ++index)
        {
            dc::dcGenericResource* element = *i;
            int item = ListBox_AddString(hListBox, element->getName().c_str());
            ListBox_SetItemData(hListBox, item, element);
            ListBox_SetSel(hListBox, selection->find(index) != selection->end(), item);
        }
    }
    else
    {
        // Add the navigability types
        for (size_t index = 0; dc::dcEnum<MapNavigability>::getString((MapNavigability)index) != 0; ++index)
        {
            int item = ListBox_AddString(hListBox, dc::dcEnum<MapNavigability>::getString((MapNavigability)index));
            ListBox_SetItemData(hListBox, item, index);
            ListBox_SetSel(hListBox, selection->find(index) != selection->end(), item);
        }
    }
}

INT_PTR AtlasEditDialog::dialogNotification(HWND hDlg, HWND hControl, UINT_PTR controlID, UINT notification)
{
    if (notification == BN_CLICKED)
    {
        switch(controlID)
        {
            case IDC_BUTTON_APPLYSPECIFIC:
                    toolbox_ApplyToSelection();
                break;
        }
    }

    // If the tab control is about to change indices, save the current data
    if (controlID == IDC_TABS && notification == TCN_SELCHANGING)
    {
        int currentTab = TabCtrl_GetCurSel(hControl);
        readTabDataFromToolbox((ToolboxTab::TabIndex)currentTab);
    }

    // When the user picks a new tab, we have to set up the display for that tab
    if (controlID == IDC_TABS && notification == TCN_SELCHANGE)
    {
        updateCurrentToolboxTab();
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
INT_PTR AtlasEditDialog::dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (hDlg == myMainWindow)
    {
        bool ret;
        myKeyboard.handleWindowsMessage(hDlg, uMsg, wParam, lParam, &ret);
        myMouse.handleWindowsMessage(hDlg, uMsg, wParam, lParam, &ret);
        if (uMsg == WM_CLOSE)
            DestroyWindow(hDlg);
        if (uMsg == WM_PAINT)
        {
            renderScene();
            ValidateRect(hDlg, NULL);
            return TRUE;
        }
        return 0;
    }
    else
    {
        if (uMsg == WM_CONTEXTMENU)
            return toolbox_ContextMenu((HWND)wParam, MAKEPOINTS(lParam));
        /*
        if (uMsg == WM_ACTIVATE)
        {
            if (LOWORD(wParam) == WA_INACTIVE)
            {
                // Minimize the toolbox if the user deactivates it
                dcx::dcxWin32SetWindowClientSize(hDlg, myToolboxClientSize.cx, 0);
            }
            else
            {
                // Restore the toolbox if it becomes active
                dcx::dcxWin32SetWindowClientSize(hDlg, myToolboxClientSize.cx, myToolboxClientSize.cy);
            }
        }*/
        return 0;
    }
}




/**
 * Sets up the current texture being used to render on the Direct3D device.  This
 * optimizes calls by only changing the texture if it's different from the current one.
 */
void AtlasEditDialog::setRenderingTexture(Texture::Preview* texture)
{
    // If this texture already is set, don't set it again
    if (texture == myCurrentRenderingTexture)
        return;

    // Assign the current texture
    myCurrentRenderingTexture = texture;

    // If the texture was being reset, kill the D3D texture and exit
    if (!texture || !texture->imagePreview)
        myD3DDevice->SetTexture(0, NULL);
    else
    {
        // Set the D3D texture
        myD3DDevice->SetTexture(0, texture->imagePreview->d3dTexture);

        // Initialize the state
        if (texture->texture)
            texture->texture->configureTextureStageState(myD3DDevice, 0);
    }
}

INT_PTR AtlasEditDialog::toolbox_ContextMenu(HWND hWnd, POINTS point)
{
    int currentTab = TabCtrl_GetCurSel(GetDlgItem(myToolbox, IDC_TABS));
    ToolboxTab* tab = getTab((ToolboxTab::TabIndex)currentTab);

    // We can only edit valid elements with collection entries
    if (tab == NULL || tab->CollectionContents == NULL)
        return TRUE;

    // Get the current selection in the list-box
    std::list<int> selectionIndices;
    if (1 != dcx::dcxWin32GetListMultipleSelection(hWnd, &selectionIndices))
        return TRUE;

    // Obtain the selected element
    int selectionIndex = selectionIndices.front();

    // Get the collection list's members
    const dc::dcGenericResource::Array& collectionMembers = tab->CollectionContents->getMembers();

    // Check the element index (this shouldn't happen, but we don't want exceptions)
    if (selectionIndex >= collectionMembers.size())
        return TRUE;

    // Obtain the selected resource
    dc::dcGenericResource* resourceElement = collectionMembers.at(selectionIndex);

    // Make sure the resource is valid
    if (resourceElement == NULL)
        return TRUE;

    // Edit this resource
    EditResourceDialog erd;
    if (erd.execute(resourceElement))
    {
        // Get lists
        const dc::dcGenericResource::Array& textureTableMembers   = textureTable->getMembers();
        const dc::dcGenericResource::Array& skinTableMembers      = skinList->getMembers();
        const dc::dcGenericResource::Array& meshTableMembers      = meshTable->getMembers();
        const dc::dcGenericResource::Array& sceneryTableMembers   = sceneryList->getMembers();
        const dc::dcGenericResource::Array& terrainTableMembers   = terrainList->getMembers();

        // Reload this element
        // TODO: if, in the future, any data needs to be freed from these elements
        // before they are reloaded, do so here!
        if (collectionMembers == terrainTableMembers)
        {
            terrain[selectionIndex].terrain->loadPreview(textures, textureTableMembers.size(), &terrain[selectionIndex]); 
        }
        else if (collectionMembers == sceneryTableMembers)
        {
            // Load this scenery element again
            scenery[selectionIndex].scenery->loadPreview(meshes, meshTableMembers.size(),
                                                          skins, skinTableMembers.size(), &scenery[selectionIndex]);

            // Find the scenery's source mesh
            Mesh::Preview* meshPreview = scenery[selectionIndex].meshPreview;
            if (meshPreview && meshPreview->mesh)
            {
                // Free this mesh, if it exists
                SAFE_RELEASE(meshPreview->d3dxMesh);

                // Re-load the mesh, just incase something about it changed
                meshPreview->mesh->loadPreview(myD3DDevice, meshPreview);
            }
        }
    }

    // Success
    return TRUE;
}


void AtlasEditDialog::toolbox_ApplyToSelection()
{
    // Update the data from the current tab, because it might not have been saved yet
    readTabDataFromToolbox((ToolboxTab::TabIndex)TabCtrl_GetCurSel(GetDlgItem(myToolbox, IDC_TABS)));

    // Keeps track of elements
    typedef std::list<DetailedLocationMap::Element*> DetailedLocationList;
    DetailedLocationList detailedLocations;
    MapCacheKeySet mcks;
    MapCacheKeySet mcksTerrainMeshOnly;

    // Re-add all of these entries to the local cache
    for (Selection::iterator i = mySelection.begin(); i != mySelection.end(); ++i)
    {
        // The selection region
        RECT sel = { i->first.x, i->first.y, i->second.x+1, i->second.y+1 };

        // Go through each location in this selection entry
        for (int x = sel.left; x < sel.right; ++x)
            for (int y = sel.top; y < sel.bottom; ++y)
            {
                // Try to find this in the current set of detailed locations
                DetailedLocationMap::Element* detailedLocation =
                    detailedLocationMap->find(DetailedLocationMap::Key::BasicType(x, y));

                // Handle the location result
                if (detailedLocation)
                {
                    // If this element was found, remove it from the internal "loaded" list so that
                    // we can add it later without problems.
                    detailedMapLocations.erase(WorldLocationType::BasicType(detailedLocation->getKey()->getValue()));
                }
                else
                {
                    // Create a new location
                    char name[64];
                    sprintf_s(name, sizeof(name), "%i, %i", x, y);
                    detailedLocation = (DetailedLocationMap::Element*)detailedLocationMap->generateElement(name);
                    detailedLocation->getKey()->setX(x);
                    detailedLocation->getKey()->setY(y);
                }

                // Add this to the local list of items that will be reconstructed
                detailedLocations.push_back(detailedLocation);

                // Remove this element from the cache so that it gets re-loaded
                removeCacheEntry(x, y);

                // Add this element to the list of entries to re-add
                mcks.insert(MapCacheKeySet::key_type(x, y));
            }
    }

    // For all of the entries being changed, make sure that the surrounding
    // terrain types are going to be updated
    for (MapCacheKeySet::iterator i = mcks.begin(); i != mcks.end(); ++i)
    {
        MapCacheKeySet::value_type neighbors[8] = {
            MapCacheKeySet::value_type(i->first - 1, i->second - 1),
            MapCacheKeySet::value_type(i->first + 0, i->second - 1),
            MapCacheKeySet::value_type(i->first + 1, i->second - 1),
            MapCacheKeySet::value_type(i->first - 1, i->second + 0),
            MapCacheKeySet::value_type(i->first + 1, i->second + 0),
            MapCacheKeySet::value_type(i->first - 1, i->second + 1),
            MapCacheKeySet::value_type(i->first + 0, i->second + 1),
            MapCacheKeySet::value_type(i->first + 1, i->second + 1),
        };

        // If any of these neighbors is invalid, add it to the terrain update set
        for (int n = 0; n < 8; ++n)
            if (mcks.end() == mcks.find(neighbors[n]) &&
                mcksTerrainMeshOnly.end() == mcksTerrainMeshOnly.find(neighbors[n]))
            {
                // Clear the terrain mesh subset flags so that they are sure to be updated
                MapCacheType::iterator neighborIter = myGlobalMapCache.find(neighbors[n]);
                if (neighborIter != myGlobalMapCache.end())
                {
                    neighborIter->second->cachedMembers &= ~(CACHE_LAYERMESHSUBSET(0)|CACHE_LAYERMESHSUBSET(1));
                }

                // Add this entry to the list of surrounding neighbors to be updated
                mcksTerrainMeshOnly.insert(neighbors[n]);
            }
    }

    // If nothing is selected, we're done
    if (detailedLocations.empty()) return;

    // Apply terrain1
    if (myTerrain1Tab.Overwrite)
    {
        // Get the selected elements
        std::vector<dcList<Terrain>::Element*> selection;
        ToolboxTab::ListBoxSelection::iterator i, end = myTerrain1Tab.Selection.end();
        for (i = myTerrain1Tab.Selection.begin(); i != end; ++i)
            selection.push_back((dcList<Terrain>::Element*)terrain[*i].terrain);

        // Get the number of options
        size_t options = selection.size();

        // Go through all of the selected locations
        for (DetailedLocationList::iterator d = detailedLocations.begin(); d != detailedLocations.end(); ++d)
        {
            // Set the overwrite flag
            (*d)->getValue()->getOverwriteTerrain1()->setValue(true);

            // Set the element
            if (options > 0)
                (*d)->getValue()->getTerrain1Ref()->setReferencedResource(selection.at(rand() % options));
            else
                (*d)->getValue()->getTerrain1Ref()->setReferencedResource(NULL);
        }
    }

    // Apply terrain2
    if (myTerrain2Tab.Overwrite)
    {
        // Get the selected elements
        std::vector<dcList<Terrain>::Element*> selection;
        ToolboxTab::ListBoxSelection::iterator i, end = myTerrain2Tab.Selection.end();
        for (i = myTerrain2Tab.Selection.begin(); i != end; ++i)
            selection.push_back((dcList<Terrain>::Element*)terrain[*i].terrain);

        // Get the number of options
        size_t options = selection.size();

        // Go through all of the selected locations
        for (DetailedLocationList::iterator d = detailedLocations.begin(); d != detailedLocations.end(); ++d)
        {
            // Set the overwrite flag
            (*d)->getValue()->getOverwriteTerrain2()->setValue(true);

            // Set the element
            if (options > 0)
                (*d)->getValue()->getTerrain2Ref()->setReferencedResource(selection.at(rand() % options));
            else
                (*d)->getValue()->getTerrain2Ref()->setReferencedResource(NULL);
        }
    }
    
    // Apply scenery
    if (mySceneryTab.Overwrite)
    {
        // Get the selected elements
        std::vector<dcList<Scenery>::Element*> selection;
        ToolboxTab::ListBoxSelection::iterator i, end = mySceneryTab.Selection.end();
        for (i = mySceneryTab.Selection.begin(); i != end; ++i)
            selection.push_back((dcList<Scenery>::Element*)scenery[*i].scenery);

        // Get the number of options
        size_t options = selection.size();

        // Go through all of the selected locations
        for (DetailedLocationList::iterator d = detailedLocations.begin(); d != detailedLocations.end(); ++d)
        {
            // Set the overwrite flag
            (*d)->getValue()->getOverwriteScenery()->setValue(true);

            // Set the element
            if (options > 0)
                (*d)->getValue()->getSceneryRef()->setReferencedResource(selection.at(rand() % options));
            else
                (*d)->getValue()->getSceneryRef()->setReferencedResource(NULL);
        }
    }

    // Apply elevation1
    if (myElevation1Tab.Overwrite)
    {
        // Get the selected elements
        std::vector<dcList<Elevation>::Element*> selection;
        ToolboxTab::ListBoxSelection::iterator i, end = myElevation1Tab.Selection.end();
        const dc::dcGenericResource::Array& groundHeightMembers = globalEditor()->getRootResource()->getElevations()->getMembers();
        for (i = myElevation1Tab.Selection.begin(); i != end; ++i)
            selection.push_back((dcList<Elevation>::Element*)groundHeightMembers.at(*i));

        // Get the number of options
        size_t options = selection.size();

        // Go through all of the selected locations
        for (DetailedLocationList::iterator d = detailedLocations.begin(); d != detailedLocations.end(); ++d)
        {
            // Set the overwrite flag
            (*d)->getValue()->getOverwriteElevation1()->setValue(true);

            // Set the element
            if (options > 0)
                (*d)->getValue()->getElevation1()->setReferencedResource(selection.at(rand() % options));
            else
                (*d)->getValue()->getElevation1()->setReferencedResource(NULL);
        }
    }

    // Apply elevation2
    if (myElevation2Tab.Overwrite)
    {
        // Get the selected elements
        std::vector<dcList<Elevation>::Element*> selection;
        ToolboxTab::ListBoxSelection::iterator i, end = myElevation2Tab.Selection.end();
        const dc::dcGenericResource::Array& groundHeightMembers = globalEditor()->getRootResource()->getElevations()->getMembers();
        for (i = myElevation2Tab.Selection.begin(); i != end; ++i)
            selection.push_back((dcList<Elevation>::Element*)groundHeightMembers.at(*i));

        // Get the number of options
        size_t options = selection.size();

        // Go through all of the selected locations
        for (DetailedLocationList::iterator d = detailedLocations.begin(); d != detailedLocations.end(); ++d)
        {
            // Set the overwrite flag
            (*d)->getValue()->getOverwriteElevation2()->setValue(true);

            // Set the element
            if (options > 0)
                (*d)->getValue()->getElevation2()->setReferencedResource(selection.at(rand() % options));
            else
                (*d)->getValue()->getElevation2()->setReferencedResource(NULL);
        }
    }

    // Apply navigability
    if (myNavigabilityTab.Overwrite)
    {
        // Get the selected elements
        std::vector<MapNavigability> selection;
        ToolboxTab::ListBoxSelection::iterator i, end = myNavigabilityTab.Selection.end();
        for (i = myNavigabilityTab.Selection.begin(); i != end; ++i)
            selection.push_back((MapNavigability)(*i));

        // Get the number of options
        size_t options = selection.size();

        // Go through all of the selected locations
        for (DetailedLocationList::iterator d = detailedLocations.begin(); d != detailedLocations.end(); ++d)
        {
            // Set the overwrite flag
            (*d)->getValue()->getOverwriteNavigability()->setValue(true);

            // Set the element
            if (options > 0)
                (*d)->getValue()->getNavigability()->setValue(selection.at(rand() % options));
            else
            {
                // We can't set a "no" navigability, so just disable here
                (*d)->getValue()->getOverwriteNavigability()->setValue(false);
            }
        }
    }

    // Get member counts necessary to re-load the MapLocationInfo::Preview 
    size_t sceneryCount = sceneryList->getMembers().size();
    size_t terrainCount = terrainList->getMembers().size();

    // Add all of the entries to the detailed locations list again
    for (DetailedLocationList::iterator d = detailedLocations.begin(); d != detailedLocations.end(); ++d)
    {
        WorldLocationType* key = (*d)->getKey();
        MapLocationInfo* value = (*d)->getValue();

        // Create entry components
        WorldLocationType::BasicType location(key->getValue());
        MapLocationInfo::Preview locationPreview;
        value->loadPreview(terrain, terrainCount, scenery, sceneryCount, &locationPreview);

        // Add this entry to the table
        detailedMapLocations.insert(DetailedLocationPreviewMap::value_type(location, locationPreview));
    }

    // Re-add these entries to the internal list
    addEntriesToCache(&mcks, &myLocalMapCache, CACHE_ALL);

    // Re-add the surrounding entries' terrain mesh
    addEntriesToCache(&mcksTerrainMeshOnly, &myLocalMapCache, CACHE_LAYERMESHSUBSET(0)|CACHE_LAYERMESHSUBSET(1));

    // Bounce the selection to show the change
    mySelectionBounceHeight = 4.0f;
    mySelectTime = GetTickCount();
}




//------------------------------------------------------------------------------------------------
// Name:  AtlasEditDialog
// Desc:  
//------------------------------------------------------------------------------------------------
AtlasEditDialog::AtlasEditDialog(Atlas* atlas)
{
    myShowScenery = TRUE;
    myShowNavigability = TRUE;
    myShowTerrain1 = TRUE;
    myShowTerrain2 = TRUE;
    myShowWireframe = FALSE;
    myToolbox = NULL;

    myAtlas = atlas;

    // Get references to all of the relavent information
    projectVolucris = globalEditor()->getRootResource();
    imageTable      = projectVolucris->getImageTable();
    textureTable    = projectVolucris->getTextureTable();
    skinList       = projectVolucris->getSkinList();
    meshTable       = projectVolucris->getMeshTable();
    sceneryList    = projectVolucris->getSceneryList();
    terrainList    = projectVolucris->getTerrainList();
    terrainRuleList = projectVolucris->getTerrainRulesList();
    mapLayerList    = myAtlas->getMapLayers();
    detailedLocationMap = myAtlas->getDetailedLocations();

    // Reset the arrays
    mapLayers = NULL;
    terrainRules = NULL;
    terrain = NULL;
    ZeroMemory(defaultTerrain, sizeof(defaultTerrain));
    scenery = NULL;
    meshes = NULL;
    skins = NULL;
    textures = NULL;
    images = NULL;

    // Reset the cursor location
    myCursorLocation.x = 0;
    myCursorLocation.y = 0;

    ZeroMemory(myNavigabilityTextures, sizeof(myNavigabilityTextures));

    myCurrentLocation = D3DXVECTOR2(myAtlas->getSpawnLocationValue().first, myAtlas->getSpawnLocationValue().second);
    myZoom = 1.0f;
    myCursorTexture = NULL;
    myViewGranularity = 1;

    myTerrainMesh = NULL;
    myCurrentRenderingTexture = NULL;

    // Initialize the toolbox tabs
    myTerrain1Tab.CollectionContents = terrainList;
    myTerrain2Tab.CollectionContents = terrainList;
    mySceneryTab.CollectionContents = sceneryList;
    myElevation1Tab.CollectionContents = projectVolucris->getElevations();
    myElevation2Tab.CollectionContents = projectVolucris->getElevations();
    myNavigabilityTab.CollectionContents = 0; // TODO: add something for navigability here

    /// This is the area that is currently stored in the local map cache
    SetRect(&myCachedViewingRegion, 0, 0, 0, 0);
}





//------------------------------------------------------------------------------------------------
// Name:  loadResources
// Desc:  
//------------------------------------------------------------------------------------------------
bool AtlasEditDialog::loadResources()
{
    // Make sure the map layer list is sorted so that we get the layers in the correct
    // priority order.
    mapLayerList->sort();

    // Get the member lists
    const dc::dcGenericResource::Array& imageTableMembers     = imageTable->getMembers();
    const dc::dcGenericResource::Array& textureTableMembers   = textureTable->getMembers();
    const dc::dcGenericResource::Array& skinTableMembers      = skinList->getMembers();
    const dc::dcGenericResource::Array& meshTableMembers      = meshTable->getMembers();
    const dc::dcGenericResource::Array& sceneryTableMembers   = sceneryList->getMembers();
    const dc::dcGenericResource::Array& terrainTableMembers   = terrainList->getMembers();
    const dc::dcGenericResource::Array& terrainRuleListMembers=terrainRuleList->getMembers();
    const dc::dcGenericResource::Array& mapLayerMembers       = mapLayerList->getMembers();
    const dc::dcGenericResource::Array& detailedLocationMembers = detailedLocationMap->getMembers();

    // Create arrays for all of the relavent tables
    images     = new Image::Preview[imageTableMembers.size()];
    textures   = new Texture::Preview[textureTableMembers.size()];
    skins      = new Skin::Preview[skinTableMembers.size()];
    meshes     = new Mesh::Preview[meshTableMembers.size()];
    scenery    = new Scenery::Preview[sceneryTableMembers.size()];
    terrain    = new Terrain::Preview[terrainTableMembers.size()];
    terrainRules=new TerrainRule::Preview[terrainRuleListMembers.size()];
    mapLayers  = new MapLayer::Preview[mapLayerMembers.size()];

    // Display a warning message if we can't allocate enough memory
    if (APP_ERROR(!images || !textures || !skins || !meshes || !scenery || !terrain || !terrainRules || !mapLayers)("Out of memory!"))
    {
        // Free any memory that was allocated
        SAFE_DELETE(images);
        SAFE_DELETE(textures);
        SAFE_DELETE(skins);
        SAFE_DELETE(meshes);
        SAFE_DELETE(scenery);
        SAFE_DELETE(terrain);
        SAFE_DELETE(terrainRules);
        SAFE_DELETE(mapLayers);

        // Failure
        return false;
    }

    // Get the source Direct3D device
    LPDIRECT3DDEVICE9 d3dDevice = globalEditor()->getD3DDevice();

    // Iterators for use during loading
    dc::dcGenericResource::Array::const_iterator i;
    dc::dcGenericResource::Array::const_iterator end;
    size_t index;

    // Load the image table
    i = imageTableMembers.begin();
    end = imageTableMembers.end();
    for (index = 0; i != end; ++i, ++index)
        ((dcTable<Image>::Element*)(*i))->getImplementation()->loadPreview(d3dDevice, &images[index]);

    // Load the texture table
    i = textureTableMembers.begin();
    end = textureTableMembers.end();
    for (index = 0; i != end; ++i, ++index)
        ((dcTable<Texture>::Element*)(*i))->getImplementation()->loadPreview(images, imageTableMembers.size(), &textures[index]);

    // Load the skin table
    i = skinTableMembers.begin();
    end = skinTableMembers.end();
    for (index = 0; i != end; ++i, ++index)
        ((dcList<Skin>::Element*)(*i))->loadPreview(textures, textureTableMembers.size(), &skins[index]);

    // Load the mesh table
    i = meshTableMembers.begin();
    end = meshTableMembers.end();
    for (index = 0; i != end; ++i, ++index)
        ((dcTable<Mesh>::Element*)(*i))->getImplementation()->loadPreview(d3dDevice, &meshes[index]);

    // Load the scenery table
    i = sceneryTableMembers.begin();
    end = sceneryTableMembers.end();
    for (index = 0; i != end; ++i, ++index)
        ((dcList<Scenery>::Element*)(*i))->loadPreview(meshes, meshTableMembers.size(), skins, skinTableMembers.size(), &scenery[index]);

    // Load the terrain table
    i = terrainTableMembers.begin();
    end = terrainTableMembers.end();
    for (index = 0; i != end; ++i, ++index)
        ((dcList<Terrain>::Element*)(*i))->loadPreview(textures, textureTableMembers.size(), &terrain[index]);

    // Load the default terrain
    for (index = 0; index < 2; ++index)
    {
        dc::dcList<Terrain>::Element* terrainElement = myAtlas->getDefaultTerrain(index)->getReferencedResource();
        if (terrainElement)
            terrainElement->loadPreview(textures, textureTableMembers.size(), &defaultTerrain[index]);
        defaultTerrainElevation[index] = myAtlas->getDefaultTerrainElevationValue(index);
    }

    // Load the terrain rules table
    i = terrainRuleListMembers.begin();
    end = terrainRuleListMembers.end();
    for (index = 0; i != end; ++i, ++index)
        ((dcList<TerrainRule>::Element*)(*i))->loadPreview(terrain, terrainTableMembers.size(), &terrainRules[index]);

    // Load the map layer list
    i = mapLayerMembers.begin();
    end = mapLayerMembers.end();
    for (index = 0; i != end; ++i, ++index)
        ((dcList<MapLayer>::Element*)(*i))->loadPreview(d3dDevice, terrain, terrainTableMembers.size(),
                                                                    scenery, sceneryTableMembers.size(),
                                                                    terrainRules, terrainRuleListMembers.size(), &mapLayers[index]);

    // Load the detailed map locations
    i = detailedLocationMembers.begin();
    end = detailedLocationMembers.end();
    for (index = 0; i != end; ++i, ++index)
    {
        dc::dcMap<WorldLocationType,MapLocationInfo>::Element* element = ((dcMap<WorldLocationType,MapLocationInfo>::Element*)(*i));
        WorldLocationType* key = element->getKey();
        MapLocationInfo* value = element->getValue();

        // Create entry components
        WorldLocationType::BasicType location(key->getValue());
        MapLocationInfo::Preview locationPreview;
        value->loadPreview(terrain, terrainTableMembers.size(), scenery, sceneryTableMembers.size(), &locationPreview);

        // Add this entry to the table
        detailedMapLocations.insert(DetailedLocationPreviewMap::value_type(location, locationPreview));
    }

    // Load navigability textures
    D3DXCreateTextureFromResource(d3dDevice, NULL, MAKEINTRESOURCE(IDB_GREEN), &myNavigabilityTextures[0]);
    D3DXCreateTextureFromResource(d3dDevice, NULL, MAKEINTRESOURCE(IDB_BLUE), &myNavigabilityTextures[1]);
    D3DXCreateTextureFromResource(d3dDevice, NULL, MAKEINTRESOURCE(IDB_DARKBLUE), &myNavigabilityTextures[2]);
    D3DXCreateTextureFromResource(d3dDevice, NULL, MAKEINTRESOURCE(IDB_BLACK), &myNavigabilityTextures[3]);
    D3DXCreateTextureFromResource(d3dDevice, NULL, MAKEINTRESOURCE(IDB_DARKRED), &myNavigabilityTextures[4]);
    D3DXCreateTextureFromResource(d3dDevice, NULL, MAKEINTRESOURCE(IDB_RED), &myNavigabilityTextures[5]);
    D3DXCreateTextureFromResource(d3dDevice, NULL, MAKEINTRESOURCE(IDB_DARKRED), &myNavigabilityTextures[6]);
    D3DXCreateTextureFromResource(d3dDevice, NULL, MAKEINTRESOURCE(IDB_WHITE), &myNavigabilityTextures[7]);

    // Create the terrain mesh
    CreateTerrainMesh(d3dDevice, &myTerrainMesh);

    // Load the cursor textures
    D3DXCreateTextureFromResource(d3dDevice, NULL, MAKEINTRESOURCE(IDB_BITMAP2), &myCursorTexture);

    // Success
    return true;
}







//------------------------------------------------------------------------------------------------
// Name:  unloadResources
// Desc:  
//------------------------------------------------------------------------------------------------
void AtlasEditDialog::unloadResources()
{
    // Get rid of the terrain mesh
    SAFE_RELEASE(myTerrainMesh);

    // Erase the cursor texture
    SAFE_RELEASE(myCursorTexture);

    // Release all of the navigability textures
    for (int i = 0; i < sizeof(myNavigabilityTextures)/sizeof(LPDIRECT3DTEXTURE9); ++i)
        SAFE_RELEASE(myNavigabilityTextures[i]);

    // Get the member lists
    const dc::dcGenericResource::Array& imageTableMembers     = imageTable->getMembers();
    const dc::dcGenericResource::Array& textureTableMembers   = textureTable->getMembers();
    const dc::dcGenericResource::Array& skinTableMembers      = skinList->getMembers();
    const dc::dcGenericResource::Array& meshTableMembers      = meshTable->getMembers();
    const dc::dcGenericResource::Array& sceneryTableMembers   = sceneryList->getMembers();
    const dc::dcGenericResource::Array& terrainTableMembers   = terrainList->getMembers();
    const dc::dcGenericResource::Array& terrainRuleListMembers= terrainRuleList->getMembers();
    const dc::dcGenericResource::Array& mapLayerMembers       = mapLayerList->getMembers();
    const dc::dcGenericResource::Array& detailedLocationMembers = detailedLocationMap->getMembers();

    // Get rid of any entries in the cache
    clearMapCache();

    // Erase allocated information from the map layers
    if (mapLayers != 0)
    {
        for (size_t i = 0; i < mapLayerMembers.size(); ++i)
        {
            SAFE_DELETE_ARRAY(mapLayers[i].locationInfoOptionArray);
            SAFE_DELETE_ARRAY(mapLayers[i].layerMaskArray);
            SAFE_DELETE_ARRAY(mapLayers[i].terrainRuleArray);
        }
    }

    // Get rid of elements in the terrain rules
    if (terrainRules != 0)
    {
        for (size_t i = 0; i < terrainRuleListMembers.size(); ++i)
        {
            for (size_t n = 0; n < 8; ++n)
                SAFE_DELETE_ARRAY(terrainRules[i].neighborOptions[n].terrainArray);
            SAFE_DELETE_ARRAY(terrainRules[i].canvasTypes.terrainArray);
            SAFE_DELETE_ARRAY(terrainRules[i].fillTypes.terrainArray);
        }
    }

    // Erase allocated information from the meshes
    if (meshes != 0)
    {
        for (size_t i = 0; i < meshTableMembers.size(); ++i)
        {
            // Get rid of the texture from the images
            SAFE_RELEASE(meshes[i].d3dxMesh);
        }
    }

    // Erase any allocated information from the images
    if (images != 0)
    {
        for (size_t i = 0; i < imageTableMembers.size(); ++i)
        {
            // Get rid of the texture from the images
            SAFE_RELEASE(images[i].d3dTexture);
        }
    }

    // Delete the arrays
    SAFE_DELETE_ARRAY(mapLayers);
    SAFE_DELETE_ARRAY(terrainRules);
    SAFE_DELETE_ARRAY(terrain);
    SAFE_DELETE_ARRAY(scenery);
    SAFE_DELETE_ARRAY(meshes);
    SAFE_DELETE_ARRAY(skins);
    SAFE_DELETE_ARRAY(textures);
    SAFE_DELETE_ARRAY(images);
}





//------------------------------------------------------------------------------------------------
// Name:  createToolbox
// Desc:  
//------------------------------------------------------------------------------------------------
void AtlasEditDialog::createToolbox()
{
    // Get lists required to build the toolbox
    const dc::dcGenericResource::Array& sceneryTableMembers   = sceneryList->getMembers();
    const dc::dcGenericResource::Array& terrainTableMembers   = terrainList->getMembers();

    dcx::dcxInitCommonControls();

    // Create the toolbox dialog window
    myToolbox = createDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ATLAS_TOOLBOX), myMainWindow);


    // Set up the tab control
    HWND hTabs = GetDlgItem(myToolbox, IDC_TABS);
    {
        // Initialize the item
        TCITEM tie;
        tie.mask = TCIF_TEXT;
        for (int i = (int)ToolboxTab::TABINDEX_TERRAIN1; i <= (int)ToolboxTab::TABINDEX_NAVIGABILITY; ++i)
        {
            // Set this entry's text
            switch(i)
            {
                case ToolboxTab::TABINDEX_TERRAIN1:     tie.pszText = "Terrain - 1"; break;
                case ToolboxTab::TABINDEX_TERRAIN2:     tie.pszText = "Terrain - 2"; break;
                case ToolboxTab::TABINDEX_SCENERY:      tie.pszText = "Scenery"; break;
                case ToolboxTab::TABINDEX_ELEVATION1:   tie.pszText = "Elevation - 1"; break;
                case ToolboxTab::TABINDEX_ELEVATION2:   tie.pszText = "Elevation - 2"; break;
                case ToolboxTab::TABINDEX_NAVIGABILITY: tie.pszText = "Navigability"; break;
            }

            // Add to the tab control box
            TabCtrl_InsertItem(hTabs, ToolboxTab::TABINDEX_NAVIGABILITY, &tie);
        }

        // Initialize the tab window
        TabCtrl_SetCurSel(hTabs, 0);
        ShowWindow(hTabs, SW_SHOW);
        UpdateWindow(hTabs);
        SetWindowPos(hTabs, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);

        // Add information to the toolbox
        updateCurrentToolboxTab();
    }

    // Initialize values
    {
        // Display these layers by default
        Button_SetCheck(GetDlgItem(myToolbox, IDC_CHECK_TERRAIN1), TRUE);
        Button_SetCheck(GetDlgItem(myToolbox, IDC_CHECK_TERRAIN2), TRUE);
        Button_SetCheck(GetDlgItem(myToolbox, IDC_CHECK_SCENERY), TRUE);

        // Get the toolbox's size
        RECT rc;
        GetClientRect(myToolbox, &rc);
        myToolboxClientSize.cx = rc.right - rc.left;
        myToolboxClientSize.cy = rc.bottom - rc.top;
    }

    // Show the tool window
    UpdateWindow(myToolbox);
    ShowWindow(myToolbox, SW_SHOW);
    SetFocus(myMainWindow);
}





//------------------------------------------------------------------------------------------------
// Name:  clearMapCache
// Desc:  
//------------------------------------------------------------------------------------------------
void AtlasEditDialog::clearMapCache()
{
    // Free all of the allocated cache entries
    for (MapCacheType::iterator mcClear = myGlobalMapCache.begin(); mcClear != myGlobalMapCache.end(); ++mcClear)
        delete mcClear->second;
    myGlobalMapCache.clear();
    myLocalMapCache.clear();
    SetRect(&myCachedViewingRegion, 0, 0, 0, 0);
}


bool TerrainInArray(Terrain::Preview* terrain, TerrainRule::Preview::TerrainArray* terrainArray)
{
    Terrain::Preview** terrainEntries = terrainArray->terrainArray;
    size_t entries = terrainArray->terrainArraySize;

    // TODO: binary search
    for (size_t i = 0; i < entries; ++i)
    {
        if (terrainEntries[i] == terrain) return true;
    }

    // Not found
    return false;
}


//------------------------------------------------------------------------------------------------
// Name:  addEntriesToCache
// Desc:  Adds all of the coordinates in the given list to the cache
//------------------------------------------------------------------------------------------------
void AtlasEditDialog::addEntriesToCache(MapCacheKeySet* coordinates, MapCacheType* localCache, DWORD flags)
{
    // Check input parameters
    if (APP_ERROR(!coordinates || !localCache)("Invalid parameter to addEntriesToCache"))
        return;

    // Go through the requested coordinates and find any locations that are already in the
    // global cache, add them to the local cache, and remove the coordinate from the
    // requested list.  Also, remove the element from the local cache if it's there
    // already so we can re-insert it.
    for (MapCacheKeySet::iterator coordIter = coordinates->begin();
                                  coordIter != coordinates->end();)
    {
        // If this entry exists in the local cache, remove it (don't deallocate, though! only
        // deallocate when removing from the global cache
        {
            MapCacheType::iterator cacheEntryLocal = localCache->find(*coordIter);
            if (cacheEntryLocal != localCache->end())
                localCache->erase(cacheEntryLocal);
        }

        // Find this element in the global cache
        MapCacheType::iterator cacheEntry = myGlobalMapCache.find(*coordIter);

        // If this entry is invalid in the global cache, create it
        if (cacheEntry == myGlobalMapCache.end())
        {
            // Allocate a new map location
            MapLocation* newLocation = new MapLocation;
            if (APP_FATAL(!newLocation)("Out of memory")) { clearMapCache(); return; }
            ZeroMemory(newLocation, sizeof(MapLocation));

            // Insert this entry in the map
            std::pair<MapCacheType::iterator,bool> result =
                myGlobalMapCache.insert(MapCacheType::value_type(*coordIter, newLocation));

            // Check to make sure the insertion succeeded
            if (!result.second)
            {
                // Warn about this
                DEBUG_WARNING("Map location (%i,%i) could not be added to cache", coordIter->first, coordIter->second);

                // Get rid of the memory
                SAFE_DELETE(newLocation);
            }
            else
            {
                // Add this entry to the local cache
                localCache->insert(*result.first);
            }

            // Move to the next set of coordinates
            ++coordIter;
        }
        else
        {
            // If this entry has all of the flags required, we don't have to
            // process it any more--all we have to do is copy it to the local cache
            if ((cacheEntry->second->cachedMembers & flags) == flags)
            {
                // Remove the current coordinates from the set, becase we don't
                // need to do any processing on them.
                MapCacheKeySet::iterator next = coordIter; next++;
                coordinates->erase(coordIter);
                coordIter = next;
            }
            else
                ++coordIter;

            // Add this entry to the local cache
            localCache->insert(*cacheEntry);
        }
    }

    // If there is nothing in the list of coordinates, exit
    if (coordinates->empty()) return;

    // Put all of the coordinate types into the local cache.  Insert them in the global cache if
    // they're not there already
    if ((flags & CACHE_MAPINFO) == CACHE_MAPINFO)
    {
        for (MapCacheKeySet::iterator coordIter = coordinates->begin();
                                      coordIter != coordinates->end(); ++coordIter)
        {
            // Find this element in the local cache (it's guaranteed to be there)
            MapCacheType::iterator cacheEntry = localCache->find(*coordIter);

            // If we didn't find this entry in the global cache, something is very wrong.
            if (APP_ERROR(cacheEntry == localCache->end())
                ("THE LOGIC USED BY THE MAP CACHING ROUTINE IS BROKEN!"))
                continue;

            // The map location for this location
            MapLocation* loc = cacheEntry->second;

            // TODO: x,y variables for coordIter->first, coordIter->second once
            // i check to make sure they're not used anywhere else
            int x = coordIter->first, y = coordIter->second;

            // Pull this location's style data from the map layers, if necessary
            if (((loc->cachedMembers & CACHE_MAPINFO) != CACHE_MAPINFO) &&
                (x >= 0) && (y >= 0))
            {
                // Go through each layer
                size_t layerMemberCount = mapLayerList->getMembers().size();
                for (size_t index = 0; index < layerMemberCount; ++index)
                {
                    // Only check if we're inside of the area of this mask
                    if (mapLayers[index].layerMaskWidth <= x ||
                        mapLayers[index].layerMaskHeight <= y) continue;

                    // The index of the element
                    unsigned int bitIndex = y * mapLayers[index].layerMaskWidth + x;

                    // Obtain the mask to check against the byte
                    BYTE mask = 0x1 << (bitIndex % 8);

                    // If the bit corresponding to this location in the mask is zero, then
                    // we don't need to do anything with the location on this layer
                    if ((mapLayers[index].layerMaskArray[bitIndex / 8] & mask) == 0)
                        continue;

                    // If we can't assign location info, skip this element
                    if (mapLayers[index].locationInfoOptions == 0)
                        continue;

                    // Pick something to put here
                    int option = rand() % mapLayers[index].locationInfoOptions;
                    MapLocationInfo::Preview* optionPreview = &mapLayers[index].locationInfoOptionArray[option];

                    // Overwrite current data, if necessary
                    if (optionPreview->overwriteScenery) loc->scenery = optionPreview->sceneryPreview;
                    if (optionPreview->overwriteTerrain1) { loc->layer[0].mapLayer = &mapLayers[index]; loc->layer[0].mapLayerTerrain = optionPreview->terrainPreview1; }
                    if (optionPreview->overwriteTerrain2) { loc->layer[1].mapLayer = &mapLayers[index]; loc->layer[1].mapLayerTerrain = optionPreview->terrainPreview2; }
                    if (optionPreview->overwriteElevation1) loc->layer[0].elevation = optionPreview->elevation1;
                    if (optionPreview->overwriteElevation2) loc->layer[1].elevation = optionPreview->elevation2;
                    if (optionPreview->overwriteNavigability) loc->navigability = optionPreview->navigability;
                }
            }

            // Get the location information from the detailed buffer
            DetailedLocationPreviewMap::iterator detailedLocation =
                detailedMapLocations.find(WorldLocationType::BasicType(x, y));
            if (detailedMapLocations.end() != detailedLocation)
            {
                // TODO: make a detailed location entry reference so the user can right-click and
                // access this detailed location entry.

                // Get the detailed map information from the entry we found
                MapLocationInfo::Preview* detailedPreview = &detailedLocation->second;

                // Apply the user's edits to this location
                if (detailedPreview->overwriteScenery)      loc->scenery = detailedPreview->sceneryPreview;
                if (detailedPreview->overwriteTerrain1)     loc->layer[0].detailedTerrain = detailedPreview->terrainPreview1;
                if (detailedPreview->overwriteTerrain2)     loc->layer[1].detailedTerrain = detailedPreview->terrainPreview2;
                if (detailedPreview->overwriteElevation1)   loc->layer[0].elevation = detailedPreview->elevation1;
                if (detailedPreview->overwriteElevation2)   loc->layer[1].elevation = detailedPreview->elevation2;
                if (detailedPreview->overwriteNavigability) loc->navigability = detailedPreview->navigability;
            }

            // Make sure the terrain is valid; the defaults are only applied if BOTH layers are invalid!
            {
                if (loc->layer[0].mapLayerTerrain == 0 && loc->layer[0].detailedTerrain == 0 &&
                    loc->layer[1].mapLayerTerrain == 0 && loc->layer[1].detailedTerrain == 0)
                {
                    for (int layerIndex = 0; layerIndex < 2; ++layerIndex)
                    {
                        loc->layer[layerIndex].detailedTerrain = &defaultTerrain[layerIndex];
                        loc->layer[layerIndex].elevation = defaultTerrainElevation[layerIndex];
                        loc->layer[layerIndex].terrainMeshSubset = TMS_FLAT(defaultTerrain[layerIndex].direction);
                    }
                }
            }

            // If this entry is missing the map layer source, we never have
            // to apply rules to it--so go ahead and set that here!
            if (!loc->layer[0].mapLayer) loc->cachedMembers |= CACHE_RULES(0);
            if (!loc->layer[1].mapLayer) loc->cachedMembers |= CACHE_RULES(1);

            // Make sure the navigability is valid
            if (loc->navigability < 0 || loc->navigability >= MAPNAV_COUNT)
                loc->navigability = myAtlas->getDefaultNavigability()->getValue();

            // This cache entry has had its map information written
            loc->cachedMembers |= CACHE_MAPINFO;
        }
    }

    // Apply terrain rules to the layered types
    if ((flags & CACHE_RULES(0)) == CACHE_RULES(0) ||
        (flags & CACHE_RULES(1)) == CACHE_RULES(1))
    {
        // Keys that are missing from the global cache--we have to have the
        // map layer terrain type of every element around the set of coordinates
        // to which we are applying rules
        MapCacheKeySet keysMissingFromGlobalCache;

        // Repeat for each coordinate
        for (MapCacheKeySet::iterator coordIter = coordinates->begin();
                                      coordIter != coordinates->end(); ++coordIter)
        {
            // Find this element in the local cache (it's guaranteed to be there)
            MapCacheType::iterator cacheEntry = localCache->find(*coordIter);

            // Get keys for all of this coordinate's neighbors
            MapCacheType::key_type neighborKey[8] = {
                MapCacheType::key_type(cacheEntry->first.first - 1, cacheEntry->first.second - 1),
                MapCacheType::key_type(cacheEntry->first.first + 0, cacheEntry->first.second - 1),
                MapCacheType::key_type(cacheEntry->first.first + 1, cacheEntry->first.second - 1),
                MapCacheType::key_type(cacheEntry->first.first + 1, cacheEntry->first.second + 0),
                MapCacheType::key_type(cacheEntry->first.first + 1, cacheEntry->first.second + 1),
                MapCacheType::key_type(cacheEntry->first.first + 0, cacheEntry->first.second + 1),
                MapCacheType::key_type(cacheEntry->first.first - 1, cacheEntry->first.second + 1),
                MapCacheType::key_type(cacheEntry->first.first - 1, cacheEntry->first.second + 0)
            };

            // Make sure all of these entries are in the global cache
            for (int n = 0; n < 8; ++n)
            {
                if (myGlobalMapCache.end() == myGlobalMapCache.find(neighborKey[n]))
                    keysMissingFromGlobalCache.insert(neighborKey[n]);
            }
        }

        // Add all of the missing entries to the global cache
        if (!keysMissingFromGlobalCache.empty())
        {
            MapCacheType rulesLocalCache;
            addEntriesToCache(&keysMissingFromGlobalCache, &rulesLocalCache, CACHE_MAPINFO);
        }

        // Get the number of map layers
        size_t mapLayerCount = mapLayerList->getMembers().size();

        // Go through all of the map layers
        for (size_t mapLayerIndex = 0; mapLayerIndex < mapLayerCount; ++mapLayerIndex)
        {
            // Get the current layer
            MapLayer::Preview* mapLayer = &mapLayers[mapLayerIndex];

            // If this layer has no rules associated with it, run through the layer and
            // just set all of the cache rule types so that we can pull this entry from
            // the global map cache explicitly.
            if (mapLayer->terrainRules == 0)
            {
                for (MapCacheKeySet::iterator coordIter = coordinates->begin();
                                              coordIter != coordinates->end(); ++coordIter)
                {
                    // Find this element in the local cache (it's smaller!)
                    MapCacheType::iterator cacheEntry = localCache->find(*coordIter);

                    // Repeat for each terrain layer
                    for (int layerIndex = 0; layerIndex < 2; ++layerIndex)
                    {
                        // Get the current layer
                        MapLocation* mapLocation = cacheEntry->second;
                        MapLocation::TerrainLayer* layer = &mapLocation->layer[layerIndex];

                        // If this entry's source map layer isn't the one we're looking for, skip it.
                        // TODO: can we safely ignore the cache flags here?
                        if (layer->mapLayer != mapLayer)
                            continue;

                        // Copy the map layer terrain type
                        layer->terrain = layer->mapLayerTerrain;

                        // Assign the cache flag
                        mapLocation->cachedMembers |= CACHE_RULES(layerIndex);
                    }
                }

                // Go to the next layer
                continue;
            }

            // Repeat for each rule in the layer
            size_t terrainRuleCount = mapLayer->terrainRules;
            for (size_t ruleIndex = 0; ruleIndex < terrainRuleCount; ++ruleIndex)
            {
                // Get this rule
                TerrainRule::Preview* rule = mapLayer->terrainRuleArray[ruleIndex];

                // If this rule is invalid, don't apply it
                if (!rule) continue;

                // Go through all of the cache entries
                for (MapCacheKeySet::iterator coordIter = coordinates->begin();
                                              coordIter != coordinates->end(); ++coordIter)
                {
                    // Find this element in the local cache (it's smaller!)
                    MapCacheType::iterator cacheEntry = localCache->find(*coordIter);

                    // Repeat for each terrain layer
                    for (int layerIndex = 0; layerIndex < 2; ++layerIndex)
                    {
                        // If the flags don't specify this layer, skip it
                        if ((flags & CACHE_RULES(layerIndex)) != CACHE_RULES(layerIndex))
                            continue;

                        // Get the current layer
                        MapLocation* mapLocation = cacheEntry->second;
                        MapLocation::TerrainLayer* layer = &mapLocation->layer[layerIndex];

                        // If this entry's source map layer isn't in the map layer currently being
                        // examined, or rules have already been applied to it, just move on.
                        if (layer->mapLayer != mapLayer ||
                            (mapLocation->cachedMembers & CACHE_RULES(layerIndex)) == CACHE_RULES(layerIndex))
                            continue;

                        // Get this entry's terrain type
                        Terrain::Preview* layerTerrain = layer->terrain;
                        if (!layerTerrain) layerTerrain = layer->mapLayerTerrain;

                        // If there is no terrain here or the fill array doesn't have anything in it,
                        // we don't need to apply rules to this location
                        if (!layerTerrain || rule->fillTypes.terrainArraySize == 0)
                        {
                            mapLocation->cachedMembers |= CACHE_RULES(layerIndex);
                            continue;
                        }

                        // If the base type doesn't apply, we don't have to worry about anything else for this rule
                        if (!TerrainInArray(layerTerrain, &rule->canvasTypes))
                            continue;

                        // Get keys for all of this location's neighbors
                        MapCacheType::key_type neighborKey[8] = {
                            MapCacheType::key_type(cacheEntry->first.first - 1, cacheEntry->first.second - 1),
                            MapCacheType::key_type(cacheEntry->first.first + 0, cacheEntry->first.second - 1),
                            MapCacheType::key_type(cacheEntry->first.first + 1, cacheEntry->first.second - 1),
                            MapCacheType::key_type(cacheEntry->first.first + 1, cacheEntry->first.second + 0),
                            MapCacheType::key_type(cacheEntry->first.first + 1, cacheEntry->first.second + 1),
                            MapCacheType::key_type(cacheEntry->first.first + 0, cacheEntry->first.second + 1),
                            MapCacheType::key_type(cacheEntry->first.first - 1, cacheEntry->first.second + 1),
                            MapCacheType::key_type(cacheEntry->first.first - 1, cacheEntry->first.second + 0)
                        };


                        // Whether or not this rule should be applied
                        bool applyRule = true;

                        // Check each of the directions to see if this rule applies
                        for (int n = 0; n < 8 && applyRule; ++n)
                        {
                            // If the neighbor doesn't have a rule associated with it, skip testing the terrain
                            // in this direction.
                            if (rule->neighborOptions[n].terrainArraySize == 0)
                                continue;

                            // Find this neighbor in the global cache
                            MapCacheType::iterator neighbor = myGlobalMapCache.find(neighborKey[n]);

                            // If this neighbor doesn't exist, this rule can't be applied
                            if (neighbor == myGlobalMapCache.end())
                            {
                                // If the terrain array had no size, this rule is still valid
                                // Go on to the next rule
                                continue;
                            }

                            // Get the terrain preview from the neighboring element's terrain
                            // type on the same layer as that we're currently checking
                            MapLocation::TerrainLayer* neighborLayer = &neighbor->second->layer[layerIndex];
                            Terrain::Preview* neighborTerrain = neighborLayer->mapLayerTerrain;

                            // Check to make sure the terrain type matches the rule
                            applyRule = TerrainInArray(neighborTerrain, &rule->neighborOptions[n]);
                        }

                        // Apply this rule, if necessary
                        if (applyRule)
                        {
                            // Fill the terrain type.  The value we modulate by here
                            // is checked to make sure it's nonzero earlier in this 'for' loop
                            layer->terrain = rule->fillTypes.terrainArray[rand()%rule->fillTypes.terrainArraySize];

                            // Don't apply any more rules to this location
                            mapLocation->cachedMembers |= CACHE_RULES(layerIndex);
                        }
                        else
                        {
                            // If this is the final rule and we haven't applied anything,
                            // just copy the source terrain type
                            if (ruleIndex + 1 == mapLayer->terrainRules)
                                layer->terrain = layer->mapLayerTerrain;
                        }
                    }
                }
            }
        }
    }

    // Go through every entry in the cache and assign detailed edits, if necessary
    for (MapCacheType::iterator cacheEntry = localCache->begin();
                                cacheEntry != localCache->end(); ++cacheEntry)
    {
        // Repeat for each terrain layer
        for (int layerIndex = 0; layerIndex < 2; ++layerIndex)
        {
            // Get the current layer
            MapLocation::TerrainLayer* layer = &cacheEntry->second->layer[layerIndex];

            // NOTE: assigning detailed edits takes essentially no time
            // when compared to the rest of the caching functions, so
            // there is no cache-key for it.

            // If this layer's detailed terrain element is valid, use it
            // TODO: this doesn't provide for the option of there being assigned a
            // null detailed terrain...
            if (layer->detailedTerrain)
                layer->terrain = layer->detailedTerrain;
        }
    }

    // Go through the whole local cache and compute mesh subsets
    if (((flags & CACHE_LAYERMESHSUBSET(0)) == CACHE_LAYERMESHSUBSET(0)) ||
        ((flags & CACHE_LAYERMESHSUBSET(1)) == CACHE_LAYERMESHSUBSET(1)))
    {
        for (MapCacheKeySet::iterator coordIter = coordinates->begin();
                                      coordIter != coordinates->end(); ++coordIter)
        {
            // Find this element in the local cache (it's smaller!)
            MapCacheType::iterator cacheEntry = localCache->find(*coordIter);

            // The map location, and which members of this location need to be filled out
            MapLocation* loc = cacheEntry->second;
            DWORD cacheEntryFlags = flags;

            // If this entry has already had both layers written, we don't have to do any more
            if ((loc->cachedMembers & CACHE_LAYERMESHSUBSET(0)) == CACHE_LAYERMESHSUBSET(0) &&
                (loc->cachedMembers & CACHE_LAYERMESHSUBSET(1)) == CACHE_LAYERMESHSUBSET(1))
                continue;

            // This requires the neighboring heights in 8 directions to be valid, so find them in
            // the local cache.  We use the global cache here because we want to include border
            // members, which were initialized above and are guaranteed to be valid.
            // This neighbors member is all of the neighbors of this entry, starting at north-west
            // and rotating clockwise.
            MapCacheType::iterator neighbors[8] = {
                myGlobalMapCache.find(MapCacheType::key_type(cacheEntry->first.first - 1, cacheEntry->first.second - 1)),
                myGlobalMapCache.find(MapCacheType::key_type(cacheEntry->first.first + 0, cacheEntry->first.second - 1)),
                myGlobalMapCache.find(MapCacheType::key_type(cacheEntry->first.first + 1, cacheEntry->first.second - 1)),
                myGlobalMapCache.find(MapCacheType::key_type(cacheEntry->first.first + 1, cacheEntry->first.second + 0)),
                myGlobalMapCache.find(MapCacheType::key_type(cacheEntry->first.first + 1, cacheEntry->first.second + 1)),
                myGlobalMapCache.find(MapCacheType::key_type(cacheEntry->first.first + 0, cacheEntry->first.second + 1)),
                myGlobalMapCache.find(MapCacheType::key_type(cacheEntry->first.first - 1, cacheEntry->first.second + 1)),
                myGlobalMapCache.find(MapCacheType::key_type(cacheEntry->first.first - 1, cacheEntry->first.second + 0))
            };

            // Repeat for each layer
            for (int layerIndex = 0; layerIndex < 2; ++layerIndex)
            {
                // If the mesh subset doesn't need to be rebuilt here, just exit
                if (CACHE_LAYERMESHSUBSET(layerIndex) == (loc->cachedMembers & CACHE_LAYERMESHSUBSET(layerIndex)))
                    continue;

                // Make sure that the terrain type exists here; if it doesn't, just skip this entry
                if (!loc->layer[layerIndex].terrain)
                {
                    // This index is now cached correctly
                    loc->cachedMembers |= CACHE_LAYERMESHSUBSET(layerIndex);

                    // Done!
                    continue;
                }

                // This information is used in various places
                unsigned int terrainDirection = loc->layer[layerIndex].terrain->direction;
                float elevation = loc->layer[layerIndex].elevation;
                float height = 0.0f;

                // If this is a wall, this layer is very easy to do
                if (loc->layer[layerIndex].terrain->isWall)
                {
                    // The elevation of the lowest neighbor
                    float lowestNeighbor = elevation;

                    // Find the lowest bordering member
                    for (int i = 0; i < 8; ++i)
                    {
                        // Get the height of this neighbor
                        if (neighbors[i] != myGlobalMapCache.end())
                        {
                            float neighborElevation = neighbors[i]->second->layer[layerIndex].elevation;
                            if (neighborElevation < lowestNeighbor) lowestNeighbor = neighborElevation;
                        }
                    }

                    // This is the wall type (draw sides, infers drawing the top as the primary texture)
                    loc->layer[layerIndex].terrainMeshSubset = TMS_WALL_SIDES;
                    loc->layer[layerIndex].height = elevation - lowestNeighbor;

                    // October 2008:  Don't allow a height of 0, otherwise normals are messed up
                    const float FLT_EPS = 0.000001f;
                    if (loc->layer[layerIndex].height + FLT_EPS >= 0.0f &&
                        loc->layer[layerIndex].height - FLT_EPS <= 0.0f) {
                      loc->layer[layerIndex].height = 1.0f;
                      //loc->layer[layerIndex].elevation -= 1.0f;
                      loc->layer[layerIndex].terrainMeshSubset = TMS_FLAT(terrainDirection); // Don't need sides!
                    }

                    // Don't do any more processing
                    continue;
                }

                // Flags that hold information about the neighbors
                bool high[8], low[8], wall[8], exists[8];

                // Get flags for all of the bordering members
                for (int i = 0; i < 8; ++i)
                {
                    // Whether or not the neighbor is higher than or lower than this location,
                    // and if the neighbor is a wall, or it even exists in this layer.
                    bool neighborIsHigher = false, neighborIsLower = false, neighborExists = false, neighborIsWall = false;

                    // Get the neighboring information
                    MapLocation::TerrainLayer* neighborLayer = &neighbors[i]->second->layer[layerIndex];
                    float neighborElevation = neighborLayer->elevation;

                    // This entry exists if the terrain is valid
                    neighborExists = neighborLayer->terrain != 0;

                    // Check the wall status
                    neighborIsWall = neighborExists && neighborLayer->terrain->isWall;

                    // Only check these if the neighbor exists
                    if (neighborExists)
                    {
                        // Always check the lower elevation
                        neighborIsLower = neighborElevation < elevation - 0.001f;

                        // Only note a higher elevation if the entry isn't a wall
                        if (!neighborIsWall)
                            neighborIsHigher = neighborElevation > elevation + 0.001f; // TODO: name this epsilon
                    }

                    // Assign the flags
                    high[i] = neighborIsHigher;
                    low[i] = neighborIsLower;
                    exists[i] = neighborExists;
                    wall[i] = neighborIsWall;
                }

                // Whether or not this type has been found
                bool foundSubset = false;
                TerrainMeshSubset subset;

                // Check high triplets to create the TMS_HIGHCORNER and TMS_RAISED_LOWCORNER styles
                for (int i = 0; i < 8 && !foundSubset; i += 2)
                {
                    // This pattern matches:
                    //
                    //     oxx       o      o       xxo
                    //      0x      2x      x4      x6
                    //       o     oxx      xxo     o
                    if (!high[i] && high[(i+1)%8] && high[(i+2)%8] && high[(i+3)%8] && !high[(i+4)%8])
                    {
                        // Assign the subset
                        subset = TMS_HIGHCORNER(((i+2)/2)%4, terrainDirection);
                        height = neighbors[(i+3)%8]->second->layer[layerIndex].elevation - elevation;

                        // The subset has been identified
                        foundSubset = true;
                    } else
                    //
                    //     xxx       x      x       xxx
                    //      0x      2x      x4      x6
                    //       x     xxx      xxx     x
                    if (high[i] && high[(i+1)%8] && high[(i+2)%8] && high[(i+3)%8] && high[(i+4)%8])
                    {
                        // Assign the subset
                        subset = TMS_RAISED_LOWCORNER(((i)/2+3)%4, terrainDirection);
                        height = neighbors[(i+3)%8]->second->layer[layerIndex].elevation - elevation;

                        // The subset has been identified
                        foundSubset = true;
                    }
                }

                // Check high triplets to create the TMS_HIGHSIDE styles
                for (int i = 0; i < 8 && !foundSubset; i += 2)
                {
                    // This pattern matches either:
                    //
                    //      xxx     x       x
                    //       0     2x   4   x6
                    //              x  xxx  x
                    // or
                    //
                    //      ?x?     ?       ?
                    //       0     2x   4   x6
                    //              ?  ?x?  ?
                    //
                    if ((high[i] || !exists[i]) && high[(i+1)%8] && (high[(i+2)%8] || !exists[(i+2)%8]))
                    {
                        // Assign the subset
                        subset = TMS_HIGHSIDE(i/2, terrainDirection);
                        height = neighbors[(i+1)%8]->second->layer[layerIndex].elevation - elevation;

                        // The subset has been identified
                        foundSubset = true;
                    }
                }

                // Check low triplets to create the TMS_LOWSIDE style.  This style is applied
                // here to correct areas that only have a width of 1 unit.
                for (int i = 1; i < 8 && !foundSubset; i += 2)
                {
                    // This pattern matches:
                        //
                        //      xx      xx      xx      xxx
                        //      1x      3x      x5      x7x
                        //      xx      xx      xx
                    if (low[i] && low[(i+1)%8] && low[(i+2)%8] && low[(i+3)%8] && low[(i+4)%8])
                    {
                        // Assign the subset
                        subset = TMS_LOWSIDE(((i+1)/2)%4, terrainDirection);
                        height = elevation - neighbors[(i+2)%8]->second->layer[layerIndex].elevation;

                        // The subset has been identified
                        foundSubset = true;
                    }
                }

                // Check low triplets to create the TMS_LOWCORNER style.
                // We only care about the non-cardinal-directions here,
                // so only check the odd neighbors.
                for (int i = 1; i < 8 && !foundSubset; i += 2)
                {
                    // This pattern matches:
                    //
                    //      xx                      xx
                    //      1x      3x      x5      x7
                    //              xx      xx
                    if (low[i] && low[(i+1)%8] && low[(i+2)%8])
                    {
                        // Assign the subset
                        subset = TMS_LOWCORNER(((i+1)/2)%4, terrainDirection);
                        height = elevation - neighbors[(i+1)%8]->second->layer[layerIndex].elevation;

                        // The subset has been identified
                        foundSubset = true;
                    }
                }

                // Check high doublets to catch the rest of the TMS_HIGHCORNER types.  In this
                // case, we only want the cardinal directions (odd neighbors)
                for (int i = 0; i < 8 && !foundSubset; i += 2)
                {
                    // This pattern matches:
                    //
                    //      xxo      x              o
                    //       0      2x      4       x6
                    //               o     oxx      x
                    if (high[i] && high[(i+1)%8] && !high[(i+2)%8])
                    {
                        // Assign the subset
                        subset = TMS_HIGHCORNER(i/2, terrainDirection);
                        height = neighbors[i]->second->layer[layerIndex].elevation - elevation;

                        // The subset has been identified
                        foundSubset = true;
                    }
                    else

                    // This pattern matches:
                    //
                    //      oxx      o              x
                    //       0      2x      4       x6
                    //               x     xxo      o 
                    if (!high[i] && high[(i+1)%8] && high[(i+2)%8])
                    {
                        // Assign the subset
                        subset = TMS_HIGHCORNER(((i+2)/2)%4, terrainDirection);
                        height = neighbors[(i+2)%8]->second->layer[layerIndex].elevation - elevation;

                        // The subset has been identified
                        foundSubset = true;
                    }
                }

                // Save the subset
                if (!foundSubset) {
                    loc->layer[layerIndex].terrainMeshSubset = TMS_FLAT(terrainDirection);
                    loc->layer[layerIndex].height = 1.0f; // This needs a height
                } else {
                    loc->layer[layerIndex].terrainMeshSubset = subset;
                    loc->layer[layerIndex].height = height;
                }

                // This entry has had its terrain mesh subset written
                loc->cachedMembers |= CACHE_LAYERMESHSUBSET(layerIndex);
            }
        }
    }



}



//------------------------------------------------------------------------------------------------
// Name:  execute
// Desc:  
//------------------------------------------------------------------------------------------------
void AtlasEditDialog::execute()
{
    // TODO: set up a progress bar dialog here

    // Load the resources to run the dialog
    if (APP_ERROR(!loadResources())("Couldn't load the data required to run the Atlas editing utility"))
        return;

    // At this point, all of the data we need to generate in order to display the map to the user has been loaded, so
    // we can go about initializing Direct3D on a dialog window.
    if (APP_ERROR(!createD3DDialog(GetModuleHandle(NULL), globalEditor()->getTopmostWindowHandle(), 0, "Preview/Edit World Atlas", 800, 600, globalEditor()->getD3DDevice()))
        ("Couldn't create a Direct3D dialog to display the world Atlas"))
    {
        // ExitD3
        unloadResources();
        return;
    }

    // Initialize the render state
    myD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    myD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255,128,128));
    myD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    myD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    myD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    // Set up alpha testing
    myD3DDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x08);
    myD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    myD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    myD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Create the toolbox window
    createToolbox();

    // Keep track of the camera's position
    D3DXVECTOR2 nextLocation = myCurrentLocation;


    // Timer to check for rendering
    //dcxWin32Clock clock;
    //dcxEventTimer timer(&clock, 1.0 / 30);    // Render at 30 FPS
    //timer.addEvent(0,  0);
    //timer.start();
    double lastFrameTime = dcxWin32Clock::getAccurateSystemTime();
    double nextTimeToRenderFrame = 0.0;

    // Run the dialog
    while(messagePump())
    {
        double currentTime = dcxWin32Clock::getAccurateSystemTime();
        double elapsedTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        myShowNavigability = Button_GetCheck(GetDlgItem(myToolbox, IDC_CHECK_NAVIGABILITY));
        myShowScenery = Button_GetCheck(GetDlgItem(myToolbox, IDC_CHECK_SCENERY));
        myShowTerrain1 = Button_GetCheck(GetDlgItem(myToolbox, IDC_CHECK_TERRAIN1));
        myShowTerrain2 = Button_GetCheck(GetDlgItem(myToolbox, IDC_CHECK_TERRAIN2));
        myShowWireframe = Button_GetCheck(GetDlgItem(myToolbox, IDC_CHECK_WIREFRAME));

        // Update mouse and keyboard
        myMouse.query();
        myKeyboard.query(NULL);

        if (myKeyboard.keyPressed(KEY_R))
        {
            dcx::dcxWin32SetWindowClientSize(myToolbox, 200, 500);
        }
        if (myKeyboard.keyPressed(KEY_T))
        {
            dcx::dcxWin32SetWindowClientSize(myToolbox, 200, 0);
        }
        if (myKeyboard.keyPressed(KEY_V))
        {
            static bool topdown = true;
            topdown = !topdown;
            myCamera.enableTopdownView(topdown);
        }
        if (myKeyboard.keyDown(KEY_UP))
        {
            myZoom -= elapsedTime * 5.0f;
            if (myZoom < 1.0f) myZoom = 1.0f;
        }
        if (myKeyboard.keyDown(KEY_DOWN))
            myZoom += elapsedTime * 5.0f;
        if (myKeyboard.keyPressed(KEY_RIGHT))
            myViewGranularity++;
        if (myKeyboard.keyPressed(KEY_LEFT) && myViewGranularity > 1)
            myViewGranularity--;

        POINTS pushedPt;
        if (myMouse.getRealClick(dcxWin32Mouse::BUTTON_RIGHT, &pushedPt, NULL))
        {
            // Project
            float wx, wz;
            myCamera.getWorldLocation(pushedPt.x, pushedPt.y, &wx, &wz);
            nextLocation.x = wx;
            nextLocation.y = wz;
        }

        // Move to the next location
        {
            double lerpAmt = 4.0 * elapsedTime;
            if (lerpAmt < 0.0) lerpAmt = 0.0;
            D3DXVec2Lerp(&myCurrentLocation, &myCurrentLocation, &nextLocation, lerpAmt < 1.0f ? lerpAmt : 1.0f);
            myCamera.setPosition(myCurrentLocation.x, myCurrentLocation.y);
        }

        // Mouse pick
        {
            POINTS ptMouse;
            myMouse.getCoordinates(&ptMouse, NULL);
            float x, z;
            myCamera.getWorldLocation(ptMouse.x, ptMouse.y, &x, &z);
            myCursorLocation.x = (long)floor(x + 0.5f);
            myCursorLocation.y = (long)floor(z + 0.5f);
        }

        // Handle mouse input
        POINTS down, up;
        if (myMouse.getRealClick(dcxWin32Mouse::BUTTON_LEFT, &down, &up))
        {
            POINT ptdown, ptup;
            // Pick these locations
            float x, z;
            myCamera.getWorldLocation(down.x, down.y, &x, &z);
            ptdown.x = (long)floor(x + 0.5f);
            ptdown.y = (long)floor(z + 0.5f);
            myCamera.getWorldLocation(up.x, up.y, &x, &z);
            ptup.x = (long)floor(x + 0.5f);
            ptup.y = (long)floor(z + 0.5f);

#define swap(a, b) { int t = a; a = b; b = t; }

            // Reorder the coordinates
            if (ptup.x < ptdown.x) swap(ptup.x, ptdown.x);
            if (ptup.y < ptdown.y) swap(ptup.y, ptdown.y);

            // Whether or not to update the toolbox with the current selection
            bool updateToolbox;

            // Todo: make a constant that lets you specify myKeyboard.keyDown(KEY_CONTROL) to unify this statement
            if (!GetAsyncKeyState(VK_CONTROL))
            {
                mySelection.clear();

                // The user cleared the selection.  If they selected a single
                // location, get this location's description
                updateToolbox = (ptdown.x == ptup.x && ptdown.y == ptup.y);
            }

            // Set the new bounce height and the time
            mySelectionBounceHeight = 4.0f;
            mySelectTime = GetTickCount();

            // Update the toolbox
            {
                // The selection region
                RECT sel = { ptdown.x, ptdown.y, ptup.x+1, ptup.y+1 };

                // All of the entries in the set
                MapCacheKeySet mcks;

                // Go through each location in this selection and remove the entries from the global cache
                for (int x = sel.left; x < sel.right; ++x)
                    for (int y = sel.top; y < sel.bottom; ++y)
                    {
                        // The key for this location
                        MapCacheType::key_type mapCacheKey(x, y);

                        // Refresh this entry in the cache (so that random layers will
                        // hopefully change, and remind the user that they are random)
                        removeCacheEntry(x, y);
                        mcks.insert(mapCacheKey);
                        /*MapCacheType::iterator i = myGlobalMapCache.find(mapCacheKey);
                        if (i != myGlobalMapCache.end())
                        {
                            // Reset the cache entries for this element
                            i->second->cachedMembers = 0;

                            // Add the element to re-formulate
                            mcks.insert(mapCacheKey);
                        }*/
                    }

                // Re-add these entries to the local cache
                addEntriesToCache(&mcks, &myLocalMapCache, CACHE_ALL);

                // TODO: update the toolbox with the current selection here
            }

            // Add this entry to the selection
            mySelection.push_back(Selection::value_type(ptdown, ptup));
        }

        // Render if necessary
	    if (nextTimeToRenderFrame < currentTime)
        {
            nextTimeToRenderFrame = currentTime + 1.0 / 30.0;

            // Draw the scene
            renderScene();
        }
    }

    // Erase everything we created for the dialog
    unloadResources();
    releaseObjects();
}




//------------------------------------------------------------------------------------------------
// Name:  compile
// Desc:  Compiles the map into the context.  This is not a very efficient algorithm.
//------------------------------------------------------------------------------------------------
bool AtlasEditDialog::compile(AtlasCompilationContext* context)
{
    DEBUG_INFO("Loading resources for Atlas...(this could take a while)");

    // Load the resources to run the dialog
    if (APP_ERROR(!loadResources())("Couldn't load the data required to run the Atlas editing utility"))
        return false;

    // Set the default location type
    {
        size_t textures[2];
        float elevations[2];
        for (size_t i = 0; i < 2; ++i)
        {
            Texture::Preview* texture = defaultTerrain[i].topTexturePreview;
            Texture* textureResource = texture ? texture->texture : 0;
            dc::dcTable<Texture>::Element* textureElement = textureResource ? (dcTable<Texture>::Element*)textureResource->getOwner() : 0;
            textures[i] = textureElement ? textureElement->getIndex() : 0xFFFFFFFF;
            elevations[i] = defaultTerrainElevation[i];
        }
        context->setDefaultLocation(textures[0], textures[1], elevations[0], elevations[1], myAtlas->getDefaultNavigability()->getValue());
    }

    // Set the spawn information
    context->setSpawnInfo(myAtlas->getSpawnLocationValue().first,
                          myAtlas->getSpawnLocationValue().second,
                          myAtlas->getSpawnRadiusValue(),
                          myAtlas->getEvilSpawnLocationValue().first,
                          myAtlas->getEvilSpawnLocationValue().second);

    // Set sound information
    context->setBackgroundSoundLoopIndex(myAtlas->getBackgroundSoundLoopIndex());

    // Find the limits of this map
    int minX = 0, minY = 0, maxX = 0, maxY = 0;
    for (size_t i = 0; i < this->mapLayerList->size(); ++i)
    {
        int width = mapLayers[i].layerMaskWidth;
        int height = mapLayers[i].layerMaskHeight;

        // Make sure the width and height fit
        if (maxX < width) maxX = width;
        if (maxY < height) maxY = height;
    }

    // Run through all of the detailed edits
    dc::dcGenericResource::Array::const_iterator i, end;
    end = myAtlas->getDetailedLocations()->getMembers().end();
    for (i = myAtlas->getDetailedLocations()->getMembers().begin(); i != end; ++i)
    {
        // Get the element at this location
        dc::dcMap<WorldLocationType,MapLocationInfo>::Element* element =
            (dcMap<WorldLocationType,MapLocationInfo>::Element*)(*i);

        // Get the location
        int x = element->getKey()->getX(),
            y = element->getKey()->getY();

        // Compare the location with the current boundaries
        if (x < minX) minX = x;
        else if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        else if (y > maxY) maxY = y;
    }

    // Increment the maxima because they are exclusive
    maxX++;
    maxY++;

    // Inflate the rectangle by 2 locations so that we can be absolutely sure that the default
    // location won't produce any artifacts
    DEBUG_WARNING("Map size inflation disabled:  default location may produce artifacts");
    /*minX -= 2;
    minY -= 2;
    maxX += 2;
    maxY += 2;*/

    // Make sure the context is large enough to support these locations
    if (APP_ERROR(!context->reserve(minX, minY, maxX, maxY))
        ("Couldn't reserve rect [%i,%i] - [%i,%i] for atlas compilation", minX, minY, maxX, maxY))
    {
        // Get rid of data in the context
        context->destroy();

        // Fail
        return false;
    }

    // Cache groups inside of the range
#define CACHE_GRANULARITY   128
    for (int x = minX; x < maxX; x += CACHE_GRANULARITY)
        for (int y = minY; y < maxY; y += CACHE_GRANULARITY)
        {
            // Clear the cache
            clearMapCache();

            // Cached region--expand by 2 locations so that the inner rectangle is valid
            RECT cachedRect;
            cachedRect.left = x - 2;
            cachedRect.right = x + 2 + CACHE_GRANULARITY;
            cachedRect.top = y - 2;
            cachedRect.bottom = y + 2 + CACHE_GRANULARITY;

            // Make sure we don't go past the right boundary
            if (cachedRect.right > maxX + 2) cachedRect.right = maxX + 2;
            if (cachedRect.bottom > maxY + 2) cachedRect.bottom = maxY + 2;

            // Build the set of keys to add to the map
            MapCacheKeySet mcks;
            for (int keyX = cachedRect.left; keyX < cachedRect.right; ++keyX)
                for (int keyY = cachedRect.top; keyY < cachedRect.bottom; ++keyY)
                {
                    MapCacheType::key_type mapCacheKey(keyX, keyY);
                    mcks.insert(mapCacheKey);
                }

            // Add all of these entries to the cache
            addEntriesToCache(&mcks, &myLocalMapCache, CACHE_ALL);

            // Now that all the entries have been inserted, shrink the cached rect to make sure
            // we only iterate over the valid entries (the border of 2 around the outside ensures
            // the center rectangle will be filled with the correct geometry
            cachedRect.left += 2;
            cachedRect.right -= 2;
            cachedRect.top += 2;
            cachedRect.bottom -= 2;
            for (MapCacheType::iterator i = myLocalMapCache.begin(); i != myLocalMapCache.end();)
            {
                POINT location = { i->first.first, i->first.second };
                if (!PtInRect(&cachedRect, location))
                {
                    // Erase this element because it's not in the inside rectangle
                    MapCacheType::iterator next = i; ++next;
                    myLocalMapCache.erase(i);
                    i = next;
                }
                else
                     ++i;
            }

            // Add all of the descriptions we just cached to the map
            context->importDescriptions(&myLocalMapCache);
        }

    // Erase everything we created for the dialog
    unloadResources();
    releaseObjects();

    // Success
    return true;
}



int QSortCompareMapCacheEntries_TerrainLayer1(const VOID* p1, const VOID* p2)
{
    const Terrain::Preview* terrainLeft = (*(AtlasEditDialog::MapCacheType::value_type**)p1)->second->layer[0].terrain;
    const Terrain::Preview* terrainRight = (*(AtlasEditDialog::MapCacheType::value_type**)p2)->second->layer[0].terrain;
    if (!terrainLeft && !terrainRight) return false;
    if ( terrainLeft && !terrainRight)
        return false;
    if (!terrainLeft &&  terrainRight)
        return true;
    if (!terrainLeft->topTexturePreview && !terrainRight->topTexturePreview) return false;
    if ( terrainLeft->topTexturePreview && !terrainRight->topTexturePreview)
        return false;
    if (!terrainLeft->topTexturePreview && terrainRight->topTexturePreview)
        return false;
    return terrainLeft->topTexturePreview->imagePreview < terrainRight->topTexturePreview->imagePreview;
}

int QSortCompareMapCacheEntries_TerrainLayer2(const VOID* p1, const VOID* p2)
{
    const Terrain::Preview* terrainLeft = (*(AtlasEditDialog::MapCacheType::value_type**)p1)->second->layer[1].terrain;
    const Terrain::Preview* terrainRight = (*(AtlasEditDialog::MapCacheType::value_type**)p2)->second->layer[1].terrain;
    if (!terrainLeft && !terrainRight) return false;
    if ( terrainLeft && !terrainRight)
        return false;
    if (!terrainLeft &&  terrainRight)
        return true;
    if (!terrainLeft->topTexturePreview && !terrainRight->topTexturePreview) return false;
    if ( terrainLeft->topTexturePreview && !terrainRight->topTexturePreview)
        return false;
    if (!terrainLeft->topTexturePreview && terrainRight->topTexturePreview)
        return false;
    return terrainLeft->topTexturePreview->imagePreview < terrainRight->topTexturePreview->imagePreview;
}


//------------------------------------------------------------------------------------------------
// Name:  renderScene
// Desc:  Draws the 3D map
//------------------------------------------------------------------------------------------------
void AtlasEditDialog::renderScene()
{
    //DEBUG_INFO("Loc:  %f, %f", myCurrentLocation.x, myCurrentLocation.y);

//    // Set up a material. The material here just has the diffuse and ambient
//    // colors set to yellow. Note that only one material can be used at a time.
//    D3DMATERIAL9 mtrl;
//    ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
//    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
//    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
//    mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
//    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
//    myD3DDevice->SetMaterial(&mtrl);
//
//    // Set up a white, directional light, with an oscillating direction.
//    // Note that many lights may be active at a time (but each one slows down
//    // the rendering of our scene). However, here we are just using one. Also,
//    // we need to set the D3DRS_LIGHTING renderstate to enable lighting
//    D3DXVECTOR3 vecDir;
//    D3DLIGHT9 light;
//    ZeroMemory(&light, sizeof(D3DLIGHT9));
//    light.Type = D3DLIGHT_POINT;
//    light.Diffuse.r = 1.0f;
//    light.Diffuse.g = 1.0f;
//    light.Diffuse.b = 1.0f;
//
//  POINTS coord;
//  myMouse.getCoordinates(&coord,0);
//  myCamera.getWorldLocation(coord.x, coord.y, &light.Position.x, &light.Position.z);
//  light.Position.x -= myCurrentLocation.x;
//  light.Position.z -= myCurrentLocation.y;
//  light.Position.y = 1.0f;
//  light.Attenuation0 = 0.0f;
//  light.Attenuation1 = 1.0f;
//  light.Attenuation2 = 0.0f;
///*
//    vecDir = D3DXVECTOR3(0.0f,
//                          cosf(timeGetTime() / 1250.0f),
//                          sinf(timeGetTime() / 1250.0f));
//    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);*/
//    light.Range = 1000.0f;
//    myD3DDevice->SetLight(0, &light);
//    myD3DDevice->LightEnable(0, TRUE);
//    myD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
//
//    // Finally, turn on some ambient light.
//    myD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00606060);
//    BYTE b = (BYTE)(255.0f*cosf(dcxWin32Clock::getAccurateSystemTime()/2.0f));

    myD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    BYTE b = 0;


    // Clear the backbuffer and the zbuffer
    myD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
					        D3DCOLOR_XRGB(b, b, b), 1.0f, 0);


    // Start rendering
    if (FAILED(myD3DDevice->BeginScene())) return;

    // Reset the rendering texture
    setRenderingTexture(NULL);

    D3DXMATRIXA16 matIdentity, matView, matProj;

    // Build the camera
    myCamera.initialize3DRendering(myD3DDevice);

    // Draw the cursor and selection
    {
        // Always draw the cursors as solids
        myD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

        // Calculate the selection bounce height
        float selectionHeight = cosf((GetTickCount()-mySelectTime)/75.0f);
        if (selectionHeight * mySelectionBounceHeight < 0.0f) // Easy check to see if they have the different signs
        {
            // Flip the sign, reduce the height
            mySelectionBounceHeight *= -0.10f;
            if (fabs(mySelectionBounceHeight) < 0.025f) mySelectionBounceHeight = 0.0f;
        }
        selectionHeight = -0.2f + sqrtf(mySelectionBounceHeight * selectionHeight);

        // Draw the edit box cursor
        D3DXMATRIXA16 editScale, editTranslate, edit;
        D3DXMatrixScaling(&editScale, 0.5f, 4.0f, 0.5f);
        myD3DDevice->SetTexture(0, myCursorTexture);
        for (Selection::iterator i = mySelection.begin(); i != mySelection.end(); ++i)
        {
            float width = i->second.x - i->first.x + 1.0f,
                 height = i->second.y - i->first.y + 1.0f;
            D3DXMatrixScaling(&editScale, width - 0.5f, 4.0f, height - 0.5f);
            D3DXMatrixTranslation(&editTranslate, i->first.x * 1.0f + width / 2 - 0.50f,
                                    selectionHeight + 4.0f,
                                    i->first.y * 1.0f + height / 2);
            D3DXMatrixMultiply(&edit, &editScale, &editTranslate);
            myCamera.setWorldMatrix(myD3DDevice, &edit);
            myTerrainMesh->DrawSubset(TMS_WALL_SIDES);
        }

        // Draw the selection cursor
        D3DXMatrixScaling(&editScale, 1.0f, 5.0f, 1.0f);
        D3DXMatrixTranslation(&editTranslate, myCursorLocation.x * 1.0f,
                                5.0f,
                                myCursorLocation.y * 1.0f);
        D3DXMatrixMultiply(&edit, &editScale, &editTranslate);
        myCamera.setWorldMatrix(myD3DDevice, &edit);
        myD3DDevice->SetTexture(0, myCursorTexture);
        myTerrainMesh->DrawSubset(TMS_WALL_SIDES);
    }

    // Get the minimum and maximum view area
    RECT viewArea = { myCurrentLocation.x - 7 * myZoom,     myCurrentLocation.y - 7 * myZoom,
                      myCurrentLocation.x + 7 * myZoom + 3, myCurrentLocation.y + 7 * myZoom + 3 };

    struct Profile
    {
        double addNewEntries,
               removeOldEntries,
               addEntriesToCache,
               buildVisibleLocations,
               sortTerrain1,
               iterateTerrain1, 
               renderTerrain1;
    };


    // Add new entries
    MapCacheKeySet mcks;
    for (int x = viewArea.left; x < viewArea.right; x++)
        for (int y = viewArea.top; y < viewArea.bottom; y++)
        {
            // Get this point
            POINT pt = { x, y };

            bool insideCurrent = PtInRect(&viewArea, pt) != FALSE,
                 insideOld = PtInRect(&myCachedViewingRegion, pt) != FALSE;

            // If this point is in the new area, but not the last one, add it to the list
            if (insideCurrent && !insideOld)
                mcks.insert(MapCacheType::key_type(x, y));
        }

    // Remove old entries
    for (MapCacheType::iterator i = myLocalMapCache.begin(); i != myLocalMapCache.end();)
    {
        // Get this point
        POINT pt = { i->first.first, i->first.second };

        bool insideCurrent = PtInRect(&viewArea, pt) != FALSE,
             insideOld = PtInRect(&myCachedViewingRegion, pt) != FALSE;

        if (insideOld && !insideCurrent)
        {
            MapCacheType::iterator next = i; next++;
            myLocalMapCache.erase(i);
            i = next;
        }
        else
            ++i;
    }

    // Add all of these entries to the local cache
    addEntriesToCache(&mcks, &myLocalMapCache, CACHE_ALL);

    // The region is updated
    CopyRect(&myCachedViewingRegion, &viewArea);

    // Build a list of the locations in the cache so that we can sort them
    std::vector<MapCacheType::value_type*> visibleLocations;
    for (MapCacheType::iterator i = myLocalMapCache.begin(); i != myLocalMapCache.end(); ++i)
    {
        if (i->first.first % myViewGranularity == 0 &&
            i->first.second % myViewGranularity == 0)
        {
            visibleLocations.push_back(&(*i));
        }
    }

    // Render the navigability first, since if it's enabled, this will occlude other elements
    if (myShowNavigability != FALSE)
    {
        // Force the navigability to render on top of everything else
        D3DVIEWPORT9 oldViewport, forceFront;
        myD3DDevice->GetViewport(&oldViewport);
        memcpy(&forceFront, &oldViewport, sizeof(D3DVIEWPORT9));
        forceFront.MaxZ = forceFront.MinZ = 0.0f;

        // Draw to the front
        myD3DDevice->SetViewport(&forceFront);

        // Render all of the navigability
        for (std::vector<MapCacheType::value_type*>::iterator i = visibleLocations.begin(); i != visibleLocations.end(); ++i)
        {
            // Get this render location
            MapLocation* location = (*i)->second;

            // Obtain the navigability
            MapNavigability nav = location->navigability;

            // Render the navigability layer
            if (nav >= 0 && nav < MAPNAV_COUNT)
            {
                // Set the scenery transform
                D3DXMATRIXA16 matSpot, mat;
                D3DXMatrixTranslation(&matSpot, (*i)->first.first * 1.0f, 0.0f, (*i)->first.second * 1.0f);
                D3DXMatrixScaling(&mat, 1.0f * myViewGranularity - 0.5f, 1.0f, 1.0f * myViewGranularity - 0.5f);
                D3DXMatrixMultiply(&mat, &mat, &matSpot);
                myCamera.setWorldMatrix(myD3DDevice, &mat);
                myD3DDevice->SetTexture(0, myNavigabilityTextures[nav]);
                myTerrainMesh->DrawSubset(TMS_FLAT_TEXN);
            }
        }

        // Restore the old viewport
        myD3DDevice->SetViewport(&oldViewport);
    }

    // Update the global render state based on internal flags
    myD3DDevice->SetRenderState(D3DRS_FILLMODE, myShowWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

    // Draw the terrain types first, since anything that's alphablended in the scenery should be blended with this
    if (myShowTerrain1 != FALSE)
    {
        // Sort the locations by texture1
        qsort(&visibleLocations[0], visibleLocations.size(), sizeof(void*), &QSortCompareMapCacheEntries_TerrainLayer2);

        for (std::vector<MapCacheType::value_type*>::iterator i = visibleLocations.begin(); i != visibleLocations.end(); ++i)
        {
            // Get the terrain layer preview elements
            MapLocation* location = (*i)->second;
            Terrain::Preview* terrain1 = location->layer[0].terrain;
            if (!terrain1 || !terrain1->terrain) continue;

            // Get the x and y coordinates
            float x = (*i)->first.first * 1.0f,
                  y = (*i)->first.second * 1.0f;

            // Get the height of this layer
            float elevation = location->layer[0].elevation;
            float height = location->layer[0].height;

            // Build the ground-type scaling matrix
            D3DXMATRIX mat;
            D3DXMatrixIdentity(&mat);
            mat._11 = mat._33 = 1.0f * myViewGranularity;
            mat._22 = height;
            mat._41 = x;
            mat._42 = elevation;
            mat._43 = y;

            // Render the terrain element
            renderTerrainLayer(&mat, &location->layer[0]);
        }
    }

    // Draw the scenery last, since it's likely to be on top of the terrain.
    // It's drawn before the second layer of terrain, though, just incase some scenery
    // pokes down into water or something like that.
    if (myShowScenery != FALSE)
    {
        for (std::vector<MapCacheType::value_type*>::iterator i = visibleLocations.begin(); i != visibleLocations.end(); ++i)
        {
            // Get the scenery preview element
            MapLocation* location = (*i)->second;
            Scenery::Preview* scenery = location->scenery;
            if (!scenery || !scenery->meshPreview || !scenery->skinPreview) continue;

            // Get the x and y coordinates
            float x = (*i)->first.first * 1.0f,
                  y = (*i)->first.second * 1.0f;

            // Get the height of this layer
            float elevation = location->layer[0].elevation;

            // Build the ground-type scaling matrix
            D3DXMATRIX mat;
            D3DXMatrixIdentity(&mat);
            mat._41 = x;
            mat._42 = elevation;
            mat._43 = y;

            // Render scenery
            renderSceneryPreview(&mat, scenery);
        }
    }

    if (myShowTerrain2 != FALSE)
    {
        // Sort elements by texture 2
        qsort(&visibleLocations[0], visibleLocations.size(), sizeof(void*), &QSortCompareMapCacheEntries_TerrainLayer2);

        for (std::vector<MapCacheType::value_type*>::iterator i = visibleLocations.begin(); i != visibleLocations.end(); ++i)
        {
            // Get the terrain layer preview elements
            MapLocation* location = (*i)->second;
            Terrain::Preview* terrain2 = location->layer[1].terrain;
            if (!terrain2) continue;

            // Get the x and y coordinates
            float x = (*i)->first.first * 1.0f,
                  y = (*i)->first.second * 1.0f;

            // Get the height of this layer
            float elevation = location->layer[1].elevation;
            float height = location->layer[1].height;

            // Build the ground-type scaling matrix
            D3DXMATRIX mat, matSpot;
            D3DXMatrixScaling(&mat, 1.0f * myViewGranularity, height, 1.0f * myViewGranularity);
            D3DXMatrixTranslation(&matSpot, x, elevation, y);
            D3DXMatrixMultiply(&mat, &mat, &matSpot);

            // Render the terrain element
            renderTerrainLayer(&mat, &location->layer[1]);
        }
    }


    // End rendering
    myD3DDevice->EndScene();

    // Display to the screen
    present();
}



//------------------------------------------------------------------------------------------------
// Name:  renderTerrainLayer
// Desc:  Draws the terrain element indicated using the provided locating matrix
//------------------------------------------------------------------------------------------------
void AtlasEditDialog::renderTerrainLayer(const D3DXMATRIX* locationMatrix, MapLocation::TerrainLayer* layer)
{
    if (APP_ERROR(!locationMatrix || !layer)("Invalid parameter to renderTerrainLayer"))
        return;

    // The terrain from this layer
    Terrain::Preview* terrain = layer->terrain;

    // If the terrain element doesn't exist, don't render anything
    if (!terrain) return;

    // Set the ground transformation matrix
    myCamera.setWorldMatrix(myD3DDevice, locationMatrix);

    if (terrain->isWall)
    {
        // Since this is a wall, it MUST have a side texture.  If one isn't specified, just
        // grab it from the top texture.
        Texture::Preview* sideTexturePreview =
            terrain->sideTexturePreview != 0 ? terrain->sideTexturePreview : terrain->topTexturePreview;

        // Draw the sides
        if (terrain && sideTexturePreview)
        {
            // Set the texture
            setRenderingTexture(terrain->sideTexturePreview);

            // Draw this element
            myTerrainMesh->DrawSubset(TMS_WALL_SIDES);
        }

        // Draw the top of this element
        if (terrain && terrain->topTexturePreview )
        {
            // Set the texture
            setRenderingTexture(terrain->topTexturePreview);

            // Draw this element
            myTerrainMesh->DrawSubset(TMS_FLAT(terrain->direction) );
        }
    }
    else
    {
        // Draw the top of this element
        if (terrain && terrain->topTexturePreview )
        {
            // Set this image
            setRenderingTexture(terrain->topTexturePreview);

            // Draw this element
            myTerrainMesh->DrawSubset(layer->terrainMeshSubset);
        }
    }
}


//------------------------------------------------------------------------------------------------
// Name:  renderSceneryPreview
// Desc:  Draws the scenery element indicated using the provided locating matrix
//------------------------------------------------------------------------------------------------
void AtlasEditDialog::renderSceneryPreview(const D3DXMATRIX* locationMatrix,Scenery::Preview* preview)
{
    // Check the parameters
    if (APP_ERROR(!locationMatrix || !preview)("Invalid parameter to renderSceneryPreview"))
        return;

    // Set the scenery transform
    D3DXMATRIXA16 mat;
    D3DXMatrixMultiply(&mat, &preview->transform, locationMatrix);
    myCamera.setWorldMatrix(myD3DDevice, &mat);

    // Make sure that the mesh preview is valid
    if (!preview->meshPreview)
        return;

    // The mesh to render
    LPD3DXMESH d3dxMesh = preview->meshPreview->d3dxMesh;

    // Make sure the mesh is valid
    if (!d3dxMesh) return;

    // Draw the mesh
    for (Skin::Preview::BasicMapType::iterator i = preview->skinPreview->map.begin();
         i != preview->skinPreview->map.end(); ++i)
    {
        if (!i->second) continue;
        if (!i->second->texture) continue;

        Image::Preview* imagePreview = i->second->imagePreview;
        Texture::Preview* texturePreview = i->second;

        // Set the texture for this subset
        if (i->second->imagePreview)
        {
            myD3DDevice->SetTexture(0, i->second->imagePreview->d3dTexture);
            i->second->texture->configureTextureStageState(myD3DDevice, 0);
        }
        else
        {
            myD3DDevice->SetTexture(0, NULL);
        }

        // Draw the mesh
        d3dxMesh->DrawSubset(i->first);
    }
}