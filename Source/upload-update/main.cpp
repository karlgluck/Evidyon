//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#include <dc/debug>
#include "../shared/evidyon_version.h"
#include <wininet.h>

#include "../shared/updater-launcher/clientupdatefile.h"
#include "../shared/updater-monitor/serverupdatefile.h"
#include "../common/md5/md5wrapper.h"


static const char* FTP_USERNAME = "FTP Username Goes Here";
static const char* FTP_PASSWORD = "FTP Password Goes Here";

const char* CLIENT_UPDATE_FILES[] = {
  "framework\\client.exe", "evidyon.exe",
  "evidyon-gamefile.evcli", "evidyon.evcli",
};

static const int NUMBER_OF_CLIENT_UPDATE_FILES = (sizeof(CLIENT_UPDATE_FILES) / sizeof(const char*)) / 2;

const char* SERVER_UPDATE_FILES[] = {
  "framework\\server.exe", "server.exe",
  "framework\\server.pdb", "server.pdb", // for debugging
  "framework\\database.cfg", "database.cfg",
  "evidyon-gamefile.evsvr", "evidyon.evsvr",
};
static const int NUMBER_OF_SERVER_UPDATE_FILES = (sizeof(SERVER_UPDATE_FILES) / sizeof(const char*)) / 2;

const char* EVIDYON_INSTALLER_SRC = "framework\\install-evidyon.msi";
const char* EVIDYON_INSTALLER_DEST = "install-evidyon.msi";

const char* SERVER_INSTALLER_SRC = "framework\\install-server.msi";
const char* SERVER_INSTALLER_DEST = "install-server.msi";


// the remote file format is:
//======================================================
// major
// minor         -   last version (if major is different, download new installer)
// <file name>
// <md5 hash>
// ...
bool ProcessFile(const std::string& local_file_name,
                 const std::string& remote_file_name,
                 const std::string& remote_hash,
                 std::string* update_file_line,
                 HINTERNET hFtp) {
  md5wrapper md5;

  std::string local_hash = md5.getHashFromFile(local_file_name);

  std::string file_size;
  HANDLE hFile = CreateFile(local_file_name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
  if (hFile != INVALID_HANDLE_VALUE) {
      DWORD high;
      DWORD low = GetFileSize(hFile, &high);
      __int64 size = ((__int64)(high)<<32)|((__int64)(low));
      char file_size_buf[512];
      sprintf_s(file_size_buf, 512, "%I64d", size);
      file_size = file_size_buf;
  }
  CloseHandle(hFile);


  if (local_hash.compare(remote_hash) != 0) {
    printf("uploading %s bytes...", file_size.c_str());
    FtpDeleteFile(hFtp, remote_file_name.c_str()); // erase the current version if it exists
    CONFIRM(TRUE == FtpPutFile(hFtp,
                               local_file_name.c_str(),
                               remote_file_name.c_str(),
                               FTP_TRANSFER_TYPE_BINARY,
                               NULL)) else {
      return false;
    }
  } else {
    printf("file is current");
  }

  update_file_line->assign(remote_file_name + + "\n" + file_size + "\n" + local_hash);
  return true;
}



bool UploadUpdateFiles(const char* update_host,         // host name for FTP login; ex. "www.unseenstudios.com"
                       const char* ftp_username,
                       const char* ftp_password,
                       const char* webaddr_update_file, // full internet path to the current update file
                       const char* update_subfolder,    // folder under 'update_host'; ex. /download/update
                       const char** filename_pairs,      // array of pairs of { local, remote } filenames as linear array
                       int number_of_file_pairs,
                       const char* update_file_name,    // name of just the update file; ex. "update.dat"
                       Evidyon::UpdateFile* remote_update_file, // data from the current online update file
                       HINTERNET* phInternet,           // returned internet connection (close this)
                       HINTERNET* phFtp                 // returned open FTP connection (close this)
                       ) {
  CONFIRM(DownloadUpdateFile(webaddr_update_file, remote_update_file)) else{
    printf("Remote update file not found\n\n");
  }
  HINTERNET hInternet = InternetOpen("EvidyonUpdateUploader",
                                     LOCAL_INTERNET_ACCESS,
                                     NULL,
                                     INTERNET_INVALID_PORT_NUMBER,
                                     INTERNET_FLAG_DONT_CACHE);
  HINTERNET hFtp = InternetConnect(hInternet,
                                   update_host, // address
                                   INTERNET_DEFAULT_FTP_PORT,   // port
                                   ftp_username,             // user
                                   ftp_password,                // password
                                   INTERNET_SERVICE_FTP,
                                   0,
                                   NULL);
  CONFIRM(hFtp) else {
    return false;
  }

  *phInternet = hInternet;
  *phFtp = hFtp;

  FtpSetCurrentDirectory(hFtp, update_subfolder);

  std::string update_file = Evidyon::UPDATE_FILE_HEADER_TEXT;
  update_file.append(MAJOR_VERSION_TEXT);
  update_file.append("\n");
  update_file.append(MINOR_VERSION_TEXT);
  update_file.append("\n");


  for (int i = 0; i < number_of_file_pairs; ++i) {
    std::string local_file_name  = filename_pairs[2*i + 0];
    std::string remote_file_name = filename_pairs[2*i + 1];
    printf("Updating '%s'->%s...", local_file_name.c_str(), remote_file_name.c_str());
    std::string update_file_line;
    std::string remote_hash = "";
    std::map<std::string,Evidyon::UpdateFileData>::iterator remote_pair =
      remote_update_file->files.find(remote_file_name);
    if (remote_pair != remote_update_file->files.end()) {
      remote_hash = remote_pair->second.md5;
    }
    CONFIRM(ProcessFile(local_file_name,
                        remote_file_name,
                        remote_hash,
                       &update_file_line,
                       hFtp)) else {
      printf("failed!");
      break;
    }

    printf("\n%s\n\n", update_file_line.c_str());

    update_file += update_file_line;
    update_file += "\n";
  }

  // Write out the update file to the server
  printf("\n\nUploading update file '%s'...\n", update_file_name);
  FILE* local_update_file = NULL;
  fopen_s(&local_update_file, update_file_name, "w+t");
  fwrite(update_file.c_str(), update_file.size(), 1, local_update_file);
  fclose(local_update_file);
  FtpDeleteFile(hFtp, update_file_name); // erase the current version if it exists
  CONFIRM(TRUE == FtpPutFile(hFtp,
                             update_file_name,
                             update_file_name,
                             FTP_TRANSFER_TYPE_BINARY,
                             NULL));

  return true;
}



int main() {

  { // update the server
    printf("\n\nUpdating the Evidyon Server\n"
           "=============================\n");
    HINTERNET hInternet, hFtp;
    Evidyon::UpdateFile remote_update_file;
    CONFIRM(UploadUpdateFiles(Evidyon::ONLINE_SERVER_UPDATE_HOST,
                              FTP_USERNAME,
                              FTP_PASSWORD,
                              Evidyon::ONLINE_SERVER_UPDATE_FILE,
                              Evidyon::ONLINE_SERVER_UPDATE_SUBFOLDER,
                              SERVER_UPDATE_FILES,
                              NUMBER_OF_SERVER_UPDATE_FILES,
                              Evidyon::SERVER_UPDATE_FILE_NAME,
                              &remote_update_file,
                              &hInternet,
                              &hFtp)) else {
      printf("Updating the server FAILED!\n");
    }

    //// Upload a new server installer
    //if (remote_update_file.major != MAJOR_VERSION ||
    //    remote_update_file.minor != MINOR_VERSION) {
    //  printf("\n\nUploading new server installer (version %s)...", VERSION_TEXT);
    //  CONFIRM(TRUE == FtpPutFile(hFtp,
    //                             SERVER_INSTALLER_SRC,
    //                             SERVER_INSTALLER_DEST,
    //                             FTP_TRANSFER_TYPE_BINARY,
    //                             NULL));
    //}

    InternetCloseHandle(hFtp);
    InternetCloseHandle(hInternet);
  }

  { // update the client
    printf("Updating the Evidyon Client\n"
           "=============================\n");

    HINTERNET hInternet, hFtp;
    Evidyon::UpdateFile remote_update_file;
    CONFIRM(UploadUpdateFiles(Evidyon::ONLINE_CLIENT_UPDATE_HOST,
                              FTP_USERNAME,
                              FTP_PASSWORD,
                              Evidyon::ONLINE_CLIENT_UPDATE_FILE,
                              Evidyon::ONLINE_CLIENT_UPDATE_SUBFOLDER,
                              CLIENT_UPDATE_FILES,
                              NUMBER_OF_CLIENT_UPDATE_FILES,
                              Evidyon::CLIENT_UPDATE_FILE_NAME,
                              &remote_update_file,
                              &hInternet,
                              &hFtp)) else {
      printf("Updating the client FAILED!\n");
    }

    //if (remote_update_file.major != MAJOR_VERSION ||
    //    remote_update_file.minor != MINOR_VERSION) {
    //  printf("\n\nUploading new client installer (version %s)...", VERSION_TEXT);
    //  CONFIRM(TRUE == FtpPutFile(hFtp,
    //                             EVIDYON_INSTALLER_SRC,
    //                             EVIDYON_INSTALLER_DEST,
    //                             FTP_TRANSFER_TYPE_BINARY,
    //                             NULL));
    //}

    InternetCloseHandle(hFtp);
    InternetCloseHandle(hInternet);
  }




  // Finalize the debug log
  DEBUG_ATEXIT();

  // Quit
  return 0;
}










#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "comctl32.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "dc_d.lib")
#pragma comment(lib, "dcx_d.lib")
#else
#pragma comment(lib, "dc.lib")
#pragma comment(lib, "dcx.lib")
#endif
