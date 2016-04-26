# AQ: A Multi-Producer Concurrent Allocating Queue
AQ is a queue data structure intended for use as an inter-thread and inter-process communications mechanism.  The features of AQ include:
* Multiple producers can write into the queue concurrently.
* A single consumer may read from the queue with its reads can be fully concurrent with the producers.
* Access to the queue (for any number of producers and a single consumer) is fully thread-safe using a lock-free algorithm.
* The queue allocates memory for each item from its internal shared memory pool.
* Two modes of operation are supported for memory allocation:
  * Fixed length items consisting of a single contiguous memory regions.
  * Variable length items consisting of a linked-list of contiguous memory regions.
* The queue content can be fully captured and examined by taking a snapshot-in-time.  This allow for recovery of previously processed items.

## Usage and Documentation
Full API documentation including usage and examples can be found [on the github pages project page](http://alewisw.github.io/aq/).

## Current Status
WARNING: AQ is under ACTIVE DEVELOPMENT.  The APIs may change dramatically.  There may be serious bugs.  Use at your own risk.

## Continuous Integration / Automated Testing
AQ incorporates two main validation test reigimes:
* Unit tests covering the full functionality of the list, including validating the lock-free algorithm under pre-emption conditions.
* Stress tests that aggressivly access the queue to validate real-world behavior for various queue configurations.

These tests are run automatically through online CI services:
* [Appveyor Windows Unit Test Resuts](https://ci.appveyor.com/project/alewisw/aq)
* [Appveyor Windows Stress Test Results](https://ci.appveyor.com/project/alewisw/aq-a7p4i)

## License
AQ is licensed under the Mozilla Public License Version 2.0; see the [LICENSE FILE](https://github.com/alewisw/aq/blob/master/LICENSE) for details.

## Contact
* For general issues and requests, please raise them on the [Github Issue Tracker](https://github.com/alewisw/aq/issues).
* For all other requests I can be contacted directly at alewis.sw@gmail.com.

