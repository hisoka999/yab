#include "Parser.h"
#include "ast/PrintNode.h"
#include "ast/NumberNode.h"
#include "ast/StringConstantNode.h"
#include "ast/BinaryOperationNode.h"
#include <string>
#include <exception>
#include <iostream>

Parser::Parser(std::vector<Token> &tokens) : m_tokens(tokens)
{
}

Parser::~Parser()
{
}

bool Parser::hasNext()
{
    return m_current != m_tokens.size();
}

Token &Parser::current()
{
    return m_tokens[m_current];
}

Token &Parser::next()
{
    if (hasNext())
    {
        m_current++;
    }
    return current();
}
bool Parser::canConsume(TokenType tokenType)
{
    return m_tokens[m_current + 1].tokenType == tokenType;
}
bool Parser::consume(TokenType tokenType)
{
    if (canConsume(tokenType))
    {
        next();
        return true;
    }
    return false;
}

std::shared_ptr<ASTNode> Parser::parseToken(const Token &token, std::vector<std::shared_ptr<ASTNode>> nodes)
{
    std::vector<std::shared_ptr<ASTNode>> args;
    switch (token.tokenType)
    {
    case TokenType::NUMBER:
    {
        auto value = std::atoll(token.lexical.data());
        auto lhs = std::make_shared<NumberNode>(value);
        if (canConsume(TokenType::PLUS) || canConsume(TokenType::MINUS))
        {
            auto op = parseToken(next(), {lhs});
            return op;
        }
        return lhs;
        // check when ever the next token is a an operator
    }
    case TokenType::STRING:
    {
        return std::make_shared<StringConstantNode>(token.lexical);
    }
    case TokenType::PLUS:
    {
        auto rhs = parseToken(next(), {});
        auto lhs = nodes[0];
        return std::make_shared<BinaryOperationNode>(Operator::PLUS, lhs, rhs);
    }
    case TokenType::MINUS:
    {
        auto rhs = parseToken(next(), {});
        auto lhs = nodes[0];
        return std::make_shared<BinaryOperationNode>(Operator::MINUS, lhs, rhs);
    }
    default:
        m_errors.push_back(ParserError{.file_name = "helloworld.yab", .token = token, .message = std::string("token type '" + std::string(token.lexical) + "' not yet implemented")});
        break;
    }
    return nullptr;
}
void Parser::parseKeyWord(const Token &currentToken, std::vector<std::shared_ptr<ASTNode>> &nodes)
{
    if (currentToken.lexical == "print")
    {
        std::vector<std::shared_ptr<ASTNode>> args;
        Token token = next();
        while (token.tokenType != TokenType::ENDLINE && token.tokenType != TokenType::T_EOF && hasNext())
        {
            args.push_back(parseToken(token, {}));
            token = next();
        }

        nodes.push_back(std::make_shared<PrintNode>(args));
    }
    else
    {
        m_errors.push_back(ParserError{.file_name = "helloworld.yab", .token = currentToken, .message = "unknown token " + std::string(currentToken.lexical)});
    }
}

std::vector<std::shared_ptr<ASTNode>> Parser::parseTokens()
{
    std::vector<std::shared_ptr<ASTNode>> nodes;

    Token currentToken = current();

    while (currentToken.tokenType != TokenType::T_EOF && hasNext())
    {
        switch (currentToken.tokenType)
        {
        case TokenType::KEYWORD:
            parseKeyWord(currentToken, nodes);
            break;

        default:
            m_errors.push_back(ParserError{.file_name = "helloworld.yab", .token = currentToken, .message = "token type not yet implemented"});
            break;
        }

        currentToken = next();
    }

    return nodes;
}

bool Parser::hasError() const
{
    return !m_errors.empty();
}

void Parser::printErrors()
{
    for (auto &error : m_errors)
    {
        std::cerr << error.file_name << ":" << error.token.row << ":" << error.token.col << ": " << error.message << "\n";
    }
}