#pragma once

#include "ASTNode.h"
#include <string>
#include <vector>

class FunctionDefinitionNode : public ASTNode
{
private:
    std::string m_name;
    std::vector<std::string> m_params;
    std::vector<std::shared_ptr<ASTNode>> m_body;

public:
    FunctionDefinitionNode(std::string name, std::vector<std::string> params, std::vector<std::shared_ptr<ASTNode>> body);
    ~FunctionDefinitionNode() = default;
    void print() override;
    void eval(Stack &stack) override;
    std::string &name();
};
