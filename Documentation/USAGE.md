# A Multi-Producer Concurrent Allocating Queue

The %AQ library provides a queue data structure intended for use as an inter-thread and inter-process communications mechanism.  The %AQ data structure is suitable for situations where:
 - All data storage for items in the queue must reside within the queue memory itself.
 - Multiple producers concurrent write items into the queue.
 - A single consumer reads and processes all items placed into the queue.

The %AQ has a number of features that make it particularly suitable for use as an inter-thread or inter-process communications mechanism; namely:
 - Access to the queue (for any number of producers and a single consumer) is fully thread-safe using a lock-free algorithm.
 - The lock-free algorithm in turn means that there is no scheduler overhead induced by operating system locking primitives such as semaphores or mutexes.
 - The queue allocates memory for each item from its internal shared memory pool - no external memory management is required.
 - Two modes of operation are supported for memory allocation:
    + Fixed length items consisting of a single contiguous memory regions.
    + Variable length items consisting of a linked-list of memory regions.
 - The queue is robust in the face of unexpected application termination, especially when the application is in the middle of performing opertions on the queue.
 - The queue content can be fully captured and examined by taking a snapshot-in-time.  This allow for recovery and examination of previously processed items in the case of unexpected application termination.

## Internal Structure and Queue Algorithm

In order to understand how to use the AQ interface a general understanding of the internal structure of the queue is required.
 - A queue consists of an array of fixed size pages.  The size of each page must be a power of two (i.e., 8, 16, 32, 64, 128, 256, etc bytes).
 - Multiple contiguous pages can be grouped together to form an item.  
 - Each item contains data supplied by the user and stored in the queue.
 - The status of each item is maintained by a control word.  There is one control word per page in the queue.
 - The control word contains the following parameters:
    + The number of pages in the item.
    + Whether the item has been claimed via a call to AQWriter::claim().
    + Whether the item has been committed via a call to AQWriter::commit().
    + Whether the item has been retrieved and then released by calling AQReader::retrieve() followed by AQReader::release().
 - The queue maintains two pointers.  
    + The head points to the page containing the start of the oldest item in the queue not yet released via AQReader::release().
    + The tail points to the first page that does not contain an unreleased item.  The next item to be written will be stored at the tail position.

Shown below is an example of the queue data structure in a particular state.  This example is described in detail below to help explain how the queue data is structured and the properties that this implies.
<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="24%" colspan="4">
    <td bgcolor="#AFAFFF" width="6%" align="center">Head
    <td width="36%" colspan="6">
    <td bgcolor="#BFBFBF" width="6%" align="center">Tail
    <td width="18%" colspan="3">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#BFBFBF" valign="middle" align="center">0
    <td bgcolor="#BFBFBF" valign="middle" align="center">1
    <td bgcolor="#FFAFAF" valign="middle" align="center">2
    <td bgcolor="#FFAFAF" valign="middle" align="center">3
    <td bgcolor="#AFAFFF" valign="middle" align="center">4
    <td bgcolor="#AFAFFF" valign="middle" align="center">5
    <td bgcolor="#AFAFFF" valign="middle" align="center">6
    <td bgcolor="#FFFFAF" valign="middle" align="center">7
    <td bgcolor="#AFFFAF" valign="middle" align="center">8
    <td bgcolor="#AFFFAF" valign="middle" align="center">9
    <td bgcolor="#AFAFFF" valign="middle" align="center">10
    <td bgcolor="#BFBFBF" valign="middle" align="center">11
    <td bgcolor="#BFBFBF" valign="middle" align="center">12
    <td bgcolor="#BFBFBF" valign="middle" align="center">13
    <td bgcolor="#8F8F8F" valign="middle" align="center">14
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#BFBFBF" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#FFAFAF" valign="top" align="center" colspan="2">2 pages<br>claim<br>released
    <td bgcolor="#AFAFFF" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#AFAFFF" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit
    <td bgcolor="#FFFFAF" valign="top" align="center">1 page<br>claim
    <td bgcolor="#AFFFAF" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>release
    <td bgcolor="#AFAFFF" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#BFBFBF" valign="top" align="center" colspan="3">3 pages<br>commit<br>released
    <td bgcolor="#8F8F8F" valign="top" align="center">waste
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#BFBFBF" valign="top" align="center" colspan="2">BB BB
    <td bgcolor="#FFAFAF" valign="top" align="center" colspan="2">cc cc
    <td bgcolor="#AFAFFF" valign="top" align="center">DD
    <td bgcolor="#AFAFFF" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#FFFFAF" valign="top" align="center">ff
    <td bgcolor="#AFFFAF" valign="top" align="center" colspan="2">GG GG
    <td bgcolor="#AFAFFF" valign="top" align="center">HH
    <td bgcolor="#BFBFBF" valign="top" align="center" colspan="3">AA AA AA
    <td bgcolor="#8F8F8F">
</tr>

</table>




## Usage

~~~{.cpp}
unsigned char mem[4096];            // In this example the shared memory region is fixed.

AQReader reader(mem, sizeof(mem));  // Construct a new reader around the shared memory region.
if (!reader.format(7, 1000))        // Format the queue; the page size is 128 bytes leading to a pageSizeShift of 7 ((1 << 7) == 128)
{                                   // the commit timeout is 1000 milliseconds; no special options are configured.
    fprintf(stderr, "Queue format failed"); 
    exit(1);
}
~~~
