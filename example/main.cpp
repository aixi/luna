#include <iostream>
#include <luna/Value.h>
#include <luna/Document.h>

using namespace luna;


int main()
{
    Document document;
    std::string json("[ 1,  1, 2,  3 ,  4 ]");
    Parser::Status status = document.Parse(json);
    double d0 = document[0].GetDouble();
    double d1 = document[1].GetDouble();
    double d2 = document[2].GetDouble();
    double d3 = document[3].GetDouble();
    double d4 = document[4].GetDouble();
    (void) status;
    (void) d0;
    (void) d1;
    (void) d2;
    return 0;
}