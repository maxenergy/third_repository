#include "peoplecountstore.h"
#include <iostream>

int PeopleCountStore::remove(PeopleCount &) {
    return 0;
}

int PeopleCountStore::update(PeopleCount &) {
    return 0;
}

int PeopleCountStore::query(unsigned int , unsigned int , PeopleCountList &) {
    return 0;
}

int PeopleCountStore::average(unsigned int, unsigned int) {
    return 0;
}

int PeopleCountStore::count(unsigned int, unsigned int) {
    return 0;
}

bool PeopleCountStore::isVaild() {
    return mVaild;
}
