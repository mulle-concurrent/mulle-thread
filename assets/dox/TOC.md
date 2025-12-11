# mulle-thread Library Documentation for AI

## 1. Introduction & Purpose

`mulle-thread` is a C library that provides a simplified, cross-platform API for fundamental concurrency primitives. It abstracts platform-specific implementations for threads, mutexes, thread-local storage (TSS), and atomic operations into a single, portable interface.

It solves the problem of writing portable multithreaded C code by creating a common API that works across POSIX-compliant systems (using pthreads), Windows, and systems with C11 threads support. Its main advantages are simplicity and portability, allowing developers to use a consistent threading model everywhere.

## 2. Key Concepts & Design Philosophy

The library's design is a **thin abstraction layer** over native platform APIs. It selects the appropriate backend at compile time (`pthreads`, Windows API, or C11 `threads.h`) and maps its own functions and data types to the native equivalents.

- **Simplicity:** The API exposes a minimal, essential set of features for multithreading, avoiding the complexity of more advanced options available in native libraries.
- **Portability:** The primary goal is to write multithreaded code once and have it compile and run correctly on different operating systems.
- **Atomic Operations:** The library provides a core set of atomic operations on `void *` pointers, which are essential for writing lock-free algorithms. It uses `mintomic` or C11's `<stdatomic.h>` as a backend.
- **Resource Management:** The library includes convenience macros like `mulle_thread_mutex_do` to simplify locking and unlocking, reducing the risk of deadlocks or forgetting to release a mutex.

## 3. Core API & Data Structures

The API is divided into four main areas, each with its own set of functions and data types defined in `mulle-thread.h` and `mulle-atomic.h`.

### 3.1. `mulle-thread.h`

#### Thread Management
- `mulle_thread_t`: A platform-independent type representing a thread handle.
- `mulle_thread_create(function, arg)`: Creates and starts a new thread that executes the given `function` with `arg` as its parameter. Returns a `mulle_thread_t` handle.
- `mulle_thread_join(thread)`: Blocks the calling thread until the specified `thread` terminates.
- `mulle_thread_yield(void)`: Causes the calling thread to relinquish the CPU.

#### Mutex (Mutual Exclusion)
- `mulle_thread_mutex_t`: A platform-independent mutex type.
- `mulle_thread_mutex_init(mutex)`: Initializes a mutex.
- `mulle_thread_mutex_done(mutex)`: Destroys a mutex.
- `mulle_thread_mutex_lock(mutex)`: Acquires a lock on the mutex, blocking if necessary.
- `mulle_thread_mutex_unlock(mutex)`: Releases a lock on the mutex.
- `mulle_thread_mutex_trylock(mutex)`: Attempts to acquire a lock without blocking. Returns `0` on success.
- `mulle_thread_mutex_do(mutex) { ... }`: A macro that locks the mutex, executes the code block, and automatically unlocks the mutex when the block is exited (including via `break` or `continue`).

#### Thread-Specific Storage (TSS)
- `mulle_thread_tss_t`: A platform-independent key for thread-specific storage.
- `mulle_thread_tss_create(destructor_function)`: Creates a new TSS key. The optional `destructor_function` is called when a thread exits if its value for this key is not `NULL`.
- `mulle_thread_tss_get(key)`: Retrieves the value for the given `key` for the current thread.
- `mulle_thread_tss_set(key, value)`: Sets the value for the given `key` for the current thread.

#### Once Initialization
- `mulle_thread_once_t`: A type used for one-time initialization control.
- `mulle_thread_once(flag, function)`: Ensures that `function` is called exactly once, even if `mulle_thread_once` is called concurrently from multiple threads.

### 3.2. `mulle-atomic.h`

This header provides atomic operations primarily for `void *` pointers.

- `_mulle_atomic_pointer_t`: The underlying atomic pointer type.
- `mulle_atomic_pointer_read(ptr)`: Atomically reads the value of a pointer.
- `mulle_atomic_pointer_write(ptr, value)`: Atomically writes a value to a pointer.
- `mulle_atomic_pointer_cas(ptr, expected, desired)`: Atomic Compare-And-Swap. If the current value at `ptr` equals `expected`, it is replaced with `desired`. Returns the original value.
- `mulle_atomic_pointer_add(ptr, value)`: Atomically adds an integer `value` to the pointer.
- `mulle_atomic_pointer_increment(ptr)`: Atomically increments the pointer value by 1.
- `mulle_atomic_pointer_decrement(ptr)`: Atomically decrements the pointer value by 1.

## 4. Performance Characteristics

- **Overhead:** As a thin wrapper, the performance is nearly identical to the underlying native implementation (pthreads, Windows threads, C11 threads). The overhead of the function call indirection is minimal.
- **Mutexes:** The performance of mutex operations is determined by the underlying OS scheduler and lock implementation.
- **Atomics:** Atomic operations are highly optimized and typically translate to single machine instructions on modern hardware.
- **Thread-Safety:** The library is, by its nature, thread-safe. All functions are designed to be called from multiple threads.

## 5. AI Usage Recommendations & Patterns

- **Best Practices:** Always `mulle_thread_join` a thread you create to ensure its resources are cleaned up and to avoid detached, "zombie" threads.
- **Mutexes:** Use `mulle_thread_mutex_do` for simple critical sections to prevent errors from forgetting to unlock. For more complex locking scenarios spanning multiple functions, use `lock` and `unlock` manually, but be extremely careful about all possible code paths.
- **TSS:** Use thread-specific storage to maintain per-thread state without passing data through every function call. A common use case is for per-thread error handlers or resource pools.
- **Atomics:** Use atomic operations for simple, high-contention state changes (like counters or flags) where a full mutex would be too heavyweight. Use `mulle_atomic_pointer_cas` as the foundation for building more complex lock-free data structures.
- **Common Pitfall:** Do not use `return` from inside a `mulle_thread_mutex_do` block, as this will exit the function without unlocking the mutex.

## 6. Integration Examples

### Example 1: Creating and Joining a Thread

This example demonstrates the basic lifecycle of a thread.
*Source: `test/threads/threads.c`*
```c
#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

static int run_thread(void *arg)
{
    long value = (long)arg;
    printf("Child thread executing with argument: %ld\n", value);
    return 0;
}

int main(void)
{
    mulle_thread_t thread;

    printf("Main thread: starting child thread.\n");
    thread = mulle_thread_create(run_thread, (void *)1848L);
    if (!thread) {
        fprintf(stderr, "Failed to create thread\n");
        return 1;
    }

    mulle_thread_join(thread);
    printf("Main thread: child thread has finished.\n");

    return 0;
}
```

### Example 2: Protecting a Shared Resource with a Mutex

This example shows how to use a mutex to safely increment a shared counter from multiple threads.
*Source: `test/mutex/mutex.c`*
```c
#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

#define NUM_THREADS 10
#define INCREMENTS_PER_THREAD 10000

static mulle_thread_mutex_t lock;
static int shared_counter = 0;

static int increment_routine(void *arg)
{
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++)
    {
        // Use the convenience macro for safe locking
        mulle_thread_mutex_do(&lock)
        {
            shared_counter++;
        }
    }
    return 0;
}

int main(void)
{
    mulle_thread_t threads[NUM_THREADS];

    mulle_thread_mutex_init(&lock);

    for (int i = 0; i < NUM_THREADS; i++)
        threads[i] = mulle_thread_create(increment_routine, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
        mulle_thread_join(threads[i]);

    mulle_thread_mutex_done(&lock);

    printf("Final counter value: %d (expected %d)\n", shared_counter, NUM_THREADS * INCREMENTS_PER_THREAD);
    return 0;
}
```

### Example 3: Using Thread-Specific Storage (TSS)

This example demonstrates how each thread can have its own unique data associated with a TSS key.
*Source: `test/tss/tss.c`*
```c
#include <mulle-thread/mulle-thread.h>
#include <stdio.h>

static mulle_thread_tss_t  thread_id_key;

static void print_thread_id(void)
{
    void *value = mulle_thread_tss_get(thread_id_key);
    printf("My thread-specific ID is %ld\n", (long)value);
}

static int thread_routine(void *arg)
{
    // Set a unique value for this thread
    mulle_thread_tss_set(thread_id_key, arg);
    print_thread_id();
    return 0;
}

int main(void)
{
    mulle_thread_t t1, t2;

    // Create the key. No destructor needed for simple long values.
    mulle_thread_tss_create(NULL);

    t1 = mulle_thread_create(thread_routine, (void *)1);
    t2 = mulle_thread_create(thread_routine, (void *)2);

    mulle_thread_join(t1);
    mulle_thread_join(t2);

    return 0;
}
```

## 7. Dependencies

- `mulle-c11`
- `mintomic` (vendored)

