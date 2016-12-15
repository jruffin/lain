#pragma once

namespace lain {

struct AbstractStream
{
    virtual ~AbstractStream() {}

    template <typename T>
    T& as()
    {
        return dynamic_cast<T&>(*this);
    }
};

struct AbstractInputStream : public AbstractStream {};
struct AbstractOutputStream : public AbstractStream {};


struct NullOutputStream : public AbstractOutputStream
{
    template <typename T>
    void put(const std::string& name, const T& d) {}
};

struct NullInputStream : public AbstractInputStream
{
    template <typename T>
    void get(const std::string& name, T& d) {}
};


};

