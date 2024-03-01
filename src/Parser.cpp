#include "Parser.h"
#include "ast/PrintNode.h"
#include "ast/InputNode.h"
#include "ast/NumberNode.h"
#include "ast/StringConstantNode.h"
#include "ast/BinaryOperationNode.h"
#include "ast/ComparissionNode.h"
#include "ast/VariableAccessNode.h"
#include "ast/LogicalExpressionNode.h"
#include "ast/IfConditionNode.h"
#include "ast/VariableAssignmentNode.h"
#include "ast/FunctionDefinitionNode.h"
#include "ast/ReturnNode.h"
#include "ast/FunctionCallNode.h"
#include "Lexer.h"
#include <magic_enum/magic_enum.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
#include <iostream>
#include <algorithm>

Parser::Parser(const std::filesystem::path &path, std::vector<Token> &tokens) : m_file_path(path), m_tokens(tokens)
{
}

Parser::~Parser()
{
}

bool Parser::hasNext()
{
    return m_current < m_tokens.size() - 1;
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

void Parser::tryConsume(TokenType tokenType)
{
    if (canConsume(tokenType))
    {
        next();
    }
}
bool Parser::consume(TokenType tokenType)
{
    if (canConsume(tokenType))
    {
        next();
        return true;
    }
    else
    {
        m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = m_tokens[m_current + 1], .message = "expected token  '" + std::string(magic_enum::enum_name(tokenType)) + "' but found " + std::string(magic_enum::enum_name(m_tokens[m_current + 1].tokenType)) + "!"});
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
    case TokenType::MUL:
    {
        auto rhs = parseToken(next(), {});
        auto lhs = nodes[0];
        return std::make_shared<BinaryOperationNode>(Operator::MUL, lhs, rhs);
    }
    case TokenType::NAMEDTOKEN:
    {

        if (canConsume(TokenType::LEFT_CURLY))
        {
            auto functionName = token.lexical;
            if (std::find(m_known_function_names.begin(), m_known_function_names.end(), functionName) == std::end(m_known_function_names))
            {
                m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = token, .message = "a function with the name '" + std::string(token.lexical) + "' is not yet defined!"});
            }
            consume(TokenType::LEFT_CURLY);

            Token subToken = next();
            std::vector<std::shared_ptr<ASTNode>> callArgs;
            while (subToken.tokenType != TokenType::RIGHT_CURLY)
            {
                switch (subToken.tokenType)
                {
                case TokenType::NAMEDTOKEN:

                    if (std::find(m_known_variable_names.begin(), m_known_variable_names.end(), subToken.lexical) == std::end(m_known_variable_names))
                    {
                        m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = subToken, .message = "a variable with the name '" + std::string(subToken.lexical) + "' is not yet defined!"});
                        return nullptr;
                    }
                    callArgs.push_back(std::make_shared<VariableAccessNode>(subToken.lexical));
                    break;
                case TokenType::COMMA:
                    break;
                default:
                    callArgs.push_back(parseToken(subToken, {}));
                }

                subToken = next();
            }
            return std::make_shared<FunctionCallNode>(std::string(functionName), callArgs);
        }
        else
        {
            if (std::find(m_known_variable_names.begin(), m_known_variable_names.end(), token.lexical) == std::end(m_known_variable_names))
            {
                m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = token, .message = "a variable with the name '" + std::string(token.lexical) + "' is not yet defined!"});
                return nullptr;
            }
            return std::make_shared<VariableAccessNode>(token.lexical);
        }

        break;
    }
    default:
        m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = token, .message = std::string("token type '" + std::string(token.lexical) + "' not yet implemented")});
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
        tryConsume(TokenType::ENDLINE);
        nodes.push_back(std::make_shared<PrintNode>(args));
    }
    else if (currentToken.lexical == "error")
    {
        std::vector<std::shared_ptr<ASTNode>> args;
        Token token = next();
        while (token.tokenType != TokenType::ENDLINE && token.tokenType != TokenType::T_EOF && hasNext())
        {
            args.push_back(parseToken(token, {}));
            token = next();
        }
        tryConsume(TokenType::ENDLINE);
        nodes.push_back(std::make_shared<PrintNode>(args)); // TODO replace node
    }
    else if (currentToken.lexical == "input")
    {
        if (consume(TokenType::STRING))
        {
            auto string = parseToken(current(), {});
            if (consume(TokenType::NAMEDTOKEN))
            {
                m_known_variable_names.push_back((current().lexical));
                nodes.push_back(std::make_shared<InputNode>(string, current().lexical));
            }
            consume(TokenType::ENDLINE);
        }
        else
        {
            m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = currentToken, .message = "first argument of the input keyword is not a string constant"});
        }
    }
    else if (currentToken.lexical == "if")
    {
        consume(TokenType::LEFT_CURLY);
        auto conditionNode = parseExpression(next());
        // consume(TokenType::RIGHT_CURLY);
        // parse expression list
        std::vector<std::shared_ptr<ASTNode>> ifExpressions;
        std::vector<std::shared_ptr<ASTNode>> elseExpressions;
        if (canConsume(TokenType::ENDLINE))
        {
            // multi line expression
            bool ifEnded = false;
            bool elseFound = false;
            while (!ifEnded)
            {
                Token currentToken = next();
                switch (currentToken.tokenType)
                {
                case TokenType::KEYWORD:
                    if (currentToken.lexical == "else")
                    {
                        elseFound = true;
                        break;
                    }
                    else if (currentToken.lexical == "end")
                    {
                        ifEnded = true;
                        consume(TokenType::KEYWORD);
                        break;
                    }
                    parseKeyWord(currentToken, (elseFound) ? elseExpressions : ifExpressions);
                    break;
                case TokenType::NAMEDTOKEN:
                    parseVariableAssignment(currentToken, (elseFound) ? elseExpressions : ifExpressions);
                    break;
                case TokenType::ENDLINE:
                    break;
                default:
                    m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = currentToken, .message = "token type not yet implemented"});
                    break;
                }
            }
        }
        else
        {

            Token currentToken = next();
            switch (currentToken.tokenType)
            {
            case TokenType::KEYWORD:
                parseKeyWord(currentToken, ifExpressions);
                break;
            case TokenType::NAMEDTOKEN:
                parseVariableAssignment(currentToken, ifExpressions);
                break;
            case TokenType::ENDLINE:
                break;
            default:
                m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = currentToken, .message = "token type not yet implemented"});
                break;
            }
        }
        nodes.push_back(std::make_shared<IfConditionNode>(conditionNode, ifExpressions, elseExpressions));
    }
    else if (currentToken.lexical == "return")
    {
        auto returnExpression = parseExpression(next());
        nodes.push_back(std::make_shared<ReturnNode>(returnExpression));
    }
    else if (currentToken.lexical == "sub")
    {
        consume(TokenType::NAMEDTOKEN);
        auto functionName = std::string(current().lexical);
        m_known_function_names.push_back(functionName);

        consume(TokenType::LEFT_CURLY);
        auto token = next();
        std::vector<std::string> functionParams;
        while (token.tokenType != TokenType::RIGHT_CURLY)
        {
            switch (token.tokenType)
            {
            case TokenType::NAMEDTOKEN:
                functionParams.push_back(std::string(token.lexical));
                if (std::find(m_known_variable_names.begin(), m_known_variable_names.end(), token.lexical) == std::end(m_known_variable_names))
                    m_known_variable_names.push_back(token.lexical);
                break;
            case TokenType::COMMA:
                break;
            default:
                break;
            }

            token = next();
        }
        consume(TokenType::ENDLINE);
        // parse function body

        std::vector<std::shared_ptr<ASTNode>> functionBody;

        Token currentToken = current();
        bool endFound = false;
        while (currentToken.tokenType != TokenType::T_EOF && hasNext() && !endFound)
        {
            switch (currentToken.tokenType)
            {
            case TokenType::KEYWORD:
                if (currentToken.lexical == "end" && canConsume(TokenType::KEYWORD))
                {
                    currentToken = next();
                    if (currentToken.lexical == "sub")
                    {
                        endFound = true;
                        break;
                    }
                }
                parseKeyWord(currentToken, functionBody);
                break;
            case TokenType::NAMEDTOKEN:
                parseVariableAssignment(currentToken, functionBody);
                break;
            case TokenType::ENDLINE:
                break;
            default:
                m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = currentToken, .message = "token type not yet implemented"});
                break;
            }

            currentToken = next();
        }
        nodes.push_back(std::make_shared<FunctionDefinitionNode>(functionName, functionParams, functionBody));
    }
    else if (currentToken.lexical == "import")
    {
        if (consume(TokenType::NAMEDTOKEN))
        {
            auto filename = std::string(current().lexical) + ".yab";
            auto path = this->m_file_path.parent_path() / filename;
            Lexer lexer;
            std::ifstream file;
            std::istringstream is;
            file.open(path, std::ios::in);
            if (!file.is_open())
            {
                m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = current(), .message = std::string(current().lexical) + " is not a valid import"});
                m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = current(), .message = path.string() + " is not a valid yab file"});

                return;
            }
            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            std::string buffer(size, ' ');
            file.seekg(0);
            file.read(&buffer[0], size);

            auto tokens = lexer.tokenize(std::string_view{buffer});
            Parser parser(path, tokens);
            for (auto &node : parser.parseTokens())
            {
                nodes.push_back(node);
            }
            for (auto &name : parser.m_known_function_names)
            {
                m_known_function_names.push_back(name);
            }

            for (auto &error : parser.m_errors)
            {
                m_errors.push_back(error);
            }
        }
    }
    else
    {
        m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = currentToken, .message = "unknown token " + std::string(currentToken.lexical)});
    }
}

std::shared_ptr<ASTNode> Parser::parseComparrision(const Token &currentToken, std::vector<std::shared_ptr<ASTNode>> &nodes)
{

    CMPOperator op = CMPOperator::EQUALS;
    switch (currentToken.tokenType)
    {
    case TokenType::GREATER:
        if (consume(TokenType::EQUAL))
        {
            op = CMPOperator::GREATER_EQUAL;
        }
        else
        {
            op = CMPOperator::GREATER;
        }
        break;
    case TokenType::LESS:
        if (consume(TokenType::EQUAL))
        {
            op = CMPOperator::LESS_EQUAL;
        }
        else
        {
            op = CMPOperator::LESS;
        }
        break;
    case TokenType::EQUAL:
        break;
    default:
        m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = currentToken, .message = "unexpected token in comparrision"});
    }
    auto rhs = parseToken(next(), {});

    if (nodes.empty())
    {
        m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = currentToken, .message = "left hand side of the comparrision is missing"});
        return nullptr;
    }

    return std::make_shared<ComparrisionNode>(op, nodes[0], rhs);
}
std::shared_ptr<ASTNode> Parser::parseExpression(const Token &currentToken)
{
    // variant 1

    Token token = currentToken;
    std::vector<std::shared_ptr<ASTNode>> nodes;

    while (token.tokenType != TokenType::ENDLINE && token.tokenType != TokenType::T_EOF)
    {
        switch (token.tokenType)
        {
        case TokenType::STRING:
        case TokenType::NUMBER:
            nodes.push_back(parseToken(token, {}));
            break;
        case TokenType::NAMEDTOKEN:
        {

            auto lhs = parseToken(token, {});
            if (canConsume(TokenType::PLUS) || canConsume(TokenType::MUL) || canConsume(TokenType::MINUS))
            {
                lhs = parseToken(next(), {lhs});
            }

            nodes.push_back(lhs);
        }
        break;
        case TokenType::KEYWORD:
        {
            LogicalOperator op = LogicalOperator::OR;
            if (token.lexical == "not")
            {
                auto rhs = parseExpression(next());
                return std::make_shared<LogicalExpressionNode>(LogicalOperator::NOT, rhs);
            }
            else if (token.lexical == "and")
            {
                op = LogicalOperator::AND;
            }
            else if (token.lexical != "or")
            {
                m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = token, .message = "keyword '" + std::string(token.lexical) + "' is not allowed here!"});

                break;
            }
            auto lhs = nodes.at(0);
            auto rhs = parseExpression(next());
            auto cmp = std::make_shared<LogicalExpressionNode>(op, lhs, rhs);
            nodes.clear();
            nodes.push_back(cmp);
            break;
        }
        case TokenType::GREATER:
        case TokenType::LESS:
        case TokenType::EQUAL:
        {
            auto cmp = parseComparrision(token, nodes);
            nodes.clear();
            nodes.push_back(cmp);
            break;
        }

        default:
            break;
        }
        token = current();

        if (token.tokenType == TokenType::ENDLINE || token.tokenType == TokenType::T_EOF || token.tokenType == TokenType::RIGHT_CURLY)
            break;

        token = next();
    }
    if (nodes.empty())
        return nullptr;
    return nodes.at(0);
}

void Parser::parseVariableAssignment(const Token &currentToken, [[maybe_unused]] std::vector<std::shared_ptr<ASTNode>> &nodes)
{
    auto variableName = currentToken.lexical;

    if (!consume(TokenType::EQUAL))
    {
        m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = currentToken, .message = "missing assignment for varaible!"});
        return;
    }

    // parse expression
    auto expression = parseExpression(next());
    m_known_variable_names.push_back(variableName);
    nodes.push_back(std::make_shared<VariableAssignmentNode>(variableName, expression));
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
        case TokenType::NAMEDTOKEN:
            parseVariableAssignment(currentToken, nodes);
            break;
        case TokenType::ENDLINE:
            break;
        default:
            m_errors.push_back(ParserError{.file_name = m_file_path.string(), .token = currentToken, .message = "token type not yet implemented"});
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