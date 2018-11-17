#include <iostream>
#include <luna/Value.h>

using namespace luna;

struct B
{
    B(int a, int b) :
        x(a),
        y(b)
    {}

    int x;
    int y;
};

union A
{
    int c;
    std::string str;
};

int main()
{

    return 0;
}