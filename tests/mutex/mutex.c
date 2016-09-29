#include <mulle_thread/mulle_thread.h>
#include <stdio.h>


int   main( void)
{
   mulle_thread_mutex_t  lock;

   printf( "init\n");
   if( mulle_thread_mutex_init( &lock))
      return( 1);

   printf( "lock\n");
   if( mulle_thread_mutex_lock( &lock))
      return( 1);

   printf( "trylock\n");
   switch( mulle_thread_mutex_trylock( &lock))
   {
   case -1 :
      return( 1);

   case 0 :
      printf( "mistake\n");
      return( 2);
   }

   printf( "unlock\n");
   if( mulle_thread_mutex_unlock( &lock))
      return( 1);

   printf( "trylock\n");
   switch( mulle_thread_mutex_trylock( &lock))
   {
   case -1 :
      return( 1);

   case 1 :
      printf( "mistake\n");
      return( 2);
   }

   printf( "unlock\n");
   if( mulle_thread_mutex_unlock( &lock))
      return( 1);

   printf( "done\n");
   if( mulle_thread_mutex_done( &lock))
      return( 1);

   return( 0);
}
