#pragma once

#include <memory>

template <typename T>
class singleton
{
protected:
    singleton() = default;
    virtual ~singleton() = default;

public:
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;

    static std::shared_ptr<T> instance()
    {
        static std::shared_ptr<T> instance{ new T() };
        return instance;
    }
};