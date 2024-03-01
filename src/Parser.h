#pragma once
#include <vector>
#include <memory>
#include "Lexer.h"
#include "ast/ASTNode.h"
#include <filesystem>

struct ParserError
{
    std::string file_name;
    Token token;
    std::string message;
};

class Parser
{
private:
    std::filesystem::path m_file_path;
    size_t m_current = 0;
    std::vector<Token> m_tokens;
    std::vector<ParserError> m_errors;
    std::vector<std::string_view> m_known_variable_names;
    std::vector<std::string> m_known_function_names;
    Token &next();
    Token &current();
    bool hasNext();
    bool consume(TokenType tokenType);
    void tryConsume(TokenType tokenType);
    bool canConsume(TokenType tokenType);
    std::shared_ptr<ASTNode> parseToken(const Token &token, std::vector<std::shared_ptr<ASTNode>> nodes);
    void parseKeyWord(const Token &currentToken, std::vector<std::shared_ptr<ASTNode>> &nodes);
    void parseVariableAssignment(const Token &currentToken, std::vector<std::shared_ptr<ASTNode>> &nodes);
    std::shared_ptr<ASTNode> parseComparrision(const Token &currentToken, std::vector<std::shared_ptr<ASTNode>> &nodes);
    std::shared_ptr<ASTNode> parseExpression(const Token &currentToken);

public:
    Parser(const std::filesystem::path &path, std::vector<Token> &tokens);
    ~Parser();
    bool hasError() const;
    void printErrors();

    std::vector<std::shared_ptr<ASTNode>> parseTokens();
};
