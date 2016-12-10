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
            std::cout << "pushing " << (void*) (this) << " onto the stack" << std::endl;
        }

        void _lain_initDone()
        {
            auto stack = getThreadContainerStack();
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
        auto stack = getThreadContainerStack();
        if (!stack.empty()) {
            std::cout << "adding field " << (void*) (this) << std::endl;
            stack.top()->_lain_addField(*this);
        }
    }

    virtual ~FieldBase() {}
};

template <typename Context, typename T>
class Field : public FieldBase
{
    public:
        T value;

        virtual void read(AbstractSerializer& s) override
        {
            s.as<typename Context::SerializerType>().read(value);
        }

        virtual void write(AbstractSerializer& s) override
        {
            s.as<typename Context::SerializerType>().write(value);
        }
};

struct StructureStart {};
struct StructureEnd {};

template <typename _SerializerType>
class Structure :
    public AbstractField,
    public ContainerBase
{
    public:
        // This context typedef must be passed as a template
        // argument to all fields in the container.
        typedef struct {
            typedef _SerializerType SerializerType;
        } C;

        Structure()
        {
            // Necessary to pop the current structure from the stack
            //_lain_initDone();
        }

        void read(AbstractSerializer& s) override
        {
            std::for_each(_lain_fields.begin(), _lain_fields.end(),
                    [&s](auto& it) { it->read(s); });
        }

        void write(AbstractSerializer& s) override
        {
            s.as<_SerializerType>().write(StructureStart());

            std::for_each(_lain_fields.begin(), _lain_fields.end(),
                    [&s](auto& it) { it->write(s); });

            s.as<_SerializerType>().write(StructureEnd());
        }
};

struct COutSerializer : public AbstractSerializer
{
    template <typename T>
    void read(T& d) {
        //...
    }

    template <typename T>
    void write(const T& d) {
        std::cout << d << "|";
    }
};

template <>
void COutSerializer::write<StructureStart>(const StructureStart& d)
{
    std::cout << "--- STRUCTURE START ---" << std::endl;
}

template <>
void COutSerializer::write<StructureEnd>(const StructureEnd& d)
{
    std::cout << std::endl << "--- STRUCTURE END ---" << std::endl;
}


};
