#include "Stack.h"

Stack::Stack() : data(), stackPointer(0)
{
}

Stack::~Stack()
{
}

void Stack::push_back(const int64_t value)
{
    data.push_back(value);
}

void Stack::push_back(const std::string_view &value)
{
    data.push_back(value);
}

void Stack::push_back(const StackObject &value)
{
    data.push_back(value);
}

StackObject Stack::pop_front()
{

    auto result = data[stackPointer];
    stackPointer++;
    return result;
}

void Stack::set_var(std::string &varName, const int64_t &value)
{
    m_variables[varName] = value;
}

void Stack::set_var(std::string &varName, const StackObject &value)
{
    m_variables[varName] = value;
}

void Stack::set_var(std::string &varName, const std::string_view &value)
{
    m_variables[varName] = value;
}

StackObject Stack::get_var(const std::string &varName)
{
    return m_variables[varName];
}