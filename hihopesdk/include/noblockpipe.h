#ifndef NOBLOCKPIPE_H
#define NOBLOCKPIPE_H

#include <array>
#include <list>
#include <mutex>
#include <iostream>
#include <condition_variable>

template<typename T, std::size_t SIZE>
class NoblockPipe {
    std::mutex mMutex;
    std::list<T> mFifo;
    std::condition_variable_any mCondFull;
    std::condition_variable_any mCondEmpty;

public:
    T pop() {
        {
            std::unique_lock<std::mutex> locker(mMutex);
            //std::cout << "mFifo pop:" << mFifo.size() << std::endl;
            if(mFifo.empty()) {
                mCondEmpty.wait(mMutex, [&]{
                    return !mFifo.empty();
                });
            }
            T obj(mFifo.front());
            mFifo.pop_front();
            return obj;
        }
    }
    void push(T &obj) {
        {
            std::unique_lock<std::mutex> locker(mMutex);
            if(mFifo.size() >= SIZE) {
               mFifo.pop_front();
            }
            mFifo.push_back(obj);
            mCondEmpty.notify_all();
        }
    }

	bool is_empty()
    {
		return mFifo.empty();
	}
};

#endif // AUTOPIPE_H
