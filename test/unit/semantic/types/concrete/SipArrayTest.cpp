#include "TipRecord.h"
#include "SipArray.h"
#include "TipInt.h"
#include "TipRef.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <sstream>

TEST_CASE("SipArray: Test getters"
          "[SipArray]")
{
  std::vector<std::shared_ptr<TipType>> elements{
      std::make_shared<TipInt>(),
      std::make_shared<TipInt>()};
  SipArray sipArray(elements);

  // REQUIRE(elements.size() == sipArray.getElements().size());
  REQUIRE(dynamic_cast<const TipInt *>(sipArray.getElements().front().get()));
  REQUIRE(dynamic_cast<const TipInt *>(sipArray.getElements().back().get()));
}

TEST_CASE("SipArray: Test arity"
          "[SipArray]")
{
  std::vector<std::shared_ptr<TipType>> elements{
      std::make_shared<TipInt>(),
      std::make_shared<TipInt>(),
      std::make_shared<TipInt>(),
      std::make_shared<TipInt>(),
      std::make_shared<TipInt>(),
  };
  SipArray sipArray(elements);

  // REQUIRE(5 == sipArray.arity());
}

TEST_CASE("SipArray: Test equality"
          "[SipArray]")
{
  std::vector<std::shared_ptr<TipType>> elementsA{
      std::make_shared<TipInt>(),
      std::make_shared<TipInt>()};
  SipArray sipArrayA(elementsA);

  SECTION("Equal when fields are of same type and length")
  {
    std::vector<std::shared_ptr<TipType>> elementsB{
        std::make_shared<TipInt>(),
        std::make_shared<TipInt>()};
    SipArray sipArrayB(elementsB);

    REQUIRE(sipArrayA == sipArrayB);
  }

  // SECTION("Not equal when arguments differ by length")
  // {
  //   std::vector<std::shared_ptr<TipType>> elementsB{
  //       std::make_shared<TipInt>(),
  //       std::make_shared<TipInt>(),
  //       std::make_shared<TipInt>()};
  //   SipArray sipArrayB(elementsB);

  //   REQUIRE(sipArrayA != sipArrayB);
  // }

  SECTION("Not equal when arguments differ by type")
  {
    std::vector<std::shared_ptr<TipType>> elementsB{
        std::make_shared<TipRef>(std::make_shared<TipInt>()),
        std::make_shared<TipRef>(std::make_shared<TipInt>()),
    };
    SipArray sipArrayB(elementsB);

    REQUIRE(sipArrayA != sipArrayB);
  }

  SECTION("Not equal when argument types differ")
  {
    TipInt tipInt;

    REQUIRE(sipArrayA != tipInt);
  }
}

TEST_CASE("SipArray: Test output stream"
          "[SipArray]")
{
  std::vector<std::shared_ptr<TipType>> elements{
      std::make_shared<TipInt>(),
      std::make_shared<TipInt>()};
  SipArray sipArray(elements);

  auto expectedValue = "int[int]";
  std::stringstream stream;
  stream << sipArray;
  std::string actualValue = stream.str();

  REQUIRE(expectedValue == actualValue);
}
