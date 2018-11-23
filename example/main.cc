#include <iostream>
#include <luna/Value.h>
#include <luna/Document.h>
#include <luna/PrettyGenerator.h>
#include <luna/StringOutputStream.h>

using namespace luna;


int main()
{
    StringOutputStream os;
    PrettyGenerator<StringOutputStream> generator(os);
    std::string_view json = "[{}, [{}]]";
    Document document;
    Parser::Status status = document.Parse(json);
    (void) status;
    document.Generate(generator);
    std::cout << os.GetStringView();
    return 0;
}