#include "peoplecount.h"

PeopleCount::PeopleCount() :
    mCount(0),
    mTimestamp(0){
}


PeopleCount::PeopleCount(unsigned int timestamp, unsigned int count) :
    mCount(count),
    mTimestamp(timestamp){

}

int PeopleCount::insert() {
    return 0;
}

int PeopleCount::remove() {
    return 0;
}

int PeopleCount::update() {
    return 0;
}
