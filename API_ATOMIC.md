
# Atomic Operations API

## Types

* `mulle_atomic_pointer_t`  : a `void *` sized atomic pointer storage type
* `mulle_functionpointer_t` : a typedef for a non-atomic function pointer. It's just there for readability.
* `mulle_atomic_functionpointer_t` : a `void (*)()` sized atomic function pointer storage type


## Atomic Pointer Operations


#### _mulle_atomic_pointer_nonatomic_read

```
void   *_mulle_atomic_pointer_nonatomic_read( mulle_atomic_pointer_t *p)
```

Read the contents of a `mulle_atomic_pointer_t` unsafely. Useful and 
faster when you know that access is single-threaded.


#### _mulle_atomic_pointer_nonatomic_write

```
void   _mulle_atomic_pointer_nonatomic_write( mulle_atomic_pointer_t *p, void *value)
```

Write into the contents of a `mulle_atomic_pointer_t` unsafely. Useful and faster when
you know that access is single-threaded.

#### _mulle_atomic_pointer_read

```
void  *_mulle_atomic_pointer_read( mulle_atomic_pointer_t *address)
```

Read the contents of a  `mulle_atomic_functionpointer_t` atomically.

Example: 

```
mulle_atomic_pointer_t   pointer;
void                     *value;

_mulle_atomic_pointer_nonatomic_write( &pointer, (void *) (intptr_t) 1848);
value = _mulle_atomic_pointer_read( &pointer);
printf( "%p\n", value);
```



#### _mulle_atomic_pointer_write

```
void  _mulle_atomic_pointer_write( mulle_atomic_pointer_t *address, 
                                   void *value)
```

Write to the contents of a  `mulle_atomic_functionpointer_t` atomically. This
is not used very often, you usually use CAS.


#### __mulle_atomic_pointer_compare_and_swap

```
void   *__mulle_atomic_pointer_compare_and_swap( mulle_atomic_pointer_t *address, 
                                                 void *value, 
                                                 void *expect)
```

Replace the function pointer value in **address** with **value**, if it's 
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
      actual = __mulle_atomic_pointer_compare_and_swap( pointer, (void *) 1848, expect);
   }
   while( actual != expect);
}

```


#### _mulle_atomic_pointer_compare_and_swap

```
int   _mulle_atomic_pointer_compare_and_swap( mulle_atomic_pointer_t *address, 
                                              void *value, 
                                              void *expect)
```

Replace the function pointer value in **address** with **value**, if it's 
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
   while( ! __mulle_atomic_pointer_compare_and_swap( pointer, (void *) 1848, expect));
}

```



#### _mulle_atomic_pointer_increment

```
void   *_mulle_atomic_pointer_increment( mulle_atomic_pointer_t *address)
```

Returns the previous value. (acts like a postincrement `*p++`)


#### _mulle_atomic_pointer_decrement

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
   return( (intptr_t) previous == 0): 
}
```


#### _mulle_atomic_pointer_add

```
void  *_mulle_atomic_pointer_add( mulle_atomic_pointer_t *address, 
                                  intptr_t diff)
```

Returns the result, not the previous value like increment/decrement



## Atomic Function Pointer Operations

Currently these functions exist to avoid C compiler type conversion warnings. 
It is assumed that `sizeof( void *)` == `sizeof( (void (*)())`. 

#### _mulle_atomic_functionpointer_nonatomic_read

```
mulle_functionpointer_t   _mulle_atomic_functionpointer_nonatomic_read( 
                                     mulle_atomic_functionpointer_t *p)
```

Read the contents of a `mulle_atomic_functionpointer_t` unsafely. Useful when
you know that access is single-threaded.


#### _mulle_atomic_functionpointer_nonatomic_write

```
void   _mulle_atomic_functionpointer_nonatomic_write( 
                                     mulle_atomic_functionpointer_t *p, 
                                     mulle_functionpointer_t *value)
```

Write into the contents of a `mulle_atomic_functionpointer_t` unsafely. Useful when
you know that access is single-threaded.

#### _mulle_atomic_functionpointer_read

```
mulle_functionpointer_t  _mulle_atomic_functionpointer_read( 
                                      mulle_atomic_functionpointer_t *address)
```

Read the contents of a  `mulle_atomic_functionpointer_t` atomically.


#### _mulle_atomic_functionpointer_write

```
void  _mulle_atomic_functionpointer_write( 
                                      mulle_atomic_functionpointer_t *address, 
                                      mulle_functionpointer_t value)
```

Write to the contents of a  `mulle_atomic_functionpointer_t` atomically. This
is not used very often, you usually use CAS.


#### __mulle_atomic_functionpointer_compare_and_swap

```
mulle_functionpointer_t  __mulle_atomic_functionpointer_compare_and_swap( 
                                       mulle_atomic_functionpointer_t *address, 
                                       mulle_functionpointer_t value, 
                                       mulle_functionpointer_t expect)
```

Replace the function pointer value in **address** with **value**, if it's contents match **expect**. 
Will return the value that was found. If the returned value is **expect**, you 
know that the swap was succesful.


#### _mulle_atomic_functionpointer_compare_and_swap

```
int   _mulle_atomic_functionpointer_compare_and_swap( 
                                       mulle_atomic_functionpointer_t *address, 
                                       mulle_functionpointer_t value, 
                                       mulle_functionpointer_t expect)
```

Replace the function pointer value in **address** with **value**, if it's contents match **expect**. 
Will return non-zero if the swap was successful.


## Memory barrier / fence

#### mulle_atomic_memory_barrier

```
void   mulle_atomic_memory_barrier( void)
```

Puts up a memory barrier.

