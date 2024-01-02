#include <type_traits>
#if defined(_MSC_VER)

/* These defines are here, because Asio conflicts other sources that includes WinSocks ot WinSocks2 header before asio. So here
we have to create a workaround this problem... For easier use I partly implement the gcc's endian.h here.
*/

#define __bswap_16(x) ((unsigned short int)((((x) >> 8) & 0xff) | (((x)&0xff) << 8)))
#define __bswap_32(x) ((((x)&0xff000000) >> 24) | (((x)&0x00ff0000) >> 8) | (((x)&0x0000ff00) << 8) | (((x)&0x000000ff) << 24))
#define __bswap_64(l)                                                                                                                                         \
((((l) >> 56) & 0x00000000000000FFLL) | (((l) >> 40) & 0x000000000000FF00LL) | (((l) >> 24) & 0x0000000000FF0000LL) | (((l) >> 8) & 0x00000000FF000000LL) \
 | (((l) << 8) & 0x000000FF00000000LL) | (((l) << 24) & 0x0000FF0000000000LL) | (((l) << 40) & 0x00FF000000000000LL)                                      \
 | (((l) << 56) & 0xFF00000000000000LL))


#define htobe16(x) __bswap_16(x)
#define htobe32(x) __bswap_32(x)
#define htobe64(x) __bswap_64(x)

#define be16toh(x) __bswap_16(x)
#define be32toh(x) __bswap_32(x)
#define be64toh(x) __bswap_64(x)

#else
#include <endian.h>
#endif

template<class T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0, std::enable_if_t<sizeof(T) == 1, int> = 0>
void big_to_native_inplace(T& value)
{
}

template<class T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0, std::enable_if_t<sizeof(T) == 2, int> = 0>
void big_to_native_inplace(T& value)
{
    value = be16toh(value);
}

template<class T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0, std::enable_if_t<sizeof(T) == 4, int> = 0>
void big_to_native_inplace(T& value)
{
    value = be32toh(value);
}

template<class T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0, std::enable_if_t<sizeof(T) == 8, int> = 0>
void big_to_native_inplace(T& value)
{
    value = be64toh(value);
}

template<class T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0, std::enable_if_t<sizeof(T) == 1, int> = 0>
void native_to_big_inplace(T& value)
{
}

template<class T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0, std::enable_if_t<sizeof(T) == 2, int> = 0>
void native_to_big_inplace(T& value)
{
    value = htobe16(value);
}

template<class T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0, std::enable_if_t<sizeof(T) == 4, int> = 0>
void native_to_big_inplace(T& value)
{
    value = htobe32(value);
}

template<class T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0, std::enable_if_t<sizeof(T) == 8, int> = 0>
void native_to_big_inplace(T& value)
{
    value = htobe64(value);
}
