#pragma once
template <typename T>
class singleton
{
protected:
    static T* instance;

public:
    static T* This()
    {
        if (instance == nullptr)
            instance = new T();

        return instance;
    }

    static void Delete()
    {
        delete instance;
    }
};

template<typename T>
T* singleton<T>::instance = nullptr;