#pragma once

template <typename T>
class singleton
{
protected:
	static T* instance_;

public:
	static T* instance()
	{
		if (instance_ == nullptr)
		{
			instance_ = new T();
		}

		return instance_;
	}

	static void teardown()
	{
		delete instance_;
	}
};

template<typename T>
T* singleton<T>::instance_ = nullptr;