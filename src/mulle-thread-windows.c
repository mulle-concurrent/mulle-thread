//
//  mulle_thread_windows.c
//  mulle-thread
//
//  Created by Nat! on 25/08/16.
//  Copyright (c) 2016 Mulle kybernetiK. All rights reserved.
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

#ifdef _WIN32

#include "mulle-thread.h"
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>


#pragma mark tss destruktor

struct destructor_table_entry
{
   mulle_thread_tss_t   key;
   void                 (*f)( void *);
};


struct destructor_table
{
   size_t    n;
   size_t    size;

   struct destructor_table_entry  entries[ 1];
};


struct destructor_table   *destructor_table_grow( struct destructor_table *table)
{
   size_t                    n;
   size_t                    size;
   size_t                    needed;
   struct destructor_table   *newtable;

   if( ! table)
   {
      n    = 0;
      size = 4;
   }
   else
   {
      n    = table->n;
      size = table->size * 2;
   }

   needed = sizeof( struct destructor_table) + sizeof( struct destructor_table_entry) * (size - 1);

   newtable = realloc( table, needed);
   if( newtable)
   {
      newtable->n    = n;
      newtable->size = size;
   }
   return( newtable);
}


static int   destructor_table_add_destructor_for_key( struct destructor_table **p_table,
                                                      void (*f)( void *),
                                                      mulle_thread_tss_t  key)
{
   struct destructor_table *table;

   assert( key);

   table = *p_table;
   if( ! f)
      return( 0);

   if( ! table || table->n == table->size)
   {
      if( ! (*p_table = destructor_table_grow( table)))
         return( -1);
      table = *p_table;
   }

   table->entries[ table->n].key = key;
   table->entries[ table->n].f   = f;

   table->n++;
   return( 0);
}


static void   destructor_table_remove_destructor_for_key( struct destructor_table *table,
                                                          mulle_thread_tss_t key)
{
   struct destructor_table_entry   *p;
   struct destructor_table_entry   *sentinel;

   if( ! table)
      return;

   p        = table->entries;
   sentinel = &p[ table->n];
   while( p < sentinel)
   {
      if( key == p->key)
      {
         p->key = 0;
         p->f   = 0;
         break;
      }
      ++p;
   }
}


static void   destructor_table_execute_destructors( struct destructor_table *table)
{
   struct destructor_table_entry   *p;
   struct destructor_table_entry   *sentinel;
   void                            *value;

   if( ! table)
      return;

   p        = table->entries;
   sentinel = &p[ table->n];
   while( p < sentinel)
   {
      if( p->f)
      {
         value = TlsGetValue( p->key);
         if( value)
         {
            TlsSetValue( p->key, NULL);
            (*p->f)( value);
         }
      }
      ++p;
   }
}


#pragma mark - "API"

static struct
{
   CRITICAL_SECTION           lock;
   struct destructor_table    *table;
   LONG                       inited;
} global;


static void   mulle_thread_tss_init( void)
{
   for(;;)
   {
      switch( InterlockedIncrement( &global.inited))
      {
      case 1  : InitializeCriticalSection( &global.lock);
                InterlockedIncrement( &global.inited);  // done
      default : return;

      case 2  : InterlockedDecrement( &global.inited);  // competing initalizer, undo
                mulle_thread_yield();                   // and wait
                continue;
      }
   }
}


static void   mulle_thread_tss_done( void)
{
   for(;;)
   {
      switch( InterlockedDecrement( &global.inited))
      {
      case 0  : assert( 0 && "mulle_thread_tss_done called too often");
                return;
      case 1  : mulle_thread_yield();
                DeleteCriticalSection( &global.lock);
                free( global.table);
                global.table = NULL;
                InterlockedDecrement( &global.inited);
      default : return;
      }
   }
}


int   mulle_thread_windows_add_tss( mulle_thread_tss_t key, void(*f)(void *))
{
   int   rval;

   mulle_thread_tss_init();

   EnterCriticalSection( &global.lock);
   {
      rval = destructor_table_add_destructor_for_key( &global.table, f, key);
   }
   LeaveCriticalSection( &global.lock);

   // rval == -1 is actually catastrophic/leaky
   assert( ! rval);
   return( rval);
}


void   mulle_thread_windows_remove_tss( mulle_thread_tss_t key)
{
   assert( global.inited);

   EnterCriticalSection( &global.lock);
   {
      destructor_table_remove_destructor_for_key( global.table, key);
   }
   LeaveCriticalSection( &global.lock);

   mulle_thread_tss_done();
}


void   mulle_thread_windows_destroy_tss( void)
{
   assert( global.inited);

   //
   // is it a problem that we are doing this while being locked ?
   // if yes, copy the table and run it outside of the lock
   //
   EnterCriticalSection( &global.lock);
   {
      destructor_table_execute_destructors( global.table);
   }
   LeaveCriticalSection( &global.lock);
}


#endif
