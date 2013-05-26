
// Copyright 2012 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is also released into the public domain.

#if !defined(BINARY_HASH_BINARY_HASH_HEADER)
#define BINARY_HASH_BINARY_HASH_HEADER

#include <boost/cstdint.hpp>
#include <boost/utility/enable_if.hpp>

#include <string>
#include <vector>
#include <list>

namespace binary_hash { namespace binary_hash_v1 {

    // Add support for a type by specializing this class.
    //
    // 'Enable' is used as a SFINAE hook.

    template <typename State, typename T, typename Enable = void>
    struct hash_impl
    {
        static void update(State&, T const&);
    };


    // A couple of basic traits for hashing binary data.

    struct enable_hash_false { enum { value = false }; };
    struct enable_hash_true { enum { value = true }; };

    template <typename T>
    struct enable_hash_binary : enable_hash_false {};

    template <typename T>
    struct enable_hash_binary_array
    {
        enum { value = enable_hash_binary<T>::value &&
            sizeof(T[2]) == sizeof(T) * 2 };
    };

    // Some general purpose hash implementations, hash_impl<T>
    // can inherit from these.

    template <typename State, typename T>
    struct hash_binary_impl
    {
        static void update(State& state, int x)
        {
            state.update(&x, sizeof(x));
        }
    };

    template <typename State, typename T>
    struct hash_container_impl
    {
        static void update(State& state, T const& x)
        {
            hash_impl<State, typename T::value_type> value_impl;

            for (typename T::const_iterator begin = x.begin(),
                end = x.end(); begin != end; ++begin)
            {
                value_impl.update(state, *begin);
            }
        }
    };

    template <typename State, typename T, bool Enable = enable_hash_binary_array<T>::value>
    struct hash_binary_container_impl;

    template <typename State, typename T>
    struct hash_binary_container_impl<State, T, false> :
        hash_container_impl<State, T> {};

    template <typename State, typename T>
    struct hash_binary_container_impl<State, T, true>
    {
        static void update(State& state, T const& x)
        {
            state.update(&*x.cbegin(), sizeof(T) * x.size());
        }
    };

    // Specialize hash_impl for various types.

    template <typename State, typename T>
    struct hash_impl<State, T,
        typename boost::enable_if_c<enable_hash_binary<T>::value>::type
    > : hash_binary_impl<State, T> {};

    template <typename State, typename T, typename Alloc>
    struct hash_impl<State, std::list<T, Alloc> > :
        hash_container_impl<State, T> {};

    template <typename State, typename T, typename Alloc>
    struct hash_impl<State, std::vector<T, Alloc> > :
        hash_binary_container_impl<State, T> {};

    template <typename State, typename T, typename Alloc>
    struct hash_impl<State, std::basic_string<T, std::char_traits<T>, Alloc> > :
        hash_binary_container_impl<State, T> {};

    // Specialize the binary trait for builtin types.

    template <> struct enable_hash_binary<bool> :
        enable_hash_true {};
    template <> struct enable_hash_binary<char> :
        enable_hash_true {};
    template <> struct enable_hash_binary<unsigned char> :
        enable_hash_true {};
    template <> struct enable_hash_binary<signed char> :
        enable_hash_true {};
    template <> struct enable_hash_binary<short> :
        enable_hash_true {};
    template <> struct enable_hash_binary<unsigned short> :
        enable_hash_true {};
    template <> struct enable_hash_binary<int> :
        enable_hash_true {};
    template <> struct enable_hash_binary<unsigned int> :
        enable_hash_true {};
    template <> struct enable_hash_binary<long> :
        enable_hash_true {};
    template <> struct enable_hash_binary<unsigned long> :
        enable_hash_true {};

#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    template <> struct enable_hash_binary<wchar_t> :
        enable_hash_true {};
#endif

#if !defined(BOOST_NO_LONG_LONG)
    template <> struct enable_hash_binary<boost::long_long_type> :
        enable_hash_true {};
    template <> struct enable_hash_binary<boost::ulong_long_type> :
        enable_hash_true {};
#endif

#if defined(BOOST_HAS_INT128)
    template <> struct enable_hash_binary<boost::int128_type> :
        boost::hash_detail::enable_hash_value {};
    template <> struct enable_hash_binary<boost::uint128_type> :
        boost::hash_detail::enable_hash_value {};
#endif

}}

namespace binary_hash {
    using binary_hash_v1::hash_impl;
}

#endif
