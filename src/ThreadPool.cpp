#include "ThreadPool.hpp"

/* #region  Public */

ThreadPool::ThreadPool(int threads)
    : threadsMax(threads)
{
}

void ThreadPool::waitForFinish()
{
    std::unique_lock<std::mutex> lk(finishedAllMTU);
    finishedAllCV.wait(lk, [this]
                       { return finishedAllBool; });
}

/* #endregion */

/* #region  Private */

void ThreadPool::waitForThread()
{
    std::unique_lock<std::mutex> waitLK(threadReadyMTU);
    threadReadyCV.wait(waitLK, [this]
                       { return activeThreads < threadsMax; });
}

void ThreadPool::activeThreadsIncrease()
{
    finishedAllMTU.lock();
    finishedAllBool = false;
    finishedAllMTU.unlock();

    threadReadyCV.notify_one();

    activeThreadsMTU.lock();
    activeThreads++;
    activeThreadsMTU.unlock();
}

void ThreadPool::activeThreadsDecrease()
{
    activeThreadsMTU.lock();
    activeThreads--;
    {
        threadReadyCV.notify_one();
    }
    if (activeThreads == 0)
    {
        std::unique_lock<std::mutex> lk(finishedAllMTU);
        finishedAllBool = true;
        lk.unlock();
        finishedAllCV.notify_one();
        lk.lock();
    }
    threadReadyMTU.unlock();
    activeThreadsMTU.unlock();
}

void ThreadPool::threadedFunc()
{
    waitForThread();
    activeThreadsIncrease();
    executeCallback();
    activeThreadsDecrease();
}

void ThreadPool::executeCallback()
{
        callbacksMTU.lock();
        std::function<void()> callback = callbacks.front();
        callbacks.pop();
        callbacksMTU.unlock();
        callback();
    }

/* #endregion */