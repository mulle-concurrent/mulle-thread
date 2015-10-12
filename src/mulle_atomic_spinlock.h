//
//  mulle_atomic_spinlock.h
//  mulle-thread
//
//  Created by Nat! on 09/10/15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
//

#ifndef mulle_atomic_spinlock_h
#define mulle_atomic_spinlock_h

# pragma mark -
# pragma mark spinlock building on primitives


// yay a small spinlock...
typedef mulle_atomic_ptr_t   mulle_spinlock_t;

static inline void   _mulle_spinlock_lock( mulle_spinlock_t *adress)
{
   while( ! _mulle_atomic_compare_and_swap_pointer( adress, (void *) 1, (void *) 0));
}


static inline void   _mulle_spinlock_unlock( mulle_spinlock_t *adress)
{
   assert( _mulle_atomic_read_pointer( adress) == (void *) 1);
   adress->_nonatomic = (void *) 0;
}

#endif /* mulle_atomic_spinlock_h */
