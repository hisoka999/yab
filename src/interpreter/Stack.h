#pragma once

#include <vector>
#include <cstdint>
#include <variant>
#include <string>

typedef std::variant<std::string_view, int64_t> StackObject;

class Stack
{
private:
    std::vector<StackObject> data;
    size_t stackPointer;

public:
    Stack(/* args */);
    ~Stack();
    void push_back(const std::string_view &value);
    void push_back(const int64_t value);
    StackObject pop_front();
    template <typename T>
    T pop_front()
    {
        auto value = pop_front();
        return std::get<T>(value);
    }
};