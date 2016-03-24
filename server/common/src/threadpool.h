#pragma once

#ifdef max
	#undef max
	#undef min
#endif

#include <threadpool11/threadpool11.hpp>

class ThreadPool
{

private:
	ThreadPool();

public:
	inline threadpool11::Pool* pool() { return &_pool; }

	static ThreadPool* get()
	{
		if (_instance == nullptr)
		{
			_instance = new ThreadPool();
		}

		return _instance;
	}

private:
    static ThreadPool* _instance;
    threadpool11::Pool _pool;
};

#define gPool ThreadPool::get()->pool()
