#include "ASTHelper.h"
#include "TypeHelper.h"
#include "SymbolTable.h"
#include "TypeConstraintCollectVisitor.h"
#include "Unifier.h"
#include "TipFunction.h"
#include "TipRef.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <set>
#include <sstream>

/*
 * Run the front-end on the program, collect the type constraints, solve the constraints
 * and return the unifier storing the inferred types for the variables in the program.
 * This code expects that no type errors are present and throws an exception otherwise.
 */
static std::pair<Unifier, std::shared_ptr<SymbolTable>> collectAndSolve(std::stringstream &program)
{
  auto ast = ASTHelper::build_ast(program);
  auto symbols = SymbolTable::build(ast.get());

  TypeConstraintCollectVisitor visitor(symbols.get());
  ast->accept(&visitor);

  auto collected = visitor.getCollectedConstraints();

  Unifier unifier(collected);
  REQUIRE_NOTHROW(unifier.solve());

  return std::make_pair(unifier, symbols);
}

/*
 * Run the front-end on the program, collect the type constraints, solve the constraints
 */

TEST_CASE("TypeConstraintVisitor: input, const, arithmetic, return type",
          "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
            // [[x]] = int, [[y]] = int, [[test]] = () -> int
            test() {
              var x, y;
              x = input;
              y = 3 + x;
              return y;
            }
         )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("test");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());

  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::intType());
}

TEST_CASE("TypeConstraintVisitor: booleans, return type",
          "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
            // [[x]] = bool, [[y]] = bool, [[test]] = () -> bool
            test() {
              var x, y, z;
              x = true;
              y = x or false;
              z = x and y;
              return z;
            }
         )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;
  std::vector<std::shared_ptr<TipType>> empty;
  auto fDecl = symbols->getFunction("test");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::boolType()));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::boolType());

  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::boolType());
}

TEST_CASE("TypeConstraintVisitor: alloc, deref, assign through ptr",
          "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
            // [[x]] = int, [[y]] = ptr to int, [[test]] = () -> ptr to int
            test() {
                var x,y,z;
                x = input;
                y = alloc x;
                *y = x;
                return y;
            }
         )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("test");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::ptrType(TypeHelper::intType())));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());

  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::ptrType(TypeHelper::intType()));
}

TEST_CASE("TypeConstraintVisitor: function reference, address of",
          "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
      // [[foo]] = [[x]] = () -> int), [[y]] = ptr to () -> int
      foo() {
        var x, y;
        x = foo;
        y = &x;
        return 13;
      }
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *unifier.inferred(fType));

  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::ptrType(TypeHelper::funType(empty, TypeHelper::intType())));
}

TEST_CASE("TypeConstraintVisitor: relop, if ", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
      // [[x]] = int, [[y]] = int, [[test]] = (int) -> int
      test(x) {
        var y;
        if (x > 0) {
          y = 0;
        } else {
          y = 1;
        }
        return y;
      }
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> oneInt{TypeHelper::intType()};

  auto fDecl = symbols->getFunction("test");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(oneInt, TypeHelper::intType()));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());

  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::intType());
}

TEST_CASE("TypeConstraintVisitor: while ", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
      // [[x]] = int, [[test]] = (int) -> int
      test(x) {
        while (x > 0) {
          x = x - 1;
        }
        return x;
      }
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> oneInt{TypeHelper::intType()};

  auto fDecl = symbols->getFunction("test");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(oneInt, TypeHelper::intType()));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());
}

TEST_CASE("TypeConstraintVisitor: error, output", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
      // [[x]] = int, [[y]] = int, [[test]] = (int,int) -> int
      test(x, y) {
        output x;
        error y;
        return 0;
      }
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> twoInt{TypeHelper::intType(), TypeHelper::intType()};

  auto fDecl = symbols->getFunction("test");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(twoInt, TypeHelper::intType()));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());

  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::intType());
}

TEST_CASE("TypeConstraintVisitor: funs with params",
          "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
      // [[x]] = int, [[foo]] = (int) -> int
      foo(x) {
        return x;
      }
      // [[bar]] = ()->int
      bar() {
        return foo(7);
      }
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> oneInt{TypeHelper::intType()};
  std::vector<std::shared_ptr<TipType>> empty;

  auto fooDecl = symbols->getFunction("foo");
  auto fooType = std::make_shared<TipVar>(fooDecl);
  REQUIRE(*unifier.inferred(fooType) == *TypeHelper::funType(oneInt, TypeHelper::intType()));

  auto barDecl = symbols->getFunction("bar");
  auto barType = std::make_shared<TipVar>(barDecl);
  REQUIRE(*unifier.inferred(barType) == *TypeHelper::funType(empty, TypeHelper::intType()));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fooDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());
}

TEST_CASE("TypeConstraintVisitor: main", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
      // [[x]] = int, [[foo]] = (int) -> int
      main(x) {
        return x;
      }
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> oneInt{TypeHelper::intType()};

  auto fDecl = symbols->getFunction("main");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(oneInt, TypeHelper::intType()));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());
}

TEST_CASE("TypeConstraintVisitor: polymorphic type inference",
          "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
      // [[p]] = ptr to \alpha, [[deref]] = (ptr to \alpha) -> \alpha
      deref(p) {
        return *p;
      }
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  auto fDecl = symbols->getFunction("deref");
  auto pDecl = symbols->getLocal("p", fDecl);

  std::vector<std::shared_ptr<TipType>> onePtrToAlpha{TypeHelper::ptrType(TypeHelper::alphaType(pDecl))};

  // Equality on alpha type variables considers the actual AST node used to generate the alpha, but we
  // only want to check that the types are some alpha -- we don't care which one.  This is a bit clunky.
  auto fType = std::make_shared<TipVar>(fDecl);
  auto funType = std::dynamic_pointer_cast<TipFunction>(unifier.inferred(fType));
  REQUIRE(funType != nullptr); // needs to be a function type

  // return type is an alpha
  auto returnType = funType->getReturnType();
  REQUIRE(Unifier::isAlpha(returnType));

  // argument type is pointer to an alpha
  auto refType = std::dynamic_pointer_cast<TipRef>(funType->getParamTypes()[0]);
  REQUIRE(refType != nullptr);
  REQUIRE(Unifier::isAlpha(refType->getReferencedType()));

  // Now we want the argument p to have the same type as the parameter type
  auto pType = std::make_shared<TipVar>(pDecl);
  REQUIRE(*unifier.inferred(pType) == *refType);
}

TEST_CASE("TypeConstraintVisitor: access expr", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
      // [[main]] = ()->int, [[r]] = {f:int, g:int}
      main() {
          var r;
          r = {f: 4, g: 13};
          return r.g;
      }

    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("main");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));

  std::vector<std::shared_ptr<TipType>> twoInts{TypeHelper::intType(), TypeHelper::intType()};
  std::vector<std::string> twoNames{"f", "g"};

  auto xType = std::make_shared<TipVar>(symbols->getLocal("r", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::recType(twoInts, twoNames));
}

/*
 * ---------------------------------------------------------------------
 * Testing type inference for records
 * ---------------------------------------------------------------------
 */

TEST_CASE("TypeConstraintVisitor: global record", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
      // [[foo]] = ()->int, [[r1]] = {f:int, g:int, n:absent}, [[r2]] = {f:int, g:absent, n: ptr to int}
      foo() {
          var r1, r2;
          r1 = {f: 4, g: 13};
          r2 = {n: alloc 3, f: 13};
          return 0;
      }

    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));

  std::vector<std::shared_ptr<TipType>> twoIntsAbsent{TypeHelper::intType(), TypeHelper::intType(), TypeHelper::absentType()};
  std::vector<std::string> threeNames{"f", "g", "n"};

  auto xType = std::make_shared<TipVar>(symbols->getLocal("r1", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::recType(twoIntsAbsent, threeNames));

  std::vector<std::shared_ptr<TipType>> intAbsentPtrInt{TypeHelper::intType(), TypeHelper::absentType(), TypeHelper::ptrType(TypeHelper::intType())};

  auto r1Type = std::make_shared<TipVar>(symbols->getLocal("r2", fDecl));
  REQUIRE(*unifier.inferred(r1Type) == *TypeHelper::recType(intAbsentPtrInt, threeNames));
}

TEST_CASE("TypeConstraintVisitor: record2", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[main]] = ()->int, [[n]] = ptr to record(p:int,q:int), [[r1]] = int
main() {
    var n, r1;
    n = alloc {p: 4, q: 2};
    *n = {p:5, q: 6};
    r1 = (*n).p;
    output r1;
    return 0;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("main");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));

  std::vector<std::shared_ptr<TipType>> twoInts{TypeHelper::intType(), TypeHelper::intType()};
  std::vector<std::string> twoNames{"p", "q"};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::ptrType(TypeHelper::recType(twoInts, twoNames)));

  auto r1Type = std::make_shared<TipVar>(symbols->getLocal("r1", fDecl));
  REQUIRE(*unifier.inferred(r1Type) == *TypeHelper::intType());
}

TEST_CASE("TypeConstraintVisitor: record4", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[main]] = ()->int
// [[n]] = record(a:absent, b: absent, c: ptr(record(a:int,b:int,c:absent,d:absent), d:int)
// [[k]] = record(a:int,b:int,c:absent,d:absent)
// [[r1]] = int
main() {
    var n, k, r1;
    k = {a: 1, b: 2};
    n = {c: &k, d: 4};
    r1 = ((*(n.c)).a); // output 1
    output r1;
    return 0;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("main");
  auto fType = std::make_shared<TipVar>(fDecl);
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));

  std::vector<std::shared_ptr<TipType>> twoIntsTwoAbsent{TypeHelper::intType(), TypeHelper::intType(), TypeHelper::absentType(), TypeHelper::absentType()};
  std::vector<std::string> fieldNames{"a", "b", "c", "d"};
  auto kType = std::make_shared<TipVar>(symbols->getLocal("k", fDecl));
  REQUIRE(*unifier.inferred(kType) == *TypeHelper::recType(twoIntsTwoAbsent, fieldNames));

  std::vector<std::shared_ptr<TipType>> twoAbsentsPtrRecInt{TypeHelper::absentType(), TypeHelper::absentType(),
                                                            TypeHelper::ptrType(TypeHelper::recType(twoIntsTwoAbsent, fieldNames)),
                                                            TypeHelper::intType()};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::recType(twoAbsentsPtrRecInt, fieldNames));

  auto r1Type = std::make_shared<TipVar>(symbols->getLocal("r1", fDecl));
  REQUIRE(*unifier.inferred(r1Type) == *TypeHelper::intType());
}

/*
 * --------------------------------------------------------------------
 * Testing array type inference and inference of elements in the array
 * --------------------------------------------------------------------
 */

TEST_CASE("TypeConstraintVisitor: array basic", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int)
foo() {
    var n;
    n = [1,2,3];
    return n;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> threeInts{TypeHelper::intType(), TypeHelper::intType(), TypeHelper::intType()};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(threeInts.back(), threeInts));

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::arrayType(threeInts.back(), threeInts)));
}

TEST_CASE("TypeConstraintVisitor: empty array", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int)
foo() {
    var n;
    n = [];
    return n;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> alpha{TypeHelper::alphaType(nullptr)};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(alpha.back(), alpha));

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::arrayType(alpha.back(), alpha)));
}

TEST_CASE("TypeConstraintVisitor: array of construction with numerical literal length", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int)
foo() {
    var n, x;
    n = [2 of 3];
    x = n;
    return x;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  std::vector<std::shared_ptr<TipType>> twoInts{TypeHelper::intType(), TypeHelper::intType()};
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::arrayType(twoInts.back(), twoInts));

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::arrayType(twoInts.back(), twoInts)));
}

TEST_CASE("TypeConstraintVisitor: array of construction with variable length", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int)
foo() {
    var n, x, y;
    x = 2;
    y = 3;
    n = [x of y];
    return x;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  std::vector<std::shared_ptr<TipType>> twoInts{TypeHelper::intType(), TypeHelper::intType()};
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(TypeHelper::intType(), TypeHelper::intType()));

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));
}

TEST_CASE("TypeConstraintVisitor: array of booleans", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->bool, [[n]] = array(bool,bool,bool)
foo() {
    var n, x;
    n = [true, false, true];
    x = n[1];
    return x;
}
  )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> threeBools{TypeHelper::boolType(), TypeHelper::boolType(), TypeHelper::boolType()};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(threeBools.back(), threeBools));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::boolType());

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::boolType()));
}

TEST_CASE("TypeConstraintVisitor: array of records", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->record(a:int,b:int), [[n]] = array(record(a:int,b:int),record(a:int,b:int),record(a:int,b:int))
foo() {
    var n, x;
    n = [{a:1, b:2}, {a:3, b:4}, {a:5, b:6}];
    x = n[1];
    return x;
}
  )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> twoInts{TypeHelper::intType(), TypeHelper::intType()};
  std::vector<std::string> twoNames{"a", "b"};
  std::vector<std::shared_ptr<TipType>> threeRecords{TypeHelper::recType(twoInts, twoNames), TypeHelper::recType(twoInts, twoNames), TypeHelper::recType(twoInts, twoNames)};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(threeRecords.back(), threeRecords));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::recType(twoInts, twoNames));

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::recType(twoInts, twoNames)));
}

TEST_CASE("TypeConstraintVisitor: array of references", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->ptr(int), [[n]] = array(ptr(int),ptr(int),ptr(int))
foo() {
    var n, x, y, z;
    x = alloc 1;
    y = alloc 2;
    z = alloc 3;
    n = [x, y, z];
    return n[1];
}
  )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(std::make_shared<TipRef>(TypeHelper::intType()), std::vector<std::shared_ptr<TipType>>{std::make_shared<TipRef>(TypeHelper::intType()), std::make_shared<TipRef>(TypeHelper::intType()), std::make_shared<TipRef>(TypeHelper::intType())}));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *std::make_shared<TipRef>(TypeHelper::intType()));

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, std::make_shared<TipRef>(TypeHelper::intType())));
}

TEST_CASE("TypeConstraintVisitor: array of array", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->array(array(int,int,int),array(int,int,int),array(int,int,int)), [[n]] = array(array(int,int,int),array(int,int,int),array(int,int,int))
foo() {
    var n, x;
    n = [[1,2,3],[4,5,6],[7,8,9]];
    x = n[1];
    return x;
}
  )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> threeInts{TypeHelper::intType(), TypeHelper::intType(), TypeHelper::intType()};
  std::vector<std::shared_ptr<TipType>> threeArrays{TypeHelper::arrayType(threeInts.back(), threeInts), TypeHelper::arrayType(threeInts.back(), threeInts), TypeHelper::arrayType(threeInts.back(), threeInts)};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(threeArrays.back(), threeArrays));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::arrayType(threeInts.back(), threeInts));

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::arrayType(threeInts.back(), threeInts)));
}

TEST_CASE("TypeConstraintVisitor: array constructed with variable length", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int), [[x]] = int
foo() {
    var n, x, y;
    x = 3;
    y = 5;
    n = [x of y];
    return n[2];
}
  )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(TypeHelper::intType(), std::vector<std::shared_ptr<TipType>>{TypeHelper::intType(), TypeHelper::intType(), TypeHelper::intType()}));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));
}

TEST_CASE("TypeConstraintVisitor: array reference", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int)
foo() {
    var n, x;
    n = [1,2,3];
    x = 1;
    x = n[1];
    return x;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> threeInts{TypeHelper::intType(), TypeHelper::intType(), TypeHelper::intType()};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(threeInts.back(), threeInts));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));
}

/*
 * -----------------------------------------------------------------
 * Testing unary operator expressions and the types that they return
 * Includes: #, not, - (unary negation), ++, --
 * -----------------------------------------------------------------
 */

TEST_CASE("TypeConstraintVisitor: unary exprs", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int)
foo() {
    var n, x, i, b, z, y;
    n = 3;
    y = n++;
    z = y--;
    x = false;
    i = not x;
    b = -z;
    return b+y;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::intType());

  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::intType());

  auto zType = std::make_shared<TipVar>(symbols->getLocal("z", fDecl));
  REQUIRE(*unifier.inferred(zType) == *TypeHelper::intType());

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::boolType());

  auto iType = std::make_shared<TipVar>(symbols->getLocal("i", fDecl));
  REQUIRE(*unifier.inferred(iType) == *TypeHelper::boolType());

  auto bType = std::make_shared<TipVar>(symbols->getLocal("b", fDecl));
  REQUIRE(*unifier.inferred(bType) == *TypeHelper::intType());

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));
}

/*
 * -------------------------------------------------------------------------------------
 * Testing binary operator expressions type constraints and the types that they return
 * Includes: +, -, *, /, %, <, <=, >, >=, ==, !=, and, or
 * -------------------------------------------------------------------------------------
 */
TEST_CASE("TypeConstraintVisitor: binary expressions", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[x]] = int, [[y]] = int
foo() {
    var x, y, z, a, b;
    x = 3;
    y = 4;
    z = x + y;
    z = x - y;
    z = x * y;
    z = x / y;
    z = x % y;
    a = x < z;
    a = x <= y;
    a = x > y;
    a = x >= y;
    a = x == y;
    a = x != y;
    b = a and true;
    b = a or b;
    return z;
}
  )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  auto zType = std::make_shared<TipVar>(symbols->getLocal("z", fDecl));
  auto aType = std::make_shared<TipVar>(symbols->getLocal("a", fDecl));
  auto bType = std::make_shared<TipVar>(symbols->getLocal("b", fDecl));

  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::intType());
  REQUIRE(*unifier.inferred(zType) == *TypeHelper::intType());
  REQUIRE(*unifier.inferred(aType) == *TypeHelper::boolType());
  REQUIRE(*unifier.inferred(bType) == *TypeHelper::boolType());

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));
}

/*
 * -------------------------------------------------------------------------------------
 * Testing ternary operator expressions type constraints and the types that they return
 * -------------------------------------------------------------------------------------
 */

TEST_CASE("TypeConstraintVisitor: ternary expression", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int)
foo() {
    var n, x, y, a, b;
    a = 1;
    b = 2;
    x = &a;
    y = &b;
    n = a+b > 2 ? x : y;
    return x;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *std::make_shared<TipRef>(TypeHelper::intType()));
}

TEST_CASE("TypeConstraintVisitor: ternary expression with records", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->record(a:int,b:int), [[n]] = record(a:int,b:int), [[m]] = record(a:int,b:int)
foo() {
    var n, m, x;
    n = {a:1, b:2};
    m = {a:3, b:4};
    x = n.a > m.a ? n : m;
    return x;
}
  )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> twoInts{TypeHelper::intType(), TypeHelper::intType()};
  std::vector<std::string> twoNames{"a", "b"};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  auto mType = std::make_shared<TipVar>(symbols->getLocal("m", fDecl));
  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));

  REQUIRE(*unifier.inferred(nType) == *TypeHelper::recType(twoInts, twoNames));
  REQUIRE(*unifier.inferred(mType) == *TypeHelper::recType(twoInts, twoNames));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::recType(twoInts, twoNames));

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::recType(twoInts, twoNames)));
}

/*
 * --------------------------------------------------------------------------------------------------------------------------
 * Testing loop statements (for and iterator loops) and ensuring variables within their header are inferred to correct types
 * --------------------------------------------------------------------------------------------------------------------------
 */

TEST_CASE("TypeConstraintVisitor: for loop", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int)
foo() {
    var n, x;
    for (x : 1 .. 10 by 2) {
        n = x;
    }
    return n;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::intType());

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::intType());

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));
}

TEST_CASE("TypeConstraintVisitor: for loop with records", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(record(a:int,b:int),record(a:int,b:int),record(a:int,b:int))
foo() {
    var n, x, y, z;
    n = [{a:1, b:2}, {a:3, b:4}, {a:5, b:6}];
    for (x : n) {
        y = x.a;
        z = y;
    }
    return z;
}
  )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> twoInts{TypeHelper::intType(), TypeHelper::intType()};
  std::vector<std::string> twoNames{"a", "b"};
  std::vector<std::shared_ptr<TipType>> threeRecords{TypeHelper::recType(twoInts, twoNames), TypeHelper::recType(twoInts, twoNames), TypeHelper::recType(twoInts, twoNames)};
  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(threeRecords.back(), threeRecords));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::recType(twoInts, twoNames));

  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  auto zType = std::make_shared<TipVar>(symbols->getLocal("z", fDecl));
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::intType());
  REQUIRE(*unifier.inferred(zType) == *TypeHelper::intType());

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));
}

TEST_CASE("TypeConstraintVisitor: iterator loop", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
// [[foo]] = ()->int, [[n]] = array(int,int,int)
foo() {
    var n, x, y;
    y = [true, false, true];
    for (x : y) {
        n = x;
    }
    return n;
}
    )";

  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> threeBools{TypeHelper::boolType(), TypeHelper::boolType(), TypeHelper::boolType()};
  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  REQUIRE(*unifier.inferred(yType) == *TypeHelper::arrayType(threeBools.back(), threeBools));

  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::boolType());

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::boolType());

  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::boolType()));
}

TEST_CASE("TypeConstraintVisitor: iterator loop with records", "[TypeConstraintVisitor]")
{
  std::stringstream program;
  program << R"(
  // [[foo]] = ()->int, [[n]] = array(record(a:int,b:int),record(a:int,b:int),record(a:int,b:int))
  foo() {
    var n, x, y, z, e;
    n = [{a:1, b:2}, {a:3, b:4}, {a:5, b:6}];
    for (x : n) {
      y = x.a;
      e = &x;
      z = y;
    }
    return z;
  }
  )";
  auto unifierSymbols = collectAndSolve(program);
  auto unifier = unifierSymbols.first;
  auto symbols = unifierSymbols.second;

  std::vector<std::shared_ptr<TipType>> empty;

  auto fDecl = symbols->getFunction("foo");
  auto fType = std::make_shared<TipVar>(fDecl);

  std::vector<std::shared_ptr<TipType>> twoInts{TypeHelper::intType(), TypeHelper::intType()};
  std::vector<std::string> twoNames{"a", "b"};
  std::vector<std::shared_ptr<TipType>> threeRecords{TypeHelper::recType(twoInts, twoNames), TypeHelper::recType(twoInts, twoNames), TypeHelper::recType(twoInts, twoNames)};

  auto nType = std::make_shared<TipVar>(symbols->getLocal("n", fDecl));
  REQUIRE(*unifier.inferred(nType) == *TypeHelper::arrayType(threeRecords.back(), threeRecords));

  auto xType = std::make_shared<TipVar>(symbols->getLocal("x", fDecl));
  REQUIRE(*unifier.inferred(xType) == *TypeHelper::recType(twoInts, twoNames));

  auto yType = std::make_shared<TipVar>(symbols->getLocal("y", fDecl));
  auto zType = std::make_shared<TipVar>(symbols->getLocal("z", fDecl));
  auto eType = std::make_shared<TipVar>(symbols->getLocal("e", fDecl));

  REQUIRE(*unifier.inferred(yType) == *TypeHelper::intType());
  REQUIRE(*unifier.inferred(eType) == *std::make_shared<TipRef>(TypeHelper::recType(twoInts, twoNames)));
  REQUIRE(*unifier.inferred(zType) == *TypeHelper::intType());
  REQUIRE(*unifier.inferred(fType) == *TypeHelper::funType(empty, TypeHelper::intType()));
}
