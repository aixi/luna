//
// Created by xi on 18-11-15.
//


#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <vector>
#include <string_view>
#include <luna/Parser.h>
#include <luna/Document.h>

using namespace luna;
using namespace std::string_view_literals;

BOOST_AUTO_TEST_SUITE(testParser)

BOOST_AUTO_TEST_CASE(tesParseError)
{
    Document document;
    Parser::Status status = document.Parse("");
    BOOST_CHECK(status == Parser::Status::kExpectValue);
    status = document.Parse(" \n\t\r true \n\r null");
    BOOST_CHECK(status == Parser::Status::kRootNotSingular);
    status = document.Parse("  trae  ");
    BOOST_CHECK(status == Parser::Status::kInvalidLiteralValue);

    status = document.Parse("+00.0");
    BOOST_CHECK(status == Parser::Status::kInvalidDigitValue);
    status = document.Parse("-a");
    BOOST_CHECK(status == Parser::Status::kInvalidDigitValue);
    status = document.Parse("10.a");
    BOOST_CHECK(status == Parser::Status::kInvalidDigitValue);
    status = document.Parse("10.123eabc");
    BOOST_CHECK(status == Parser::Status::kInvalidDigitValue);
}

BOOST_AUTO_TEST_CASE(testParseLiteral)
{
    Document document;
    Parser::Status status = document.Parse(" null  ");
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document.GetType() == ValueType::kNull);
    status = document.Parse("\n\ttrue");
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document.GetType() == ValueType::kBool);
    BOOST_CHECK(document.GetBool());
    status = document.Parse("\n\tfalse  ");
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document.GetType() == ValueType::kBool);
    BOOST_CHECK(!document.GetBool());
}

BOOST_AUTO_TEST_CASE(testParseNumber)
{
    Document document;
    std::vector<std::string_view> numbers = {
            "0"sv,
            "-0"sv,
            "-0.0"sv,
            "1"sv,
            "-1"sv,
            "1.5"sv,
            "-1.5"sv,
            "3.1415"sv,
            "1E10"sv,
            "1e10"sv,
            "1E+10"sv,
            "1E-10"sv,
            "-1E10"sv,
            "-1e10"sv,
            "-1E-10"sv,
            "1.234E+10"sv,
            "1.234E-10"sv
    };
    for (std::string_view str : numbers)
    {
        Parser::Status status = document.Parse(str);
        BOOST_CHECK(status == Parser::Status::kOK);
        BOOST_CHECK_CLOSE(strtod(str.data(), nullptr), document.GetDouble(), 0.000001);
    }
}

BOOST_AUTO_TEST_SUITE_END()