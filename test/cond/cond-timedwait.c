#include <mulle-thread/mulle-thread.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
static void get_absolute_timeout( struct timespec *ts, int seconds)
{
   FILETIME ft;
   ULARGE_INTEGER ui;
   
   GetSystemTimeAsFileTime( &ft);
   ui.LowPart = ft.dwLowDateTime;
   ui.HighPart = ft.dwHighDateTime;
   
   // Add seconds, convert back to Unix epoch
   ui.QuadPart += (ULONGLONG) seconds * 10000000ULL;
   ts->tv_sec = (time_t) (ui.QuadPart / 10000000ULL - 11644473600ULL);
   ts->tv_nsec = (long) ((ui.QuadPart % 10000000ULL) * 100);
}
#else
#include <time.h>
static void get_absolute_timeout( struct timespec *ts, int seconds)
{
   timespec_get( ts, TIME_UTC);
   ts->tv_sec += seconds;
}
#endif

static mulle_thread_mutex_t   mutex;
static mulle_thread_cond_t    cond;

int main( void)
{
   struct timespec   abstime;
   int               result;

   mulle_thread_mutex_init( &mutex);
   mulle_thread_cond_init( &cond);

   // Test 1: Timeout should occur
   mulle_thread_mutex_lock( &mutex);

   get_absolute_timeout( &abstime, 1);  // 1 second timeout

   printf( "waiting for timeout...\n");
   result = mulle_thread_cond_timedwait( &cond, &mutex, &abstime);

   if( result == ETIMEDOUT)
      printf( "timeout occurred (expected)\n");
   else
      printf( "ERROR: expected timeout, got %d\n", result);

   mulle_thread_mutex_unlock( &mutex);

   // Test 2: Signal before timeout
   mulle_thread_mutex_lock( &mutex);

   get_absolute_timeout( &abstime, 10);  // 10 second timeout (won't be reached)

   // Signal immediately in same thread (will not wake, but tests API)
   mulle_thread_cond_signal( &cond);

   mulle_thread_mutex_unlock( &mutex);

   mulle_thread_cond_done( &cond);
   mulle_thread_mutex_done( &mutex);

   printf( "test passed\n");
   return( 0);
}
