#include "ExceptionContainsWhat.h"
#include "FrontEnd.h"
#include "ParseError.h"
#include "ParserHelper.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SIP Parser: boolean assignment true", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x=true;
        y=false;
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean assignment false", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z;
        x=true;
        if (x) {
          y = y + 1
        } else {
          z = z + 1;
        }
        return z;
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

TEST_CASE("SIP Parser: boolean and operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z, a;
        x=true;
        y=true;
        z=4;
        if (x and y) {
          return z;
        } else {
          return a;
        }
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean and operator", "[SIP Parser]")
{
  std::stringstream stream;
  stream << R"(
      short() {
        var x, y, z, a;
        x=false;
        y=true;
        z=4;
        if (not x and y) {
          return z;
        } else {
          return a;
        }
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean and operator", "[SIP Parser]")
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
          return z;
        } else {
          return 0;
        }
      }
    )";

  REQUIRE(ParserHelper::is_parsable(stream));
}

TEST_CASE("SIP Parser: boolean or operator", "[SIP Parser]")
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

// TEST_CASE("SIP Parser: operators", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() {
//         var x;
//         x = y + 1;
//         x = y - 1;
//         x = y * 1;
//         x = y / 1;
//         x = -1;
//         x = 1 > 0;
//         x = 1 == 0;
//         x = 1 != 0;
//         return z;
//       }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: pointers", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() {
//         var x, y;
//         y = alloc 13;
//         x = &y;
//         *y = 42;
//         **x = 7;
//         x = null;
//         return *y;
//       }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: funs", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       foo(f, a) { return f(a); }
//       bar(x) { return x + 1; }
//       baz(y) { return foo(bar, y); }
//       main(z) { return baz(z); }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: decls", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       main() { var x; var y; var z; return 0; }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: parens", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       main() { return ((1 + 2) * 3) - ((((2 - 1)))); }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: io stmts", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       main() { var x; x = input; output x; error x; output x * x; error (x * x); return x; }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: records", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       r() { var x, y; x = {f1:0, f2:13}; y = x.f1; x.f2 = y + a; return x.f2; }
//       nr() { var x, y; x = {f1: {nf1:1, nf2:2}, f2:13}; y = x.f1.nf1; x.f1.nf2 = x.f2; return x.f1.nf1; }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: block stmts", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       main() { var x, y; { x = 0; { y = x + 1; } } return x + y; }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: identifiers and literals", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       main() { var __314, __; __314 = 00007; __ = 0000; return __; }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: dangling else", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var x; if (x==0) if (x==0) x = x + 1; else x = x-1; return x; }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: input", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var x; if (input) if (input) x = 1; else x = -1; return x; }
//       outin() { output input; return 0; }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: address of field access", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var x, y; y = &(x.f); return *y; }
//     )";

//   REQUIRE(ParserHelper::is_parsable(stream));
// }

// /* These tests checks for operator precedence.
//  * They access the parse tree and ensure that the higher precedence
//  * operator is nested more deeply than the lower precedence operator.
//  */
// TEST_CASE("SIP Parser: mul higher precedence than add", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(main() { return 1 + 2 * 3; })";
//   std::string expected = "(expr (expr 1) + (expr (expr 2) * (expr 3)))";
//   std::string tree = ParserHelper::parsetree(stream);
//   REQUIRE(tree.find(expected) != std::string::npos);
// }

// TEST_CASE("SIP Parser: access higher precedence than deref", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(main() { var p; return *p.f; })";
//   std::string expected = "(expr * (expr (expr p) . f))";
//   std::string tree = ParserHelper::parsetree(stream);
//   REQUIRE(tree.find(expected) != std::string::npos);
// }

// TEST_CASE("SIP Parser: fun app higher precedence than deref", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(main() { var p; return *p(); })";
//   std::string expected = "(expr * (expr (expr p) ( )))";
//   std::string tree = ParserHelper::parsetree(stream);
//   REQUIRE(tree.find(expected) != std::string::npos);
// }

// /************ The following are expected to fail parsing ************/

// TEST_CASE("SIP Parser: decl after stmt", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       main() { var x; x = 0; var z; return 0; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: missing semi-colon", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       short() { var x; if (x>0) x = x + 1 return 0; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: missing paren", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       short() { var x; if x>0 x = x + 1; return 0; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: unbalanced blocks", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       main() { var x, y; { x = 0; y = x + 1; } } return x + y; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: unbalanced binary expr", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var x; x = y + + 1; return -x; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: bad field delimiter", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var x; x = {a:0, b 0}; return x.a; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: bad field separator", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var x; x = {a:0 b:0}; return x.a; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: no expression statements", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var x, y; x = y = 1; return x; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Parser: keywords as ids", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       if() { var x; if (x <= 0) x = x + 1; return x; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Lexer: illegal comparison token", "[SIP Lexer]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var x; if (x <= 0) x = x + 1; return x; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Lexer: illegal operator token", "[SIP Lexer]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var x; if (x == 0) x = x % 2; return x; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Lexer: illegal identifier token", "[SIP Lexer]")
// {
//   std::stringstream stream;
//   stream << R"(
//       operators() { var $x; if ($x == 0) $x = $x + 2; return $x; }
//     )";

//   REQUIRE_FALSE(ParserHelper::is_parsable(stream));
// }

// TEST_CASE("SIP Lexer: Lexing exceptions are thrown", "[SIP Lexer]")
// {
//   std::stringstream stream;
//   stream << R"(
//       main() {
//         return ";
//       }
//     )";

//   REQUIRE_THROWS_MATCHES(FrontEnd::parse(stream), ParseError,
//                          ContainsWhat("token recognition error"));
// }

// TEST_CASE("SIP Parser: Parsing exceptions are thrown", "[SIP Parser]")
// {
//   std::stringstream stream;
//   stream << R"(
//       main() {
//         return 0
//       }
//     )";

//   REQUIRE_THROWS_MATCHES(FrontEnd::parse(stream), ParseError,
//                          ContainsWhat("missing ';'"));
// }
