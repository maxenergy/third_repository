#ifndef AISERVICE_H
#define AISERVICE_H

#include "aiinterface.h"
#include <mutex>
#include <time.h>


template<typename AIINTERFACE>
class AIService
{
protected:
    int mCnt;
    double mCost;
    time_t mCurTime;
    time_t mLastTime;

    AIINTERFACE *mImpl;
    std::string mVendor, mNet;
    std::mutex mMutex;

public:
    AIService(std::string vendor, std::string net) :
        mVendor(vendor), mNet(net) {

        mCnt = 0;
        mCost = 0;
        mLastTime = time(nullptr);

        mImpl = static_cast<AIINTERFACE *>(AIInterface::getImpl(vendor, net));      
    }

    virtual ~AIService(){

    }

    virtual bool load(int device) {
        return (mImpl != nullptr && mImpl->load(device));
    }

    virtual bool detect(int device, Frame &frame, typename AIINTERFACE::Out &out) {
        int ret;
        if (frame.mFrameData.empty()&&frame.mRawdata.empty()) {
            std::cout << mNet << " empty frame" << std::endl;
            return false;
        }

        clock_t start = clock();
        {
            std::lock_guard<std::mutex> locker(mMutex);
            ret = (mImpl != nullptr && mImpl->detect(device, frame, out));
        }
        clock_t end = clock();
        //std::cout << mNet << "cost: " << (end-start)/1000.0f << std::endl;

        mCnt++;
        mCost += end-start;
        if (mCnt >= 100) {
            mCurTime = time(nullptr);
            //std::cout << mNet << device << " fps: " << mCnt/(mCurTime-mLastTime) << std::endl;
            //std::cout << mNet << device << " clock fps: " << mCnt/(mCost/CLOCKS_PER_SEC) << std::endl;
            mCnt = 0;
            mCost = 0;
            mLastTime = mCurTime;
        }
        return ret;
    }

virtual bool detect(int device,typename AIINTERFACE::Out &out) {
	int ret;
	Frame null_frame;
	clock_t start = clock();
	{
		std::lock_guard<std::mutex> locker(mMutex);
		ret = (mImpl != nullptr && mImpl->detect(device, null_frame, out));
	}
	clock_t end = clock();
	//std::cout << mNet << "cost: " << (end-start)/1000.0f << std::endl;

	mCnt++;
	mCost += end-start;
	if (mCnt >= 100) {
		mCurTime = time(nullptr);
		//std::cout << mNet << device << " fps: " << mCnt/(mCurTime-mLastTime) << std::endl;
		//std::cout << mNet << device << " clock fps: " << mCnt/(mCost/CLOCKS_PER_SEC) << std::endl;
		mCnt = 0;
		mCost = 0;
		mLastTime = mCurTime;
	}
	return ret;
}



#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    virtual bool isValid(int device) {
        return true;
    }
#pragma GCC diagnostic pop
};

#endif // AISERVICE_H
