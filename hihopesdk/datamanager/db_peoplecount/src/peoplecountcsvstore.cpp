#include "peoplecountcsvstore.h"
#include <iostream>
#include <algorithm>

PeopleCountCSVStore::PeopleCountCSVStore(std::string filePath) {
    mVaild = false;
    setFilePath(filePath);
}

int PeopleCountCSVStore::setFilePath(std::string filePath) {
    std::ofstream fileStream(filePath);
    if (fileStream.fail()){
        return 1;
    }
    fileStream.close();

    if (mFileStream.is_open()) {
        mFileStream.flush();
        mFileStream.close();
    }
    mFileStream.open(filePath);
    if(mFileStream.fail()){
        mVaild = false;
        return 1;
    }
    mVaild = true;
    return 0;
}

int PeopleCountCSVStore::insert(PeopleCount &item) {
    if (!mVaild) {
        return 1;
    }
    mFileStream << item.toCSVLine();
    mFileStream.flush();
    return 0;
}

int PeopleCountCSVStore::insert(PeopleCountList &peopleCountList) {
    if (!mVaild) {
        return 1;
    }

    std::stringstream ss;
    std::for_each(peopleCountList.cbegin(), peopleCountList.cend(), [&](PeopleCount item){
        ss << item.toCSVLine();
    });

    mFileStream << ss.str();
    mFileStream.flush();
    return 0;
}

