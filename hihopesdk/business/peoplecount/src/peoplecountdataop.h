#ifndef PEOPLECOUNTDATAOP_H
#define PEOPLECOUNTDATAOP_H

#include "../../base/core/src/timer.h"
#include "../../datamanager/db_peoplecount/src/peoplecountdbstore.h"
#include "../../datamanager/db_peoplecount/src/peoplecountcsvstore.h"

class PeopleCountDetect;
class PeopleCountDataOp {
public:
    PeopleCountDataOp();
    ~PeopleCountDataOp();

    void stop();
    void start();

    void enableCSVStore(bool enable);
    void setCSVStoreInterval(unsigned int intervalSec);
    void setCSVStoreFilePath(std::string filePath);

    int query(unsigned int startTimestamp, unsigned int endTimeStamp, PeopleCountList &peopleCountList);
    int count(unsigned int startTimestamp, unsigned int endTimeStamp);
    int average(unsigned int startTimestamp, unsigned int endTimeStamp);

    int lastHours(PeopleCountList &peopleCountList);
    int lastHoursAverage();

    int exportDataToCSV(std::string filePath, PeopleCountList &peopleCountList);
    int queryAndexportDataToCSV(unsigned int startTimestamp, unsigned int endTimeStamp, std::string filePath, PeopleCountList &peopleCountList);

    PeopleCountDetect *mPeopleCountDetect;

private:
    void update();
    void getData();

    Timer mTimer;

    unsigned int mDBStoreInterval;
    PeopleCountDBStore mDbStore;
    PeopleCountList mDbStoreCache;
    std::mutex mDbStoreCacheMutex;
    PeopleCount mDBLastPeopleCount;

    bool mEnableCSVStore;
    unsigned int mCSVStoreInterval;
    PeopleCountCSVStore mCsvStore;
    PeopleCountList mCSVStoreCache;
    std::mutex mCSVStoreCacheMutex;
    PeopleCount mCSVLastPeopleCount;

    bool mGetDataFisrtLoop;
    bool mRuning;
};


#endif // PEOPLECOUNTDATAOP_H
