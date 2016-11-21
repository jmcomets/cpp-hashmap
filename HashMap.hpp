#ifndef HASH_MAP_HPP
#define HASH_MAP_HPP

typedef unsigned int Size;

namespace probing_methods {

template <typename K>
class Linear
{
    public:
        Size operator()(const K& key, Size hash, Size index) const;
};

template <typename K>
class Quadratic
{
    typedef K Key;

    public:
        Quadratic();

        Size operator()(const Key& key, Size hash, Size index);

    private:
        Size m_count;
};

template <typename H>
struct DoubleHashFactory
{
    template <typename K>
    class DoubleHash
    {
        typedef K Key;
        typedef H HashFunction;

        public:
            DoubleHash();

            Size operator()(const Key& key, Size hash, Size index);

        private:
            Size m_count;
    };
};

}

namespace detail {

template <typename K, typename V>
struct Bucket
{
    Bucket();

    K key;
    V value;
    bool empty;
    bool sentinel;
};

}

template <typename K, typename V, typename H, typename P = probing_methods::Linear<K> >
class HashTable
{
    typedef HashTable<K, V, H, P> Self;

    typedef H HashFunction;
    typedef P ProbingMethod;

    typedef detail::Bucket<K, V> Bucket;

    public:
        typedef K Key;
        typedef V Value;

        HashTable();

        ~HashTable();

        Value& operator[](const Key& key);

        /// Insert the (key, value) pair, overwriting the previous entry.
        ///
        /// @param[in] key The key of the pair.
        /// @param[in] value The value of the pair.
        ///
        /// @return If a previous association was overwritten.
        bool put(const Key& key, const Value& value);

        /// Remove the (key, value) pair given the key.
        ///
        /// @param[in] key The key of the pair.
        ///
        /// @return If a previous association was removed.
        bool remove(const Key& key);

        /// Get the value given the key if it exists.
        ///
        /// @param[in] key The key of the pair.
        ///
        /// @return The value if it exists, or NULL if it doesn't.
        const Value* get(const Key& key) const;

    private:
        std::pair<Size, bool> putWithReallocation(const Key& key, const Value& value);

        std::pair<Size, bool> putWithoutReallocation(const Key& key, const Value& value);

        float getLoadFactor() const;

        Size find(const Key& key) const;

        void maybeGrow();

        void maybeShrink();

        void reallocateAndCopy(Size newBucketCount);

    private:
        HashTable(const Self& other);
        Self& operator=(const Self& other);

    private:
        Bucket* m_buckets;
        Size m_bucketCount;
        Size m_fillCount;
        Size m_sentinelCount;
        float m_minLoadFactor;
        float m_maxLoadFactor;
        Size m_initialSize;
};

#include "HashMap.inl.hpp"

#endif // HASH_MAP_HPP
