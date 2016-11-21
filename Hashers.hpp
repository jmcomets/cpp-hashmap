#ifndef HASHERS_HPP
#define HASHERS_HPP

#include <string>
#include "Person.hpp"

template <typename Hash>
struct NameHasher
{
    Size operator()(const Person& person) const
    {
        return Hash()(person.getName());
    }
};

template <typename Hash>
struct EmailHasher
{
    Size operator()(const Person& person) const
    {
        return Hash()(person.getEmail());
    }
};

class StringHasher
{
    static const unsigned int P0 = 31;
    static const unsigned int P1 = 54059;
    static const unsigned int P2 = 76963;
    static const unsigned int P3 = 86969;

    public:
        unsigned int operator()(const std::string& str) const
        {
            unsigned int h = P0;
            for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
            {
                char c = *it;
                h = (h * P1) ^ (c * P2);
            }
            return h;
        }
};

#endif // HASHERS_HPP
