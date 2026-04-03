#include <mulle-thread/mulle-thread.h>
#include <stdio.h>
#include <stdint.h>

#define DEBUG_VERBOSE

#ifdef DEBUG_VERBOSE
# define trace( ...)     fprintf( stderr, __VA_ARGS__)
#else
# define trace( ...)
#endif

static mulle_thread_id_t     memo[ 4];


static mulle_thread_rval_t   thread_main( void *arg)
{
   intptr_t   i;

   i        = (intptr_t) arg;
   memo[ i] = mulle_thread_id();
   trace( "thread_main #%td is %p\n", i, (void *) memo[ i]);
   return( (mulle_thread_rval_t) 0);
}


#ifdef _WIN32
# include <tchar.h>
#endif


int   main( void)
{
   mulle_thread_t     threads[ 4];
   mulle_thread_id_t  thread_id;
   intptr_t           i;
   int                flag;

   thread_main( (void *) 0);
   threads[ 0] = mulle_thread_self();

   for( i = 1; i <= 3; i++)
   {
      if( mulle_thread_create( thread_main, (void *) i, &threads[ i]))
      {
         fprintf( stderr, "thread creation failed at #%td\n", i);
         return( 1);
      }

      thread_id = mulle_thread_get_id( threads[ i]);
      if( ! thread_id)
      {
#ifdef _WIN32
         LPVOID   lpMsgBuf;
         DWORD    dw = GetLastError();

         FormatMessage(
           FORMAT_MESSAGE_ALLOCATE_BUFFER |
           FORMAT_MESSAGE_FROM_SYSTEM |
           FORMAT_MESSAGE_IGNORE_INSERTS,
           NULL,
           dw,
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
           (LPTSTR) &lpMsgBuf,
           0, NULL);

         _tprintf(_T("Error = %s\n"), (char *) lpMsgBuf);
         LocalFree(lpMsgBuf);
#endif
         fprintf( stderr, "thread id fetch failed at #%td\n", i);
         return( 1);
      }

      flag = 0;
      while( ! memo[ i])
      {
         mulle_thread_yield();
         if( ! flag)
         {
            trace( "%td: waiting for %tx to start\n", i, mulle_thread_get_id( threads[ i]));
            flag = 1;
         }
      }
   }

   trace( "Comparing thread values\n");

   flag = 0;
   for( i = 0; i < 4; i++)
   {
      if( mulle_thread_get_id( threads[ i]) != memo[ i])
      {
         fprintf( stderr, "peculiar mismatch at #%td between 0x%tx and 0x%tx\n", i, mulle_thread_get_id( threads[ i]),  memo[ i]);
         flag = 1;
      }
   }


   for( i = 1; i <= 3; i++)
   {
      mulle_thread_join( threads[ i]);  // pedantic
   }

   if( flag)
   {
      printf( "FAIL\n");
      return( 1);
   }
   trace( "All good\n");

   return( 0);
}
