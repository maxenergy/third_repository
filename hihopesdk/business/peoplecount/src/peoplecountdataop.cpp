#include "peoplecountdataop.h"
#include "peoplecountdetect.h"
#include <time.h>
#include <unistd.h>
#include <thread>
#include <iostream>

PeopleCountDataOp::PeopleCountDataOp() {
    mRuning = false;
    mDBStoreInterval = 1;
    mDbStore.setFilePath("./peoplecount_cache.db");

    mEnableCSVStore = true;
    mCSVStoreInterval = 5;
    mCsvStore.setFilePath("./peoplecount_data.csv");
}

PeopleCountDataOp::~PeopleCountDataOp() {

}

void PeopleCountDataOp::stop(){
    mTimer.Expire();
}

void PeopleCountDataOp::start() {
    if (mRuning) {
        return;
    }

    mGetDataFisrtLoop = true;
    mTimer.StartTimer(1000, [&]{
        getData();
    });
    mTimer.StartTimer(30*1000, [&]{
        update();
    });
}

void PeopleCountDataOp::getData(){
    PeopleCount curPeopleCount;
    mPeopleCountDetect->getCurretPeopleCount(curPeopleCount);
    if(curPeopleCount.mTimestamp <= 0) {
        return;
    }
    if(mGetDataFisrtLoop) {
        mDBLastPeopleCount = curPeopleCount;
        mCSVLastPeopleCount = curPeopleCount;
        mGetDataFisrtLoop = false;
    }

    if(curPeopleCount.mTimestamp-mDBLastPeopleCount.mTimestamp == mDBStoreInterval){
        {
            std::lock_guard<std::mutex> locker(mDbStoreCacheMutex);
            mDbStoreCache.push_back(curPeopleCount);
        }
        mDBLastPeopleCount = curPeopleCount;
    }

    if(mEnableCSVStore) {
        if(curPeopleCount.mTimestamp-mCSVLastPeopleCount.mTimestamp == mCSVStoreInterval){
            {
                std::lock_guard<std::mutex> locker(mCSVStoreCacheMutex);
                mCSVStoreCache.push_back(curPeopleCount);
            }
            mCSVLastPeopleCount = curPeopleCount;
        }
    }

}

void PeopleCountDataOp::update() {
    PeopleCountList dbStoreCache;
    {
        std::lock_guard<std::mutex> locker(mDbStoreCacheMutex);
        dbStoreCache = mDbStoreCache;
        mDbStoreCache.clear();
    }
    mDbStore.insert(dbStoreCache);

    if(mEnableCSVStore) {
        PeopleCountList csvStoreCache;
        {
            std::lock_guard<std::mutex> locker(mCSVStoreCacheMutex);
            csvStoreCache = mCSVStoreCache;
            mCSVStoreCache.clear();
        }
        mCsvStore.insert(csvStoreCache);
    }
}

void PeopleCountDataOp::enableCSVStore(bool enable) {
    mEnableCSVStore = enable;
}

void PeopleCountDataOp::setCSVStoreInterval(unsigned int intervalSec) {
    mCSVStoreInterval = intervalSec;
}

void PeopleCountDataOp::setCSVStoreFilePath(std::string filePath) {
    mCsvStore.setFilePath(filePath);
}

int PeopleCountDataOp::query(unsigned int startTimestamp, unsigned int endTimeStamp, PeopleCountList &peopleCountList) {
    update();
    return mDbStore.query(startTimestamp, endTimeStamp, peopleCountList);
}

int PeopleCountDataOp::count(unsigned int startTimestamp, unsigned int endTimeStamp) {
    update();
    return mDbStore.count(startTimestamp, endTimeStamp);
}

int PeopleCountDataOp::average(unsigned int startTimestamp, unsigned int endTimeStamp) {
    update();
    return mDbStore.average(startTimestamp, endTimeStamp);
}

int PeopleCountDataOp::lastHours(PeopleCountList &peopleCountList) {
    time_t curTime = time(nullptr);
    return query(curTime-3600, curTime, peopleCountList);
}

int PeopleCountDataOp::lastHoursAverage() {
    time_t curTime = time(nullptr);
    return average(curTime-3600, curTime);
}

int PeopleCountDataOp::queryAndexportDataToCSV(unsigned int startTimestamp, unsigned int endTimeStamp, std::string filePath, PeopleCountList &peopleCountList) {
    PeopleCountCSVStore csvStore(filePath);
    if (csvStore.isVaild()) {
        query(startTimestamp, endTimeStamp, peopleCountList);
        return csvStore.insert(peopleCountList);
    }
    return 1;
}

int PeopleCountDataOp::exportDataToCSV(std::string filePath, PeopleCountList &peopleCountList) {
    PeopleCountCSVStore csvStore(filePath);
    if (csvStore.isVaild()) {
        return csvStore.insert(peopleCountList);
    }
    return 1;
}

