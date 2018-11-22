//
// Created by xi on 18-11-16.
//

#include <luna/Document.h>
#include <memory>

namespace luna
{

Document::Document() :
    Value()
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
            case ValueType::kObject:
                o_ = value.o_;
                value.o_ = nullptr;
                break;
        }
        value.type_ = ValueType::kNull;
        return this;
    }
    else
    {
        Level& top = stack_.back();
        if (top.GetType() == ValueType::kArray)
        {
            (void) value;
            top.value->AddArrayElement(value);
            return top.LastValue();
        }
        else
        {
            assert(top.GetType() == ValueType::kObject);
            if (top.value_count % 2 == 0)
            {
                assert(value.type_ == ValueType::kString);
                ++top.value_count;
                key_ = std::move(value);
                return &key_;
            }
            else
            {
                top.value->AddObjectElement(std::move(key_), std::move(value));
                ++top.value_count;
                return top.LastValue();
            }

        }
    }
}


} //namespace luna