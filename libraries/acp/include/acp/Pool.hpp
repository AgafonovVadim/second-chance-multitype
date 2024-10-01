#ifndef ACP_POOL_HPP
#define ACP_POOL_HPP

#include <cstddef>
#include <initializer_list>
#include <new>
#include <vector>

class PoolAllocator {
public:
    PoolAllocator(std::size_t const /*block_size*/, std::initializer_list<std::size_t> /*sizes*/);

    void* allocate(std::size_t const /*n*/);

    void deallocate(void const* /*ptr*/);

protected:
    struct Slab {
        Slab(std::size_t _size, std::size_t _slab_size, std::byte& _ptr);

        std::size_t slab_size;

        std::byte* ptr;

        std::vector<bool> used_map;
    };

private:
    std::size_t m_block_size;

    std::vector<std::byte> m_data;

    std::vector<Slab> m_slabs_storage;
};

#endif  // ACP_POOL_HPP
