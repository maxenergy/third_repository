#ifndef PEOPLECOUNTBACKEND_H
#define PEOPLECOUNTBACKEND_H

#include "../../datamanager/db_peoplecount/src/peoplecount.h"
#include <opencv2/opencv.hpp>

class PeopleCountDetect;
class PeopleCountDataOp;

class PeopleCountBackend {

public:
    PeopleCountBackend(std::string vendor, std::string algorithm);
    void stop();
    void start();

    int query(unsigned int startTimestamp, unsigned int endTimeStamp, PeopleCountList &peopleCountList);
    int count(unsigned int startTimestamp, unsigned int endTimeStamp);
    int lastHours(PeopleCountList &peopleCountList);
    int lastHoursAverage();
    int exportDataToCSV(std::string filePath, PeopleCountList &peopleCountList);
    int queryAndexportDataToCSV(unsigned int startTimestamp, unsigned int endTimeStamp, std::string filePath, PeopleCountList &peopleCountList);

    void enableStore(bool enable);
    void setStoreFilePath(std::string filePath);
    void setStoreInterval(unsigned int intervalSec = 5);

    bool setCamera(int camera);
    bool setCamera(std::string camera);
    void setDetectInterval(unsigned int intervalMsec = 0);
    void getCurretFrame(cv::Mat &image);
    void getCurretPeopleCount(PeopleCount &peopleCount);

private:
    PeopleCountDetect *mDetect;
    PeopleCountDataOp *mDataOp;
};

#endif // PEOPLECOUNT_H
