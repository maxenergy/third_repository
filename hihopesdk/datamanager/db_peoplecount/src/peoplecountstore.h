#ifndef PEOPLECOUNTSTORE_H
#define PEOPLECOUNTSTORE_H

#include "peoplecount.h"

class PeopleCountStore
{
public:
    PeopleCountStore() = default;
    virtual ~PeopleCountStore() = default;

    virtual int setFilePath(std::string filePath) = 0;
    virtual int insert(PeopleCount &item) = 0;
    virtual int insert(PeopleCountList &peopleCountList) = 0;
    virtual int remove(PeopleCount &item);
    virtual int update(PeopleCount &item);
    virtual int query(unsigned int startTimestamp, unsigned int endTimeStamp, PeopleCountList &peopleCountList);
    virtual int average(unsigned int startTimestamp, unsigned int endTimeStamp);
    virtual int count(unsigned int startTimestamp, unsigned int endTimeStamp);

    virtual bool isVaild();

protected:
    bool mVaild;
};

#endif // PEOPLECOUNTSTORE_H
