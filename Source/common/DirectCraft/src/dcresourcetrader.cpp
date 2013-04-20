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
#include <map>
#include "../dc/dcinternal_begin.h"
#include "../dc/dcresourcetrader.h"
#include "../dc/dcinterface.h"
#include "../dc/dccollection.h"
#include "../dc/dcbufferstream.h"


// TODO: importing is not yet secured!!  buffers are created that are not sanity checked, etc...
// should put a type key before all strings, including name strings, so that we can be sure that
// the data coming next is actually a string....


using namespace dc;

/// Upper limit on the number of members to help make sure resources are not corrupted
/// TODO: put this in a dcconstants.h file
#define DC_MAX_RESOURCE_MEMBERS  65535



//-----------------------------------------------------------------------------
// Name: dcResourceTrader
// Desc: Initializes the class
//-----------------------------------------------------------------------------
dcResourceTrader::dcResourceTrader()
{
    // Reset internal values
    myOverwriteCallback = 0;
    myContext = 0;

    // Get all of the managers for this application
    std::list<dcResourceManager*> managers;
    dcInterface<dcGenericResource>::getSubclassManagerList( &managers );

    // Add each type to the internal types map
    for( std::list<dcResourceManager*>::iterator i = managers.begin(); i != managers.end(); ++i )
        myProgramTypeManagers.insert( ResourceManagers::value_type( (*i)->uniqueID(), *i ) );

}


//-----------------------------------------------------------------------------
// Name: exportTo
// Desc: Exports the provided resource into the output stream
//-----------------------------------------------------------------------------
bool dcResourceTrader::exportTo( dcGenericResource* rootResource, dcStreamOut* stream )
{
    // Get all of the managers for this application
    std::list<dcResourceManager*> managers;
    dcInterface<dcGenericResource>::getSubclassManagerList( &managers );

    // Write the number of types this resource contains
    size_t types = managers.size();
    if( !stream->write( &types, sizeof(types) ) ) return false;

    // Write each type of resource into the file
    for( std::list<dcResourceManager*>::iterator i = managers.begin(); i != managers.end(); ++i )
    {
        // Write this resource's type ID
        unsigned long type = (*i)->uniqueID();
        if( !stream->write( &type, sizeof(type) ) ) return false;

        // Write the name of this manager
        if( !stream->writeString( (*i)->typeName() ) ) return false;
    }

    // Export this resource into the stream
    return exportRecursive( rootResource, stream );
}


//-----------------------------------------------------------------------------
// Name: importFrom
// Desc: Imports a resource from the stream
//-----------------------------------------------------------------------------
bool dcResourceTrader::importFrom( dcGenericResource* rootResource, dcStreamIn* stream )
{
    // Clear import variables
    myErrors.clear();
    myWarnings.clear();
    myUnresolvedImportTypes.clear();
    myImportSourceTypes.clear();

    // Read the number of types from the file
    size_t types = 0;
    if( !stream->read( &types, sizeof(types) ) ) return false;

    // Read each type declaration
    for( size_t i = 0; i < types; ++i )
    {
        // Read this type ID
        unsigned long type;
        if( !stream->read( &type, sizeof(type) ) ) return false;

        // Load the name of this type
        std::string name;
        if( !stream->readString( &name, 65535 ) ) return false;

        // TODO: we could do a type conversion here in the future...
        bool typeTranslated = false;

        // Look up this type internally
        ResourceManagers::iterator manager = myProgramTypeManagers.find( type );
        if( manager == myProgramTypeManagers.end() )
        {
            // This is an unresolved type
            if( typeTranslated )
                myWarnings.push_back( std::string( "\"" ) + name + "\" was translated to a non-existant type; is this intentional?" );
            else
                myUnresolvedImportTypes.insert( ResourceTypes::value_type( type, name ) );
        }
        else
        {
            // This type could be found OK; make sure that if the type wasn't translated explicitly
            // that the type names match (otherwise we might have a type collision!)
            if( !typeTranslated && (0 != manager->second->typeName().compare( name )) )
                myWarnings.push_back( std::string( "\"" ) + name + "\" collides with \"" + manager->second->typeName() + "\"" );
        }

        // Add to the collection of source types
        myImportSourceTypes.insert( ResourceTypes::value_type( type, name ) );
    }

    // Read the root resource's type ID
    unsigned long type;
    if( !stream->read( &type, sizeof(type) ) ) return false;

    // Load the name of the root resource
    std::string rootName;
    if( !stream->readString( &rootName ) ) return false;

    // Get the name of this resource
    std::string importTypeName;
    ResourceTypes::iterator sourceType = myImportSourceTypes.find( type );
    if( sourceType == myImportSourceTypes.end() )
    {
        // This type is invalid
        importTypeName = "<invalid>";

        // Build a string for the type ID
        char typeString[16];
        sprintf_s( typeString, sizeof(typeString), "0x%08X", type );

        // Output an error; this shouldn't happen
        myErrors.push_back( std::string("Root resource's type (") + typeString + ") wasn't exported correctly!"\
                            "  The source may be corrupted." );
    }
    else
    {
        // Set the type name
        importTypeName = sourceType->second;
    }

    // Make sure the context is empty
    myOperationContext.clear();

    // Check the type ID to see if it's the same
    if( type != rootResource->getTypeID() )
        myWarnings.push_back( "Type ID of the root resource doesn't match the type being imported" \
                              "(importing \"" + importTypeName + "\" as \"" + rootResource->getTypeName()
                              + "\"" );

    // Read the byte size of this resource
    size_t byteSize = 0;
    if( !stream->read( &byteSize, sizeof(byteSize) ) ) return false;

    // TODO: Print the number of bytes to myInfo
    // laters...

    // Import the children of this resource
    return importRecursive( rootResource, stream );
}


//-----------------------------------------------------------------------------
// Name:  merge
// Desc:  Merges two resources together
//-----------------------------------------------------------------------------
void dcResourceTrader::merge( dcGenericResource* into, dcGenericResource* from,
                              OverwriteCallback overwriteCallback, void* context )
{
    // Save the overwrite callback and context variables
    myOverwriteCallback = overwriteCallback;
    myContext = context;

    // Set the current state
    myLastOperation = OPERATION_MERGE;

    // Merge the resources
    mergeRecursive( into, from );
}



//-----------------------------------------------------------------------------
// Name:  getErrors
// Desc:  Gets the error output that was generated during an import/export operation
//-----------------------------------------------------------------------------
const dcResourceTrader::Errors& dcResourceTrader::getErrors()
{
    return myErrors;
}



//-----------------------------------------------------------------------------
// Name:  getWarnings
// Desc:  Gets the warning output that was generated during an import/export operation
//-----------------------------------------------------------------------------
const dcResourceTrader::Warnings& dcResourceTrader::getWarnings()
{
    return myWarnings;
}



//-----------------------------------------------------------------------------
// Name:  getUnresolvedImportTypes
// Desc:  Obtains the resource types that couldn't be imported to a resource class
//-----------------------------------------------------------------------------
const dcResourceTrader::ResourceTypes& dcResourceTrader::getUnresolvedImportTypes()
{
    return myUnresolvedImportTypes;
}



//-----------------------------------------------------------------------------
// Name:  generateLogFile
// Desc:  Writes all of the errors/warnings that occured during the last operation
//-----------------------------------------------------------------------------
void dcResourceTrader::generateLogFile( std::string* logFile ) const
{
    // Generate a temporary file name
    char fileName[512];
    tmpnam_s( fileName, sizeof(fileName) );
    logFile->assign( fileName );

    // Open a log file
    FILE* pFile;
    fopen_s( &pFile, fileName, "w+t" );

    // Start the log
    fprintf_s( pFile, "%lu Errors - %lu Warnings", myErrors.size(), myWarnings.size() );

    // Errors section
    fprintf_s( pFile, "\n\n\nErrors:\n\n" );

    // Keeps track of indices
    unsigned int index = 0;

    // Write all of the errors
    for( Errors::const_iterator e = myErrors.begin(); e != myErrors.end(); ++e, ++index )
        fprintf_s( pFile, "%03i:   %s\n", index, e->c_str() );

    // Warnings section
    fprintf_s( pFile, "\n\n\nWarnings:\n\n" );

    // Reset the index counter
    index = 0;

    // Write all of the warnings
    for( Warnings::const_iterator w = myWarnings.begin(); w != myWarnings.end(); ++w, ++index )
        fprintf_s( pFile, "  %04i:   %s\n", index, w->c_str() );

    // Check to see if we had unresolved import types
    if( !myUnresolvedImportTypes.empty() )
    {
        // Warn about types that weren't converted correctly
        fprintf_s( pFile, "\n\n\nThe following types were not recognized:\n\n" );

        // Write all of the unknown types
        for( ResourceTypes::const_iterator i = myUnresolvedImportTypes.begin(); i != myUnresolvedImportTypes.end(); ++i )
            fprintf_s( pFile, "  0x%08X    %s\n", i->first, i->second.c_str() );
    }

    // If we imported stuff, print the import list
    if( !myImportSourceTypes.empty() )
    {
        // Let the user know which types existed in the source file
        fprintf_s( pFile, "\n\n\nThe following types were found in the import file:\n\n" );

        // Write all of the types in the source file
        for( ResourceTypes::const_iterator i = myImportSourceTypes.begin(); i != myImportSourceTypes.end(); ++i )
            fprintf_s( pFile, "  0x%08X    %s\n", i->first, i->second.c_str() );
    }

    // Write the list of known program types
    fprintf_s( pFile, "\n\n\nThese resource types are used by this application:\n\n" );

    // Print all of the types
    for( ResourceManagers::const_iterator i = myProgramTypeManagers.begin();
         i != myProgramTypeManagers.end(); ++i )
         fprintf_s( pFile, "  0x%08X    %s\n", i->first, i->second->typeName().c_str() );

    // Close the output file
    fclose( pFile );
}


//-----------------------------------------------------------------------------
// Name:  printAllResourceTypes
// Desc:  Writes all of the resource types in the program as text to the output string
//-----------------------------------------------------------------------------
void dcResourceTrader::printAllResourceTypes( std::string* output )
{
    // Get all of the managers for this application
    std::list<dcResourceManager*> managers;
    dcInterface<dcGenericResource>::getSubclassManagerList( &managers );

    // This structure will hold all of the entries
    std::map<std::string,unsigned long> types;

    // Add each type to the map so that they are sorted
    for( std::list<dcResourceManager*>::iterator i = managers.begin(); i != managers.end(); ++i )
        types.insert( std::pair<std::string,unsigned long>( (*i)->typeName(), (*i)->uniqueID() ) );

    // Print each one into the string
    for( std::map<std::string,unsigned long>::iterator j = types.begin(); j != types.end(); ++j )
    {
        // Build a string for the type ID
        char typeString[16];
        sprintf_s( typeString, sizeof(typeString), "0x%08X", j->second );

        // Append to the output
        output->append( std::string( typeString ) + "    " + j->first + "\n" );
    }
}


//-----------------------------------------------------------------------------
// Name:  exportRecursive
// Desc:  
//-----------------------------------------------------------------------------
bool dcResourceTrader::exportRecursive( dcGenericResource* resource, dcStreamOut* stream )
{
    // Save this resource's type ID
    unsigned long type = resource->getTypeID();
    if( !stream->write( &type, sizeof(type) ) ) return false;

    // Save the name of this resource
    if( !stream->writeString( resource->getName() ) ) return false;

    // Make sure there is room for the byte length by saving a temporary value
    size_t byteLength = 0;
    if( !stream->write( &byteLength, sizeof(byteLength) ) ) return false;

    // Figure out where we are in the file before writing this resource into the stream
    size_t begin = stream->tell();

    // Get the members of this resource
    const dcGenericResource::Array& members = resource->getMembers();

    // Write the number of members of this resource
    size_t memberCount = members.size();
    if( !stream->write( &memberCount, sizeof(memberCount) ) ) return false;

    // Check to see whether we should write children
    if( memberCount > 0 )
    {
        // Export all of the child resources
        for( dcGenericResource::Array::const_iterator i = members.begin(); i != members.end(); ++i )
            exportRecursive( *i, stream );
    }
    else
    {
        // Just save this resource into the stream
        if( !resource->save( stream ) ) return false;
    }

    // Save the current location
    size_t end = stream->tell();

    // Write the byte length of the entry so that it can be skipped if the type isn't
    // recognized during a load.
    byteLength = (size_t)(end - begin);
    if( !stream->seek( begin - sizeof(byteLength) ) ) return false;
    if( !stream->write( &byteLength, sizeof(byteLength) ) ) return false;

    // Move the stream back to the current write location
    if( !stream->seek( end ) ) return false;

    // Success
    return true;
}



//-----------------------------------------------------------------------------
// Name:  importRecursive
// Desc:  Helper method used internally to import a resource
//-----------------------------------------------------------------------------
bool dcResourceTrader::importRecursive( dcGenericResource* resource, dcStreamIn* stream )
{
    // Add to the context
    myOperationContext.push_back( resource->getName() + "<" + resource->getTypeName() + ">" );

    // Read the number of members this resource has
    size_t memberCount = 0;
    if( !stream->read( &memberCount, sizeof(memberCount) ) ) return false;

    // Depending on whether or not there are members, load this resource
    if( memberCount == 0 )
    {
        if( resource->isCollection() || resource->isInterface() )
        {
            // Warn that there are no members
            myWarnings.push_back( getOperationContext() + ":  " + "\"" + resource->getPathString() + "\" has no members" );

            // Read!  If this fails, just let the super-class take care of it
            if( !resource->load( stream ) )
            {
                // Something might have changed
                myWarnings.push_back( getOperationContext() + ":  " + "\"" + resource->getPathString() + "\" failed to read an empty member list" );
            }
        }
        else
        {
            // Read this resource from the stream
            if( !resource->load( stream ) )
            {
                // Warn about this; it's not an error because the calling method knows how many
                // bytes should be read, and if that doesn't happen, it scans to the right place
                // in the stream automatically.
                myWarnings.push_back( getOperationContext() + ":  Couldn't load resource \"" + resource->getPathString() + "\"" );
            }
        }
    }
    else
    {
        // Sanity check on the member count
        if( memberCount > DC_MAX_RESOURCE_MEMBERS )
        {
            myErrors.push_back( getOperationContext() + ":  \"" + resource->getPathString() + "\" has too many members!" );
            return true;
        }

        // Check to make sure this is correct
        if( resource->isInterface() )
        {
            // Spit out a warning if this is the case
            if( memberCount > 1 )
            {
                // This is an error!  Only read one member.
                myErrors.push_back( getOperationContext() + ":  \"" + resource->getPathString() +
                                    "\" appears to be an interface of type \""
                                    + resource->getTypeName() + "\", but the file specifies that it has "\
                                    "more than one member.  Only the first member of the correct type "\
                                    "will be interpreted." );
            }
        }
        else
        {
            // If this is also not a collection, make sure the number of members match
            if( false == resource->isCollection() && memberCount != resource->getMembers().size() )
            {
                // Warn about this case (it's not really a problem, though)
                myWarnings.push_back( getOperationContext() + ":  \"" + resource->getPathString() +
                                      "\" has a different number of members than are" \
                                      " present in the import stream.  Some members will not be loaded." );
            }
        }

        // Read each of the members
        for( size_t m = 0; m < memberCount; ++m )
        {
            // Read the resource's type ID
            unsigned long type;
            if( !stream->read( &type, sizeof(type) ) ) return false;

            // Load the name of the resource
            std::string name;
            if( !stream->readString( &name ) ) return false;

            // Read the byte size of this resource
            size_t byteSize = 0;
            if( !stream->read( &byteSize, sizeof(byteSize) ) ) return false;

            // Get the location of the start of the stream
            size_t begin = stream->tell();
            size_t end = begin + byteSize;

            // Find this resource type in the application list
            ResourceManagers::iterator typeManager = myProgramTypeManagers.find( type );
            if( typeManager == myProgramTypeManagers.end() )
            {
                // Output a warning
                myWarnings.push_back( getOperationContext() + ":  Resource \"" + name +
                                      "\" cannot be loaded because its type could not be deduced" );

                // Skip forward in the stream
                if( !stream->move( (int)byteSize ) ) return false;
            }
            else
            {
                // Get the manager implementation for this type
                dcResourceManager* manager = typeManager->second;

                // Allocate an instance of this class
                dcGenericResource* temporary = manager->generate();

                // Find the resource to merge this child with
                // TODO: change parameter 2 to accept const std::string&
                dcGenericResource* member = obtainMemberLike( resource, name.c_str(), type );

                // If a member could be found, merge the one we just loaded into it
                if( member )
                {
                    // Set the name of the temporary member
                    temporary->setName( member->getName() );

                    // Read this class
                    bool importSucceeded = importRecursive( temporary, stream );

                    // Exit the context
                    myOperationContext.pop_back();

                    // Check to see whether or not this was successful
                    if( false == importSucceeded )
                    {
                        // Free the resource
                        dcResourceManager::deallocate( temporary );

                        // Return the error
                        return false;
                    }

                    // Merge the resource we just read into the member that was found
                    dcBufferStream stream;
                    temporary->save( &stream );
                    stream.seek( 0 );
                    member->load( &stream );

                    // If the parent type is an interface, break the loop (this should
                    // occur naturally by there only being 1 member, but this makes
                    // absolutely sure)
                    if( resource->isInterface() ) break;
                }
                else
                {
                    // Warn the user about this import
                    myWarnings.push_back( getOperationContext() + ":  Member \"" + name + "\" of " + resource->getPathString() + " can't be imported" );
                }
            }

            // Make sure that the correct number of bytes were read
            if( end != stream->tell() )
            {
                // Warn about this problem.  The most likely cause would be that a basic
                // resource member (on which save() is called) has changed the number of
                // bytes that it takes when it saves
                myWarnings.push_back( getOperationContext() + ":  Incorrect number of bytes were read; one or more "\
                                      "members of \"" + resource->getPathString() + "\" may be corrupted/incomplete" );
                stream->seek( end );
            }
        }
    }

    // Success
    return true;
}




//-----------------------------------------------------------------------------
// Name:  merge
// Desc:  Merges two resources together
//-----------------------------------------------------------------------------
void dcResourceTrader::mergeRecursive( dcGenericResource* into, dcGenericResource* from )
{
    if( into->isCollection() )
    {
        // Can only merge entries of the same type
        if( into->getTypeID() != from->getTypeID() )
            myErrors.push_back( "Can't merge collections of different types" );
    
        // Get the collection types
        dcGenericCollection* intoCollection = (dcGenericCollection*)into;
        dcGenericCollection* fromCollection = (dcGenericCollection*)from;
        
        // Sort the collections by name
        intoCollection->sortByName();
        fromCollection->sortByName();

        // Collection end interators
        dcGenericResource::Array::const_iterator intoEnd = intoCollection->getMembers().end();
        dcGenericResource::Array::const_iterator fromEnd = fromCollection->getMembers().end();
        
        // Go through the collection and merge members with the same name
        dcGenericResource::Array::const_iterator intoIterator = intoCollection->getMembers().begin();
        dcGenericResource::Array::const_iterator fromIterator = fromCollection->getMembers().begin();

        // Try to merge any pair of elements that have the same name
        while( fromIterator != fromEnd )
        {
            // Determines what to do with the source element
            int comparison;

            // Get the element from the source collection
            dcGenericResource* fromElement = (dcGenericResource*)*fromIterator;
            dcGenericResource* intoElement = (intoIterator == intoEnd) ? 0 : (*intoIterator);

            // If the destination element is valid, compare its name with the source one; otherwise,
            // compare the names to each other.
            if( intoElement == 0 )
                comparison = +1;
            else
                comparison = intoElement->getName().compare( fromElement->getName() );

            // Move the lower iterator up
            if( comparison < 0 ) intoIterator++;
            else if( comparison > 0 )
            {
                // This element name exists in the source collection but not the destination one
                dcGenericResource* mergeInto = 0;

                // If we can't duplicate element values, then search the collection for an element that
                // has the same value as the one we're trying to insert.  If it's found, set it up as
                // the "mergeInto" resource.
                if( intoCollection->allowDuplicates() == false )
                {
                    // Look through the destination members to see if another member has the same value
                    dcGenericResource::Array::const_iterator intoSearch;
                    for( intoSearch = intoCollection->getMembers().begin();
                         intoSearch != intoEnd; ++intoSearch )
                    {
                        // Check to see whether or not these values match
                        if( (*intoSearch)->compareTo( fromElement ) == 0 )
                        {
                            // Set the resource to merge into
                            mergeInto = *intoSearch;
                        
                            // Exit the loop
                            break;
                        }
                    }
                }

                // If an element already in the collection into which to merge wasn't found,
                // just use the collection to create a new element and use that one.
                if( mergeInto == 0 )
                {
                    // Create a new element.  At this point, we know that the fromElement isn't in the
                    // collection either by name or by value, so we can safely overwrite whatever
                    // is generated by this method.
                    mergeInto = intoCollection->generateElement( fromElement->getName() );

                    // Reset the destination iterators, because the collection changed.
                    // TODO: this shouldn't be necessary...it really slows down the insert process
                    intoEnd = intoCollection->getMembers().end();
                    intoIterator = intoCollection->getMembers().begin();

                    // Overwrite the element.  We don't merge because the source element may
                    // have data that isn't valid
                    dcBufferStream stream;
                    fromElement->save( &stream );
                    stream.seek( 0 );
                    mergeInto->load( &stream );
                }
                else
                {
                    // Merge into the element that was found
                    mergeRecursive( mergeInto, fromElement );
                }

            
                // Increment the source iterator
                fromIterator++;
            }
            else
            {
                // The names are equal; merge these elements
                mergeRecursive( intoElement, fromElement );

                // Advance past this element in both collections
                intoIterator++;
                fromIterator++;
            }
        }
    }
    else if( into->isInterface() )
    {
        // Get the interface types
        dcGenericInterface* intoInterface = (dcGenericInterface*)into;
        dcGenericInterface* fromInterface = (dcGenericInterface*)from;
        
        // Check to see if the implementation types are the same
        if( intoInterface->isImplemented() )
        {
            // Merge the source interface with this interface.  If the source interface isn't
            // implemented, we don't have to do anything!
            if( fromInterface->isImplemented() )
                mergeRecursive( intoInterface->getGenericImplementation(), fromInterface->getGenericImplementation() );
        }
        else
        {
            // Copy the source interface (if it exists)
            if( fromInterface->isImplemented() )
            {
                // The destination interface has no implementation, so we can just
                // copy the implementation from the source type
                dcGenericResource* implementation = intoInterface->implementByType( fromInterface->getImplementationType() );
                
                // If the implementation couldn't be created, then these interfaces can't be merged.  We
                // hold off on type-checking until now because a sub-interface type can be merged into
                // a higher interface type.  For example, dcInterface<dcNumber> could be merged into
                // dcInterface<dcGenericResource>.
                if( implementation )
                {
                    // Use a buffer to copy the source
                    dcBufferStream stream;
                    fromInterface->getGenericImplementation()->save( &stream );
                    stream.seek( 0 );
                    implementation->load( &stream );
                }
                else
                {
                    // The implementation couldn't be created.  Warn the user about this.
                    myErrors.push_back( "ERROR MERGING INTERFACES" );
                }
            }
        }
    }
    else
    {
        // Can only merge entries of the same type
        if( into->getTypeID() != from->getTypeID() )
            myErrors.push_back( "\"" + into->getPathString() + "\"/\"" + from->getPathString() + "\" - Can't merge non-collection, non-interface resources of different types" );

        // Get the members of each resource
        const dcGenericResource::Array& intoMembers = into->getMembers();
        const dcGenericResource::Array& fromMembers = from->getMembers();

        // These are the same type, so they should have the same number of members
        if( intoMembers.size() != fromMembers.size() )
        {
            // This is an error
            myErrors.push_back( "\"" + into->getPathString() + "\"/\"" + from->getPathString() + "\" - Can't merge non-collection, non-interface resources with different numbers of members" );

            // Exit
            return;
        }
        
        // If the members exist, merge them; if not, compare these resources
        if( intoMembers.empty() )
        {
            if( 0 != into->compareTo( from ) )
            {
                // Run the callback (if it was set).  If the callback doesn't exist, keep all of the current data.
                if( (myOverwriteCallback != 0) && (*myOverwriteCallback)( into, from, myContext ) )
                {
                    // Use save/load to copy the new resource in
                    dcBufferStream stream;
                    if( from->save( &stream ) )
                    {
                        // Only load if the resource was saved correctly
                        stream.seek( 0 );
                        if( !into->load( &stream ) )
                        {
                            // Let the user know about the error
                            myWarnings.push_back( "\"" + into->getPathString() + "\" failed to copy correctly" );
                        }
                    }
                }
                else
                {
                    // Print into the log!
                    //myLog += "[INFO]  " + into->getPathString() + " = <" + into->toString() + "> was not overwritten by " +
                    //                      from->getPathString() + " = <" + from->toString() + ">";
                }
            }
        }
        else
        {
            dcGenericResource::Array::const_iterator intoMember = intoMembers.begin();
            dcGenericResource::Array::const_iterator fromMember = fromMembers.begin();
            dcGenericResource::Array::const_iterator intoEnd = intoMembers.end();
            dcGenericResource::Array::const_iterator fromEnd = fromMembers.end();

            // Merge all of the resources together
            for( ; intoMember != intoEnd && fromMember != fromEnd; ++intoMember, ++fromMember )
                mergeRecursive( *intoMember, *fromMember );
        }
    }
}



//-----------------------------------------------------------------------------
// Name:  obtainMemberLike
// Desc:  Finds the most likely member of the parent resource that is synonomous with the search
//-----------------------------------------------------------------------------
dcGenericResource* dcResourceTrader::obtainMemberLike( dcGenericResource* parent, const char* name, unsigned long type ) const
{
    // Check the resource's type
    if( parent->isCollection() )
    {
        // Obtain a collection pointer
        dcGenericCollection* parentCollection = (dcGenericCollection*)parent;

        // Create an element in this collection with the same name
        dcGenericResource* member = parentCollection->generateElement( name );

        // Check to make sure the types are the same
        if( member->getTypeID() != type )
        {
            // Get rid of this member
            parentCollection->releaseElement( member );

            // We couldn't find anything
            return 0;
        }
        else
            return member;
    }
    else if( parent->isInterface() )
    {
        // Obtain an interface pointer
        dcGenericInterface* parentInterface = (dcGenericInterface*)parent;

        // Create an implementation
        return parentInterface->implementByType( type );
    }
    else // this is a property-containing resource
    {
        // Copy all of the members of the parent
        dcGenericResource::Array members( parent->getMembers() );

        for( size_t index = members.size(); index > 0; --index )
        {
            if( members.at(index-1)->getTypeID() != type )
            {
                // Get an iterator here
                dcGenericResource::Array::iterator erase = members.begin();
                for( size_t i = 1; i < index; ++i, ++erase );
                members.erase( erase );
            }
        }

        // If there is only one entry of this type, return it
        if( members.size() == 1 ) return members.front();

        // The name of the resource we're looking for
        //const std::string& searchName = search->getName();

        // Look through the list for names that match the search name exactly
        for( dcGenericResource::Array::iterator j = members.begin(); j != members.end(); ++j )
        {
            // Check the name
            if( 0 == (*j)->getName().compare( name ) )
            {
                // It matches!  Yay!
                return *j;
            }
        }

        // We couldn't find anything
        return 0;
    }
}



//-----------------------------------------------------------------------------
// Name:  getOperationContext
// Desc:  Builds a string form of the operation context list
//-----------------------------------------------------------------------------
std::string dcResourceTrader::getOperationContext() const
{
    // The context string we're constructing
    std::string context = "";
    
    // Go through the context list
    for( std::list<std::string>::const_iterator i = myOperationContext.begin();
         i != myOperationContext.end(); ++i )
    {
        // Add the slash to the context, unless we're at the very start
        if( i != myOperationContext.begin() )
            context.append( "/" );

        // Add this entry
        context.append( "[" + *i + "]" );
    }

    // Return the string we built
    return context;
}