/**
 * @file Adds make_unique (available in c++14) to standard memory library.
 */
#ifndef SIK_UDP_MEMORY_H
#define SIK_UDP_MEMORY_H


#include <memory>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif //SIK_UDP_MEMORY_H
