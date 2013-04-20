#ifndef __EVIDYON_VERSION_H__
#define __EVIDYON_VERSION_H__
#pragma once



/// KARL : REMEMBER TO CHANGE THE VERSION IN THE INSTALLER !!!! ///

#define MAJOR_VERSION  3  /* client has to change to update; large changes */
#define MINOR_VERSION  7  /* client has to change to update; small changes */
#define BUILD_VERSION  0  /* client doesn't have to change, but a new version is available */
#define DEBUG_VERSION  0000   /* ignored */

/// This is the version stamp of the client & server
#define VERSION   (MAJOR_VERSION<<24|MINOR_VERSION<<16|BUILD_VERSION<<8|DEBUG_VERSION<<0)

#define VERSIONS_ARE_COMPATIBLE(a, b) ((a&0xFFFF0000)==(b&0xFFFF0000))

#define QUOTE_HELPER(a) #a
#define QUOTE(a) QUOTE_HELPER(a)
#define VERSION_TEXT  QUOTE(MAJOR_VERSION)"."QUOTE(MINOR_VERSION)"."QUOTE(BUILD_VERSION)
#define MAJOR_VERSION_TEXT QUOTE(MAJOR_VERSION)
#define MINOR_VERSION_TEXT QUOTE(MINOR_VERSION)



#endif