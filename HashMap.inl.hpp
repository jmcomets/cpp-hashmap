#ifndef HASH_MAP_INL_HPP
#define HASH_MAP_INL_HPP

#include "HashMap.hpp"

#include <algorithm>

namespace probing_methods {

template <typename K>
Size Linear<K>::operator()(const K&, Size, Size index) const
{
    return index + 1;
}

template <typename K>
Quadratic<K>::Quadratic():
    m_count(0)
{
}

template <typename K>
Size Quadratic<K>::operator()(const K&, Size, Size index)
{
    ++m_count;
    return index + m_count * m_count;
}

template <typename H>
template <typename K>
DoubleHashFactory<H>::DoubleHash<K>::DoubleHash():
    m_count(0)
{
}

template <typename H>
template <typename K>
Size DoubleHashFactory<H>::DoubleHash<K>::operator()(const Key& key, Size hash, Size)
{
    HashFunction hashFn;
    ++m_count;
    return hash + m_count * hashFn(key);
}

}

namespace detail {

template <typename K, typename V>
Bucket<K, V>::Bucket():
    empty(true),
    sentinel(false)
{
}

}

template <typename K, typename V, typename H, typename P>
HashTable<K, V, H, P>::HashTable():
    m_buckets(0),
    m_bucketCount(0),
    m_fillCount(0),
    m_sentinelCount(0),
    m_minLoadFactor(0.1f),
    m_maxLoadFactor(0.7f),
    m_initialSize(10)
{
    m_bucketCount = m_initialSize;
    m_buckets = new Bucket[m_bucketCount];
}

template <typename K, typename V, typename H, typename P>
HashTable<K, V, H, P>::~HashTable()
{
    delete [] m_buckets;
}

template <typename K, typename V, typename H, typename P>
V& HashTable<K, V, H, P>::operator[](const K& key)
{
    Size index = putWithReallocation(key, Value()).first;
    return m_buckets[index].value;
}

template <typename K, typename V, typename H, typename P>
bool HashTable<K, V, H, P>::put(const K& key, const V& value)
{
    return putWithReallocation(key, value).second;
}

template <typename K, typename V, typename H, typename P>
std::pair<Size, bool> HashTable<K, V, H, P>::putWithReallocation(const K& key, const V& value)
{
    maybeGrow();

    return putWithoutReallocation(key, value);
}

template <typename K, typename V, typename H, typename P>
std::pair<Size, bool> HashTable<K, V, H, P>::putWithoutReallocation(const K& key, const V& value)
{
    Size index = find(key);
    Bucket& bucket = m_buckets[index];

    // handle replacing previous buckets
    bool replaced = !bucket.empty && !bucket.sentinel;

    // insert a bucket
    bucket.key = key;
    bucket.value = value;
    bucket.empty = false;
    bucket.sentinel = false;

    if (!replaced)
    {
        ++m_fillCount;
    }

    return std::make_pair(index, replaced);
}

template <typename K, typename V, typename H, typename P>
bool HashTable<K, V, H, P>::remove(const K& key)
{
    if (m_fillCount == 0)
    {
        return false;
    }

    Size index = find(key);
    Bucket& bucket = m_buckets[index];
    if (!bucket.empty)
    {
        bucket.sentinel = true;
        ++m_sentinelCount;
        --m_fillCount;

        maybeGrow();

        return true;
    }
    else
    {
        return false;
    }
}

template <typename K, typename V, typename H, typename P>
const V* HashTable<K, V, H, P>::get(const K& key) const
{
    if (m_fillCount == 0)
    {
        return 0;
    }

    Size index = find(key);
    const Bucket& bucket = m_buckets[index];
    if (!bucket.empty)
    {
        return bucket.value;
    }
    else
    {
        return 0;
    }
}

template <typename K, typename V, typename H, typename P>
Size HashTable<K, V, H, P>::find(const K& key) const
{
    // look for an empty bucket
    HashFunction hashFn;
    ProbingMethod probeFn;
    Size hash = hashFn(key);
    Size index = hash % m_bucketCount;
    Bucket& bucket = m_buckets[index];
    while (!bucket.empty && (bucket.sentinel || bucket.key != key))
    {
        hash = probeFn(key, hash, index);
        index = hash % m_bucketCount;
        bucket = m_buckets[index];
    }
    return index;
}

template <typename K, typename V, typename H, typename P>
float HashTable<K, V, H, P>::getLoadFactor() const
{
    return static_cast<float>(m_fillCount + m_sentinelCount) / static_cast<float>(m_bucketCount);
}

template <typename K, typename V, typename H, typename P>
void HashTable<K, V, H, P>::maybeGrow()
{
    float loadFactor = getLoadFactor();
    if (loadFactor > m_maxLoadFactor)
    {
        Size doubled = m_bucketCount * 2;
        //std::cout << "growing to " << doubled << std::endl;

        reallocateAndCopy(doubled);
    }
}

template <typename K, typename V, typename H, typename P>
void HashTable<K, V, H, P>::maybeShrink()
{
    float loadFactor = getLoadFactor();
    if (loadFactor < m_minLoadFactor)
    {
        Size halfed = m_bucketCount / 2;
        Size fit = static_cast<Size>(m_fillCount / loadFactor);
        Size newBucketCount = loadFactor != 0 ? std::max(halfed, fit) : m_initialSize;
        //std::cout << "shrinking to " << newBucketCount << std::endl;

        reallocateAndCopy(newBucketCount);
    }
}

template <typename K, typename V, typename H, typename P>
void HashTable<K, V, H, P>::reallocateAndCopy(Size newBucketCount)
{
    Size oldBucketCount = m_bucketCount;
    Bucket* oldBuckets = m_buckets;

    m_bucketCount = newBucketCount;
    m_buckets = new Bucket[m_bucketCount];
    m_fillCount = 0;
    m_sentinelCount = 0;

    for (Size i = 0; i < oldBucketCount; ++i)
    {
        Bucket& bucket = oldBuckets[i];
        if (!bucket.empty && !bucket.sentinel)
        {
            putWithoutReallocation(bucket.key, bucket.value);
        }
    }
    delete [] oldBuckets;
}

#endif // HASH_MAP_INL_HPP
