#ifndef PEOPLECOUNTDBSTORE_H
#define PEOPLECOUNTDBSTORE_H

#include "peoplecount.h"
#include "peoplecountstore.h"
#include <string>
#include <QSqlDatabase>

class PeopleCountDBStore : public PeopleCountStore
{
public:
    PeopleCountDBStore() = default;
    PeopleCountDBStore(std::string filePath);
    virtual ~PeopleCountDBStore();
    virtual int setFilePath(std::string filePath);
    virtual int insert(PeopleCount &item);
    virtual int insert(PeopleCountList &peopleCountList);
    virtual int query(unsigned int startTimestamp, unsigned int endTimeStamp, PeopleCountList &peopleCountList);
    virtual int count(unsigned int startTimestamp, unsigned int endTimeStamp);
    virtual int average(unsigned int startTimestamp, unsigned int endTimeStamp);

private:
    int mStatus;
    std::string mFilePath;
    QSqlDatabase mSqlDatabase;
    int initSqlite();
    int initDbTable();
};

#endif // PEOPLECOUNTDBSTORE_H
