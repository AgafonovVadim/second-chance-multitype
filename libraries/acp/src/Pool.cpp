#include "acp/Pool.hpp"

#include "algorithm"

PoolAllocator::PoolAllocator(const std::size_t _block_size, std::initializer_list<std::size_t> _sizes)
    : m_block_size(_block_size), m_data(_block_size * _sizes.size()) {
    auto it = _sizes.begin();
    for (std::size_t i = 0; it != _sizes.end(); ++i, it++) {
        m_slabs_storage.emplace_back(m_block_size, *it, m_data[i * m_block_size]);
    }
    std::sort(m_slabs_storage.begin(), m_slabs_storage.end(),
              [](const Slab& lhs, const Slab& rhs) { return lhs.slab_size < rhs.slab_size; });
}

PoolAllocator::Slab::Slab(std::size_t _size, std::size_t _slab_size, std::byte& _ptr)
    : slab_size(_slab_size), ptr(&_ptr), used_map(_size / _slab_size, false) {}

void* PoolAllocator::allocate(const std::size_t n) {
    Slab& slab = m_slabs_storage[std::lower_bound(
                                     m_slabs_storage.begin(), m_slabs_storage.end(), Slab(0, n, m_data[0]),
                                     [](const Slab& lhs, const Slab& rhs) { return lhs.slab_size < rhs.slab_size; }) -
                                 m_slabs_storage.begin()];
    for (std::size_t index = 0; index < slab.used_map.size(); index++) {
        if (!slab.used_map[index]) {
            slab.used_map[index] = true;
            return (slab.ptr + (slab.slab_size * index));
        }
    }
    throw std::bad_alloc{};
}

void PoolAllocator::deallocate(const void* ptr) {
    auto cur_ptr = static_cast<const std::byte*>(ptr);
    if (&m_data[0] <= cur_ptr && cur_ptr <= &m_data.back()) {
        std::size_t pos = (cur_ptr - &m_data[0]) / m_block_size;
        m_slabs_storage[pos].used_map[(cur_ptr - m_slabs_storage[pos].ptr) / m_slabs_storage[pos].slab_size] = false;
    }
}
