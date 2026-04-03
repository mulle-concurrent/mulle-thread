# Condition Variables API

## Overview

Condition variables provide a synchronization mechanism that allows threads to wait for specific conditions to become true. They are always used in conjunction with a mutex.

## Types

### `mulle_thread_cond_t`

Platform-specific condition variable type:
- **POSIX**: `pthread_cond_t`
- **Windows**: `CONDITION_VARIABLE`
- **C11**: `cnd_t`

## Functions

### `mulle_thread_cond_init`

```c
int mulle_thread_cond_init( mulle_thread_cond_t *cond);
```

Initialize a condition variable.

**Parameters:**
- `cond` - Pointer to condition variable to initialize

**Returns:**
- `0` on success
- Non-zero on error

**Example:**
```c
mulle_thread_cond_t cond;
if( mulle_thread_cond_init( &cond))
{
   fprintf( stderr, "Failed to initialize condition variable\n");
   return -1;
}
```

---

### `mulle_thread_cond_done`

```c
int mulle_thread_cond_done( mulle_thread_cond_t *cond);
```

Destroy a condition variable and release associated resources.

**Parameters:**
- `cond` - Pointer to condition variable to destroy

**Returns:**
- `0` on success
- Non-zero on error

**Note:** On Windows, this is a no-op as condition variables don't require cleanup.

---

### `mulle_thread_cond_wait`

```c
int mulle_thread_cond_wait( mulle_thread_cond_t *cond,
                            mulle_thread_mutex_t *mutex);
```

Wait for a condition variable to be signaled. The mutex must be locked before calling this function. The function atomically unlocks the mutex and waits for the condition. When the condition is signaled, the function re-acquires the mutex before returning.

**Parameters:**
- `cond` - Pointer to condition variable
- `mutex` - Pointer to locked mutex

**Returns:**
- `0` on success
- Non-zero on error

**Important:** This function can return spuriously (without being signaled). Always use it in a loop that checks the actual condition:

```c
mulle_thread_mutex_lock( &mutex);
while( ! condition_is_true)
   mulle_thread_cond_wait( &cond, &mutex);
// condition is now true and mutex is locked
mulle_thread_mutex_unlock( &mutex);
```

---

### `mulle_thread_cond_signal`

```c
int mulle_thread_cond_signal( mulle_thread_cond_t *cond);
```

Wake up one thread waiting on the condition variable. If no threads are waiting, this has no effect.

**Parameters:**
- `cond` - Pointer to condition variable

**Returns:**
- `0` on success
- Non-zero on error

**Example:**
```c
mulle_thread_mutex_lock( &mutex);
data_ready = 1;
mulle_thread_cond_signal( &cond);
mulle_thread_mutex_unlock( &mutex);
```

---

### `mulle_thread_cond_broadcast`

```c
int mulle_thread_cond_broadcast( mulle_thread_cond_t *cond);
```

Wake up all threads waiting on the condition variable. If no threads are waiting, this has no effect.

**Parameters:**
- `cond` - Pointer to condition variable

**Returns:**
- `0` on success
- Non-zero on error

**Example:**
```c
mulle_thread_mutex_lock( &mutex);
shutdown = 1;
mulle_thread_cond_broadcast( &cond);  // wake all waiting threads
mulle_thread_mutex_unlock( &mutex);
```

## Usage Pattern

The typical usage pattern for condition variables:

### Waiting Thread

```c
mulle_thread_mutex_lock( &mutex);

while( ! condition_is_true)
   mulle_thread_cond_wait( &cond, &mutex);

// Condition is true, do work
process_data();

mulle_thread_mutex_unlock( &mutex);
```

### Signaling Thread

```c
mulle_thread_mutex_lock( &mutex);

// Make condition true
condition_is_true = 1;

mulle_thread_cond_signal( &cond);  // or broadcast
mulle_thread_mutex_unlock( &mutex);
```

## Platform-Specific Notes

### POSIX
- Requires cleanup with `mulle_thread_cond_done()`
- Supports spurious wakeups

### Windows
- No cleanup required (but calling `mulle_thread_cond_done()` is safe)
- Supports spurious wakeups
- Requires Windows Vista or later

### C11
- Requires cleanup with `mulle_thread_cond_done()`
- Supports spurious wakeups

## Common Pitfalls

1. **Not using a loop**: Always check the condition in a loop, as spurious wakeups can occur on all platforms.

2. **Forgetting to lock the mutex**: The mutex must be locked before calling `mulle_thread_cond_wait()`.

3. **Signaling without holding the mutex**: While not strictly required, it's best practice to hold the mutex when signaling to avoid race conditions.

4. **Forgetting cleanup**: Always call `mulle_thread_cond_done()` when done with a condition variable (even though it's a no-op on Windows, it's required on other platforms).
