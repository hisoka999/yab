#include "BinaryOperationNode.h"
#include <iostream>
#include "interpreter/Stack.h"

BinaryOperationNode::BinaryOperationNode(Operator op, const std::shared_ptr<NumberNode> &lhs,
                                         const std::shared_ptr<NumberNode> &rhs) : ASTNode(), m_lhs(lhs), m_rhs(rhs), m_operator(op)
{
}

void BinaryOperationNode::print()
{
    m_lhs->print();
    std::cout << static_cast<char>(m_operator);
    m_rhs->print();
}

void BinaryOperationNode::eval(Stack &stack)
{
    m_lhs->eval(stack);
    m_rhs->eval(stack);
    auto lhs = stack.pop_front<int64_t>();
    auto rhs = stack.pop_front<int64_t>();
    stack.push_back(lhs + rhs);
}