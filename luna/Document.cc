//
// Created by xi on 18-11-16.
//

#include <luna/Document.h>

namespace luna
{

Document::Document() :
    Value()
{}

Value* Document::AddValue(Value&& value)
{
    type_ = value.GetType();
    value.type_ = ValueType::kNull;
    switch (type_)
    {
        case ValueType::kNull:
        case ValueType::kBool:
            b_ = value.b_;
            break;
        case ValueType::kNumber:
            d_ = value.d_;
            break;
        case ValueType::kString:
            s_ = value.s_;
            value.s_ = nullptr;
            break;
    }
    return this;
}

} //namespace luna