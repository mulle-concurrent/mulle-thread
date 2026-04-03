#include <mulle-thread/mulle-thread.h>
#include <stdio.h>
#include <stdlib.h>

static mulle_thread_mutex_t   mutex;
static mulle_thread_cond_t    cond;
static int                    ready = 0;

static mulle_thread_rval_t   waiter_thread( void *arg)
{
   mulle_thread_mutex_lock( &mutex);
   
   while( ! ready)
      mulle_thread_cond_wait( &cond, &mutex);
   
   printf( "waiter: received signal\n");
   mulle_thread_mutex_unlock( &mutex);
   
   mulle_thread_return();
}

int main( void)
{
   mulle_thread_t   thread;
   
   mulle_thread_mutex_init( &mutex);
   mulle_thread_cond_init( &cond);
   
   mulle_thread_create( waiter_thread, NULL, &thread);
   
   // Give waiter time to start waiting
   mulle_thread_yield();
   
   mulle_thread_mutex_lock( &mutex);
   ready = 1;
   mulle_thread_cond_signal( &cond);
   mulle_thread_mutex_unlock( &mutex);
   
   mulle_thread_join( thread);
   
   mulle_thread_cond_done( &cond);
   mulle_thread_mutex_done( &mutex);
   
   printf( "test passed\n");
   return( 0);
}
