#pragma once
#include <vector>
#include <memory>
#include "Lexer.h"
#include "ast/ASTNode.h"

struct ParserError
{
    std::string_view file_name;
    Token token;
    std::string message;
};

class Parser
{
private:
    size_t m_current = 0;
    std::vector<Token> m_tokens;
    std::vector<ParserError> m_errors;
    Token &next();
    Token &current();
    bool hasNext();
    bool consume(TokenType tokenType);
    bool canConsume(TokenType tokenType);
    std::shared_ptr<ASTNode> parseToken(const Token &token, std::vector<std::shared_ptr<ASTNode>> nodes);
    void parseKeyWord(const Token &currentToken, std::vector<std::shared_ptr<ASTNode>> &nodes);

public:
    Parser(std::vector<Token> &tokens);
    ~Parser();
    bool hasError() const;
    void printErrors();

    std::vector<std::shared_ptr<ASTNode>> parseTokens();
};
