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
#include <luna/StringOutputStream.h>
#include <luna/Generator.h>
#include <luna/PrettyGenerator.h>
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
    std::vector<std::string> numbers = {
            "0",
            "-0",
            "-0.0",
            "1",
            "-1",
            "1.5",
            "-1.5",
            "3.1415",
            "1E10",
            "1e10",
            "1E+10",
            "1E-10",
            "-1E10",
            "-1e10",
            "-1E-10",
            "1.234E+10",
            "1.234E-10"
    };
    for (std::string_view str : numbers)
    {
        Parser::Status status = document.Parse(str);
        BOOST_CHECK(status == Parser::Status::kOK);
        BOOST_CHECK_CLOSE(strtod(str.data(), nullptr), document.GetDouble(), 0.000001);
    }
}

BOOST_AUTO_TEST_CASE(testParseString)
{
    std::vector<std::string> expect = {
            "",
            "Hello",
            "Hello\nWorld",
            "\" \\ / \b \f \n \r \t",
            "\x24",
            "\xC2\xA2",
            "\xE2\x82\xAC",
             "\xF0\x9D\x84\x9E"
    };
    std::vector<std::string> jsons = {
            "\"\"",
            "\"Hello\"",
            "\"Hello\\nWorld\"",
            "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"",
            "\"\\u0024\"",
            "\"\\u00A2\"",
            "\"\\u20AC\"",
            "\"\\uD834\\uDD1E\""
    };
    Document document;
    Parser::Status status;
    for (size_t i = 0; i < expect.size(); ++i)
    {
        status = document.Parse(jsons[i]);
        BOOST_CHECK(status == Parser::Status::kOK);
        BOOST_CHECK_EQUAL(expect[i], document.GetString());
    }
}

BOOST_AUTO_TEST_CASE(testParseArray)
{
    Document document;
    Parser::Status status;
    std::string json("[]");
    status = document.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document.GetType() == ValueType::kArray);
    BOOST_CHECK(document.GetArray().empty());

    json = "[ 0\n\r,  \r1\t, 2,  3 ,  4 ]";
    Document document1;
    status = document1.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document1.GetType() == ValueType::kArray);
    BOOST_CHECK_EQUAL(document1.GetArray().size(), 5);
    for (int i = 0; i < 5; ++i)
    {
        BOOST_CHECK(document1[i].GetType() == ValueType::kNumber);
        BOOST_CHECK_CLOSE(document1[i].GetDouble(), i, 0.000001);
    }

    json = "[\"hehe\", true, null, false, 0.0]";
    Document document2;
    status = document2.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document2.GetType() == ValueType::kArray);
    BOOST_CHECK(document2[0].GetType() == ValueType::kString);
    BOOST_CHECK_EQUAL(document2[0].GetString(), "hehe");
    BOOST_CHECK(document2[1].GetType() == ValueType::kBool);
    BOOST_CHECK(document2[1].GetBool());
    BOOST_CHECK(document2[2].GetType() == ValueType::kNull);
    BOOST_CHECK(document2[3].GetType() == ValueType::kBool);
    BOOST_CHECK(!document2[3].GetBool());
    BOOST_CHECK(document2[4].GetType() == ValueType::kNumber);
    BOOST_CHECK_CLOSE(document2[4].GetDouble(), 0.0, 0.000001);

    json = "[0, [1, [2]]]";
    Document document3;
    status = document3.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document3.GetType() == ValueType::kArray);
    BOOST_CHECK_EQUAL(document3.GetArray().size(), 2);
    BOOST_CHECK(document3[0].GetType() == ValueType::kNumber);
    BOOST_CHECK_CLOSE(document3[0].GetDouble(), 0.0, 0.000001);
    BOOST_CHECK(document3[1].GetType() == ValueType::kArray);
    BOOST_CHECK_EQUAL(document3[1].GetArray().size(), 2);
    BOOST_CHECK(document3[1][0].GetType() == ValueType::kNumber);
    BOOST_CHECK_CLOSE(document3[1][0].GetDouble(), 1, 0.000001);
    BOOST_CHECK(document3[1][1].GetType() == ValueType::kArray);
    BOOST_CHECK_EQUAL(document3[1][1].GetArray().size(), 1);
    BOOST_CHECK(document3[1][1][0].GetType() == ValueType::kNumber);
    BOOST_CHECK_CLOSE(document3[1][1][0].GetDouble(), 2, 0.000001);

    json = "[0, {\"1\" : 1, \"t\" : true}]";
    Document document4;
    status = document4.Parse(json);
    (void) status;
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document4.GetType() == ValueType::kArray);
    BOOST_CHECK_EQUAL(document4.GetArray().size(), 2);
    BOOST_CHECK(document4[0].GetType() == ValueType::kNumber);
    BOOST_CHECK_CLOSE(document4[0].GetDouble(), 0, 0.000001);
    BOOST_CHECK(document4[1].GetType() == ValueType::kObject);
    BOOST_CHECK(document4[1]["t"].GetType() == ValueType::kBool);
    BOOST_CHECK(document4[1]["t"].GetBool());
}

BOOST_AUTO_TEST_CASE(testParseObject)
{
    Document document;
    Parser::Status status;
    std::string json("{\"n\" : null, \"t\" : true}");
    status = document.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document.GetType() == ValueType::kObject);
    BOOST_CHECK(document["n"].GetType() == ValueType::kNull);
    BOOST_CHECK(document["t"].GetType() == ValueType::kBool);
    BOOST_CHECK(document["t"].GetBool());

    json = "{\"0\" : 0, \"1\" : 1}";
    Document document1;
    status = document1.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document1.GetType() == ValueType::kObject);
    BOOST_CHECK(document1["0"].GetType() == ValueType::kNumber);
    BOOST_CHECK_CLOSE(document1["0"].GetDouble(), 0, 0.000001);
    BOOST_CHECK(document1["1"].GetType() == ValueType::kNumber);
    BOOST_CHECK_CLOSE(document1["1"].GetDouble(), 1, 0.000001);

    json = "{\"s\" : \"string\", \"a\": [0, 1], \"o\" : {\"n\" : null}}";
    Document document2;
    status = document2.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document2.GetType() == ValueType::kObject);
    BOOST_CHECK(document2["s"].GetType() == ValueType::kString);
    BOOST_CHECK_EQUAL(document2["s"].GetStringView(), "string");
    BOOST_CHECK(document2["a"].GetType() == ValueType::kArray);
    BOOST_CHECK_EQUAL(document2["a"].GetArray().size(), 2);
    BOOST_CHECK(document2["a"][0].GetType() == ValueType::kNumber);
    BOOST_CHECK_CLOSE(document2["a"][0].GetDouble(), 0, 0.000001);
    BOOST_CHECK(document2["a"][1].GetType() == ValueType::kNumber);
    BOOST_CHECK_CLOSE(document2["a"][1].GetDouble(), 1, 0.000001);
    BOOST_CHECK(document2["o"].GetType() == ValueType::kObject);
    BOOST_CHECK(document2["o"]["n"].GetType() == ValueType::kNull);

    json = "[{}]";
    Document document3;
    status = document3.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document3.GetType() == ValueType::kArray);
    BOOST_CHECK_EQUAL(document3.GetArray().size(), 1);
    BOOST_CHECK(document3[0].GetType() == ValueType::kObject);
    BOOST_CHECK_EQUAL(document3[0].GetObject().size(), 0);

    json = "{\"a\":[]}";
    Document document4;
    status = document4.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);

    json = "[{}, [{}]]";
    Document document5;
    status = document5.Parse(json);
    BOOST_CHECK(status == Parser::Status::kOK);
    BOOST_CHECK(document5.GetType() == ValueType::kArray);
    BOOST_CHECK_EQUAL(document5.GetArray().size(), 2);
    BOOST_CHECK(document5[0].GetType() == ValueType::kObject);
    BOOST_CHECK_EQUAL(document5[0].GetObject().size() , 0);
    BOOST_CHECK(document5[1].GetType() == ValueType::kArray);
    BOOST_CHECK_EQUAL(document5[1].GetArray().size(), 1);
    BOOST_CHECK(document5[1][0].GetType() == ValueType::kObject);
    BOOST_CHECK_EQUAL(document5[1][0].GetObject().size(), 0);
}


BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(testGeneratorRoundtrip)

#define TEST_ROUNDTRIP(json) do {\
    Document document; \
    Parser::Status status = document.Parse(json); \
    BOOST_CHECK(status == Parser::Status::kOK); \
    StringOutputStream os; \
    Generator<StringOutputStream> generator(os); \
    document.Generate(generator); \
    BOOST_CHECK_EQUAL(json, os.GetStringView()); \
} while(false)

BOOST_AUTO_TEST_CASE(testGenerateLiterial)
{
    std::string json(" null ");
    Document document;
    document.Parse(json);
    StringOutputStream os;
    Generator<StringOutputStream> generator(os);
    document.Generate(generator);
    BOOST_CHECK_EQUAL(os.GetStringView(), "null");

    json = "true";
    Document document1;
    document1.Parse(json);
    StringOutputStream os1;
    Generator<StringOutputStream> generator1(os1);
    document1.Generate(generator1);
    BOOST_CHECK_EQUAL(os1.GetStringView(), "true");

    json = "false";
    Document document2;
    document2.Parse(json);
    StringOutputStream os2;
    Generator<StringOutputStream> generator2(os2);
    document2.Generate(generator2);
    BOOST_CHECK_EQUAL(os2.GetStringView(), "false");

}

BOOST_AUTO_TEST_CASE(testGenerateDouble)
{
    //FIXME: roundtrip test failed when using sprintf float point number representation, using gtest ?
    std::vector<std::string> numbers = {
            "0",
            "-0",
            "-0.0",
            "1",
            "-1",
            "1.5",
            "-1.5",
            "3.1415",
            "1E10",
            "1e10",
            "1E+10",
            "1E-10",
            "-1E10",
            "-1e10",
            "-1E-10",
            "1.234E+10",
            "1.234E-10"
    };
    for (size_t i = 0; i < numbers.size(); ++i)
    {
        Document document;
        document.Parse(numbers[i]);
        StringOutputStream os;
        Generator<StringOutputStream> generator(os);
        document.Generate(generator);
        BOOST_CHECK_EQUAL(os.GetStringView(), numbers[i]);
    }
}

BOOST_AUTO_TEST_CASE(testGenerateString)
{
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"hello\\nworld\"");
    TEST_ROUNDTRIP("\" \\\" \\\\ / \\b \\f \\n \\r \\t \"");
    TEST_ROUNDTRIP("\"hello\\u0000world\"");
}

BOOST_AUTO_TEST_CASE(testGenerateArray)
{
    TEST_ROUNDTRIP("[]");
    TEST_ROUNDTRIP("[1.0,2.0,3.0]");
    TEST_ROUNDTRIP("[null,true,false]");
    TEST_ROUNDTRIP("[false,true,\"abc\",[1.0,2.0,3.0]]");
    TEST_ROUNDTRIP("[null,{\"n\":null,\"t\":true}]");
}

BOOST_AUTO_TEST_CASE(testGenerateObject)
{
    TEST_ROUNDTRIP("{}");
    TEST_ROUNDTRIP("{\"n\":null,\"t\":true,\"f\":false}");
    TEST_ROUNDTRIP("{\"a\":[null,true,{\"f\":false,\"t\":true}]}");
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(testPrettyGenerator)

BOOST_AUTO_TEST_CASE(testIndent)
{
    StringOutputStream os;
    PrettyGenerator<StringOutputStream> pg(os);
    (void) pg;
}

BOOST_AUTO_TEST_SUITE_END()