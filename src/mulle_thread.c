//
//  mulle_thread.c
//  mulle-thread
//
//  Created by Nat! on 29/09/16.
//  Copyright (c) 2016 Mulle kybernetiK. All rights reserved.
//  Copyright (c) 2016 Codeon GmbH. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
#include "mulle_thread.h"

#include <stdlib.h>
#include <assert.h>


struct mulle_thread_bounceinfo   *mulle_thread_bounceinfo_create( mulle_thread_rval_t (*f)( void *),
                                                                  void *arg)
{
   struct mulle_thread_bounceinfo   *info;
   
   info = calloc( 1, sizeof( struct mulle_thread_bounceinfo));
   if( ! info)
      return( NULL);
   
   info->f   = f;
   info->arg = arg;
   return( info);
}


mulle_thread_native_rval_t   mulle_thread_bounceinfo_bounce( void *_info);

mulle_thread_native_rval_t   mulle_thread_bounceinfo_bounce( void *_info)
{
   mulle_thread_rval_t              rval;
   struct mulle_thread_bounceinfo   *info;
   
   info = _info;
   rval = (*info->f)( info->arg);
   free( info);

   mulle_thread_exit( rval);
   
   assert( 0);
   return( (mulle_thread_native_rval_t) rval);
}

