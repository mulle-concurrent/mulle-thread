
# Atomic Operations API

## Types

Type                             | Description
---------------------------------|------------------------------------------------
`mulle_atomic_functionpointer_t` | `void (*)()` sized atomic function pointer storage type.
`mulle_atomic_intptr_t`          | `intptr_t` sized atomic integer storage type, for counters.
`mulle_atomic_pointer_t`         | `void *` sized atomic pointer storage type.
`mulle_functionpointer_t`        | typedef of a non-atomic function pointer for readability.

Static initializers: `MULLE_ATOMIC_POINTER_INIT` and `MULLE_ATOMIC_INTPTR_INIT`.

Use `mulle_atomic_intptr_t` for counters, statistics and similar plain
integers. Do not store counters in a `mulle_atomic_pointer_t`: because that
type is `_Atomic(void *)` on the C11 backend, using the atomic itself where a
pointer to it is expected (a forgotten `&`) silently loads and passes the
value instead of being diagnosed. With `mulle_atomic_intptr_t` the same
mistake is a compile error.


## Atomic Pointer Operations


#### `_mulle_atomic_pointer_nonatomic_read`

```
void   *_mulle_atomic_pointer_nonatomic_read( mulle_atomic_pointer_t *p)
```

Read the contents of a `mulle_atomic_pointer_t` unsafely. Useful and
faster when you know that access is single-threaded.


#### `_mulle_atomic_pointer_nonatomic_write`

```
void   _mulle_atomic_pointer_nonatomic_write( mulle_atomic_pointer_t *p, void *value)
```

Write into the contents of a `mulle_atomic_pointer_t` unsafely. Useful and
faster when you know that access is single-threaded.

#### `_mulle_atomic_pointer_read`

```
void  *_mulle_atomic_pointer_read( mulle_atomic_pointer_t *address)
```

Read the contents of a `mulle_atomic_functionpointer_t` atomically.

Example:

```
mulle_atomic_pointer_t   pointer;
void                     *value;

_mulle_atomic_pointer_nonatomic_write( &pointer, (void *) (intptr_t) 1848);
value = _mulle_atomic_pointer_read( &pointer);
printf( "%p\n", value);
```



#### `_mulle_atomic_pointer_write`

```
void  _mulle_atomic_pointer_write( mulle_atomic_pointer_t *address,
                                   void *value)
```

Write to the contents of a  `mulle_atomic_functionpointer_t` atomically. This
is not used very often, you usually use CAS.


#### `__mulle_atomic_pointer_cas`

```
void   *__mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address,
                                    void *value,
                                    void *expect)
```

CAS: Replace the function pointer value in **address** with **value**, if it's
contents match **expect**.  Will return the value that was found. If the
returned value is **expect**, you know that the swap was successful.

Example:

```
void   set_1848_if_zero( mulle_atomic_pointer_t *pointer)
{
   void   *actual;
   void   *expect;

   actual = _mulle_atomic_pointer_read( pointer);
   do
   {
   	  expect = actual;
	  if( expect)
	     break;
      actual = __mulle_atomic_pointer_cas( pointer, (void *) 1848, expect);
   }
   while( actual != expect);
}

```


#### `_mulle_atomic_pointer_cas`

```
int   _mulle_atomic_pointer_cas( mulle_atomic_pointer_t *address,
                                 void *value,
                                 void *expect)
```

CAS: Replace the function pointer value in **address** with **value**, if it's
contents match **expect**.
Will return non-zero if the swap was successful.


Example:

```
void   set_1848_if_zero( mulle_atomic_pointer_t *pointer)
{
   void   *expect;

   do
   {
   	  expect = _mulle_atomic_pointer_read( pointer);
	  if( expect)
	     break;
   }
   while( ! __mulle_atomic_pointer_cas( pointer, (void *) 1848, expect));
}

```

#### `_mulle_atomic_pointer_increment`

```
void   *_mulle_atomic_pointer_increment( mulle_atomic_pointer_t *address)
```

Returns the previous value. (acts like a postincrement `*p++`)


#### `_mulle_atomic_pointer_decrement`

```
void  *_mulle_atomic_pointer_decrement( mulle_atomic_pointer_t *address)
```

Returns the previous value. (acts like a postdecrement `*p--`)

Example:

```
int   decrement_was_zero( mulle_atomic_pointer_t *pointer)
{
   void   *previous;

   previous = _mulle_atomic_pointer_decrement( pointer);
   return( (intptr_t) previous == 0);
}
```


#### `_mulle_atomic_pointer_add`

```
void  *_mulle_atomic_pointer_add( mulle_atomic_pointer_t *address,
                                  intptr_t diff)
```

Returns the result, not the previous value like increment/decrement



## Atomic Integer Operations

These operate on `mulle_atomic_intptr_t` and return `intptr_t`. They are all
`relaxed` and are intended for disconnected counters and statistics only. Keep
refcounts and anything that gates access to other memory on
`mulle_atomic_pointer_t` with its stronger orderings.

All of them return the **previous** value, for consistency.

#### `_mulle_atomic_intptr_read_relaxed`

```
intptr_t   _mulle_atomic_intptr_read_relaxed( mulle_atomic_intptr_t *address)
```

Read the counter.

#### `_mulle_atomic_intptr_read_nonatomic`

```
intptr_t   _mulle_atomic_intptr_read_nonatomic( mulle_atomic_intptr_t *address)
```

Read the counter unsafely. For single-threaded setup and final verification only.

#### `_mulle_atomic_intptr_write_relaxed`

```
void   _mulle_atomic_intptr_write_relaxed( mulle_atomic_intptr_t *address,
                                           intptr_t value)
```

Write the counter.

#### `_mulle_atomic_intptr_write_nonatomic`

```
void   _mulle_atomic_intptr_write_nonatomic( mulle_atomic_intptr_t *address,
                                             intptr_t value)
```

Write the counter unsafely. For single-threaded setup only.

#### `_mulle_atomic_intptr_increment_relaxed`

```
intptr_t   _mulle_atomic_intptr_increment_relaxed( mulle_atomic_intptr_t *address)
```

Returns the previous value (like a postincrement).

#### `_mulle_atomic_intptr_decrement_relaxed`

```
intptr_t   _mulle_atomic_intptr_decrement_relaxed( mulle_atomic_intptr_t *address)
```

Returns the previous value (like a postdecrement).

#### `_mulle_atomic_intptr_add_relaxed`

```
intptr_t   _mulle_atomic_intptr_add_relaxed( mulle_atomic_intptr_t *address,
                                             intptr_t diff)
```

Returns the previous value. Note that `_mulle_atomic_pointer_add` instead
returns the result; this integer variant is consistent with the rest here.


## Atomic Function Pointer Operations

These functions exist to avoid C compiler type conversion warnings.
Currently it is assumed that `sizeof( void *)` == `sizeof( (void (*)())`.

#### `_mulle_atomic_functionpointer_nonatomic_read`

```
mulle_functionpointer_t   _mulle_atomic_functionpointer_nonatomic_read(
                                     mulle_atomic_functionpointer_t *p)
```

Read the contents of a `mulle_atomic_functionpointer_t` unsafely. Useful when
you know that access is single-threaded.


#### `_mulle_atomic_functionpointer_nonatomic_write`

```
void   _mulle_atomic_functionpointer_nonatomic_write(
                                     mulle_atomic_functionpointer_t *p,
                                     mulle_functionpointer_t *value)
```

Write into the contents of a `mulle_atomic_functionpointer_t` unsafely. Useful
when you know that access is single-threaded.

#### `_mulle_atomic_functionpointer_read`

```
mulle_functionpointer_t  _mulle_atomic_functionpointer_read(
                                      mulle_atomic_functionpointer_t *address)
```

Read the contents of a  `mulle_atomic_functionpointer_t` atomically.


#### `_mulle_atomic_functionpointer_write`

```
void  _mulle_atomic_functionpointer_write(
                                      mulle_atomic_functionpointer_t *address,
                                      mulle_functionpointer_t value)
```

Write to the contents of a  `mulle_atomic_functionpointer_t` atomically. This
is not used very often, you usually use CAS.


#### `__mulle_atomic_functionpointer_cas`

```
mulle_functionpointer_t  __mulle_atomic_functionpointer_cas(
                                       mulle_atomic_functionpointer_t *address,
                                       mulle_functionpointer_t value,
                                       mulle_functionpointer_t expect)
```

Replace the function pointer value in **address** with **value**, if it's
contents match **expect**. Will return the value that was found. If the
returned value is **expect**, you know that the swap was succesful.


#### `_mulle_atomic_functionpointer_cas`

```
int   _mulle_atomic_functionpointer_cas(
                                       mulle_atomic_functionpointer_t *address,
                                       mulle_functionpointer_t value,
                                       mulle_functionpointer_t expect)
```

Replace the function pointer value in **address** with **value**, if it's
contents match **expect**. Will return non-zero if the swap was successful.


## Memory barrier / fence

#### `mulle_atomic_memory_barrier`

```
void   mulle_atomic_memory_barrier( void)
```

Puts up a memory barrier.

