#include <mulle-thread/mulle-thread.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>


int   main( void)
{
   mulle_thread_recursive_mutex_t   lock;

   printf( "init\n");
   if( mulle_thread_recursive_mutex_init( &lock))
      return( 1);

   // basic lock/unlock
   printf( "lock\n");
   mulle_thread_recursive_mutex_lock( &lock);
   printf( "unlock\n");
   mulle_thread_recursive_mutex_unlock( &lock);

   // trylock when free
   printf( "trylock\n");
   if( mulle_thread_recursive_mutex_trylock( &lock))
      return( 1);
   mulle_thread_recursive_mutex_unlock( &lock);

   // recursive lock (same thread)
   printf( "recursive lock\n");
   mulle_thread_recursive_mutex_lock( &lock);
   mulle_thread_recursive_mutex_lock( &lock);
   mulle_thread_recursive_mutex_unlock( &lock);
   mulle_thread_recursive_mutex_unlock( &lock);

   // recursive trylock (same thread, already locked)
   printf( "recursive trylock\n");
   mulle_thread_recursive_mutex_lock( &lock);
   if( mulle_thread_recursive_mutex_trylock( &lock))
      return( 1);
   mulle_thread_recursive_mutex_unlock( &lock);
   mulle_thread_recursive_mutex_unlock( &lock);

   printf( "done\n");
   if( mulle_thread_recursive_mutex_done( &lock))
      return( 1);

   return( 0);
}
