//
// Created by xi on 18-11-15.
//

#ifndef LUNA_VALUE_H
#define LUNA_VALUE_H

#include <stddef.h>
#include <assert.h>
#include <vector>
#include <atomic>
#include <memory>
#include <string_view>

namespace luna
{

enum class ValueType
{
    kNull,
    kBool,
    kNumber,
    kString,
//    kArray,
//    kObject
};

class Value
{
public:

    Value(const Value& rhs);

    Value& operator=(const Value& rhs);

    //FIXME: move ctor move assign pass by value ?
    //HACK: only kString kArray kObject need move semantic
    Value(Value&& rhs) noexcept;

    Value& operator=(Value&& rhs) noexcept;

    ~Value();

    explicit Value(ValueType type = ValueType::kNull);

    explicit Value(bool b);

    explicit Value(double d);

    explicit Value(std::string_view sv);



private:
    ValueType type_;

    template <typename T>
    struct SimpleRefCount
    {
        template <typename... Args>
        explicit SimpleRefCount(Args&&... args) :
            refs(1),
            data(std::forward<Args>(args)...)
        {}

        ~SimpleRefCount()
        {
            assert(refs == 0);
        }

        int IncrAndGet()
        {
            //HACK: The combination of atomic operations is not atomic
            //refs.store(refs.load(std::memory_order_relaxed) + 1);
            //FIXME: memory_order_relaxed ?
            return ++refs;
        }

        int DecrAndGet()
        {
            assert(refs > 0);
            //FIXME: memory_order_relaxed ?
            return --refs;
        }

        std::atomic<int> refs;
        T data;
    };

    using StringWithRefCount = SimpleRefCount<std::vector<char>>;

    union
    {
        bool b_;
        double d_;
        StringWithRefCount* s_;
    };
};

} //namespace luna

#endif //LUNA_VALUE_H
