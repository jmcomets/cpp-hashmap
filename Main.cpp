#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <map>
#include <memory>

#include "HashMap.hpp"
#include "Hashers.hpp"

using namespace probing_methods;

typedef NameHasher<StringHasher> H1;
typedef EmailHasher<StringHasher> H2;

namespace std {

template <>
struct hash<Person>
{
    std::size_t operator()(const Person& person) const
    {
        return H1()(person);
    }
};

}

template <typename K, typename V>
class Map
{
    public:
        virtual void put(const K& key, const V& value) = 0;
};

template <typename M, typename K, typename V>
class MapImpl : public Map<K, V>
{
    public:
        void put(const K& key, const V& value)
        {
            m_impl[key] = value;
        }

    private:
        M m_impl;
};

typedef std::unordered_map<Person, int> StandardMap;
typedef MapImpl<StandardMap, Person, int> StandardMapImpl;

typedef HashTable<Person, int, H1, Linear<Person> > CustomMapWithLinearHashing;
typedef HashTable<Person, int, H1, Quadratic<Person> > CustomMapWithQuadraticHashing;
typedef HashTable<Person, int, H1, DoubleHashFactory<H2>::DoubleHash<Person> > CustomMapWithDoubleHashing;

typedef MapImpl<CustomMapWithLinearHashing, Person, int> CustomMapWithLinearHashingImpl;
typedef MapImpl<CustomMapWithQuadraticHashing, Person, int> CustomMapWithQuadraticHashingImpl;
typedef MapImpl<CustomMapWithDoubleHashing, Person, int> CustomMapWithDoubleHashingImpl;

template <typename OutputIterator>
void split(const std::string& str, char delimiter, OutputIterator outputIterator)
{
    std::istringstream iss(str);
    for(std::string item; std::getline(iss, item, delimiter); )
    {
        *outputIterator++ = item;
    }
}

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "no dictionary given" << std::endl;
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    std::ifstream ifs(filename);
    if (!ifs.good())
    {
        std::cerr << "could not open file " << filename << std::endl;
        return EXIT_FAILURE;
    }

    typedef std::unique_ptr<Map<Person, int> > MapUPtr;
    std::map<std::string, MapUPtr> maps;
    maps["standard"] = MapUPtr(new StandardMapImpl());
    maps["linear"] = MapUPtr(new CustomMapWithLinearHashingImpl());
    maps["quadratic"] = MapUPtr(new CustomMapWithQuadraticHashingImpl());
    maps["double"] = MapUPtr(new CustomMapWithDoubleHashingImpl());

    for (std::string line; std::getline(ifs, line, '\n'); )
    {
        std::vector<std::string> parts;
        split(line, ';', std::back_inserter(parts));

        // ignore invalid lines
        if (parts.size() != 3 || parts[0].empty() || parts[1].empty() || parts[2].empty())
        {
            std::cerr << "ignoring line: " << line << std::endl;
            continue;
        }

        // convert data
        std::stringstream ss;
        ss << parts[0] << parts[1] << parts[2];
        std::string name;   ss >> name;
        int age;            ss >> age;
        std::string email;  ss >> email;

        Person person(name, age, email);

        //std::cout << name << " -> " << email << std::endl;

        for (auto& kv : maps)
        {
            auto& label = kv.first;
            auto& m = *kv.second;

            auto t1 = std::chrono::high_resolution_clock::now();
            m.put(person, 42);
            auto t2 = std::chrono::high_resolution_clock::now();

            // integral duration: requires duration_cast
            auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
            std::cout << label << ' ' << d.count() << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
