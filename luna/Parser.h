//
// Created by xi on 18-11-15.
//

#ifndef LUNA_PARSER_H
#define LUNA_PARSER_H

#include <errno.h>
#include <math.h>

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
        kRootNotSingular,
        kDoubleTooBig
    };

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

    template <typename ReadStream, typename Handler>
    static Status ParseString(ReadStream& is, Handler& handler)
    {
        //TODO
        return Status::kOK;
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
