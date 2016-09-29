#include <mulle_thread/mulle_thread.h>
#include <stdio.h>

static char                 *shared_value  = "VfL Bochum 1848";
static char                 *shared_value2 = "VfL Bochum";
static mulle_thread_tss_t   key;


static void   destruktor( void *p)
{
   if( p != shared_value2)
      printf( "destruktor fail\n");
   else
      printf( "destruktor\n");
}


static mulle_thread_rval_t   thread_main( void *arg)
{
   // destruktor may only run if tss has been set in current thread
   printf( "thread set\n");
   if( mulle_thread_tss_set( key, shared_value2))
      return( 1);

  return( (mulle_thread_rval_t) 0);
}


//
// destruktor doesn't run in current thread, which is not neccessarily a
// pthread (?)
//
int   main( void)
{
   mulle_thread_t      thread;

   printf( "create\n");
   if( mulle_thread_tss_create( destruktor, &key))
      return( 1);

   printf( "get empty\n");
   if( mulle_thread_tss_get( key))
      return( 1);

   printf( "set\n");
   if( mulle_thread_tss_set( key, shared_value))
      return( 1);

   printf( "get\n");
   if( mulle_thread_tss_get( key) != shared_value)
      return( 1);

   printf( "thread start\n");
   if( mulle_thread_create( thread_main, NULL, &thread))
      return( 1);
   if( mulle_thread_join( thread))
      return( 1);

   return( 0);
}
