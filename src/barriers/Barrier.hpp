#ifndef CLIENTBRAVERATS_BARRIER_HPP
#define CLIENTBRAVERATS_BARRIER_HPP

#include <mutex>
#include <condition_variable>

class Barrier {
private:
    int counter, waiting, thread_count;
    std::mutex m;
    std::condition_variable cv;

public:
    Barrier(int _thread_count) : counter(0), waiting(0), thread_count(_thread_count) {}
    void wait() {
        std::unique_lock<std::mutex> lk(m);
        ++counter;
        ++waiting;
        cv.wait(lk, [&]{ return counter >= thread_count; });
        if (--waiting == 0)
            counter = 0;
        cv.notify_one();
        lk.unlock();
    }
};

#endif //CLIENTBRAVERATS_BARRIER_HPP