#include "TipRecord.h"
#include "SipArrayOf.h"
#include "TipInt.h"
#include "TipBool.h"
#include "TipRef.h"

#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <sstream>

TEST_CASE("SipArrayOf: Test getters"
          "[SipArrayOf]")
{
  std::shared_ptr<TipType> length{
      std::make_shared<TipInt>()};
  std::shared_ptr<TipType> element{
      std::make_shared<TipRef>(std::make_shared<TipInt>())};
  SipArrayOf sipArrayOf(length, element);

  REQUIRE(dynamic_cast<const TipInt *>(sipArrayOf.getLength()));
  REQUIRE(dynamic_cast<const TipRef *>(sipArrayOf.getElement()));
}

TEST_CASE("SipArrayOf: Test equality"
          "[SipArrayOf]")
{
  std::shared_ptr<TipType> lengthA{
      std::make_shared<TipInt>()};
  std::shared_ptr<TipType> elementA{
      std::make_shared<TipRef>(std::make_shared<TipInt>())};
  SipArrayOf sipArrayA(lengthA, elementA);

  SECTION("Equal when fields are of same type and length")
  {
    std::shared_ptr<TipType> lengthB{
        std::make_shared<TipInt>()};
    std::shared_ptr<TipType> elementB{
        std::make_shared<TipRef>(std::make_shared<TipInt>())};
    SipArrayOf sipArrayB(lengthB, elementB);

    REQUIRE(sipArrayA == sipArrayB);
  }

  SECTION("Not equal when arguments differ by type")
  {
    std::shared_ptr<TipType> lengthB{
        std::make_shared<TipInt>()};
    std::shared_ptr<TipType> elementB{
        std::make_shared<TipBool>()};
    SipArrayOf sipArrayB(lengthB, elementB);

    REQUIRE(sipArrayA != sipArrayB);
  }

  SECTION("Not equal when argument types differ")
  {
    TipInt tipInt;

    REQUIRE(sipArrayA != tipInt);
  }
}

TEST_CASE("SipArrayOf: Test output stream"
          "[SipArrayOf]")
{
  std::shared_ptr<TipType> length{
      std::make_shared<TipInt>()};
  std::shared_ptr<TipType> element{
      std::make_shared<TipRef>(std::make_shared<TipInt>())};
  SipArrayOf sipArray(length, element);

  auto expectedValue = "\u2B61int[int of \u2B61int]";
  std::stringstream stream;
  stream << sipArray;
  std::string actualValue = stream.str();

  REQUIRE(expectedValue == actualValue);
}
