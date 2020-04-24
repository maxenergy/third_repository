#ifndef PEOPLECOUNTCSVSTORE_H
#define PEOPLECOUNTCSVSTORE_H

#include "peoplecountstore.h"
#include <fstream>

class PeopleCountCSVStore: public PeopleCountStore
{
public:
    PeopleCountCSVStore() = default;
    PeopleCountCSVStore(std::string filePath);
    virtual int setFilePath(std::string filePath);
    virtual int insert(PeopleCount &item);
    virtual int insert(PeopleCountList &peopleCountList);

private:
    std::ofstream mFileStream;
};

#endif // PEOPLECOUNTCSVSTORE_H
