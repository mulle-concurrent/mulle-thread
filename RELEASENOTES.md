## 4.8.0







feature: expose inline threading helpers for portable C API

* Add header-level static inline helpers for thread IDs `(mulle_thread_id,` `mulle_thread_get_id).`
* Provide inline condition variable APIs `(mulle_thread_cond_init/done/wait/signal/broadcast/timedwait),` TSS helpers, and `once/once_recursive` helpers so callers can use a consistent API across platforms.
