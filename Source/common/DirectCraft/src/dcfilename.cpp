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
#include "../dc/filename"



using namespace dc;


// Use caseless filename comparisons
#define DC_FILENAME_CASELESS
#define DC_MAX_PATH_LENGTH      512


/// The path to convert from/to when loading/saving file names
std::string dcFileName::theCurrentPath;


//-----------------------------------------------------------------------------
// Name:  dcFileName
// Desc:  Initializes the class
//-----------------------------------------------------------------------------
dcFileName::dcFileName()
{
    myValue = "";
}


//-----------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//-----------------------------------------------------------------------------
std::string dcFileName::toString() const
{
    return myValue;
}



//-----------------------------------------------------------------------------
// Name:  save
// Desc:  Saves this resource
//-----------------------------------------------------------------------------
bool dcFileName::save( dcStreamOut* stream )
{
    // Convert the path into a relative path
    char relativeFile[DC_MAX_PATH_LENGTH];
    convertToRelativePath( theCurrentPath.c_str(), myValue.c_str(), relativeFile, DC_MAX_PATH_LENGTH );

    // Save the relative string
    if( !stream->writeString( relativeFile ) ) return false;

    // Success
    return true;
}


//-----------------------------------------------------------------------------
// Name:  load
// Desc:  Loads this resource
//-----------------------------------------------------------------------------
bool dcFileName::load( dcStreamIn* stream )
{
    // Read the relative file name from the file
    std::string relativeFile;
    if( !stream->readString( &relativeFile ) ) return false;

    // Convert this relative file into an absolute path
    char absoluteFile[DC_MAX_PATH_LENGTH];
    absoluteFile[0] = '\0';
    convertToAbsolutePath( theCurrentPath.c_str(), relativeFile.c_str(), absoluteFile, DC_MAX_PATH_LENGTH );

    // Copy the string to our buffer
    myValue.assign( absoluteFile );

    // Success
    return true;
}



//----[  serialize  ]----------------------------------------------------------
void dcFileName::serialize(std::string* string) {
  char relative_path[DC_MAX_PATH_LENGTH];
  convertToRelativePath(theCurrentPath.c_str(),
                        myValue.c_str(),
                        relative_path,
                        DC_MAX_PATH_LENGTH);

  // Save the relative string
  string->assign(relative_path);
}


//----[  interpret  ]----------------------------------------------------------
void dcFileName::interpret(const char* string) {
  char absolute_path[DC_MAX_PATH_LENGTH];
  absolute_path[0] = '\0';
  convertToAbsolutePath(theCurrentPath.c_str(),
                        string,
                        absolute_path,
                        DC_MAX_PATH_LENGTH);

  // Copy the string to our buffer
  myValue.assign(absolute_path);

}



//-----------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Obtains the name of this class type
//-----------------------------------------------------------------------------
std::string dcFileName::staticTypeName()
{
    return "File Name";
}



//-----------------------------------------------------------------------------
// Name:  convertToRelativePath
// Desc:  Converts the absolute path provided into a path relative to the current path
//-----------------------------------------------------------------------------
void dcFileName::convertToRelativePath( const char* currentPath, const char* absolutePath,
                                   char* relativePath, size_t maxPathLen )
{
    // Get the paths into internal buffers
    char mrlPath[DC_MAX_PATH_LENGTH], absolutePathBuffer[DC_MAX_PATH_LENGTH];
    strcpy_s( mrlPath, DC_MAX_PATH_LENGTH, currentPath );
    strcpy_s( absolutePathBuffer, DC_MAX_PATH_LENGTH, absolutePath );

    // Reset the output path
    memset( relativePath, 0, maxPathLen );

    // Look for the first place that these strings don't match
    char* pMRLFirstDifference = mrlPath;
    char* pAbsoluteFirstDifference = absolutePathBuffer;
#if defined(DC_FILENAME_CASELESS)
    while( _tolower(*pMRLFirstDifference) == _tolower(*pAbsoluteFirstDifference) )
#else
    while( *pMRLFirstDifference == *pAbsoluteFirstDifference )
#endif
    {
        pMRLFirstDifference++;
        pAbsoluteFirstDifference++;

        // We're done if we read the terminating character
        if( *pMRLFirstDifference == '\0' )
            break;
        
        // If the absolute path ends, exit the loop
        if( *pAbsoluteFirstDifference == '\0' )
            break;
    }

    // Find out how many directories were missing from the absolute path
    int missingDirectories = -1;
    const char* pMRLCounter = pMRLFirstDifference;
    while( pMRLCounter && (*pMRLCounter != '\0') ) { pMRLCounter++; pMRLCounter = strchr( pMRLCounter, '\\' ); missingDirectories++; }

    // Write a "..\" for each missing directory
    for( int d = 0; d < missingDirectories; ++d )
        strcat_s( relativePath, maxPathLen, "..\\" );

    // Get a pointer to the first directory difference in the absolute path
    while( pAbsoluteFirstDifference > absolutePathBuffer && ((*pAbsoluteFirstDifference) != '\\') )
        pAbsoluteFirstDifference--;

    // If we couldn't find the directory, just use the start of the path
    if( pAbsoluteFirstDifference != absolutePathBuffer )
        pAbsoluteFirstDifference++; // advance past the backslash

    // Append what's left of the buffer
    strcat_s( relativePath, maxPathLen, pAbsoluteFirstDifference );
}


//-----------------------------------------------------------------------------
// Name:  convertToRelativePath
// Desc:  Converts the relative path provided into an absolute path
//-----------------------------------------------------------------------------
void dcFileName::convertToAbsolutePath( const char* currentPath, const char* relativePath,
                                        char* absolutePath, size_t maxPathLen )
{
    // Get the paths into internal buffers
    char mrlPath[DC_MAX_PATH_LENGTH], relativePathBuffer[DC_MAX_PATH_LENGTH];
    strcpy_s( mrlPath, DC_MAX_PATH_LENGTH, currentPath );
    strcpy_s( relativePathBuffer, DC_MAX_PATH_LENGTH, relativePath );

    // For each of the "..\" in the path, go up one directory
    char* pUpDirectory = relativePathBuffer;
    int directoriesUp = 0;
    while( strncmp( pUpDirectory, "..\\", 3 ) == 0 )
    {
        pUpDirectory += 3;
        directoriesUp++;
    }

    // Find our directory
    char* pPath = mrlPath;
    int i = -1;
    do
    {
        // Look for the last directory marker
        pPath = strrchr( mrlPath, '\\' );

        // If we can't find a directory, this is bad
        if( !pPath )
            return;

        *pPath = '\0';
        ++i;
    } while( i < directoriesUp );

    // Print the paths
    sprintf_s( absolutePath, maxPathLen, "%s\\%s", mrlPath, pUpDirectory );
}


//-----------------------------------------------------------------------------
// Name:  setCurrentPath
// Desc:  Sets the current directory for file name resolution
//-----------------------------------------------------------------------------
void dcFileName::setCurrentPath( const char* currentPath )
{
    theCurrentPath.assign( currentPath );
}



//-----------------------------------------------------------------------------
// Name:  setCurrentPathFromFileDirectory
// Desc:  Calls setCurrentPath after removing the file name from the end of the path
//-----------------------------------------------------------------------------
void dcFileName::setCurrentPathFromFileDirectory( const char* fileNameInPath )
{
    // Copy the file name into the temporary path buffer
    char path[DC_MAX_PATH_LENGTH];
    strcpy_s( path, DC_MAX_PATH_LENGTH, fileNameInPath );

    // Get rid of anything after the final backslash (the file name)
    char* lastBackslash = strrchr( path, '\\' );
    if( lastBackslash ) { lastBackslash++; *lastBackslash = '\0'; }

    // Set the path
    setCurrentPath( path );
}

//-----------------------------------------------------------------------------
// Name:  compareValue
// Desc:  Compares the value stored in this class to another value
//-----------------------------------------------------------------------------
int dcFileName::compareValue( const std::string& otherValue ) const
{
    return myValue.compare( otherValue );
}