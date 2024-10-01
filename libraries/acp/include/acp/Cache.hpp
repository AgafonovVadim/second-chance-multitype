#ifndef ACP_CACHE_HPP
#define ACP_CACHE_HPP

#include <cstddef>
#include <new>
#include <ostream>

#include "list"

template <class Key, class KeyProvider, class Allocator>
class Cache {
public:
    template <class... AllocArgs>
    explicit Cache(const std::size_t cache_size, AllocArgs &&...alloc_args)
        : m_max_size(cache_size), m_alloc(std::forward<AllocArgs>(alloc_args)...) {}

    std::size_t size() const { return m_queue.size(); }

    bool empty() const { return size() == 0; }

    template <class T>
    T &get(const Key &key);

    std::ostream &print(std::ostream &strm) const;

    friend std::ostream &operator<<(std::ostream &strm, const Cache &cache) { return cache.print(strm); }

private:
    struct Element {
        explicit Element(KeyProvider *element_, bool marked_ = false) : element(element_), marked(marked_) {}
        KeyProvider *element;
        bool marked = false;
    };

    const std::size_t m_max_size;
    Allocator m_alloc;
    std::list<Element> m_queue;
};

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T &Cache<Key, KeyProvider, Allocator>::get(const Key &key) {
    auto it = m_queue.begin();
    for (; it != m_queue.end(); ++it) {
        if (*it->element == key) {
            break;
        }
    }
    if (it != m_queue.end()) {
        it->marked = true;
        return *static_cast<T *>(it->element);
    }
    if (size() == m_max_size) {
        while (m_queue.back().marked) {
            m_queue.splice(m_queue.begin(), m_queue, std::prev(m_queue.end()));
            m_queue.front().marked = false;
        }
        m_alloc.template destroy<KeyProvider>(m_queue.back().element);
        m_queue.pop_back();
    }
    T *element = m_alloc.template create<T>(key);
    m_queue.emplace_front(element);
    return *element;
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream &Cache<Key, KeyProvider, Allocator>::print(std::ostream &strm) const {
    return strm << "<empty>\n";
}
#endif  // ACP_CACHE_HPP
