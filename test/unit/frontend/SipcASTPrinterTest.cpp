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

TEST_CASE("ASTPrinterTest: basic array", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun() {
      var x, y, z;
        x = [];
        z = [1, 2, 3];
        return 0;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTArrayExpr>(ast);
  std::vector<std::string> expected{"[]", "[1,2,3]"};
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

TEST_CASE("ASTPrinterTest: of array", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun() {
      var x, y, z, i;
        y = [4 of i+x];
        x = [y-z of z+i]
        return 0;
      }
    )";

  std::vector<std::string> expected{
      "[(i+x),(i+x),(i+x),(i+x)]",
      "[(y-z) of (z+i)]"};

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

TEST_CASE("ASTPrinterTest: ternary statement", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun() {
      var a, b, c, d;
        a = b ? c : d;
        return 0;
      }
    )";

  std::vector<std::string> expected{
      "b ? c : d",
  };

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

TEST_CASE("ASTPrinterTest: array refrence expression", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun() {
      var a, b, c, d;
        a = [b, c, d];
        c = a[1];
        return 0;
      }
    )";

  std::vector<std::string> expected{
      "[b,c,d]",
      "a[1]",
  };

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

TEST_CASE("ASTPrinterTest: unary expressions", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun() {
      var a, b, c, d;
        a = b++;
        b = a--;
        c = -d;
        d = not c;
        return 0;
      }
    )";

  std::vector<std::string> expected{
      "(++b)",
      "(--a)",
      "(-d)",
      "(!c)"};

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

TEST_CASE("ASTPrinterTest: for loops", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun() {
      var x, y;
        for (y : 1 .. 10 by 2) {
            x = x - 1;
         }
        return 0;
      }
    )";

  std::vector<std::string> expected{
      "for (y : 1 .. 10 by 2) { x = (x-1); }"};

  auto ast = ASTHelper::build_ast(stream);

  auto f = ast->findFunctionByName("fun");

  int i = 0;
  int numStmts = 1;
  for (auto s : f->getStmts())
  {
    auto forLoopStmt = dynamic_cast<ASTForLoopStmt *>(s);
    stream = std::stringstream();
    stream << *forLoopStmt;
    auto actual = stream.str();
    REQUIRE(actual == expected.at(i++));
    if (i == numStmts)
      break;
  }
}

TEST_CASE("ASTPrinterTest: iter statement", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun(x) {
         var y, i;
         for (y : x) {
            i = i + (y - 1);
         }
         return i;
      }
    )";

  std::vector<std::string> expected{
      "for (y : x) { i = (i+(y-1)); }"};

  auto ast = ASTHelper::build_ast(stream);

  auto f = ast->findFunctionByName("fun");

  int i = 0;
  int numStmts = 1;
  for (auto s : f->getStmts())
  {
    auto iterStmt = dynamic_cast<ASTIterStmt *>(s);
    stream = std::stringstream();
    stream << *iterStmt;
    auto actual = stream.str();
    REQUIRE(actual == expected.at(i++));
    if (i == numStmts)
      break;
  }
}

TEST_CASE("ASTPrinterTest: inc/dec statement", "[ASTNodePrint]")
{
  std::stringstream stream;
  stream << R"(
      fun(x) {
         var y, i;
         i++;
         y--;
         return i;
      }
    )";

  std::vector<std::string> expected{
      "i = (i+1);", "y = (y-1);"};

  auto ast = ASTHelper::build_ast(stream);

  auto f = ast->findFunctionByName("fun");

  int i = 0;
  int numStmts = 2;
  for (auto s : f->getStmts())
  {
    auto incdecStmt = dynamic_cast<ASTAssignStmt *>(s);
    stream = std::stringstream();
    stream << *incdecStmt;
    auto actual = stream.str();
    REQUIRE(actual == expected.at(i++));
    if (i == numStmts)
      break;
  }
}