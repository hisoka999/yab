#include "FunctionDefinitionNode.h"
#include <iostream>
#include "interpreter/Stack.h"

FunctionDefinitionNode::FunctionDefinitionNode(std::string name, std::vector<std::string> params, std::vector<std::shared_ptr<ASTNode>> body)
    : m_name(name), m_params(params), m_body(body)
{
}

void FunctionDefinitionNode::print()
{
    std::cout << "sub " + m_name + "(";
    for (auto &param : m_params)
    {
        std::cout << param + ",";
    }
    std::cout << ")\n";
    for (auto &node : m_body)
    {
        node->print();
    }
    std::cout << "end sub\n";
}

void FunctionDefinitionNode::eval([[maybe_unused]] Stack &stack)
{
    for (auto param : m_params)
    {
        auto value = stack.pop_front();
        stack.set_var(param, value);
    }
    for (auto &node : m_body)
    {
        node->eval(stack);
    }
}

std::string &FunctionDefinitionNode::name()
{
    return m_name;
}