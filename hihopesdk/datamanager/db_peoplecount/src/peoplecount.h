#ifndef PEOPLECOUNT_H
#define PEOPLECOUNT_H
#include <time.h>
#include <string>
#include <vector>
#include <string>
#include <sstream>
#include <vector>

class PeopleCount {
public:
    unsigned int mCount;
    unsigned int mTimestamp;

    PeopleCount();
    PeopleCount(unsigned int timestamp, unsigned int count);
    virtual ~PeopleCount() = default;
    virtual int insert();
    virtual int remove();
    virtual int update();

    std::string toString() {
        std::stringstream ss;
        ss << "{";
        ss << "mCount:" << mCount << ", " ;
        ss << "mTimestamp:" << mTimestamp << "]" ;
        return ss.str();
    }

    std::string toCSVLine() {
        char timeStr[64];
        time_t stimeStamp = mTimestamp;
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&stimeStamp));

        std::stringstream ss;
        ss << "time" << "," << timeStr << "," ;
        ss << "count" << "," << mCount << " \n";
        return ss.str();
    }
};

typedef std::vector<PeopleCount> PeopleCountList;

#endif // PEOPLECOUNT_H
