#pragma once
#include <string>
#include <sstream>
#include <stdexcept>
#include "Field.h"

namespace lain {

template <typename Context, typename T>
class Constant : public FieldBase
{
    public:
        Constant() {}
        Constant(const std::string& name) : FieldBase(name) {}
        Constant(const std::string& name, const T& value)
            : FieldBase(name), value(value) {}

        virtual void read(AbstractInputStream& s) override
        {
            T readValue;
            s.as<typename Context::InputStreamType>().get(getName(), readValue);
            if (readValue != value) {
                std::stringstream ss;
                ss  << "Constant " << getName() << " read from stream "
                    << "should have value " << value << ", but has value "
                    << readValue;
                throw std::runtime_error(ss.str());
            }
        }

        virtual void write(AbstractOutputStream& s) override
        {
            s.as<typename Context::OutputStreamType>().put(getName(), value);
        }

        const T value;
};


};
