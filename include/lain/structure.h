#pragma once
#include <list>
#include <stack>
#include <algorithm>
#include <memory>

namespace lain {

static auto getThreadContainerStack()
{
    static thread_local std::stack<AbstractContainerBase*> _stack;
    return _stack;
}

struct AbstractContainerBase
{
    void _lain_addField(AbstractField& f) = 0;
};

struct AbstractSerializer
{
    template <typename T>
    T& castTo()
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

template <typename SerializerType, typename FieldType>
class FieldTypeErasure : public AbstractField
{
    public:
        FieldTypeErasure(FieldType& f)
            : heldField(f)
        {
        }

        void read(AbstractSerializer& s) override
        {
            s.castTo<SerializerType>().read<FieldType>(heldField);
        }

        void write(AbstractSerializer& s) override
        {
            s.castTo<SerializerType>().write<FieldType>(heldField);
        }

    private:
        FieldType& heldField;
}

template <typename Context, typename FieldType>
struct FieldBase
{
    FieldBase()
    {
        auto stack = getThreadContainerStack();

        if (!stack.empty()) {
            auto typeErasedField = new FieldTypeErasure<C::SerializerType, FieldType>(f);
            stack.top()->_lain_addField(typeErasedField);
        }
    }

    virtual void ~FieldBase() {}
}

template <typename Context, typename T>
class Field : public FieldBase<Context, Field<T>>
{
    public:
        T value;
}

template <typename SerializerType>
class ContainerBase : public AbstractContainerBase
{
    public:
        ContainerBase()
        {
            // Initialize the TLS, push this onto
            // the thread's current structure stack
            getThreadContainerStack().push(this);
        }

        // This context typedef must be passed as a template
        // argument to all fields in the container.
        struct C {
            typedef ContainerBase<SerializerType> ContainerType;
            typedef SerializerType SerializerType;
        };


    protected:
        friend template <typename C, typename F> class Context;

        void _lain_addField(std::unique_ptr<AbstractField>&& f) override
        {
            _lain_fields.push_back(f);
        }

        // TODO implement the operator= and such
        // to *not* copy the contents of _lain_fields
        // but reinitialize the list instead

        void _lain_initDone()
        {
            auto stack = getThreadContainerStack();
            // Pop the current structure stack
            if (!stack.empty()) {
                stack.pop();
            }
        }

        std::list< std::unique_ptr<AbstractField> > _lain_fields;
};


template <typename StructType, typename SerializerType>
class Structure : public AbstractField, public ContainerBase, public StructType
{
    Structure() {
        // Necessary to pop the current structure from the stack
        _lain_initDone();
    }

    void read(AbstractSerializer& s) override
    {
        std::for_each(_lain_fields.begin(), _lain_fields.end(),
                [s](auto& it) { it->read(s) });
    }

    void write(AbstractSerializer& s) override
    {
        std::for_each(_lain_fields.begin(), _lain_fields.end(),
                [s](auto& it) { it->write(s) });
    }
};

struct Foo : public lain::Structure<Foo, COutSerializer>
{
    lain::Field<C, int> a;
    lain::Field<C, std::string> text;
};


};
