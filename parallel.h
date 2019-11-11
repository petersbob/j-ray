#ifndef PARALLELH
#define PARALLELH

#include <thread>
#include <future>
#include <vector>

class JoinThreads {
    std::vector<std::thread>& threads;
    public:
        explicit JoinThreads(std::vector<std::thread>& _threads) : threads(_threads) {}
        ~JoinThreads() {
            for(unsigned long i=0;i<threads.size();i++) {
                if(threads[i].joinable()) {
                    threads[i].join();
                }
            }
        }
};

void parallelForEach(int first, int last, const std::function<void(int)> &f){
    unsigned long const length = last-first;

    if (!length) return;

    unsigned long const minPerThread=25;
    unsigned long const maxThreads=(length+minPerThread-1)/minPerThread;

    unsigned long const hardwareThreads= std::thread::hardware_concurrency();

    unsigned long const numThreads=std::min(hardwareThreads!=0?hardwareThreads:2,maxThreads);
    unsigned long const blockSize=length/numThreads;

    std::vector<std::future<void>> futures(numThreads-1);
    std::vector<std::thread> threads(numThreads-1);
    JoinThreads joiner(threads);

    int blockStart=first;

    for (unsigned long i=0;i<(numThreads-1);i++) {
        int blockEnd=blockStart;
        blockEnd += blockSize;
        std::packaged_task<void(void)> task(
            [=]() {
                for (unsigned long j=blockStart;j<blockEnd;j++) {
                    f(j);
                }
            });
        futures[i]=task.get_future();
        threads[i]=std::thread(std::move(task));
        blockStart=blockEnd;
    }
    for (unsigned long i=blockStart;i<last;i++) f(i);

    for(unsigned long i=0;i<(numThreads-1);i++) {
        futures[i].get();
    }
}

#endif