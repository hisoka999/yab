cmake_minimum_required(VERSION 3.8)

project(yab VERSION 0.1 DESCRIPTION "yet another basic.")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


file(GLOB SRC 
    src/ast/ASTNode.cpp
    src/ast/PrintNode.cpp
    src/ast/NumberNode.cpp
    src/ast/StringConstantNode.cpp
    src/ast/BinaryOperationNode.cpp
    src/ast/InputNode.cpp
    src/ast/ComparrisionNode.cpp
    src/ast/VariableAccessNode.cpp
    src/ast/LogicalExpressionNode.cpp
    src/ast/IfConditionNode.cpp
    src/ast/VariableAssignmentNode.cpp
    src/ast/FunctionDefinitionNode.cpp
    src/ast/ReturnNode.cpp
    src/ast/FunctionCallNode.cpp
    src/interpreter/Stack.cpp
    src/Lexer.cpp
    src/Parser.cpp
    main.cpp )
INCLUDE_DIRECTORIES("src")



# Let's set compiler-specific flags
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # G++
    add_compile_options(-Wall -Wextra -Werror)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    # MSVC
    target_compile_options(${PROJECT_NAME} PRIVATE /EHsc /MTd /W2 /c)

    # Set the DLLEXPORT variable to export symbols
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN_EXPORT)
endif()

add_executable(${PROJECT_NAME} "main.cpp" ${SRC} )
