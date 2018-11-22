//
// Created by xi on 18-11-15.
//

#include <luna/Value.h>
#include <algorithm>
#include "Value.h"

namespace luna
{

Value::Value(const Value& rhs) :
    type_(rhs.type_)
{
    switch (type_)
    {
        case ValueType::kNull:
        case ValueType::kBool:
            b_ = rhs.b_;
            break;
        case ValueType::kNumber:
            d_ = rhs.d_;
            break;
        case ValueType::kString:
            s_ = rhs.s_;
            s_->IncrAndGet();
            break;
        case ValueType::kArray:
            a_ = rhs.a_;
            a_->IncrAndGet();
            break;
        case ValueType::kObject:
            o_ = rhs.o_;
            o_->IncrAndGet();
            break;
    }
}


Value& Value::operator=(const Value& rhs)
{
    //FIXME: copy and swap ?
    if (&rhs == this)
    {
        return *this;
    }
    this->~Value();
    type_ = rhs.type_;
    switch (type_)
    {
        case ValueType::kNull:
        case ValueType::kBool:
            b_ = rhs.b_;
            break;
        case ValueType::kNumber:
            d_ = rhs.d_;
            break;
        case ValueType::kString:
            s_ = rhs.s_;
            s_->IncrAndGet();
            break;
        case ValueType::kArray:
            a_ = rhs.a_;
            a_->IncrAndGet();
            break;
        case ValueType::kObject:
            o_ = rhs.o_;
            o_->IncrAndGet();
            break;
    }
    return *this;
}

//NOTE: only kString kArray kObject need move semantic

Value::Value(Value&& rhs) noexcept :
    type_(rhs.type_)
{
    rhs.type_ = ValueType::kNull;
    switch (type_)
    {
        case ValueType::kNull:
        case ValueType::kBool:
            b_ = rhs.b_;
            break;
        case ValueType::kNumber:
            d_ = rhs.d_;
            break;
        case ValueType::kString:
            s_ = rhs.s_;
            rhs.s_ = nullptr;
            break;
        case ValueType::kArray:
            a_ = rhs.a_;
            rhs.a_ = nullptr;
            break;
        case ValueType::kObject:
            o_ = rhs.o_;
            rhs.o_ = nullptr;
            break;
    }
}

Value& Value::operator=(Value&& rhs) noexcept
{
    if (&rhs == this)
    {
        return *this;
    }
    this->~Value();
    type_ = rhs.type_;
    rhs.type_ = ValueType::kNull;
    switch (type_)
    {
        case ValueType::kNull:
        case ValueType::kBool:
        case ValueType::kNumber:
            break;
        case ValueType::kString:
            s_ = rhs.s_;
            rhs.s_ = nullptr;
            break;
        case ValueType::kArray:
            a_ = rhs.a_;
            rhs.a_ = nullptr;
            break;
        case ValueType::kObject:
            o_ = rhs.o_;
            rhs.o_ = nullptr;
            break;
    }
    return *this;
}

Value::~Value()
{
    switch (type_)
    {
        case ValueType::kNull:
        case ValueType::kBool:
        case ValueType::kNumber:
            break;
        case ValueType::kString:
            if (s_->DecrAndGet() == 0)
            {
                delete s_;
            }
            break;
        case ValueType::kArray:
            if (a_->DecrAndGet() == 0)
            {
                delete a_;
            }
            break;
        case ValueType::kObject:
            if (o_->DecrAndGet() == 0)
            {
                delete o_;
            }
            break;
    }
}


Value::Value(ValueType type) :
        type_(type)
{
    switch (type_)
    {
        case ValueType::kNull:
        case ValueType::kBool:
        case ValueType::kNumber:
            break;
        case ValueType::kString:
            s_ = new StringWithRefCount;
            break;
        case ValueType::kArray:
            a_ = new ArrayWithRefCount;
            break;
        case ValueType::kObject:
            o_ = new ObjectWithRefCount;
            break;
    }
}

Value::Value(bool b) :
        type_(ValueType::kBool),
        b_(b)
{}

Value::Value(double d) :
        type_(ValueType::kNumber),
        d_(d)
{}

Value::Value(std::string_view sv) :
        type_(ValueType::kString),
        s_(new StringWithRefCount(sv.begin(), sv.end()))
{}

Value& Value::AddObjectElement(luna::Value&& key, luna::Value&& value)
{
    //duplicate key is not allowed
    o_->data.emplace_back(std::move(key), std::move(value));
    return o_->data.back().value;
}

Value::ObjectIterator Value::FindObjectElement(std::string_view key)
{
    assert(type_ == ValueType::kObject);
    return std::find_if(o_->data.begin(), o_->data.end(), [key](const Member& m){
        return m.key.GetStringView() == key;
    });
}

Value::ConstObjectIterator Value::FindObjectElement(std::string_view key) const
{
    assert(type_ == ValueType::kObject);
    return std::find_if(o_->data.begin(), o_->data.end(), [key](const Member& m){
        return m.key.GetStringView() == key;
    });
}

Value& Value::operator[](std::string_view key)
{
    return const_cast<Value&>(static_cast<const Value&>(*this)[key]);
}

const Value& Value::operator[](std::string_view key) const
{
    assert(type_ == ValueType::kObject);
    auto iter = FindObjectElement(key);
    if (iter != o_->data.end())
    {
        return iter->value;
    }
    assert(false && "key not exist");
    static Value fake(ValueType::kNull);
    return fake;
}




} //namespace luna
