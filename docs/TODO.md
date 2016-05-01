# TODO List
1. Look at test points in walk() to see if we need unit tests for them.
2. Buffer overrun on bad CRC?
3. Corruption resilience - esp. in snapshot
4. Add built-in support for streaming into normal and extendable items.
5. commitCounter() is not sufficient for reader polling - also need a timer due to commit timeouts.
6. better way of handling format failures - reporting why they failed, maybe via an exception.
7. extend coverage of StressTest by adding incomplete item support
