
# Bounce code to automatically call mulle_thread_exit 


`thread_bounceinfo.h`: 

```
#include <mulle_thread/mulle_thread.h>
#include <stdlib.h>

struct thread_bounceinfo
{
   mulle_thread_rval_t   (*f)( void *arg);
   void                  *arg;
};


struct thread_bounceinfo   *thread_bounceinfo_create( mulle_thread_rval_t (*f)( void *), 
                                                      void *arg);


static inline void   thread_bounceinfo_free( struct thread_bounceinfo *info)
{
   free( info);
}

void   thread_bounceinfo_bounce( void *_info);

```

`thread_bounceinfo.c`: 

```
#include "thread_bounceinfo.h"


struct thread_bounceinfo   *thread_bounceinfo_create( mulle_thread_rval_t (*f)( void *),
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


void   thread_bounceinfo_bounce( void *_info)
{
   mulle_thread_rval_t        rval;
   struct thread_bounceinfo   *info;
   void                       (*f)( void *);   
   void                       *arg;

   info = _info;
   f    = info->f;
   arg  = info->arg;
   free( _info);

   rval = (*f)( arg);

   mulle_thread_exit( (mulle_thread_native_rval_t) rval);
   assert( 0 && "mulle_thread_exit must not return");
}
```


And the call 

`thread.c`: 

```
#include "thread_bounceinfo.h"

static inline int   thread_create( mulle_thread_rval_t (*f)( void *),
                                   void *arg,
                                   mulle_thread_t *thread)
{
   struct thread_bounceinfo   *info;
   
   info = thread_bounceinfo_create( f, arg);
   if( ! info)
      return( -1);

   return( mulle_thread_create( thread, thread_bounceinfo_bounce, info));
}
```
