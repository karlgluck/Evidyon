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
#include <windows.h>
#include <dcx/cpu>
#include <dc/debug>
#include "commctrl.h"
#include <dcx/win32>
#include <windowsx.h>
#include "../shared/evidyon_version.h"
#include <vector>
#include <direct.h>
#include "resource.h"
#include "../common/md5/md5wrapper.h"
#include "../shared/updater-launcher/clientupdatefile.h"
#include <dcx/dcxwin32internetstream.h>
#include <dcx/inifilereader>
#include <dcx/inifilewriter>


#pragma comment(lib, "d3d9.lib")



struct EvidyonUpdateFile {
  std::string file_name;
  __int64 file_size;
};







#define Progress_Enable(hwndCtl, flags)         EnableWindow((hwndCtl), (fEnable))
#define Progress_Show(hwndCtl, fShow)           ShowWindow((hwndCtl), (fShow) ? SW_SHOWNORMAL : SW_HIDE)
#define Progress_SetRange(hwndCtl, min, max)    ((LPARAM)(DWORD)SNDMSG((hwndCtl), PBM_SETRANGE, 0L, (LPARAM)MAKELPARAM( (min), (max) )))
#define Progress_SetRange32(hwndCtl, min, max)    ((LPARAM)(DWORD)SNDMSG((hwndCtl), PBM_SETRANGE, (WPARAM)(int)(min), (LPARAM)(int)(max)))
#define Progress_SetPos(hwndCtl, pos)           ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETPOS, (WPARAM)(int)(pos), 0L ))
#define Progress_DeltaPos(hwndCtl, delta)       ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_DELTAPOS, (WPARAM)(int)(delta), 0L ))
#define Progress_SetStep(hwndCtl, step)         ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETSTEP, (WPARAM)(int)(step), 0L ))
#define Progress_StepIt(hwndCtl)                ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_STEPIT, 0L, 0L ))
#define Progress_SetMarquee(hwndCtl, fMarquee, msPerUpdate)  ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETMARQUEE, (WPARAM)(BOOL)(fMarquee), (LPARAM)(UINT)(msPerUpdate) ))
#define Progress_SetBkColor(hwndCtl, clrBk)     ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETBKCOLOR, 0L, (LPARAM)(COLORREF)(clkBk) ))
#define Progress_SetBarColor(hwndCtl, clrBar)   ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETBARCOLOR, 0L, (LPARAM)(COLORREF)(clrBar) ))

// PBST_NORMAL, PBST_ERROR, PBST_PAUSED
#define Progress_SetState(hwndCtl, state)       ((LRESULT)(DWORD)SNDMSG((hwndCtl), PBM_SETSTATE, (WPARAM)(int)(state), 0L ))



#define ScrollBar_SetPos(hwndCtl, pos, fRedraw)     SetScrollPos((hwndCtl), SB_CTL, (pos), (fRedraw))
#define ScrollBar_GetPos(hwndCtl)                   GetScrollPos((hwndCtl), SB_CTL)

#define ScrollBar_SetRange(hwndCtl, posMin, posMax, fRedraw)    SetScrollRange((hwndCtl), SB_CTL, (posMin), (posMax), (fRedraw))
#define ScrollBar_GetRange(hwndCtl, lpposMin, lpposMax)         GetScrollRange((hwndCtl), SB_CTL, (lpposMin), (lpposMax))



const char* STRING_ERROR_BOX_TITLE = "Launcher Error";
const char* STRING_ERROR_PROCESSORINCOMPATIBLE = "Your computer is incompatible with Evidyon.\n\nThis game runs on AMD/Intel processors only.";
const char* STRING_ERROR_CANTCONNECTTOSERVER = "Couldn't connect to update server";

#include <dc/filestream>


#include <d3d9.h>

struct DisplayMode {
  std::string text;
  bool windowed;
  int width, height, bpp;
};

int CompareDisplayMode(const void* p1, const void* p2) {
  const DisplayMode* dm1 = (const DisplayMode*)p1;
  const DisplayMode* dm2 = (const DisplayMode*)p2;
  // fullscreen first
  if (!dm1->windowed &&  dm2->windowed) return -1;
  if ( dm1->windowed && !dm2->windowed) return +1;
  // sort resolution, bpp backward
  if (dm1->width < dm2->width) return +1;
  if (dm1->width > dm2->width) return -1;
  if (dm1->height < dm2->height) return +1;
  if (dm1->height > dm2->height) return -1;
  if (dm1->bpp < dm2->bpp) return +1;
  if (dm1->bpp > dm2->bpp) return -1;
  return 0;
}

typedef std::vector<DisplayMode> DisplayModeList;
DWORD EnumDisplayModes(int config_width,
                       int config_height,
                       int config_bpp,
                       bool config_windowed,
                       DisplayModeList& list) {
  list.clear();
  LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
  CONFIRM(d3d) else return -1;

  D3DDISPLAYMODE d3ddm_current;
  d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm_current);
  DWORD current_mode = -1, config_mode = -1, best_windowed_mode = -1;

  typedef std::set<std::pair<int,int>> WindowedModes;
  WindowedModes windowed_modes;

  static const D3DFORMAT ALLOWED_FORMATS[] = { D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, D3DFMT_R8G8B8, D3DFMT_X8R8G8B8 };
  static const int ALLOWED_FORMAT_BPP[] = { 15, 16, 24, 32 };
  static const int ALLOWED_FORMAT_COUNT = 4;
  for (int format = 0; format < ALLOWED_FORMAT_COUNT; ++format) {
    DWORD modes = d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, ALLOWED_FORMATS[format]);
    DWORD best_windowed_mode_area = 0;
    DisplayMode dm;
    dm.bpp = ALLOWED_FORMAT_BPP[format];
    for (DWORD i =0; i < modes; ++i) {
      D3DDISPLAYMODE d3ddm;
      d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, ALLOWED_FORMATS[format], i, &d3ddm);
      if (d3ddm.Width < 800 || d3ddm.Height < 600) continue; // throw out low resolutions

      dm.width = d3ddm.Width;
      dm.height = d3ddm.Height;

      { // kick out ridiculously widescreen formats
        const float aspect_ratio = (float)d3ddm.Width / (float)d3ddm.Height;
        if (aspect_ratio > 2.0f) continue;
      }

      if (d3ddm_current.Width == d3ddm.Width &&
          d3ddm_current.Height == d3ddm.Height &&
          d3ddm_current.Format == d3ddm.Format) {
        current_mode = list.size();
      }

      if (config_width == dm.width &&
          config_height == dm.height &&
          config_bpp == dm.bpp &&
          config_windowed == false) {
        config_mode = list.size();
      }

      char text[256];
      dm.windowed = false;
      sprintf_s(text, 256, "fullscreen : %i x %i (%i-bit color)", dm.width, dm.height, dm.bpp);
      dm.text = text;
      list.push_back(dm);

      DWORD area = dm.width * dm.height;
      WindowedModes::value_type wm_value(dm.width, dm.height);
      if (area <= (1280*1024) && // only add lower resolutions as windowed modes
          windowed_modes.find(wm_value) == windowed_modes.end()) { // prevent duplication
        windowed_modes.insert(wm_value);
        dm.windowed = true;
        sprintf_s(text, 256, "windowed : %i x %i", dm.width, dm.height);
        dm.text = text;

        if (best_windowed_mode_area < area) {
          best_windowed_mode = list.size();
        }

        if (config_width == dm.width &&
            config_height == dm.height &&
            config_windowed == true) {
          config_mode = list.size();
        }

        list.push_back(dm);
      }
    }
  }

  d3d->Release();
  if (config_mode < list.size()) return config_mode;
  if (current_mode < list.size()) return current_mode;
  return best_windowed_mode;
}




class DownloadDialog : public dcx::dcxWin32DialogController
{
    static const int ID_PROGRESS = 1100;
    static const int ID_EDITBOX_STATUS = 1101;
    static const int ID_BUTTON_STARTGAME = 1102;
    static const int ID_BUTTON_SETTINGS = 1103;
    static const int ID_BUTTON_EXIT = 1104;
    static const int ID_COMBO_DISPLAYMODE = 1105;
    static const COLORREF SKIN_TRANSPARENT_COLOR = RGB(0,255,0);

    public:

        HWND myMainWindow;

        HWND getProgressBar() { return GetDlgItem( myMainWindow, ID_PROGRESS ); }
        HWND getEditboxStatus() { return GetDlgItem( myMainWindow, ID_EDITBOX_STATUS ); }
        HWND getStartgameButton() { return GetDlgItem( myMainWindow, ID_BUTTON_STARTGAME ); }
        HWND getSettingsButton() { return GetDlgItem( myMainWindow, ID_BUTTON_SETTINGS ); }
        HWND getExitButton() { return GetDlgItem( myMainWindow, ID_BUTTON_EXIT ); }
        HWND getDisplayModeCombo() { return GetDlgItem( myMainWindow, ID_COMBO_DISPLAYMODE ); }

        void appendStatusLine( const char* text )
        {
            size_t newTextLen = strlen(text);
            HWND editBox = getEditboxStatus();
            int len = Edit_GetTextLength( editBox );

            size_t combinedLength = len + 1 + newTextLen + 5;
            char* currentText = new char[combinedLength];
            Edit_GetText( editBox, currentText, combinedLength );
            strcat_s( currentText, combinedLength, "\r\n" );
            strcat_s( currentText, combinedLength, text );
            Edit_SetText( editBox, currentText );
            delete[] currentText;

            Edit_SetSel( editBox, combinedLength, combinedLength );
            Edit_ScrollCaret( editBox );
        }

        struct UpdateFileData
        {
            std::string myNewerVersion;
            std::vector<std::string> myDownloadWebsites;
            std::vector<std::pair<std::string,size_t>> myFilesAndSizes;
            std::string myUpdateText;
        };


        void execute( HINSTANCE hInstance, dcx::dcxCPUFeatures* cpuFeatures ) {
            myMainWindow = createMainDialog( hInstance );

            dcx::dcxIniFileWriter ini_file_writer;
            DisplayModeList display_mode_list;
            {
              dcx::dcxIniFileReader ini_file_reader;
              ini_file_reader.read("config.ini");
              ini_file_writer.scan(&ini_file_reader);
              DWORD current_display_mode_index = 
                EnumDisplayModes(ini_file_reader.getSetting("Graphics", "ResolutionX", 0),
                                 ini_file_reader.getSetting("Graphics", "ResolutionY", 0),
                                 ini_file_reader.getSetting("Graphics", "ColorDepth", 0),
                                 ini_file_reader.getSetting("Graphics", "Fullscreen", 0) == 0, display_mode_list);

              { // re-sort
                DisplayMode current;
                if (current_display_mode_index < display_mode_list.size()) current = display_mode_list.at(current_display_mode_index);
                qsort(&display_mode_list.at(0), display_mode_list.size(), sizeof(DisplayModeList::value_type), CompareDisplayMode);
                DWORD index = 0;
                for (DisplayModeList::iterator i = display_mode_list.begin(); i != display_mode_list.end(); ++i, ++index) {
                  DisplayMode& dm = *i;
                  if (dm.width == current.width &&
                      dm.height == current.height &&
                      dm.windowed == current.windowed &&
                      (current.windowed == true || dm.bpp == current.bpp)) {
                    current_display_mode_index = index; break;
                  }
                }
              }


              HWND hDisplayModeCombo = getDisplayModeCombo();
              DWORD index = 0;
              for (DisplayModeList::iterator i = display_mode_list.begin(); i != display_mode_list.end(); ++i, ++index) {
                int k = ComboBox_AddString(hDisplayModeCombo, i->text.c_str());
                ComboBox_SetItemData(hDisplayModeCombo, k, index);
              }
              ComboBox_SetCurSel(hDisplayModeCombo, current_display_mode_index);
            }

            Evidyon::UpdateFile update_file;
            std::list<EvidyonUpdateFile> files_to_update;
            if (!Evidyon::DownloadUpdateFile(Evidyon::ONLINE_CLIENT_UPDATE_FILE, &update_file)) {
                errorMessageBox( "Couldn't contact the update server; please try again later" );
                goto EXIT;
            }

            if (update_file.major != MAJOR_VERSION) {
              // download the full installer
              MessageBox(myMainWindow, "The game has been significantly updated and you\n"
                                       "should re-download from www.unseenstudios.com", "Major Update!", MB_OK);
              goto EXIT;
            } else {
              ShowWindow( getEditboxStatus(), SW_SHOW);
              for (std::map<std::string, Evidyon::UpdateFileData>::const_iterator i = update_file.files.begin();
                   i != update_file.files.end(); ++i) {
                const std::string& filename = i->first;
                md5wrapper wrappy;
                std::string local_md5 = wrappy.getHashFromFile(filename);
                if (local_md5.compare(i->second.md5) != 0) {  // this file needs to be updated
                  EvidyonUpdateFile file;
                  file.file_name = filename;
                  file.file_size = i->second.file_bytes;
                  files_to_update.push_back(file);
                }
              }
            }
            // Update the client
            bool allowGameStart = false;
            if (files_to_update.size() > 0) {
              if (downloadUpdates(Evidyon::ONLINE_CLIENT_UPDATE_FOLDER, files_to_update)) {
                allowGameStart = true;
              } else {
                appendStatusLine( "ERROR!  Unable to download updates at this time.  If you are running Vista, this could "\
                                  "be due to the permissions for the directory in which Evidyon is installed being set to read-only." );
              }
            }
            else
                allowGameStart = true;

            // Add the update text to the output
            {
              //dc::dcFileStream fs;
              //if (fs.open("notes.txt", STREAM_TEXT)) {
              //  std::string notes;
              //  size_t len = fs.size();
              //  char text = new char[len+1];
              //  fs.read(text, len);
              //  text[len] = '\0';
              //  fs.close();
              //  appendStatusLine(text);
              //  delete[] text;
              //}
            }


            if (allowGameStart) {
                // Re-enable the "launch" button
                Button_Enable( getStartgameButton(), TRUE );
            }

            ShowWindow(getStartgameButton(), SW_SHOW);
            ShowWindow(getProgressBar(), SW_HIDE);

            // Wait for the user to do something
            myLaunchGameFlag = false;
            ComboBox_Enable(getDisplayModeCombo(), TRUE);
            while( dcx::dcxWin32StdMessagePump( myMainWindow ) && !myLaunchGameFlag );

            // Get the display mode selection
            int current_sel = ComboBox_GetCurSel(getDisplayModeCombo());
            current_sel = min(current_sel, display_mode_list.size() - 1);
            current_sel = max(current_sel, 0);
            DisplayMode& dm = display_mode_list.at(current_sel);
            ini_file_writer.setSetting("Graphics", "ResolutionX", dm.width),
            ini_file_writer.setSetting("Graphics", "ResolutionY", dm.height),
            ini_file_writer.setSetting("Graphics", "ColorDepth", dm.bpp),
            ini_file_writer.setSetting("Graphics", "Fullscreen", dm.windowed ? 0 : 1);
            {
              dc::dcFileStream ini_fs;
              ini_fs.open("config.ini", STREAM_OVERWRITE|STREAM_TEXT);
              ini_file_writer.write(&ini_fs);
              ini_fs.close();
            }

            // Close the window
            if( IsWindow( myMainWindow ) )
            {
                EndDialog( myMainWindow, 0 );
                DestroyWindow( myMainWindow );
                dcx::dcxWin32StdMessagePump( 0 );

                // Launch the client
                std::string exeFile = "evidyon";

                STARTUPINFO startupInfo;
                ZeroMemory( &startupInfo, sizeof(startupInfo) );

                PROCESS_INFORMATION processInfo;
                /*
                // Build the version to run based on the computer's capabilities
                if( cpuFeatures->sse2 ) {
                    exeFile.append( "_sse2" );
                }
                else if( cpuFeatures->sse ) {
                    exeFile.append( "_sse" );
                }*/

                exeFile.append( ".exe" );
                
                // Start the server
                if( !CreateProcessA( exeFile.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE|ABOVE_NORMAL_PRIORITY_CLASS,
                                     NULL, NULL, &startupInfo, &processInfo ) )
                {
                    DWORD error = GetLastError();
                    MessageBox( NULL, "Failed to launch Evidyon", "Error", MB_OK );
                    goto EXIT;
                }

                // Run the client
                WaitForSingleObject( processInfo.hProcess, INFINITE );

                // Free the process and exit
                CloseHandle( processInfo.hProcess );
                CloseHandle( processInfo.hThread );
            }

EXIT:
            DestroyWindow( myMainWindow );
        }

        bool myLaunchGameFlag;

        void errorMessageBox( const char* text )
        {
            MessageBox(myMainWindow, text, "Error", MB_OK);
        }

        bool downloadUpdates( const char* downloadWebsite, const std::list<EvidyonUpdateFile>& filesAndSizes )
        {
            // Hide the combo box, show the progress bar
            ComboBox_Enable( getDisplayModeCombo(), FALSE );
            ShowWindow( getProgressBar(), SW_SHOWDEFAULT );

            std::vector<std::pair<std::string,std::string>> tempFilesToLocalFiles;

            // Initialize the progress bar.  All files are scaled to 65535 since the 32-bit range
            // doesn't seem to be working correctly.
            Progress_SetRange( getProgressBar(), 0, 65535 );

            // Repeat for each of the files
            for (std::list<EvidyonUpdateFile>::const_iterator p = filesAndSizes.begin(); p != filesAndSizes.end(); ++p)
            {
                // Update the display window
                {
                    // Get the number of bytes
                    char bytes[16];
                    if( p->file_size > 1000 )
                    {
                        sprintf_s( bytes, "%lu KB", p->file_size / 1000 );
                    } else if( p->file_size > 1000000 )
                    {
                        sprintf_s( bytes, "%lu MB", p->file_size / 1000000 );

                    } else
                    {
                        sprintf_s( bytes, "%lu bytes", p->file_size );
                    }

                    std::string text = "Downloading ";
                    text.append( p->file_name );
                    text.append( " - " );
                    text.append( bytes );
                    appendStatusLine( text.c_str() );
                }

                // Get the file's name on the website
                std::string fileUrl = downloadWebsite;
                fileUrl.append( p->file_name );

                // Get the file's local name
                std::string fileLocalName = "~";
                fileLocalName.append( p->file_name );

                // Open the local file
                HANDLE hDestination;
                hDestination = CreateFile( fileLocalName.c_str(),
                                           GENERIC_WRITE,
                                           FILE_SHARE_READ,
                                           NULL,
                                           CREATE_ALWAYS,
                                           FILE_FLAG_WRITE_THROUGH | 
                                           FILE_FLAG_SEQUENTIAL_SCAN,
                                           NULL );
                CONFIRM( hDestination ) else return false;

                // Add to the list of local files
                tempFilesToLocalFiles.push_back( std::pair<std::string,std::string>( fileLocalName, p->file_name ) );

                // Download this file
                dcx::dcxWin32InternetStream reader;
                if( !reader.open( "EvidyonClient", fileUrl.c_str() ) )
                {
                    CloseHandle( hDestination );
                    return false;
                }

                // Reset the progress bar
                Progress_SetPos( getProgressBar(), 0 );

                char buffer[512];
                size_t bytesRead = 0;
                size_t bytes;
                while( reader.scan( buffer, sizeof(buffer), &bytes ) && !reader.end() )
                {
                    // Write this data into the output file
                    DWORD bytesWritten;
                    BOOL success = WriteFile( hDestination,
                                              buffer,
                                              bytes,
                                             &bytesWritten,
                                              NULL );

                    // Exit if the data couldn't be written or the user exited the launcher
                    if( !dcx::dcxWin32StdMessagePump( myMainWindow ) ||
                        APP_WARNING( !success || !bytesWritten )( "Couldn't write data to temporary file" ) )
                        break;

                    // Add to the number of bytes that were read
                    bytesRead += bytes;

                    // Update the progress bar
                    Progress_SetPos( getProgressBar(), (WORD)(((double)bytesRead / (double)p->file_size) * 65535) );
                }

                bool completed = reader.end();

                // Get rid of the output file and internet connection
                reader.close();
                CloseHandle( hDestination );

                // If we didn't finish the download, fail
                if( APP_ERROR( !completed )( "Didn't complete download of %s", fileUrl.c_str() ) )
                    return false;
            }

            // Delete original files and copy files over
            for( std::vector<std::pair<std::string,std::string>>::iterator i  = tempFilesToLocalFiles.begin();
                                                                           i != tempFilesToLocalFiles.end(); ++i )
            {
                // Copy over the existing file
                if (CopyFile( i->first.c_str(), i->second.c_str(), FALSE )) {
                  // Erase the temporary file
                  DeleteFile( i->first.c_str() );
                }
            }

            // Max out the bar
            Progress_SetPos( getProgressBar(), 65535 );

            // Success
            return true;
        }


        HWND createMainDialog( HINSTANCE hInstance ) {
            dcx::dcxWin32DialogTemplate tpl;

            // Create the dialog
            tpl.setSizeInDialogUnits( 469, 469 );
            tpl.setStyleAsPrompt();
            tpl.setStyle( WS_POPUP | WS_CLIPSIBLINGS | WS_SYSMENU | DS_ABSALIGN | DS_NOIDLEMSG | DS_CENTER );
            tpl.setTitle( "Evidyon" );
            tpl.addEditBoxInDialogUnits( ID_EDITBOX_STATUS, WS_VSCROLL|WS_BORDER|ES_MULTILINE|ES_READONLY|ES_LEFT|ES_AUTOVSCROLL, 10+55, 10 +50, 180, 70, "Contacting update server..." );
            tpl.addButtonInDialogUnits( ID_BUTTON_STARTGAME, WS_DISABLED, 10+55, 90+10+5 + 35 , 180, 25, "Play Evidyon" );
            tpl.addProgressBarInDialogUnits( ID_PROGRESS, PBS_SMOOTH, 10+55, 90+10+5 +35 + 5 , 180, 15 );
            tpl.addButtonInDialogUnits( ID_BUTTON_EXIT, 0, 10+80+30, 90+10+10+15+10+15+5 +50+20+5+15, 70, 20, "Quit" );
            tpl.addComboBoxInDialogUnits( ID_COMBO_DISPLAYMODE, WS_DISABLED|CBS_DROPDOWN|CBS_HASSTRINGS|CBS_DROPDOWNLIST| WS_VSCROLL, 10+55, 90+10+10+15+10+15+5 +50+20, 180, 150 );

            BYTE buffer[1024];
            HWND hWnd = createDialog( hInstance, tpl.compile( buffer, sizeof(buffer) ), NULL );

            { // set the window's icon
              HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
              SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
              SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            }

            if( dcx::dcxWin32WindowsVersionIs2000OrXP() )
            {
                // Set this window up as a layered window, which is only supported on 2000/xp systems 

                #define LWA_COLORKEY  0x00000001
                #define WS_EX_LAYERED  0x00080000

                typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
                lpfnSetLayeredWindowAttributes SetLayeredWindowAttributes;

                HMODULE hUser32 = GetModuleHandle("user32.dll");
                SetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32,"SetLayeredWindowAttributes");
                SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong( hWnd, GWL_EXSTYLE) | WS_EX_LAYERED );
                SetLayeredWindowAttributes( hWnd, SKIN_TRANSPARENT_COLOR, 0, LWA_COLORKEY );
            }
            else
            {
                // Use a region to define this window.  This is less efficient on new systems than using the layered approach
                HRGN hRegion = CreateEllipticRgn( 0, 0, 469, 469 );
                SetWindowRgn( hWnd, hRegion, TRUE );
            }

            // Update the window.  Get rid of the progress bar for now, since it doesn't do
            // anything until an update needs to be downloaded.
            ShowWindow( GetDlgItem( hWnd, ID_PROGRESS ), SW_HIDE );
            ShowWindow( GetDlgItem( hWnd, ID_EDITBOX_STATUS ), SW_HIDE );
            ShowWindow( GetDlgItem( hWnd, ID_BUTTON_STARTGAME ), SW_HIDE );
            ShowWindow( hWnd, SW_SHOW );

            // Return the window we just created
            return hWnd;
        }


        INT_PTR dialogNotification( HWND hDlg, HWND hControl, UINT_PTR ControlID, UINT Notification )
        {
            if( ControlID == ID_BUTTON_EXIT )
            {
                EndDialog( hDlg, 0 );
                DestroyWindow( hDlg );
                return 0;
            }
            if( ControlID == ID_BUTTON_STARTGAME )
            {
                myLaunchGameFlag = true;
            }
            return 0;
        }

//------------------------------------------------------------------------------------------------
// Name: dialogEvent
// Desc: Default implementation
//------------------------------------------------------------------------------------------------
INT_PTR dialogEvent( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( uMsg == WM_CLOSE ) { EndDialog( hDlg, 0 ); DestroyWindow( hDlg ); }
    if( uMsg == WM_INITDIALOG )
    {
    }

    switch( uMsg )
    {
        case WM_NCPAINT:
        {
            PAINTSTRUCT ps;

            //HANDLE hBm = LoadImage( NULL, "evidyon-bg.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE );
            HANDLE hBm = LoadImage( (HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_SHARED );

            HDC hImgDc = CreateCompatibleDC( 0 );
            HGDIOBJ oldBM = SelectObject( hImgDc, (HGDIOBJ)hBm );

            BeginPaint( hDlg, &ps );
            BitBlt( ps.hdc, 0, 0, 469, 469, hImgDc, 0, 0, SRCCOPY );
            SelectObject( hImgDc, oldBM );
            DeleteObject( hBm );
            DeleteDC( hImgDc );
            EndPaint( hDlg, &ps );

            /*
            //ValidateRect( GetDlgItem( myMainWindow, 100 ), NULL );

            PAINTSTRUCT ps;
            HDC hdc = GetWindowDC(hDlg);//GetDCEx(hWnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
            //HDC hdc = BeginPaint( hWnd, &ps );

            //HBITMAP hBm = LoadBitmap( GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BTRLOGINWINDOW) );
            HGDIOBJ oldBM = SelectObject( hImgDc, (HGDIOBJ)hBm );

            BitBlt( hdc, 0, 0, 400, 300, hImgDc, 0, 0, SRCCOPY );

            SelectObject( hImgDc, oldBM );

            DeleteObject( hBm ) ;

            DeleteDC( hImgDc );

            ReleaseDC( hDlg, hdc );
            EndPaint( hDlg, &ps );*/

        } break;

        case WM_LBUTTONDOWN:
        {
          // ---------------------------------------------------------
          // this is a common trick for easy dragging of the window.
          // this message fools windows telling that the user is
          // actually dragging the application caption bar.
          // ---------------------------------------------------------
          SendMessage(hDlg, WM_NCLBUTTONDOWN, HTCAPTION,NULL);
        } break;

        // Change the color of the dialog's background
        case WM_CTLCOLORDLG:
        {
            LOGBRUSH lb;
            lb.lbColor = SKIN_TRANSPARENT_COLOR;
            lb.lbHatch = 0;
            lb.lbStyle = BS_SOLID;
            return (INT_PTR)CreateBrushIndirect( &lb );
        } break;
    }
    return 0;
}

};

#include <aclapi.h>
#pragma comment (lib, "Advapi32.lib")

HRESULT ChangeACLtoAllowUserRW( CHAR* strDir )
{
  EXPLICIT_ACCESS explicitaccess;
  PACL NewAcl = NULL;
  DWORD dwError;

  BuildExplicitAccessWithName( &explicitaccess, "BUILTIN\\Users",
                               GENERIC_ALL, GRANT_ACCESS,
                               SUB_CONTAINERS_AND_OBJECTS_INHERIT );
                               
  dwError = SetEntriesInAcl( 1, &explicitaccess, NULL, &NewAcl );
  if( dwError == ERROR_SUCCESS) 
  {
      dwError = SetNamedSecurityInfo( strDir, SE_FILE_OBJECT,
                                      DACL_SECURITY_INFORMATION,
                                      NULL, NULL, NewAcl, NULL );
      if( dwError == ERROR_SUCCESS)
      {
          if( NewAcl != NULL ) AccFree( NewAcl );
          return S_OK;
      }
  }

  if( NewAcl != NULL ) AccFree( NewAcl );
  return E_FAIL;
}



//------------------------------------------------------------------------------------------------
// Name: WinMain
// Desc: 
//------------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, INT nArgs )
{
    // Find out what the CPU supports
    dcx::dcxCPUFeatures cpuFeatures;

    // this method was disabled
    memset(&cpuFeatures, 0, sizeof(cpuFeatures));
    //if( !dcx::dcxGetCPUFeatures( &cpuFeatures ) )
    //{
    //  MessageBox( NULL, STRING_ERROR_PROCESSORINCOMPATIBLE, STRING_ERROR_BOX_TITLE, MB_OK );
    //  goto EXIT;
    //}

    {
      INITCOMMONCONTROLSEX InitCtrlEx;
      InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
      InitCtrlEx.dwICC = ICC_TAB_CLASSES|ICC_PROGRESS_CLASS;
      InitCommonControlsEx(&InitCtrlEx);
    }

    { // make this user able to read/write the update dir
      char path[MAX_PATH];
      GetCurrentDirectory(MAX_PATH, path);
      ChangeACLtoAllowUserRW(path);
    }

    {
      DownloadDialog d;
      d.execute( hInstance, &cpuFeatures );
    }

    // Finalize the debug log
    DEBUG_ATEXIT();

    // Quit
    return 0;
}
