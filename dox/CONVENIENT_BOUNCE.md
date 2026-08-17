
# Automatically call mulle_thread_exit on thread function exit

If you are prone to forget calling `mulle_thread_exit` or want to easily
interface with functions, that are not aware, that they are running as a
thread. I've got some code for you:


`thread_bounceinfo.h`:

```
#include <mulle-thread/mulle-thread.h>
#include <stdlib.h>

struct thread_bounceinfo
{
   mulle_thread_function_t   *f;
   void                      *arg;
};


struct thread_bounceinfo   *thread_bounceinfo_create( mulle_thread_function_t *f,
                                                      void *arg);


static inline void   thread_bounceinfo_free( struct thread_bounceinfo *info)
{
   free( info);
}

mulle_thread_rval_t MULLE_THREAD_CALL   thread_bounceinfo_bounce( void *_info);

```

`thread_bounceinfo.c`:

```
#include "thread_bounceinfo.h"


struct thread_bounceinfo   *thread_bounceinfo_create( mulle_thread_function_t *f,
                                                      void *arg)
{
   struct thread_bounceinfo   *info;

   info = calloc( 1, sizeof( struct thread_bounceinfo));
   if( ! info)
      return( NULL);

   info->f   = f;
   info->arg = arg;
   return( info);
}


mulle_thread_rval_t MULLE_THREAD_CALL   thread_bounceinfo_bounce( void *_info)
{
   mulle_thread_rval_t        rval;
   struct thread_bounceinfo   *info;
   mulle_thread_function_t    *f;
   void                       *arg;

   info = _info;
   f    = info->f;
   arg  = info->arg;
   free( _info);

   rval = (*f)( arg);

   // mulle_thread_exit takes an int; the rval is opaque (a `void *` on
   // pthreads), so round-trip it through intptr_t to avoid conversions
   mulle_thread_exit( (int) (intptr_t) rval);
   assert( 0 && "mulle_thread_exit must not return");
   return( (mulle_thread_rval_t) 0);   // never reached
}
```


And the call

`thread.c`:

```
#include "thread_bounceinfo.h"

static inline int   thread_create( mulle_thread_function_t *f,
                                   void *arg,
                                   mulle_thread_t *thread)
{
   struct thread_bounceinfo   *info;

   info = thread_bounceinfo_create( f, arg);
   if( ! info)
      return( -1);

   return( mulle_thread_create( thread_bounceinfo_bounce, info, thread));
}
```
