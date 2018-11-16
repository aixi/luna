//
// Created by xi on 18-11-15.
//

#ifndef LUNA_PARSER_H
#define LUNA_PARSER_H

#include <errno.h>
#include <math.h>
#include <vector>
#include <luna/Value.h>



namespace luna
{

class Parser
{
public:
    enum class Status
    {
        kOK,
        kExpectValue,
        kInvalidLiteralValue,
        kInvalidDigitValue,
        kInvalidStringEscape,
        kStringMissingQuotationMark,
        kInvalidUnicodeSurrogate,
        kInvalidStringChar,
        kInvalidUnicodeHex,
        kRootNotSingular,
        kDoubleTooBig
    };

private:
    template <typename ReadStream>
    static void ParseWhitespace(ReadStream& is)
    {
        while (is.HasNext())
        {
            char c = is.Peek();
            if (c == ' ' || c == '\n' || c == '\t' || c == '\r')
            {
                is.Next();
            }
            else
            {
                break;
            }
        }
    }

    template <typename ReadStream, typename Handler>
    static Status ParseLiteral(ReadStream& is, Handler& handler, const char* literal, ValueType type)
    {
        char ch = is.Peek();
        is.Expect(*literal++);
        while (*literal != '\0' && is.Peek() == *literal)
        {
            ++literal;
            is.Next();
        }
        if (*literal != '\0')
        {
            return Status::kInvalidLiteralValue;
        }
        switch (type)
        {
            case ValueType::kNull:
                handler.Null();
                return Status::kOK;
            case ValueType::kBool:
                handler.Bool(ch == 't');
                return Status::kOK;
            default:
                return Status::kInvalidLiteralValue;
        }

    }

    template <typename ReadStream, typename Handler>
    static Status ParseNumber(ReadStream& is, Handler& handler)
    {
        auto start = is.GetIter();
        if (is.Peek() == '-')
        {
            is.Next();
        }
        if (is.Peek() == '0')
        {
            is.Next();
        }
        else
        {
            if (!isdigit(is.Peek()))
            {
                return Status::kInvalidDigitValue;
            }
            for (; isdigit(is.Peek()); is.Next());
        }
        if (is.Peek() == '.')
        {
            is.Next();
            if (!isdigit(is.Peek()))
            {
                return Status::kInvalidDigitValue;
            }
            for (; isdigit(is.Peek()); is.Next());
        }
        if (is.Peek() == 'e' || is.Peek() == 'E')
        {
            is.Next();
            if (is.Peek() == '-' || is.Peek() == '+')
            {
                is.Next();
            }
            if (!isdigit(is.Peek()))
            {
                return Status::kInvalidDigitValue;
            }
            for (; isdigit(is.Peek()); is.Next());
        }
        errno = 0;
        double d = strtod(&*start, nullptr);
        if (errno == ERANGE && (d == HUGE_VAL || d == -HUGE_VAL))
        {
            return Status::kDoubleTooBig;
        }
        handler.Double(d);
        return Status::kOK;
    }

    template <typename ReadStream>
    static bool Parse4Hex(unsigned& u, ReadStream& is)
    {
        u = 0;
        for (size_t i = 0; i < 4; ++i)
        {
            char ch = is.Next();
            u <<= 4;
            if (ch >= '0' && ch <= '9')
            {
                u |= ch - '0';
            }
            else if (ch >= 'A' && ch <= 'F')
            {
                u |= ch - ('A' - 10);
            }
            else if (ch >= 'a' && ch <= 'f')
            {
                u |= ch - ('a' - 10);
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    static void EncodeUTF8(std::string& buffer, unsigned u);

    template <typename ReadStream, typename Handler>
    static Status ParseString(ReadStream& is, Handler& handler)
    {
        is.Expect('\"');
        char ch;
        unsigned u = 0, u2 = 0;
        std::string buffer;
        while (is.HasNext())
        {
            switch (ch = is.Next())
            {
                case '\"':
                    handler.String(buffer);
                    return Status::kOK;
                case '\\':
                    switch (ch = is.Next())
                    {
                        case '\"':
                            buffer.push_back('\"');
                            break;
                        case '\\':
                            buffer.push_back('\\');
                            break;
                        case '/':
                            buffer.push_back('/');
                            break;
                        case 'b':
                            buffer.push_back('\b');
                            break;
                        case 'f':
                            buffer.push_back('\f');
                            break;
                        case 'n':
                            buffer.push_back('\n');
                            break;
                        case 'r':
                            buffer.push_back('\r');
                            break;
                        case 't':
                            buffer.push_back('\t');
                            break;
                        case 'u':

                            if (!Parse4Hex(u, is))
                            {
                                return Status::kInvalidUnicodeHex;
                            }
                            if (u >= 0xD800 && u <= 0xD8FF) //surrogate pair
                            {
                                if ((ch = is.Next()) != '\\')
                                {
                                    return Status::kInvalidUnicodeSurrogate;
                                }
                                if ((ch = is.Next()) != 'u')
                                {
                                    return Status::kInvalidUnicodeSurrogate;
                                }
                                if (!Parse4Hex(u2, is))
                                {
                                    return Status::kInvalidUnicodeHex;
                                }
                                if (u2 < 0xDC00 || u2 > 0xDFFF)
                                {
                                    return Status::kInvalidUnicodeSurrogate;
                                }
                                u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                            }
                            EncodeUTF8(buffer, u);
                            break;
                        default:
                            return Status::kInvalidStringEscape;
                    }
                    break;
                case '\0':
                    return Status::kStringMissingQuotationMark;
                default:
                    if (static_cast<unsigned char>(ch) < 0x20)
                    {
                        return Status::kInvalidStringChar;
                    }
                    buffer.push_back(ch);
            }
        }
        return Status::kStringMissingQuotationMark;
    }

    template <typename ReadStream, typename Handler>
    static Status ParseArray(ReadStream& is, Handler& handler)
    {
        //TODO
        return Status::kOK;
    }

    template <typename ReadStream, typename Handler>
    static Status ParseObject(ReadStream& is, Handler& handler)
    {
        //TODO
        return Status::kOK;
    }

    template <typename ReadStream, typename Handler>
    static Status ParseValue(ReadStream& is, Handler& handler)
    {
        if (!is.HasNext())
        {
            return Status::kExpectValue;
        }
        switch (is.Peek())
        {
            case 'n':
                return ParseLiteral(is, handler, "null", ValueType::kNull);
            case 't':
                return ParseLiteral(is, handler, "true", ValueType::kBool);
            case 'f':
                return ParseLiteral(is, handler, "false", ValueType::kBool);
            case '\"':
                return ParseString(is, handler);
            case '[':
                return ParseArray(is, handler);
            case '{':
                return ParseObject(is, handler);
            default:
                return ParseNumber(is, handler);
        }
    }

public:
    template <typename ReadStream, typename Handler>
    static Status Parse(ReadStream& is, Handler& handler)
    {
        ParseWhitespace(is);
        Status status = ParseValue(is, handler);
        if (status == Status::kOK)
        {
            ParseWhitespace(is);
            if (is.HasNext())
            {
                return Status::kRootNotSingular;
            }
        }
        return status;
    }

}; //class Parser

} //namespace luna

#endif //LUNA_PARSER_H
