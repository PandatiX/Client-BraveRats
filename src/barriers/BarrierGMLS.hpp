#ifndef CLIENTBRAVERATS_BARRIERGMLS_HPP
#define CLIENTBRAVERATS_BARRIERGMLS_HPP

#include <mutex>
#include <condition_variable>

class BarrierGMLS {
private:
    int counter, waiting;
    std::mutex m;
    std::condition_variable cv;

public:
    BarrierGMLS() : counter(0), waiting(0) {}
    void wait() {
        std::unique_lock<std::mutex> lk(m);
        ++counter;
        ++waiting;
        cv.wait(lk, [&]{ return counter >= 2; });
        if (--waiting == 0)
            counter = 0;
        cv.notify_one();
        lk.unlock();
    }
};

#endif //CLIENTBRAVERATS_BARRIERGMLS_HPP