//
// Created by xi on 18-11-15.
//

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <luna/Parser.h>
#include <luna/Document.h>

using namespace luna;

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
}

BOOST_AUTO_TEST_SUITE_END()