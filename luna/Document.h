//
// Created by xi on 18-11-16.
//

#ifndef LUNA_DOCUMENT_H
#define LUNA_DOCUMENT_H

#include <luna/Value.h>
#include <luna/Parser.h>
#include <luna/StringReadStream.h>

namespace luna
{

class Document : public Value
{
public:

    Document();

    Parser::Status Parse(std::string_view json)
    {
        StringReadStream is(json);
        return ParseStream(is);
    }

public: //Handler

    bool Null()
    {
        AddValue(Value(ValueType::kNull));
        return true;
    }

    bool Bool(bool b)
    {
        AddValue(Value(b));
        return true;
    }

    bool Double(double d)
    {
        AddValue(Value(d));
        return true;
    }

    bool String(std::string_view sv)
    {
        AddValue(Value(sv));
        return true;
    }

private:
    template <typename ReadStream>
    Parser::Status ParseStream(ReadStream& is)
    {
        return Parser::Parse(is, *this);
    }

    Value* AddValue(Value&& value);
}; //class Document

} //namespace luna

#endif //LUNA_DOCUMENT_H
