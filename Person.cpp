#include "Person.hpp"

Person::Person()
{
}

Person::Person(const std::string& name, int age, const std::string& email):
    m_name(name),
    m_age(age),
    m_email(email)
{
}

bool Person::operator==(const Person& other) const
{
    return m_name == other.m_name
        && m_age == other.m_age
        && m_email == other.m_email;
}

bool Person::operator!=(const Person& other) const
{
    return !(*this == other);
}

const std::string& Person::getName() const
{
    return m_name;
}

int Person::getAge() const
{
    return m_age;
}

const std::string& Person::getEmail() const
{
    return m_email;
}
