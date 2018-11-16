//
// Created by xi on 18-11-15.
//

#include <luna/Parser.h>

namespace luna
{

#pragma GCC diagnostic ignored  "-Wconversion"

void Parser::EncodeUTF8(std::string& buffer, unsigned u)
{
    if (u <= 0x7F)
    {
        buffer.push_back((u & 0xFF));
    }
    else if (u <= 0x7FF)
    {
        buffer.push_back((0xC0 | ((u >> 6) & 0xFF)));
        buffer.push_back((0x80 | (u & 0x3F)));
    }
    else if (u <= 0xFFFF)
    {
        buffer.push_back((0xE0 | ((u >> 12) & 0xFF)));
        buffer.push_back((0x80 | ((u >> 6) & 0x3F)));
        buffer.push_back((0x80 | (u & 0x3F)));
    }
    else
    {
        assert(u <= 0x10FFFF);
        buffer.push_back((0xF0 | ((u >> 18) & 0xFF)));
        buffer.push_back((0x80 | ((u >> 12) & 0x3F)));
        buffer.push_back((0x80 | ((u >> 6) & 0x3F)));
        buffer.push_back((0x80 | (u & 0x3F)));
    }
}

#pragma GCC diagnostic error "-Wconversion"


} //namespace luna
