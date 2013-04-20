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
#ifndef __DCXMANAGEDMEMORYPOOL_H__
#define __DCXMANAGEDMEMORYPOOL_H__


namespace dcx {

template <typename O> class dcxManagedMemoryPoolPointer {
public:
  typedef size_t Hash;

public:
    dcxManagedMemoryPoolPointer() {
      reset();
    }

    // Used by the managed memory pool to initialize a pointer
    dcxManagedMemoryPoolPointer(O* reference, Hash* hash_reference) {
      reference_ = reference;
      hash_reference_ = hash_reference;
      internal_hash_ = *hash_reference;
    }

    inline bool invalid() const {
      return !reference_ || (*hash_reference_ != internal_hash_);
    }

    inline O* dereference() {
      return invalid() ? 0 : reference_;
    }

    inline const O* dereference() const {
      return invalid() ? 0 : reference_;
    }

    inline O* dereferenceAssumingValid() {
      return reference_;
    }

    inline O* dereferenceAssumingValid() const {
      return reference_;
    }

    // Duplicate the source pointer.  Does not reference-count.
    inline void copy(const dcxManagedMemoryPoolPointer& source) {
      internal_hash_ = source.internal_hash_;
      hash_reference_ = source.hash_reference_;
      reference_ = source.reference_;
    }

    // Duplicate the source pointer.  Does not reference-count.
    inline void copy(const dcxManagedMemoryPoolPointer* source) {
      if (source == 0) {
        internal_hash_ = 0;
        hash_reference_ = 0;
        reference_ = 0;
      } else {
        internal_hash_ = source->internal_hash_;
        hash_reference_ = source->hash_reference_;
        reference_ = source->reference_;
      }
    }

    // Invalidates this pointer
    inline void reset() {
      internal_hash_ = 0;
      reference_ = 0;
    }

    // Invalidates all other pointers to this object
    inline void becomeUniquePointer() {
      if (invalid()) return;
      ++internal_hash_;
      *hash_reference_ = internal_hash_;
    }

    // Checks to see if this pointer is the same as another
    inline bool equals(const dcxManagedMemoryPoolPointer& rhs) {
      return reference_ == rhs.reference_ &&
             hash_reference_ == rhs.hash_reference_ &&
             internal_hash_ == rhs.internal_hash_;
    }

private:
  O* reference_;
  Hash* hash_reference_;
  Hash internal_hash_;
  //dcxManagedMemoryPoolPointer& operator =(dcxManagedMemoryPoolPointer&);
  //dcxManagedMemoryPoolPointer(dcxManagedMemoryPoolPointer&);
};

/**
 * A managed memory pool makes it easy to manage a pool of objects where each
 * object in the pool can have many outstanding references that all need to be
 * invalidated when the object is freed.
 * As long as all pointers to objects use the Pointer type defined in this
 * class, when an object is freed all pointers become invalid.
 * This class does not employ reference-counting.
 */
template <typename O, const size_t PoolSize> class dcxManagedMemoryPool {

  typedef size_t Hash;

  /**
   * This type is stored in the memory pool.  It is not available to the
   * external class for use.
   */
  class Object {
  public:

    Object() {
      hash_ = 0;
      changeHash();
    }

    inline O* getObject() const {
      return const_cast<O*>(&object_);
    }

    inline Hash* getHashRef() const {
      return const_cast<Hash*>(&hash_);
    }

    // Changes the hash value that pointers use to validate themselves against
    // this object
    void changeHash() {
      ++hash_;
    }

  private:
    O object_;  // Memory of whatever type the pool is for
    Hash hash_; // The hash value of this object
  };

  typedef dcx::dcxMemoryPool<Object,PoolSize> ObjectMemoryPool;


public:

  // Allows iteration through the pool of objects
  class Iterator {
  public:
    Iterator(dcxManagedMemoryPool* pool) :
        iterator(&pool->object_pool_) {
    }

    const O* get() const { return iterator.get()->getObject(); }
    O* get() { return iterator.get()->getObject(); }
    bool hasNext() const { return iterator.hasNext(); }
    void advance() { iterator.advance(); }
    void remove() { remove(); }

  private:
    typename ObjectMemoryPool::Iterator iterator;
  };

  typedef dcxManagedMemoryPoolPointer<O> Pointer;


public:

  dcxManagedMemoryPool() {
  }

  // Set up the memory pool for use.  Must be called before acquire().  The
  // calling program must satisfy the preconditions for destroy().
  bool create() {
    return object_pool_.create();
  }

  // Free memory used by this pool.  All pointers must be reset before this
  // method is called, or the program must be sure not to call any methods
  // on those pointers except reset() as they will cause memory access
  // faults.
  void destroy() {
    object_pool_.destroy();
  }

  // Get a new pointer to a managed memory object
  bool acquire(Pointer* pointer) {
    //if (!pointer) return false;
    Object* object = object_pool_.acquire();
    if (!object) return false;
    Pointer local_pointer(object->getObject(), object->getHashRef());
    pointer->copy(local_pointer);
    return true;
  }

  // Free the managed memory object that a pointer references.  If the pointer
  // is invalid, this method does nothing.  If the method succeeds, all
  // pointers referencing the same object become invalid.
  void release(Pointer* pointer) {
    //if (!pointer) return;
    Object* object = reinterpret_cast<Object*>(pointer->dereference());
    if (!object) return;
    object->changeHash();
    pointer->reset();
    object_pool_.release(object);
  }

  // Turns a raw object pointer into a hash-managed pointer type.  Use with
  // caution.
  void convert(O* object, Pointer* pointer) {
    if (!pointer) return;
    size_t index = object_pool_.indexOf(reinterpret_cast<Object*>(object));
    Object* managed_object = object_pool_.get(index);
    if (managed_object == NULL) {
      pointer->reset();
    } else {
      Pointer local_pointer(managed_object->getObject(), managed_object->getHashRef());
      pointer->copy(local_pointer);
    }
  }

  // Returns the index referenced by the pointer.  If the pointer is invalid, this
  // method returns (size_t)(-1)
  size_t indexOf(const Pointer* pointer) {
    if (pointer == NULL) return (size_t)(-1);
    Object* object = reinterpret_cast<Object*>(pointer->dereferenceAssumingValid());
    return object_pool_.indexOf(object);
  }

  size_t numberOfAllocatedEntries() {
    return object_pool_.numberOfAllocatedEntries();
  }

  size_t numberOfFreeEntries() {
    return object_pool_.numberOfFreeEntries();
  }

private:

  // Holds the set of objects to which references are made
  ObjectMemoryPool object_pool_;
};


}


#endif