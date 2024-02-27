#include "ComparissionNode.h"
#include "interpreter/Stack.h"
#include <iostream>

ComparrisionNode::ComparrisionNode(CMPOperator op, const std::shared_ptr<ASTNode> &lhs,
                                   const std::shared_ptr<ASTNode> &rhs) : m_lhs(lhs), m_rhs(rhs), m_operator(op)
{
}

void ComparrisionNode::print()
{
    m_lhs->print();
    switch (m_operator)
    {
    case CMPOperator::EQUALS:
        std::cout << "=";
        break;
    case CMPOperator::GREATER:
        std::cout << ">";
        break;
    case CMPOperator::GREATER_EQUAL:
        std::cout << ">=";
        break;
    case CMPOperator::LESS:
        std::cout << "<";
        break;
    case CMPOperator::LESS_EQUAL:
        std::cout << "<=";
        break;
    default:
        break;
    }
    m_rhs->print();
}

void ComparrisionNode::eval(Stack &stack)
{
    m_lhs->eval(stack);
    m_rhs->eval(stack);
    auto lhs = stack.pop_front<int64_t>();
    auto rhs = stack.pop_front<int64_t>();
    switch (m_operator)
    {
    case CMPOperator::EQUALS:
        stack.push_back(static_cast<int64_t>(lhs == rhs));
        break;
    case CMPOperator::GREATER:
        stack.push_back(static_cast<int64_t>(lhs > rhs));
        break;
    case CMPOperator::GREATER_EQUAL:
        stack.push_back(static_cast<int64_t>(lhs >= rhs));
        break;
    case CMPOperator::LESS:
        stack.push_back(static_cast<int64_t>(lhs < rhs));
        break;
    case CMPOperator::LESS_EQUAL:
        stack.push_back(static_cast<int64_t>(lhs <= rhs));
        break;
    default:
        break;
    }
}