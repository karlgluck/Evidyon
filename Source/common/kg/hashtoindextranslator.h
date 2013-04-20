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
#ifndef __COMMON_KG_KG_H__
#error "must be included by kg.h"
#endif

#include <hash_map>
#include <vector>

namespace kg {


  
//----[  HashToIndexTranslator  ]----------------------------------------------
template <typename _Hash, typename _Index>
    class CLASSNAME(HashToIndexTranslator) {
  typedef _Hash Hash;
  typedef _Index Index;

  typedef stdext::hash_map<Hash,Index> HashToIndexMap;
  typedef std::vector<Hash> IndexToHashTable;

public:

  void addIndexHashPair(Index index, Hash hash) {
    hash_to_index_map_.insert(HashToIndexMap::value_type(hash,index));
    assert(index_to_hash_table_.size() == index);
    index_to_hash_table_.reserve(index+1);
    index_to_hash_table_.push_back(hash);
  }

#if defined(SAFE)
  bool hashOf(Index index, Hash* hash) const {
    confirm(index_to_hash_table_.size() > index) else return false;
    *hash = index_to_hash_table_[index];
    return true;
  }
  bool indexOf(Hash hash, Index* index) const {
    HashToIndexMap::const_iterator i = hash_to_index_map_.find(hash);
    confirm(i != hash_to_index_map_.end()) else return false;
    *index = i->second;
    return true;
  }
#endif
#if defined(NOTSAFE)
  Hash hashOf(Index index) const {
    return index_to_hash_table_[index];
  }
  Index indexOf(Hash hash) const {
    HashToIndexMap::const_iterator i = hash_to_index_map_.find(hash);
    assert(i != hash_to_index_map_.end());
    return i->second;
  }
#endif

  bool findHash(Index index, Hash* hash) const {
    if (index_to_hash_table_.size() > index) return false;
    *hash = index_to_hash_table_[index];
    return true;
  }

  bool findIndex(Hash hash, Index* index) const {
    HashToIndexMap::const_iterator i = hash_to_index_map_.find(hash);
    if(i == hash_to_index_map_.end()) return false;
    *index = i->second;
    return true;
  }

private:
  HashToIndexMap hash_to_index_map_;
  IndexToHashTable index_to_hash_table_;
};

}
