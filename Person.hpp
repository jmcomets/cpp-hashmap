#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>

class Person
{
    public:
        Person();

        Person(const std::string& name, int age, const std::string& email);

        bool operator==(const Person& other) const;

        bool operator!=(const Person& other) const;

        const std::string& getName() const;

        int getAge() const;

        const std::string& getEmail() const;

    private:
        std::string m_name;
        int m_age;
        std::string m_email;
};

#endif // PERSON_HPP
