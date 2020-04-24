#include "peoplecountbackend.h"
#include "peoplecountdetect.h"
#include "peoplecountdataop.h"

PeopleCountBackend::PeopleCountBackend(std::string vendor, std::string algorithm) {
    mDetect = new PeopleCountDetect(vendor, algorithm);
    mDataOp = new PeopleCountDataOp();
    mDataOp->mPeopleCountDetect = mDetect;
}

void PeopleCountBackend::stop() {
    mDetect->stop();
    mDataOp->stop();
}

void PeopleCountBackend::start() {
    mDetect->start();
    mDataOp->start();
}

int PeopleCountBackend::query(unsigned int startTimestamp, unsigned int endTimeStamp, PeopleCountList &peopleCountList) {
    return mDataOp->query(startTimestamp, endTimeStamp, peopleCountList);
}

int PeopleCountBackend::count(unsigned int startTimestamp, unsigned int endTimeStamp) {
    return mDataOp->count(startTimestamp, endTimeStamp);
}

int PeopleCountBackend::lastHours(PeopleCountList &peopleCountList) {
    return mDataOp->lastHours(peopleCountList);
}

int PeopleCountBackend::lastHoursAverage() {
    return mDataOp->lastHoursAverage();
}

int PeopleCountBackend::exportDataToCSV(std::string filePath, PeopleCountList &peopleCountList) {
    return mDataOp->exportDataToCSV(filePath, peopleCountList);
}
int PeopleCountBackend::queryAndexportDataToCSV(unsigned int startTimestamp, unsigned int endTimeStamp, std::string filePath, PeopleCountList &peopleCountList) {
    return mDataOp->queryAndexportDataToCSV(startTimestamp, endTimeStamp, filePath, peopleCountList);
}

void PeopleCountBackend::enableStore(bool enable) {
    return mDataOp->enableCSVStore(enable);
}

void PeopleCountBackend::setStoreInterval(unsigned int intervalSec) {
    return mDataOp->setCSVStoreInterval(intervalSec);
}

void PeopleCountBackend::setStoreFilePath(std::string filePath) {
    return mDataOp->setCSVStoreFilePath(filePath);
}

bool PeopleCountBackend::setCamera(int camera) {
    return mDetect->setVideo(camera);
}

bool PeopleCountBackend::setCamera(std::string camera) {
    return mDetect->setVideo(camera);
}

void PeopleCountBackend::setDetectInterval(unsigned int intervalMsec) {
    mDetect->setDetectInterval(intervalMsec);
}

void PeopleCountBackend::getCurretFrame(cv::Mat &image) {
    return mDetect->getCurretFrame(image);
}

void PeopleCountBackend::getCurretPeopleCount(PeopleCount &peopleCount) {
    return mDetect->getCurretPeopleCount(peopleCount);
}


