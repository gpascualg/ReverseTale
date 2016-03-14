#pragma once

#include <threadpool11/threadpool11.hpp>

class ThreadPool
{

private:
    ThreadPool()
    {};

private:
    static ThreadPool* _instance;
    threadpool11::Pool pool;
};
