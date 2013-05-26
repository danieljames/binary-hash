
// Copyright 2012 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is also released into the public domain.

#if !defined(BINARY_HASH_SIPHASH_HEADER)
#define BINARY_HASH_SIPHASH_HEADER

#include <boost/cstdint.hpp>
#include <binary_hash/binary_hash.hpp>

namespace binary_hash { namespace siphash_v1 {

    // Random key used to make sure the hash function isn't predictable.

    struct sipkey
    {
        boost::uint64_t k0, k1;
    };

    // Generate a secure sipkey.
    // This only works when boost::random_device is available.
    sipkey generate_sipkey();

    // This is the class that does the actual hashing.

    struct siphash_state
    {
        boost::uint64_t v0, v1, v2, v3;
        union {
            boost::uint64_t m;
            unsigned char buffer[8];
        };
        unsigned char buffered;
        unsigned char b;

        explicit siphash_state(sipkey const&);
        void update(void const*, unsigned);
        boost::uint64_t finalize();

    private:
        void sip_round(unsigned);
    };

    // The genereric hash function.
    //
    // Don't sepecialize this. Unless you really want to.

    template <typename T>
    struct siphash
    {
        sipkey key;

        /* implicit */ siphash(sipkey const& k) : key(k) {}
        std::size_t operator()(T const&) const;
    };

    // The implementation of the generic hash function.

    template <typename T>
    std::size_t siphash<T>::operator()(T const& x) const
    {
        siphash_state state(key);
        binary_hash::hash_impl<siphash_state, T>::update(state, x);
        return static_cast<std::size_t>(state.finalize());
    }

}}

namespace binary_hash {
    using siphash_v1::generate_sipkey;
    using siphash_v1::siphash;
}

#endif
