#include <iostream>
#include <luna/Value.h>
#include <luna/Document.h>

using namespace luna;


int main()
{
    Document document;
    std::string json("[0, 1, 2]");
    Parser::Status status = document.Parse(json);
    (void) status;
    return 0;
}