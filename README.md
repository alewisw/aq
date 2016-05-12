# AQ: A Multi-Producer Concurrent Allocating Queue
AQ is a queue data structure intended for use as an inter-thread and inter-process communications mechanism.  The features of AQ include:
* Multiple producers can write into the queue concurrently.
* A single consumer may read from the queue with its reads being fully concurrent with the producers' writes.
* Access to the queue (for any number of producers and a single consumer) is fully thread-safe using a lock-free algorithm.
* The queue allocates memory for each item from its internal shared memory pool.
* Two modes of operation are supported for memory allocation:
  * Fixed length items consisting of a single contiguous memory regions.
  * Variable length items consisting of a linked-list of contiguous memory regions.
* The queue content can be fully captured and examined by taking a snapshot-in-time.  This allow for recovery of previously processed items.

## Usage and Documentation
Full API documentation including usage and examples can be found [on the github pages project page](http://alewisw.github.io/aq/appveyor/aq_doxygen).

## Current Status
WARNING: AQ is under ACTIVE DEVELOPMENT.  The APIs may change dramatically.  There may be serious bugs.  Use at your own risk.

## Continuous Integration / Automated Testing
AQ incorporates two main validation test reigimes:
* Unit tests covering the full functionality of the list, including validating the lock-free algorithm under pre-emption conditions.
* Stress tests that aggressivly access the queue to validate real-world behavior for various queue configurations.

These tests are run automatically through online CI services:

| Operating System | CI Provider | Build | Status |
| ---------------- | ----------- | ----- | ------ |
|         | Coverity  | [Static Code Analysis](https://scan.coverity.com/projects/alewisw-aq) | [![Coverity Scan Build Status](https://scan.coverity.com/projects/8838/badge.svg)](https://scan.coverity.com/projects/alewisw-aq) |
| Linux   | Travis CI | [Unit & Stress Tests](https://travis-ci.org/alewisw/aq) | [![Build Status](https://travis-ci.org/alewisw/aq.svg?branch=master)](https://travis-ci.org/alewisw/aq) |
| Linux   | Travis CI | [Unit Test Code Coverage](http://alewisw.github.io/aq/travisci/coverage/aq_unittest) | |
| Linux   | Travis CI | [Stress Test Code Coverage](http://alewisw.github.io/aq/travisci/coverage/aq_stresstest) | |
| Linux   | Coveralls | [Unit Test Code Coverage](https://coveralls.io/github/alewisw/aq?branch=master) | [![Coverage Status](https://coveralls.io/repos/github/alewisw/aq/badge.svg?branch=master)](https://coveralls.io/github/alewisw/aq?branch=master) |
| Windows | Appveyor  | [Unit Tests](https://ci.appveyor.com/project/alewisw/aq) | [![Build status](https://ci.appveyor.com/api/projects/status/avfwa8c4nl9dox1i/branch/master?svg=true)](https://ci.appveyor.com/project/alewisw/aq/branch/master) |
| Windows | Appveyor  | [API Documentation](http://alewisw.github.io/aq/appveyor/aq_doxygen) | |
| Windows | Appveyor  | [Stress Tests](https://ci.appveyor.com/project/alewisw/aq-a7p4i) | [![Build status](https://ci.appveyor.com/api/projects/status/0ox809apiv7kqh50/branch/master?svg=true)](https://ci.appveyor.com/project/alewisw/aq-a7p4i/branch/master) |

## License
AQ is licensed under the Mozilla Public License Version 2.0; see the [LICENSE FILE](https://github.com/alewisw/aq/blob/master/LICENSE) for details.

## Contact
* For general issues and requests, please raise them on the [Github Issue Tracker](https://github.com/alewisw/aq/issues).
* For all other requests I can be contacted directly at alewis.sw@gmail.com.

