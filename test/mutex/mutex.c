#include <mulle-thread/mulle-thread.h>
#include <stdio.h>


int   main( void)
{
   mulle_thread_mutex_t  lock;
   int                   rval;

   printf( "init\n");
   if( mulle_thread_mutex_init( &lock))
      return( 1);

   printf( "lock\n");
   if( mulle_thread_mutex_lock( &lock))
      return( 1);

   printf( "trylock\n");
   rval = mulle_thread_mutex_trylock( &lock);
   if( ! rval)
   {
      printf( "mistake\n");
      return( 2);
   }

   if( rval != EBUSY)
      return( 1);

   printf( "unlock\n");
   if( mulle_thread_mutex_unlock( &lock))
      return( 1);

   printf( "trylock\n");
   rval = mulle_thread_mutex_trylock( &lock);
   if( rval == EBUSY)
   {
      printf( "mistake\n");
      return( 2);
   }

   if( rval)
      return( 1);

   printf( "unlock\n");
   if( mulle_thread_mutex_unlock( &lock))
      return( 1);

   printf( "done\n");
   if( mulle_thread_mutex_done( &lock))
      return( 1);

   return( 0);
}
