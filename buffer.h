#ifndef SIK_UDP_BUFFER_H
#define SIK_UDP_BUFFER_H


#include <memory>
#include <array>

/**
 * Cyclic buffer with specified size.
 * @tparam T type of element.
 * @tparam buffer_size maximum buffer size.
 */
template <typename T, std::size_t buffer_size>
class Buffer {
    static_assert(buffer_size > 0, "Buffer buffer_size must be greater than zero");
private:
    /// Elements in the buffer.
    std::array<T, buffer_size> data;
    /// Starting index.
    std::size_t start = 0u;
    /// Number of items in the buffer.
    std::size_t length = 0u;

    /**
     * Increases the starting index by one.
     */
    void increase_index() noexcept {
        start = (start + 1) % buffer_size;
    }

    /**
     * Returns the mapping of abstract buffer index to the actual index in the
     * data array.
     * @param index index in the buffer.
     * @return index in the data array.
     */
    inline std::size_t get_index(std::size_t index) const noexcept {
        return (start + index) % buffer_size;
    }

public:
    Buffer() = default;
    Buffer(const Buffer&) = delete;

    /**
     * Access the buffer item.
     * @param index index in the buffer.
     * @return item at the index
     */
    T& operator[](const std::size_t index) {
        if (index >= length) {
            throw std::out_of_range("index out of range");
        }
        return data[get_index(index)];
    }

    /**
     * @return number of elements in the buffer.
     */
    std::size_t size() const noexcept {
        return length;
    }

    /**
     * Inserts new item at the end of the buffer.
     * If the buffer is full removes the first item.
     * @param item item to insert.
     */
    void push(T item) noexcept {
        if (length == buffer_size) {
            increase_index();
        } else {
            length++;
        }
        data[get_index(length - 1)] = std::move(item);
    }

    /**
     * Returns the first item in the buffer and removes it from the data array.
     * @return first item in the buffer.
     * @throws std::out_of_range if the buffer is empty.
     */
    T pop() {
        if (length == 0) {
            throw std::out_of_range("buffer is empty");
        }
        T item = std::move((*this)[0]);
        increase_index();
        length--;
        return std::move(item);
    }

    /**
     * Buffer iterator. Helper class for range based loop to work.
     */
    class iterator {
    private:
        Buffer<T, buffer_size> *buffer;
        std::size_t index;
    public:
        iterator(Buffer<T, buffer_size> *buffer, std::size_t index)
                : buffer(buffer), index(index) {}

        iterator operator++() noexcept {
            index++;
            return iterator(buffer, index);
        }

        bool operator!=(const iterator &other) const noexcept {
            return index != other.index;
        }

        const T& operator*() {
            return (*buffer)[index];
        }
    };

    /**
     * @return iterator to the buffer first item.
     */
    iterator begin() {
        return iterator(this, 0);
    }

    /**
     * @return iterator to the buffer last item.
     */
    iterator end() {
        return iterator(this, length);
    }
};

#endif //SIK_UDP_BUFFER_H
