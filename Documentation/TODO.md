# TODO List
1. Reimplement walk() so that it becomes readable.
2. Look at test points in walk() to see if we need unit tests for them.
3. Buffer overrun on bad CRC?
4. Corruption resilience - esp. in snapshot
5. Add built-in support for formatted printing into normal and extendable items.
6. Add built-in support for streaming into normal and extendable items.
7. commitCounter() is not sufficient for reader polling - also need a timer due to commit timeouts.
