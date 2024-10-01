#include "acp/Allocator.hpp"

AllocatorWithPool::AllocatorWithPool(const std::size_t size, std::initializer_list<std::size_t> sizes)
    : PoolAllocator(size, sizes) {}
