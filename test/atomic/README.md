I had bench.c fail on me occasionally, with:

```
better-cas is slower than read-cas, which is faster than double-cas        |  better-cas is faster than read-cas, which is faster than double-cas
```

because this is extremely rare, I still think that better-cas is better.
If this changes on a platform we gotta ifdef around, so that read-cas
becomes better-cas.
