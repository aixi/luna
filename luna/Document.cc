//
// Created by xi on 18-11-16.
//

#include <luna/Document.h>
#include <memory>

namespace luna
{

Document::Document() :
    Value(),
    see_value_(false)
{}

Value* Document::AddValue(Value&& value)
{
    if (stack_.empty())
    {
        type_ = value.type_;
        switch (type_)
        {
            case ValueType::kNull:
                break;
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
            case ValueType::kArray:
                a_ = value.a_;
                value.a_ = nullptr;
                break;
            default:
                break;
        }
        value.type_ = ValueType::kNull;
        return this;
    }
    else
    {
        auto& top = stack_.back();
        if (top.GetType() == ValueType::kArray)
        {
            top.value->AddArrayElement(std::move(value));
            return top.LastValue();
        }
        else
        {
            return top.LastValue();
        }
    }
}

} //namespace luna