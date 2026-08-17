#include <mulle-thread/mulle-thread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define S_TREASURE   32
#define LOOPS        1000
#define MAX_THREADS  4


struct shared
{
   mulle_thread_recursive_mutex_t   lock;
   char                             treasure[ S_TREASURE];
};


static void   run_with_depth( struct shared *s, unsigned int depth)
{
   int    c;
   char   tmp[ S_TREASURE];

   c = (int)(uintptr_t) mulle_thread_id();
   memset( tmp, c, S_TREASURE);

   if( depth)
   {
      mulle_thread_recursive_mutex_lock( &s->lock);
         memcpy( s->treasure, tmp, S_TREASURE);
         MULLE_THREAD_UNPLEASANT_RACE_YIELD();
         if( memcmp( tmp, s->treasure, S_TREASURE))
            abort();
         run_with_depth( s, depth - 1);
      mulle_thread_recursive_mutex_unlock( &s->lock);
   }
}


static mulle_thread_rval_t MULLE_THREAD_CALL   thread_fn( struct shared *s)
{
   unsigned int   i;

   for( i = 0; i < LOOPS; i++)
   {
      MULLE_THREAD_UNPLEASANT_RACE_YIELD();
      run_with_depth( s, rand() % 8);
   }
   return( 0);
}


int   main( void)
{
   struct shared    s;
   mulle_thread_t   threads[ MAX_THREADS];
   int              i;

   mulle_thread_recursive_mutex_init( &s.lock);
   memset( s.treasure, 0, S_TREASURE);

   for( i = 1; i < MAX_THREADS; i++)
      if( mulle_thread_create( (void *) thread_fn, &s, &threads[ i]))
         abort();

   thread_fn( &s);

   for( i = 1; i < MAX_THREADS; i++)
      mulle_thread_join( threads[ i]);

   mulle_thread_recursive_mutex_done( &s.lock);
   printf( "ok\n");
   return( 0);
}
