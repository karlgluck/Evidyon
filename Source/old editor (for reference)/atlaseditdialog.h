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
#ifndef __ATLASEDITDIALOG_H__
#define __ATLASEDITDIALOG_H__
#pragma once

class ProjectVolucris;
class AtlasCompilationContext;

struct ToolboxTab
{
    ToolboxTab()
    {
        Overwrite = false;
    }

    enum TabIndex
    {
        TABINDEX_TERRAIN1,
        TABINDEX_TERRAIN2,
        TABINDEX_SCENERY,
        TABINDEX_ELEVATION1,
        TABINDEX_ELEVATION2,
        TABINDEX_NAVIGABILITY,

        TABINDEX_COUNT,
    };

    typedef std::set<int> ListBoxSelection;

    ListBoxSelection Selection;
    bool Overwrite;

    // TODO: dc::dcGenericEnum
    dc::dcGenericCollection* CollectionContents;
    //dcGenericEnum* EnumContents;
};


#define CACHE_MAPINFO                    1
#define CACHE_LAYERMESHSUBSET(layer)  (2<<layer)
#define CACHE_RULES(layer)            (8<<layer)

#define CACHE_ALL                       (1|2|4|8|16)


/**
 * TODO: build an on-screen cache, sort by layer (1) and texture (2) for rendering
 */
class AtlasEditDialog : public dcx::dcxD3DDialogController
{
public:

    // Compile information for each of the map elements that is currently on screen
    struct MapLocation
    {
        struct TerrainLayer
        {
            /// The original map-layer-specified terrain type for
            /// this location.  This is used to apply rules.
            Terrain::Preview* mapLayerTerrain;

            /// The map layer that specified the map-layer terrain.  This
            /// is used to apply rules.
            MapLayer::Preview* mapLayer;

            /// The terrain type specified by detailed editing.  If
            /// no editing applies to this element, this is null.
            Terrain::Preview* detailedTerrain;

            /// The terrain type used in this layer
            Terrain::Preview* terrain;

            /// How much to translate the mesh used to render this terrain type along Y 
            float elevation;

            /// The subset of the terrain mesh to use to render this terrain
            TerrainMeshSubset terrainMeshSubset;

            /// How much to scale this element vertically
            float height;
        };

        // Set of flags designating which members have been completed
        unsigned long cachedMembers;

        /// What kind of walkability the map has here
        MapNavigability navigability;

        /// The scenery here
        Scenery::Preview* scenery;

        /// The two layers of terrain
        TerrainLayer layer[2];
    };

    public:

        typedef std::map<std::pair<int,int>,MapLocation*> MapCacheType;

        typedef std::set<MapCacheType::key_type> MapCacheKeySet;


    public:

        AtlasEditDialog(Atlas* atlas);

        void execute();

        /**
         * Compiles the map into the context.  This is not a very efficient algorithm.
         */
        bool compile(AtlasCompilationContext* context);


    protected:

        bool loadResources();

        void unloadResources();

        void createToolbox();

        void renderScene();

        /**
         * Draws the terrain element indicated using the provided locating matrix
         */
        void renderTerrainLayer(const D3DXMATRIX* locationMatrix, MapLocation::TerrainLayer* layer);

        /**
         * Draws the scenery element indicated using the provided locating matrix
         */
        void renderSceneryPreview(const D3DXMATRIX* locationMatrix, Scenery::Preview* preview);

        /**
         * Erases all of the entries from the map cache
         */
        void clearMapCache();

        /**
         * Loads the information for the map at the given location and creates a
         * cache entry for it.  If this entry already exists, it is deleted and reloaded.
         */
        MapCacheType::iterator addEntryToCache(int x, int y, unsigned long entryFlags);

        /**
         * Adds all of the coordinates in the given list to the local cache.  If the entries are
         * in the global cache, they are pulled from there without recalculating their
         * attributes.
         * Rules are never applied to the global cache, only to the local one.  As a result,
         * rules cannot be guaranteed valid in a border of 2 squares around any location that
         * is not in the list of coordinates.
         */
        void addEntriesToCache(MapCacheKeySet* coordinates, MapCacheType* localCache, DWORD flags);

        void removeCacheEntry(int x, int y);

        /**
         * Fills in the 'rules' member of the cache entry.  This is not calculated in the
         * addEntryToCache method because it requires calling addEntryToCache for the
         * neighboring members; this can cause infinite recursion.
         */
        void applyRules(MapCacheType::iterator cacheEntry);

        /**
         *
         */
        bool CheckCanvasTypes(MapCacheType::iterator location, TerrainRule::OptionList* canvasTypes);


    protected:

        void toolbox_ApplyToSelection();

        ToolboxTab* getTab(ToolboxTab::TabIndex index);
        void readTabDataFromToolbox(ToolboxTab::TabIndex tabIndex);
        void updateCurrentToolboxTab();

        INT_PTR dialogNotification(HWND hDlg, HWND hControl, UINT_PTR controlID, UINT notification);

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
        INT_PTR dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

        INT_PTR toolbox_ContextMenu(HWND hWnd, POINTS point);

        /**
         * Sets up the current texture being used to render on the Direct3D device.  This
         * optimizes calls by only changing the texture if it's different from the current one.
         */
        void setRenderingTexture(Texture::Preview* texture);

    protected:

        Atlas* myAtlas;

        dcxWin32Mouse myMouse;
        dcxWin32Keyboard myKeyboard;

        /// The standard Volucris overhead camera
        IsometricCamera myCamera;

        // Rendering flags
        BOOL myShowScenery, myShowNavigability, myShowTerrain1, myShowTerrain2, myShowWireframe;

        MapCacheType myGlobalMapCache;

        /// The map cache for the elements that are currently visible
        MapCacheType myLocalMapCache;

        /// This is the area that is currently stored in the local map cache (inclusive top/left, exclusive bottom/right)
        RECT myCachedViewingRegion;

        HWND myToolbox;
        SIZE myToolboxClientSize; // used for minimizing the toolbox....not used now

        ToolboxTab myTerrain1Tab, myTerrain2Tab, mySceneryTab, myElevation1Tab, myElevation2Tab, myNavigabilityTab;

        /// The currently selected elements
        typedef std::list<std::pair<POINT,POINT>> Selection;
        Selection mySelection;
        POINT myCursorLocation;

        /// How high the selection bounces
        float mySelectionBounceHeight;

        /// The time at which the selection changed
        DWORD mySelectTime;

        /// Textures for navigability types
        LPDIRECT3DTEXTURE9 myNavigabilityTextures[8];

        float myZoom;
        D3DXVECTOR2 myCurrentLocation;
        int myViewGranularity;

        LPDIRECT3DTEXTURE9 myCursorTexture;

        LPD3DXMESH myTerrainMesh;

        /// The texture that's currently being used to render
        Texture::Preview* myCurrentRenderingTexture;

    protected:

        typedef dc::dcMap<WorldLocationType,MapLocationInfo> DetailedLocationMap;
        typedef std::map<WorldLocationType::BasicType,MapLocationInfo::Preview> DetailedLocationPreviewMap;

        // Get references to all of the relavent information
        ProjectVolucris*  projectVolucris;
        dc::dcTable<Image>*   imageTable;
        dc::dcTable<Texture>* textureTable;
        dc::dcList<Skin>*    skinList;
        dc::dcTable<Mesh>*    meshTable;
        dc::dcList<Scenery>* sceneryList;
        dc::dcList<Terrain>* terrainList;
        dc::dcList<TerrainRule>* terrainRuleList;
        dc::dcList<MapLayer>* mapLayerList;
        DetailedLocationMap* detailedLocationMap;

        // Create arrays for all of the relavent tables
        Image::Preview* images;
        Texture::Preview* textures;
        Skin::Preview* skins;
        Mesh::Preview* meshes;
        Scenery::Preview* scenery;
        Terrain::Preview defaultTerrain[2];
        float defaultTerrainElevation[2];
        Terrain::Preview* terrain;
        TerrainRule::Preview* terrainRules;
        MapLayer::Preview* mapLayers;
        DetailedLocationPreviewMap detailedMapLocations;
};








#endif