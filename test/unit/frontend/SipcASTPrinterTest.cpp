#include "ASTHelper.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

TEST_CASE("ASTPrinterTest: new relational operators", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun() {
        var x, y, z;
        if (x >= y) x = 0;
        if (x <= y) x = 0;
        if (x < y) x = 0;
        return 0;
      }
    )";

  std::vector<std::string> expected{"(x>=y)", "(x<=y)", "(x<y)"};

  auto ast = ASTHelper::build_ast(stream);

  auto f = ast->findFunctionByName("fun");

  int i = 0;
  int numStmts = f->getStmts().size() - 1; // skip the return
  for (auto s : f->getStmts())
  {
    auto ifstmt = dynamic_cast<ASTIfStmt *>(s);
    stream = std::stringstream();
    stream << *ifstmt->getCondition();
    auto actual = stream.str();
    REQUIRE(actual == expected.at(i++));
    if (i == numStmts)
      break;
  }
}

TEST_CASE("ASTPrinterTest: booleans", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun() {
        var x, y, z;
        x = true;
        y = false;
        return 0;
      }
    )";

  std::vector<std::string> expected{"1", "0"};

  auto ast = ASTHelper::build_ast(stream);

  auto f = ast->findFunctionByName("fun");

  int i = 0;
  int numStmts = f->getStmts().size() - 1; // skip the return
  for (auto s : f->getStmts())
  {
    auto assignStmt = dynamic_cast<ASTAssignStmt *>(s);
    stream = std::stringstream();
    stream << *assignStmt->getRHS();
    auto actual = stream.str();
    REQUIRE(actual == expected.at(i++));
    if (i == numStmts)
      break;
  }
}

TEST_CASE("ASTPrinterTest: and+or", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun() {
        var x, y, z;
        x = y and z;
        y = x or z;
        return 0;
      }
    )";

  std::vector<std::string> expected{"(y&z)", "(x|z)"};

  auto ast = ASTHelper::build_ast(stream);

  auto f = ast->findFunctionByName("fun");

  int i = 0;
  int numStmts = f->getStmts().size() - 1; // skip the return
  for (auto s : f->getStmts())
  {
    auto assignStmt = dynamic_cast<ASTAssignStmt *>(s);
    stream = std::stringstream();
    stream << *assignStmt->getRHS();
    auto actual = stream.str();
    REQUIRE(actual == expected.at(i++));
    if (i == numStmts)
      break;
  }
}
