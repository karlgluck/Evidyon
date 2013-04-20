//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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