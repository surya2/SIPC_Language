#include "ASTHelper.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

TEST_CASE("ASTBinaryExprTest: Test modulo operator. ",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo() {
         var x;
         x = x % foo();
         return x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *expr->getRight();
  REQUIRE(o2.str() == "foo()");

  std::stringstream o3;
  o3 << expr->getOp();
  REQUIRE(o3.str() == "%");

  REQUIRE(expr->getChildren().size() == 2);
}

TEST_CASE("ASTBinaryExprTest: Test and operator. ",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo() {
         var x;
         x = x and foo();
         return x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *expr->getRight();
  REQUIRE(o2.str() == "foo()");

  std::stringstream o3;
  o3 << expr->getOp();
  REQUIRE(o3.str() == "&");

  REQUIRE(expr->getChildren().size() == 2);
}

TEST_CASE("ASTBinaryExprTest: Test or operator. ",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo() {
         var x;
         x = x or foo();
         return x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *expr->getRight();
  REQUIRE(o2.str() == "foo()");

  std::stringstream o3;
  o3 << expr->getOp();
  REQUIRE(o3.str() == "|");

  REQUIRE(expr->getChildren().size() == 2);
}

TEST_CASE("ASTBinaryExprTest: Test >= operator. ",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo() {
         var x;
         if(x >= foo()){
            x = 1;
         }
         return x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *expr->getRight();
  REQUIRE(o2.str() == "foo()");

  std::stringstream o3;
  o3 << expr->getOp();
  REQUIRE(o3.str() == ">=");

  REQUIRE(expr->getChildren().size() == 2);
}

TEST_CASE("ASTBinaryExprTest: Test < operator. ",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo() {
         var x;
         if(x < foo()){
            x = 1;
         }
         return x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *expr->getRight();
  REQUIRE(o2.str() == "foo()");

  std::stringstream o3;
  o3 << expr->getOp();
  REQUIRE(o3.str() == "<");

  REQUIRE(expr->getChildren().size() == 2);
}

TEST_CASE("ASTBinaryExprTest: Test <= operator. ",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo() {
         var x;
         if(x <= foo()){
            x = 1;
         }
         return x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBinaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLeft();
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *expr->getRight();
  REQUIRE(o2.str() == "foo()");

  std::stringstream o3;
  o3 << expr->getOp();
  REQUIRE(o3.str() == "<=");

  REQUIRE(expr->getChildren().size() == 2);
}

TEST_CASE("ASTTernaryExprTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(c) {
         var x, y, z;
         x = 7;
         y = 8;
         z = 9;
         return x > 5 ? y : z;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto ternaryExpr = ASTHelper::find_node<ASTTernaryExpr>(ast);
  REQUIRE(ternaryExpr != nullptr);

  REQUIRE(ternaryExpr->getChildren().size() == 3);
}

TEST_CASE("ASTBooleanExprTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo() {
         return false;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTBooleanExpr>(ast);

  REQUIRE(expr->getValue() == false);
}

TEST_CASE("ASTForStmtTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y;
         for (y : 1 .. 10 by 2) {
            x = x - 1;
         }
         return x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto stmt = ASTHelper::find_node<ASTForLoopStmt>(ast);

  std::stringstream o1;
  o1 << *stmt->getVar();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "y");
  std::stringstream o2;
  o2 << *stmt->getStart();
  // std::cout << *stmt->getStart() << std::endl;
  REQUIRE(o2.str() == "1");
  std::stringstream o3;
  o3 << *stmt->getEnd();
  // std::cout << *stmt->getEnd() << std::endl;
  REQUIRE(o3.str() == "10");
  std::stringstream o4;
  o4 << *stmt->getStep();
  REQUIRE(o4.str() == "2");

  std::stringstream o5;
  o5 << *stmt->getBody();
  REQUIRE(o5.str() == "{ x = (x-1); }");

  REQUIRE(stmt->getChildren().size() == 5);
}

TEST_CASE("ASTForStmtTest2: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y;
         for (y : 1 .. 10) {
            x = x - 1;
         }
         return x;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto stmt = ASTHelper::find_node<ASTForLoopStmt>(ast);

  std::stringstream o1;
  o1 << *stmt->getVar();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "y");
  std::stringstream o2;
  o2 << *stmt->getStart();
  // std::cout << *stmt->getStart() << std::endl;
  REQUIRE(o2.str() == "1");
  std::stringstream o3;
  o3 << *stmt->getEnd();
  // std::cout << *stmt->getEnd() << std::endl;
  REQUIRE(o3.str() == "10");

  REQUIRE(stmt->getStep() == nullptr);

  std::stringstream o5;
  o5 << *stmt->getBody();
  REQUIRE(o5.str() == "{ x = (x-1); }");

  REQUIRE(stmt->getChildren().size() == 4);
}

TEST_CASE("ASTIterStmtTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y, i;
         for (y : x) {
            i = i + (y - 1);
         }
         return i;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto stmt = ASTHelper::find_node<ASTIterStmt>(ast);

  std::stringstream o1;
  o1 << *stmt->getElement();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "y");
  std::stringstream o2;
  o2 << *stmt->getIterable();
  // std::cout << *stmt->getStart() << std::endl;
  REQUIRE(o2.str() == "x");

  std::stringstream o5;
  o5 << *stmt->getBody();
  REQUIRE(o5.str() == "{ i = (i+(y-1)); }");

  REQUIRE(stmt->getChildren().size() == 3);
}

TEST_CASE("ASTIncTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y;
         y = 2;
         i=y++;
         return y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTUnaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getExpr();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "y");

  REQUIRE(expr->getChildren().size() == 1);
}

TEST_CASE("ASTDecTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y;
         y = 2;
         i=x--;
         return y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTUnaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getExpr();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "x");

  REQUIRE(expr->getChildren().size() == 1);
}

TEST_CASE("ASTNotTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y;
         y = 2;
         x = not y;
         return y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTUnaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getExpr();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "y");

  REQUIRE(expr->getChildren().size() == 1);
}

TEST_CASE("ASTNegationTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y;
         x = x + 3;
         y = -x;
         return y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTUnaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getExpr();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "x");

  REQUIRE(expr->getChildren().size() == 1);
}

TEST_CASE("ASTArrayTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y;
         x = x + 3;
         y = [x, x+4, 6, 9+5];
         return y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTArrayExpr>(ast);

  REQUIRE(expr->getItems().size() == 4);
  REQUIRE(expr->getLen() == 4);

  std::vector<ASTExpr *> items = expr->getItems();
  std::stringstream o1;
  o1 << *items[0];
  REQUIRE(o1.str() == "x");

  std::stringstream o2;
  o2 << *items[1];
  REQUIRE(o2.str() == "(x+4)");

  std::stringstream o3;
  o3 << *items[2];
  REQUIRE(o3.str() == "6");

  std::stringstream o4;
  o4 << *items[3];
  REQUIRE(o4.str() == "(9+5)");

  REQUIRE(expr->getChildren().size() == expr->getLen());
}

TEST_CASE("ASTArrayOfTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y, i;
         i = x + 3;
         y = [4 of i + x];
         return y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTArrayExpr>(ast);

  REQUIRE(expr->getItems().size() == 4);
  REQUIRE(expr->getLen() == 4);

  std::vector<ASTExpr *> items = expr->getItems();
  std::stringstream o1;
  o1 << *items[0];
  REQUIRE(o1.str() == "(i+x)");

  std::stringstream o2;
  o2 << *items[1];
  REQUIRE(o2.str() == "(i+x)");

  std::stringstream o3;
  o3 << *items[2];
  REQUIRE(o3.str() == "(i+x)");

  std::stringstream o4;
  o4 << *items[3];
  REQUIRE(o4.str() == "(i+x)");

  REQUIRE(expr->getChildren().size() == expr->getLen());
}

TEST_CASE("ASTArrayOfTest2: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y, i;
         i = x + 3;
         y = [x + 4 of i + x];
         return y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTArrayOfExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getLength();
  REQUIRE(o1.str() == "(x+4)");

  std::stringstream o2;
  o2 << *expr->getElement();
  REQUIRE(o2.str() == "(i+x)");

  REQUIRE(expr->getChildren().size() == 2);
}

TEST_CASE("ASTLenTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y, i;
         i = x + 3;
         y = [x + 4 of i + x];
         return #y;
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTUnaryExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getExpr();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "y");

  std::stringstream o2;
  o2 << expr->getOp();
  REQUIRE(o2.str() == "#");

  REQUIRE(expr->getChildren().size() == 1);
}

TEST_CASE("ASTArrayRefTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y, i;
         i = x + 3;
         y = [x + 4 of i + x];
         return y[4];
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTArrayRefExpr>(ast);

  std::stringstream o1;
  o1 << *expr->getArray();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "y");

  std::stringstream o2;
  o2 << *expr->getIndex();
  REQUIRE(o2.str() == "4");

  REQUIRE(expr->getChildren().size() == 2);
}

TEST_CASE("ASTIncDecStmtTest: Test methods of AST subtype.",
          "[ASTNodes]")
{
  std::stringstream stream;
  stream << R"(
      foo(x) {
         var y, i;
         i++;
         return y[4];
      }
    )";

  auto ast = ASTHelper::build_ast(stream);
  auto expr = ASTHelper::find_node<ASTAssignStmt>(ast);

  std::stringstream o1;
  o1 << *expr->getLHS();
  // std::cout << *stmt->getVar() << std::endl;
  REQUIRE(o1.str() == "i");

  // std::stringstream o2;
  auto o2 = dynamic_cast<ASTBinaryExpr *>(expr->getRHS());
  std::stringstream o3;
  o3 << *o2->getLeft();
  std::stringstream o4;
  o4 << *o2->getRight();
  std::stringstream o5;
  o5 << o2->getOp();
  std::cout << o3.str() << std::endl;
  REQUIRE(o3.str() == "i");
  REQUIRE(o4.str() == "1");
  REQUIRE(o5.str() == "+");

  REQUIRE(expr->getChildren().size() == 2);
}
