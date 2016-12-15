#pragma once
#include <string>
#include <sstream>
#include "AbstractStream.h"

namespace lain {

class StringInputStream : public AbstractInputStream
{
public:
    StringInputStream(const std::string& s) : ss(s) {}

    template <typename T>
    void get(const std::string& name, T& d)
    {
        ss >> d;
    }

private:
    std::stringstream ss;
};

template <>
void StringInputStream::get<StructureStart>(const std::string& name, StructureStart& d)
{
}

template <>
void StringInputStream::get<StructureEnd>(const std::string& name, StructureEnd& d)
{
}

};

