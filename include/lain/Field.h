#pragma once
#include "ThreadContainerStack.h"
#include "AbstractField.h"

namespace lain {

class FieldBase : public AbstractField
{
public:
    FieldBase()
    {
        doRegistration();
    }

    FieldBase(const std::string& name)
        : _lain_name(name)
    {
        doRegistration();
    }

    void doRegistration()
    {
        auto stack = internal::getThreadContainerStack();
        if (!stack.empty()) {
            stack.top()->_lain_addField(*this);
        }
    }

    const std::string& getName() const override
    {
        return _lain_name;
    }

    virtual ~FieldBase() {}

private:
    std::string _lain_name;
};


template <typename Context, typename T>
class Field : public FieldBase
{
public:
    T value;

    Field() {}
    Field(const std::string& name) : FieldBase(name) {}
    Field(const std::string& name, const T& value) : FieldBase(name), value(value) {}

    virtual void read(AbstractInputStream& s) override
    {
        s.as<typename Context::InputStreamType>().get(getName(), value);
    }

    virtual void write(AbstractOutputStream& s) override
    {
        s.as<typename Context::OutputStreamType>().put(getName(), value);
    }
};

};

