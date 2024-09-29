#include "ExceptionContainsWhat.h"
#include "FrontEnd.h"
#include "ParseError.h"
#include "ParserHelper.h"

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <iostream>

/*
 * -----------------------------------------------------------------------------------------------------
 * Boolean constant (true/false) assignment and boolean unary and binary operators (not/and/or) tests
 * -----------------------------------------------------------------------------------------------------
 */
TEST_CASE("SIP Parser: boolean assignment true", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x;
        x=true;
        return x;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean assignment false", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x;
        x=false;
        return x;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean not operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x=false;
        if (not x) {
          y = y + 1;
        } else {
          z = z + 1;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean and operator both true", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x=true;
        y=true;
        if (x and y) {
          z = z + 1;
        } else {
          z = z - 1;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean and operator first false", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x=false;
        y=true;
        z=4;
        if (not x and y) {
          z = z + 1;
        } else {
          z = z - 1;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean and operator two ands", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z, a;
        x=false;
        y=true;
        z=4;
        a=2;
        if (not x and y and z>0) {
          while(y and z>0){
            z = z - 1;
          }
        } else {
          return a;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean or operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x=false;
        y=true;
        z=4;
        if (x or y) {
          z = z + 1;
        } else {
          z = 0;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean or operator two ors", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z, a;
        x=false;
        y=true;
        z=4;
        a=2;
        if (x or y or z>0) {
          while(y or z>0){
            z = z - 1;
          }
        } else {
          return a;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

/*
 * -----------------------------------------------------------------------------------------------------
 * Array instantiation and reference tests
 * -----------------------------------------------------------------------------------------------------
 */
TEST_CASE("SIP Parser: array assignment", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x;
        x=[1,2,3];
        return 0;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array assignment with identifiers", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x,y,z,a,b;
        y = 1;
        z = 2;
        a = 3;
        b = 4;
        x=[y,z,a,b];
        return 0;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array assignment with arrays", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x,y,z,a,b;
        y = [1,2,3];
        z = [4,5,6];
        a = [7,8,9];
        b = [10,11,12];
        x=[y,z,a,b];
        return 0;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array reference", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x=[1,2,3];
        y = x[0];
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array reference nontrivial", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x=[1,2,3];
        y = x[2];
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array empty", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x;
        x=[];
        return 0;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array empty length", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x;
        x = #[];
        return x;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array empty length assignment", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x=[];
        y = #x;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array non-empty length assignment", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x=[1, 2];
        y = #x;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: array non-empty length return", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x=[1, 2];
        return #x;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

/*
 * -----------------------------------------------------------------------------------------------------
 * Extended Arithmetic operator (% and negation) tests
 * -----------------------------------------------------------------------------------------------------
 */
TEST_CASE("SIP Parser: mod operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x=3%2;
        return x;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: mod operator with variables", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x=3;
        y = x % 2;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: negation operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x = -3;
        y = x + 2;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: negation operator with variables", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x = 3;
        y = -x + 2;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

/*
 * -----------------------------------------------------------------------------------------------------
 * Relational operator (<, <=, >=) tests
 * -----------------------------------------------------------------------------------------------------
 */
TEST_CASE("SIP Parser: less than operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x = 3;
        y = x < 2;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: less than or equal operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x = 3;
        y = x <= 2;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: greater than or equal operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x = 3;
        y = x >= 2;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

/*
 * -----------------------------------------------------------------------------------------------------
 * Ternary Condition expression operator tests
 * -----------------------------------------------------------------------------------------------------
 */
TEST_CASE("SIP Parser: ternary condition expression", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x = 3;
        y = 2;
        z = x > y ? x : y;
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: multiple ternary condition expression", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x = 3;
        y = 2;
        z = x > y ? x : y > 1 ? y : x;
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

/*
 * -----------------------------------------------------------------------------------------------------
 * Increment and Decrement operator tests
 * -----------------------------------------------------------------------------------------------------
 */
TEST_CASE("SIP Parser: increment operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x = 3;
        y = x++;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: decrement operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x = 3;
        y = x--;
        return y;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: increment operator in while loop", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x = 3;
        while(x < 10) {
          x++;
        }
        return x;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: decrement operator in while loop", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y;
        x = 10;
        while(x > 0) {
          x--;
        }
        return x;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

/*
 * -----------------------------------------------------------------------------------------------------
 * Iterator and For Loop statment tests
 * -----------------------------------------------------------------------------------------------------
 */
TEST_CASE("SIP Parser: iterator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        y = [1, 2, 3];
        for(x : y) {
          z = z + x;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: for loop", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var i, x, y, z;
        x = 0;
        y = 10;
        for(i : x .. y) {
          z = z + i;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: for loop with inrement", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var i, x, y, z;
        x = 0;
        y = 10;
        for(i : x .. y by 2) {
          z = z + i;
        }
        return z;
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

/*
 * -----------------------------------------------------------------------------------------------------
 * Operator precedence and parse tree expected result tests across the scenarios above
 * -----------------------------------------------------------------------------------------------------
 */
TEST_CASE("SIP Parser: precedence of boolean operators", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x = true;
        y = false;
        z = x and y or x;
        return z;
      }
    )";
  std::string expected = "(expr (expr (expr x) and (expr y)) or (expr x))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: precedence of boolean unary and binary operators 1", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x = true;
        y = false;
        z = not x and y or x;
        return z;
      }
    )";
  std::string expected = "(expr (expr (expr not (expr x)) and (expr y)) or (expr x))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: precedence of boolean unary and binary operators 2", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x = true;
        y = false;
        z = x and not y or x;
        return z;
      }
    )";
  std::string expected = "(expr (expr (expr x) and (expr not (expr y))) or (expr x))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: precedence of arithmetic operators with modulo", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x = 3;
        y = 2;
        z = x % y + 1;
        return z;
      }
    )";
  std::string expected = "(expr (expr (expr x) % (expr y)) + (expr 1))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: precedence of arithmetic operators with negation", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x = 3;
        y = 2;
        z = -x + y;
        return z;
      }
    )";
  std::string expected = "(expr (expr - (expr x)) + (expr y))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: precedence of relational operators with ternary condition expression", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        z = x > y ? x : y > 1 ? y : x;
      }
    )";
  std::string expected = "(expr (expr (expr x) > (expr y)) ? (expr x) : (expr (expr (expr y) > (expr 1)) ? (expr y) : (expr x)))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}

TEST_CASE("SIP Parser: precedence of relational operators with ternary condition expression involving double nested if", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        z = x > y ? y > 1 ? y : x : 0;
      }
    )";
  std::string expected = "(expr (expr x) > (expr y)) ? (expr (expr (expr y) > (expr 1)) ? (expr y) : (expr x)) : (expr 0))";
  std::string tree = ParserHelper::parsetree(stream);
  REQUIRE(tree.find(expected) != std::string::npos);
}