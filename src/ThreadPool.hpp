#include <mutex>
#include <functional>
#include <thread>
#include <queue>
#include <condition_variable>

class ThreadPool
{
public:
    ThreadPool(int threads);

    template <typename Function, typename... Args>
    void queueFunction(Function &&f, Args... args);

    template <typename Function, typename object, typename... Args>
    void queueFunction(Function &&f, object *instance, Args... args);

    void waitForFinish();

private:
    const unsigned int threadsMax;
    std::mutex threadReadyMTU;
    std::condition_variable threadReadyCV;
    bool threadReadyBool = 1;

    unsigned int activeThreads = 0;
    std::mutex activeThreadsMTU;

    std::queue<std::function<void()>> callbacks;
    std::mutex callbacksMTU;

    std::mutex finishedAllMTU;
    std::condition_variable finishedAllCV;
    bool finishedAllBool = 0;

    void threadedFunc();
    void waitForThread();
    void activeThreadsIncrease();
    void executeCallback();
    void activeThreadsDecrease();
};

template <typename Function, typename object, typename... Args>
void ThreadPool::queueFunction(Function &&f, object *instance, Args... args)
{
    callbacksMTU.lock();
    callbacks.push([f, args..., instance]
                   { (instance->*f)(args...); });
    callbacksMTU.unlock();

    std::thread t = std::thread(&ThreadPool::threadedFunc, this);
    t.detach();
}

template <typename Function, typename... Args>
void ThreadPool::queueFunction(Function &&f, Args... args)
{
    callbacksMTU.lock();
    callbacks.push([f, args...]
                   { (f)(args...); });
    callbacksMTU.unlock();

    std::thread t = std::thread(&ThreadPool::threadedFunc, this);
    t.detach();
}