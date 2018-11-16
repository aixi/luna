//
// Created by xi on 18-11-15.
//

#ifndef LUNA_PARSER_H
#define LUNA_PARSER_H

namespace luna
{

class Parser
{
public:
    enum class Status
    {
        kNotInit,
        kOK,
        kExpectValue,
        kInvalidValue,
        kRootNotSingular
    };

    template <typename ReadStream, typename Handler>
    bool Parse(ReadStream& is, Handler& handler)
    {
        ParseWhiteSpace(is);
        if (is.HasNext())
        {

        }
    }

    template <typename ReadStream>
    void ParseWhitespace(ReadStream& is)
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
    bool ParseValue(ReadStream& is, Handler& handler)
    {

    }

    //Intentional copyable

private:
    Status status_;
}; //class Parser

} //namespace luna

#endif //LUNA_PARSER_H
