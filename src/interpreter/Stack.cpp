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

StackObject Stack::pop_front()
{

    auto result = data[stackPointer];
    stackPointer++;
    return result;
}