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
#ifndef __CLIENTUPDATEFILE_H__
#define __CLIENTUPDATEFILE_H__
#pragma once

#include "../updater/updatefile.h"

namespace Evidyon {

#define UPDATE_FILE_NAME_DEF        "update.txt"
#define WEBADDR_UNSEENSTUDIOS       "www.example.com"
#define WEBADDR_DOWNLOAD_SUBFOLDER  "/foo/bar/"
#define WEBADDR_DOWNLOAD_FOLDER     "http://" WEBADDR_UNSEENSTUDIOS WEBADDR_DOWNLOAD_SUBFOLDER
#define WEBADDR_UPDATE_FILE         WEBADDR_DOWNLOAD_FOLDER "update.txt"

static const char* ONLINE_CLIENT_UPDATE_HOST      = WEBADDR_UNSEENSTUDIOS;
static const char* ONLINE_CLIENT_UPDATE_SUBFOLDER = WEBADDR_DOWNLOAD_SUBFOLDER;
static const char* ONLINE_CLIENT_UPDATE_FOLDER    = WEBADDR_DOWNLOAD_FOLDER;
static const char* ONLINE_CLIENT_UPDATE_FILE      = WEBADDR_UPDATE_FILE;
static const char* CLIENT_UPDATE_FILE_NAME        = UPDATE_FILE_NAME_DEF;

}


#endif