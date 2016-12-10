#pragma once
#include <list>
#include <stack>
#include <algorithm>
#include <memory>

#include <iostream>

namespace lain {

class ContainerBase;
class AbstractField;

static auto& getThreadContainerStack()
{
    static thread_local std::stack<ContainerBase*> _stack;
    return _stack;
}

template <typename Serializer>
struct MakeContext
{
    typedef Serializer SerializerType;
};

struct AbstractSerializer
{
    virtual ~AbstractSerializer() {}

    template <typename T>
    T& as()
    {
        return dynamic_cast<T&>(*this);
    }
};

struct AbstractField
{
    virtual ~AbstractField() {}

    virtual void read(AbstractSerializer&) = 0;
    virtual void write(AbstractSerializer&) = 0;
};

class ContainerBase
{
    public:
        ContainerBase()
        {
            // Initialize the TLS, push this onto
            // the thread's current structure stack
            getThreadContainerStack().push(this);
        }

        void _lain_initDone()
        {
            auto& stack = getThreadContainerStack();
            // Pop the current structure stack
            if (!stack.empty()) {
                stack.pop();
            }
        }

    protected:
        friend class FieldBase;

        void _lain_addField(AbstractField& f)
        {
            _lain_fields.push_back(&f);
        }

        // TODO implement the operator= and such
        // to *not* copy the contents of _lain_fields
        // but reinitialize the list instead


        std::list<AbstractField*> _lain_fields;
};

struct FieldBase : public AbstractField
{
    FieldBase()
    {
        doRegistration();
    }

    FieldBase(const std::string& name)
        : name(name)
    {
        doRegistration();
    }

    void doRegistration()
    {
        auto stack = getThreadContainerStack();
        if (!stack.empty()) {
            stack.top()->_lain_addField(*this);
        }
    }

    virtual ~FieldBase() {}

    std::string name;
};

template <typename Context, typename T>
class Field : public FieldBase
{
    public:
        T value;

        Field() {}
        Field(const std::string& name) : FieldBase(name) {}
        Field(const std::string& name, const T& value) : FieldBase(name), value(value) {}

        virtual void read(AbstractSerializer& s) override
        {
            s.as<typename Context::SerializerType>().read(value);
        }

        virtual void write(AbstractSerializer& s) override
        {
            s.as<typename Context::SerializerType>().write(name, value);
        }
};

struct StructureStart {};
struct StructureEnd {};

template <typename Context>
class StructureBase :
    public FieldBase,
    public ContainerBase
{
    public:
        StructureBase() {}
        StructureBase(const std::string& name) : FieldBase(name) {}

        void read(AbstractSerializer& s) override
        {
            std::for_each(_lain_fields.begin(), _lain_fields.end(),
                    [&s](auto& it) { it->read(s); });
        }

        void write(AbstractSerializer& s) override
        {
            s.as<typename Context::SerializerType>().write(name, StructureStart());

            std::for_each(_lain_fields.begin(), _lain_fields.end(),
                    [&s](auto& it) { it->write(s); });

            s.as<typename Context::SerializerType>().write(name, StructureEnd());
        }
};

template <typename Context, template <typename> typename StructureType>
struct Structure
: public StructureBase<Context>,
  public StructureType<Context>
{
    Structure()
    {
        StructureBase<Context>::_lain_initDone();
    }

    Structure(const std::string& name)
        : StructureBase<Context>(name), StructureType<Context>()
    {
        StructureBase<Context>::_lain_initDone();
    }
};


struct COutSerializer : public AbstractSerializer
{
    template <typename T>
    void read(T& d) {
        //...
    }

    template <typename T>
    void write(const std::string& name, const T& d)
    {
        writeIndent();
        std::cout << name << " = " << d << std::endl;
    }

    COutSerializer()
        : indent(0)
    {
    }

    void writeIndent()
    {
        std::cout << std::string(indent, ' ');
    }

    size_t indent;
};

template <>
void COutSerializer::write<StructureStart>(const std::string& name, const StructureStart& d)
{
    writeIndent();
    indent += 2;
    std::cout << name << " {" << std::endl;
}

template <>
void COutSerializer::write<StructureEnd>(const std::string& name, const StructureEnd& d)
{
    indent = std::max((size_t) 0, indent-2);
    writeIndent();
    std::cout << "}" << std::endl;
}


};
