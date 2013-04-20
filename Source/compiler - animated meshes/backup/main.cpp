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
#include "rmxftmpl.h"
//#include <direct.h>

#if defined(DEBUG)||defined(_DEBUG)
#pragma comment(lib, "dc_d.lib")
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib, "dc.lib")
#pragma comment(lib, "d3dx9.lib")
#endif

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")



bool loadXFile( const std::string& filename, FrameNode* rootFrameNode, LPDIRECT3DDEVICE9 d3dDevice )
{
    ID3DXFile* dxFile = 0;
    ID3DXFileEnumObject* dxFileEnum = 0;

    // Create the file parsing structure
    if( APP_ERROR( FAILED( D3DXFileCreate( &dxFile ) ) )( "Couldn't create IDirectXFile to parse source file" ) )
        goto FAILED;

    // Register templates for parsing an X file
    if( APP_ERROR( FAILED( dxFile->RegisterTemplates( (LPVOID)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES ) ) )
            ( "Failed to register D3DRM X-file templates" ) )
        goto FAILED;

    // Register extended templates for parsing an X file
    if( APP_ERROR( FAILED( dxFile->RegisterTemplates( (LPVOID)XSKINEXP_TEMPLATES, sizeof(XSKINEXP_TEMPLATES) - 1 ) ) )
            ( "Failed to register extended X-file templates" ) )
        goto FAILED;

    // Create an enumerator so that we can look at the top-level entries in the file
    if( APP_ERROR( FAILED( dxFile->CreateEnumObject( (LPCVOID)filename.c_str(), D3DXF_FILELOAD_FROMFILE, &dxFileEnum ) ) )
            ( "Couldn't create enumerator for .X file in memory" ) )
        goto FAILED;

    // Scan all of the top-level objects in the file
    bool continueLoading = true;
    SIZE_T numberOfChildren;
    dxFileEnum->GetChildren( &numberOfChildren );
    for( SIZE_T i = 0; continueLoading && i < numberOfChildren; ++i )
    {
        ID3DXFileData* dxFileDataObject = NULL;
        continueLoading = !APP_ERROR( FAILED( dxFileEnum->GetChild( i, &dxFileDataObject ) ) || !rootFrameNode->load( dxFileDataObject, d3dDevice ) )( "Couldn't load element" );
        SAFE_RELEASE( dxFileDataObject );
    }

    // Free the file objects
    SAFE_RELEASE( dxFileEnum );
    SAFE_RELEASE( dxFile );

    // Success
    return true;


FAILED:

    SAFE_RELEASE( dxFileEnum );
    SAFE_RELEASE( dxFile );

    return false;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


struct AnimationSegment
{
    std::string name;
    DWORD start, end;
    UnseenSkeletalAnimation animation;

    AnimationSegment( const std::string& n, DWORD s, DWORD e ) { name = n; start = s; end = e; }
};


// adjusts the animation so that each frame's tick value is that of its frame
void normalizeAnimationTicksToFrameIndices(UnseenSkeletalAnimation* animation) {
  for (DWORD i = 0; i < animation->info.numberOfKeyframes; ++i) {
    animation->keyframes[i].tick = i;
  }
  animation->info.tickLength = animation->info.numberOfKeyframes-1;
  animation->info.averageTicksPerKeyframe = 1;
  animation->info.secondsPerTick = 1.0 / 24.0;
}


bool splitUnseenSkeletalAnimation(const UnseenSkeletalAnimation* inputAnimation,
                                  std::list<AnimationSegment>* animationSegments,
                                  bool segments_are_ticks)
{
    DWORD highestKeyframeIndex = inputAnimation->info.numberOfKeyframes - 1;

    // Go through the input animation and find the tick ranges, then copy out the keyframesi nto the animation
    for( std::list<AnimationSegment>::iterator i = animationSegments->begin(); i != animationSegments->end(); ++i )
    {
        AnimationSegment* segment = &(*i);

        DWORD lowerBoundKeyframe = segment->start;
        DWORD upperBoundKeyframe = segment->end;

        { // bounds-check the keyframes
          DWORD highest_keyframe = inputAnimation->info.numberOfKeyframes - 1;
          upperBoundKeyframe = min(highest_keyframe, upperBoundKeyframe);
          lowerBoundKeyframe = min(upperBoundKeyframe, lowerBoundKeyframe);
        }

        DWORD endTick = inputAnimation->keyframes[upperBoundKeyframe].tick;
        DWORD startTick = inputAnimation->keyframes[lowerBoundKeyframe].tick;

        // if using ticks instead of keyframe indices, find the keyframe with the
        // given tick index
        if (segments_are_ticks) {
          while (inputAnimation->keyframes[upperBoundKeyframe].tick > segment->end &&
                 upperBoundKeyframe > 0) {
            upperBoundKeyframe--;
          }
          CONFIRM(upperBoundKeyframe > 0) else continue;
          lowerBoundKeyframe = upperBoundKeyframe - 1;
          while (inputAnimation->keyframes[lowerBoundKeyframe].tick > segment->start &&
                 lowerBoundKeyframe > 0) {
            lowerBoundKeyframe--;
          }

          // make sure the tick indices were found
          CONFIRM(inputAnimation->keyframes[lowerBoundKeyframe].tick == segment->start &&
                  inputAnimation->keyframes[upperBoundKeyframe].tick == segment->end) else {
            printf("Tick index for %s not matched exactly; %lu-%lu >>> %lu-%lu\n",
                   segment->name,
                   segment->start, segment->end,
                   inputAnimation->keyframes[lowerBoundKeyframe].tick,
                   inputAnimation->keyframes[upperBoundKeyframe].tick);
          }

        } else {
          endTick = inputAnimation->keyframes[upperBoundKeyframe].tick;
          startTick = inputAnimation->keyframes[lowerBoundKeyframe].tick;
        }


        DWORD numberOfAnimatedFrames = inputAnimation->info.numberOfAnimatedFrames;
        DWORD numberOfKeyframes = upperBoundKeyframe - lowerBoundKeyframe + 1;


        // Copy information
        UnseenSkeletalAnimation* outputAnimation = &segment->animation;
        outputAnimation->info.tickLength = endTick - startTick;
        outputAnimation->info.loop = false;
        outputAnimation->info.numberOfSkeletalFrames = inputAnimation->info.numberOfSkeletalFrames;
        outputAnimation->info.numberOfAnimatedFrames = inputAnimation->info.numberOfAnimatedFrames;
        outputAnimation->info.numberOfKeyframes      = numberOfKeyframes;
        outputAnimation->info.averageTicksPerKeyframe = inputAnimation->info.averageTicksPerKeyframe;
        outputAnimation->info.secondsPerTick          = inputAnimation->info.secondsPerTick;

        // Copy the mapping
        outputAnimation->frameIndexToAnimatedFrameTransformTable = new DWORD[inputAnimation->info.numberOfSkeletalFrames];
        memcpy( outputAnimation->frameIndexToAnimatedFrameTransformTable,
                 inputAnimation->frameIndexToAnimatedFrameTransformTable, sizeof(DWORD) * inputAnimation->info.numberOfSkeletalFrames );

        // Copy the keyframes
        outputAnimation->keyframes = new UnseenSkeletalAnimation::Keyframe[numberOfKeyframes];
        for( DWORD keyframeIndex = 0; keyframeIndex < numberOfKeyframes; ++keyframeIndex )
        {
            UnseenSkeletalAnimation::Keyframe* sourceKeyframe = &inputAnimation->keyframes[keyframeIndex + lowerBoundKeyframe];
            UnseenSkeletalAnimation::Keyframe* destKeyframe =  &outputAnimation->keyframes[keyframeIndex];

            // Set the tick time
            destKeyframe->tick = sourceKeyframe->tick - startTick;

            // Copy the transforms
            destKeyframe->animatedFrameTransforms = new D3DXMATRIX[numberOfAnimatedFrames];
            memcpy( destKeyframe->animatedFrameTransforms, sourceKeyframe->animatedFrameTransforms, sizeof(D3DXMATRIX) * numberOfAnimatedFrames );
        }
    }

    // Success
    return true;
}



int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Usage: compileanimatedmeshes \"folder 1\" [\"folder 2\", ...]");
    return -1;
  }

  // Register the window class
  WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                    GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                    "D3D Tutorial", NULL };
  RegisterClassEx( &wc );

  // Create the application's window
  HWND hWnd = CreateWindow( "D3D Tutorial", "D3D Tutorial 02: Vertices",
                            WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
                            NULL, NULL, wc.hInstance, NULL );

  // Create the D3D object.
  IDirect3D9* d3d = 0;
  if( NULL == ( d3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
      return -1;

  // Set up the structure used to create the D3DDevice
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory( &d3dpp, sizeof(d3dpp) );
  d3dpp.Windowed = TRUE;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

  // Create the D3DDevice
  IDirect3DDevice9* d3dDevice = 0;
  if( FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                 D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                &d3dpp, &d3dDevice ) ) )
  {
      SAFE_RELEASE( d3d );
      return -1;
  }

  for (int arg_index = 1; arg_index < argc; ++arg_index) {

    // Find all .X files in the target folder
    WIN32_FIND_DATA findFileData;
    std::string folder = argv[arg_index];
    HANDLE hFind = FindFirstFile( (folder + "\\*.x").c_str(), &findFileData );
    printf("Processing .X files in %s:\n", argv[arg_index]);

    // Find all files in this folder
    bool is_first_file = true;
    while (hFind != INVALID_HANDLE_VALUE) {

      if (!is_first_file) {
        if (!FindNextFile(hFind, &findFileData)) {
          FindClose(hFind);
          hFind = INVALID_HANDLE_VALUE;
          continue;
        }
      } else {
        is_first_file = false;
      }

      std::string file_name = findFileData.cFileName;
      file_name = file_name.substr(0, file_name.find_last_of("."));
      std::string config_file = folder + "\\" + file_name + ".cfg";

      printf("\n - %s", findFileData.cFileName);

      bool segments_are_ticks = true;
      std::list<AnimationSegment> animation_segments;
      {
        FILE* animation_sequences;
        if (0 != fopen_s(&animation_sequences, config_file.c_str(), "rt")) {
          printf(" (assuming unique animation tracks)\n",
                 config_file.c_str(), findFileData.cFileName);
        } else {
          char ticks_or_frames[64];
          bool read_type = 1 == fscanf_s(animation_sequences, "%s [\r\n][\r\n] ", ticks_or_frames, sizeof(ticks_or_frames));
          bool type_found = false;
          if (0 == _stricmp(ticks_or_frames, "ticks")) {
            type_found = true; segments_are_ticks = true;
          } else if (0 == _stricmp(ticks_or_frames, "frames")) {
            type_found = true; segments_are_ticks = false;
          }
          CONFIRM(read_type && type_found) else {
            printf("Malformed .cfg file: first line should be either \"ticks\" or \"frames\"");
            continue;
          }
          while (!feof(animation_sequences)) {
            char animation_name[256];
            int animation_start, animation_end;
            CONFIRM(3 == fscanf_s(animation_sequences,
                                  "%[^:]: %i - %i [\r\n][\r\n] ",
                                  animation_name, sizeof(animation_name),
                                  &animation_start,
                                  &animation_end)) else {
              printf("Malformed .cfg file:  format is \"NAME: start frame - end frame\"");
              break;
            }

            // add this animation
            animation_segments.push_back(
                AnimationSegment(animation_name,
                                 animation_start,
                                 animation_end));
          }

          fclose(animation_sequences);
        }

        FrameNode::UnseenAnimationSet animations;
        std::string output_folder = (folder + "\\" + file_name).c_str();
        FrameNode root;
        { // load the file
          CONFIRM(loadXFile((folder + "\\" + findFileData.cFileName).c_str(), &root, d3dDevice)) else {
            printf("Unable to load '%s'\n", file_name.c_str());
            continue;
          }

          UnseenSkinnedMesh mesh;
          root.bake(d3dDevice, &mesh, &animations, 0.0f);

          // Make sure the directory exists, then access it
          CreateDirectoryA(output_folder.c_str(), NULL);
          SetCurrentDirectory(output_folder.c_str());

          { // save this mesh
            dc::dcFileStream fs;
            std::string output_file = output_folder + "\\" + file_name + ".usm";
            CONFIRM(fs.open(output_file.c_str(),
                            STREAM_OVERWRITE));
            saveUnseenSkinnedMesh(&fs, &mesh);
            fs.close();
          }

          deallocateUnseenSkinnedMesh(&mesh);
        }

        if (animation_segments.empty()) { // just use embedded tracks
          for( FrameNode::UnseenAnimationSet::iterator i = animations.begin(); i != animations.end(); ++i ) {
            dc::dcFileStream fs;
            CONFIRM(fs.open((output_folder + "\\" + i->first + ".uka").c_str(), STREAM_OVERWRITE)) else {
              continue;
            }
            CONFIRM(saveUnseenSkeletalAnimation( &fs, i->second ));
            fs.close();
          }
        } else { // split the first track into sections
          CONFIRM(animations.size() == 1) else {
            printf("Split animation cannot proceed; only one animation is allowed\n");
            continue;
          }
          CONFIRM(splitUnseenSkeletalAnimation(animations.front().second,
                                              &animation_segments,
                                               segments_are_ticks)) else {
            printf("Split animation failed\n");
            continue;
          }

          for (std::list<AnimationSegment>::iterator i = animation_segments.begin();
               i != animation_segments.end(); ++i) {
            UnseenSkeletalAnimation* animation = &i->animation;
            normalizeAnimationTicksToFrameIndices(animation);

            dc::dcFileStream fs;
            CONFIRM(fs.open((output_folder + "\\" + i->name + ".uka").c_str(), STREAM_OVERWRITE));
            CONFIRM(saveUnseenSkeletalAnimation(&fs, animation));
            fs.close();

            deallocateUnseenSkeletalAnimation(animation);
          }
        }

        root.deallocate( &animations );
        root.destroy();
      }
    }
  }

  SAFE_RELEASE( d3dDevice );
  SAFE_RELEASE( d3d );



  // Display debugging messages
  DEBUG_ATEXIT();
}


