## Second Chance
This is a memory management policy algorithm used to manage virtual memory pages.

The cache is implemented as a FIFO queue, where each element has a usage flag in addition to the cached value. When a new element is added, it is inserted at the beginning of the queue, potentially causing an element to be evicted from the end of the queue. The usage flag of the newly added element is not set.

When an element is evicted, its usage flag is checked. If it is set, the element is not removed entirely, but is instead moved to the beginning of the queue (potentially causing another eviction), and the usage flag of this element is reset.

The return of the evicted but "saved" by the usage flag element to the queue occurs _before_ the insertion of the new element that caused the eviction.
Thus, the newly inserted cache element will always be at the beginning of the queue, and any changes related to eviction and possible return to the queue will occur before its insertion (but with a queue whose effective size is one less than the size of the cache).

It is possible that all elements in the queue have the usage flag set - then adding a new element and evicting one of the old elements:
* resets all usage flags
* removes the element from the cache at the end of the queue

When searching for an element, if it is found, it does not change its position in the queue, but its usage flag is set.

### Implementation assumption
In the implementation of the cache, it is assumed that all objects stored there have a common ancestor in the inheritance hierarchy, specified by the template parameter KeyProvider, which in turn has an equality operator with the key (specified by the template parameter Key).

## Modification of the pool allocator to support multiple object types

It is necessary to extend the implementation of the pool allocator with the ability to place objects of several different types (of different size).
Various approaches can be taken to implement this idea, but we will stop at the variant that resembles a simplified model of the [SLAB allocator](https://en.wikipedia.org/wiki/Slab_allocation):
* the pool consists of a set of blocks (slab)
* each block is used to allocate memory for objects of one size
* all blocks have the same size, specified when the pool is created
* if there is no more space in the block, then an attempt to allocate memory for an object of the corresponding size will fail

The pool should be created with parameters "block size" and "list of object sizes". The following requirements are placed on memory consumption:
* let the pool be created with `N` blocks
* let the block size be equal to `K`
* let the size of element `i` of the block be equal to `S(i)`
* then the maximum memory consumption for the pool should be `M <= N * (K + 8 * sizeof(void *)) + 10 * sizeof(void *) * sum(K / S(i) for i in range(N))`

Memory for an object should be allocated in a block allocated for the size of such an object.

Obviously, the main advantage of such a scheme (in the context of tasks for allocating memory for objects of different sizes) is the absence of fragmentation. The main disadvantage is the inflexible allocation of memory - if different sizes have different usage frequencies, then some blocks may idle with low occupancy.
In addition, if some object sizes do not divide the block size evenly, then some memory will be wasted.
In more realistic implementations, blocks are likely to be allocated dynamically - when one block is fully occupied, then a request to allocate memory for an object of the corresponding size will cause the creation of a new block for servicing this size.

Think about how to optimize the administrative expenses of such an allocator and how the limitations of the language standard hinder this task. Can you bypass these obstacles if you slightly sacrifice the universality of the allocator?
