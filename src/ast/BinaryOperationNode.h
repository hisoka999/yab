#include "ASTNode.h"
#include "NumberNode.h"

enum class Operator : char
{
    PLUS = '+',
    MINUS = '-'
};

class BinaryOperationNode : public ASTNode
{
private:
    std::shared_ptr<NumberNode> m_lhs;
    std::shared_ptr<NumberNode> m_rhs;
    Operator m_operator;

public:
    BinaryOperationNode(Operator op, const std::shared_ptr<NumberNode> &lhs,
                        const std::shared_ptr<NumberNode> &rhs);
    ~BinaryOperationNode() = default;

    void print() override;
    void eval(Stack &stack) override;
};