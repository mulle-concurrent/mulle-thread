#include <mulle-thread/mulle-thread.h>
#include <stdio.h>


static mulle_thread_rval_t   thread_main( void *arg)
{
   if( arg != (void *) 0x1847)
      printf( "mistaken arg\n");

   printf( "thread done\n");
   return( (mulle_thread_rval_t) 1848);
}


//
// destruktor doesn't run in current thread, which is not neccessarily a
// pthread (?)
//
// parameters different to pthreads!

static void   call_me_once( void)
{
   printf( "thread once\n");
}


int   main( void)
{
   mulle_thread_t               thread;
   mulle_thread_rval_t          rval;

   static mulle_thread_once_t   once = MULLE_THREAD_ONCE_INIT;

   mulle_thread_once( &once, call_me_once);
   mulle_thread_once( &once, call_me_once);

   printf( "thread start\n");
   if( mulle_thread_create( thread_main, (void *) 0x1847, &thread))
      return( 1);
   rval = mulle_thread_join( thread);
   if( rval == (mulle_thread_rval_t) -1)
      return( 1);
   if( rval != (mulle_thread_rval_t) 1848)
   {
      printf( "wrong return value\n");
      return( 1);
   }

   printf( "thread joined\n");

   return( 0);
}
