#include "peoplecountdbstore.h"
#include <QtCore/QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#define CHECK_STATUS(RET) if (mStatus != 0){ return RET;}
#define QSQL_CONNECTNAME "peoplecount"

PeopleCountDBStore::PeopleCountDBStore(std::string filePath) {
    setFilePath(filePath);
}

PeopleCountDBStore::~PeopleCountDBStore() {
     if (mSqlDatabase.isOpen()) {
         mSqlDatabase.close();
     }
}

int PeopleCountDBStore::setFilePath(std::string filePath) {
    if (mSqlDatabase.isValid() && mSqlDatabase.isOpen()) {
        mSqlDatabase.close();
    }

    std::string oldFile = mFilePath;

    mFilePath = filePath;
    mStatus = initSqlite();
    if (mStatus != 0) {
        if (mSqlDatabase.isValid() && mSqlDatabase.isOpen()) {
            mSqlDatabase.close();
        }
        mFilePath = oldFile;
        mStatus = initSqlite();
    }
    mVaild = !mStatus;
    return mStatus;
}

int PeopleCountDBStore::initSqlite() {
    QSqlDatabase db;

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        qDebug() << "Don't support driver: QSQLITE!!!";
        return 1;
    }

    if (QSqlDatabase::contains(QSQL_CONNECTNAME)) {
        db = QSqlDatabase::database(QSQL_CONNECTNAME);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", QSQL_CONNECTNAME);
    }

    if (!db.isValid()) {
        qDebug() << "Create database failed: " << db.lastError();
        return 1;
    }

    db.setDatabaseName(QString::fromStdString(mFilePath));
    db.setUserName("admin");
    db.setPassword("admin");

    if (!db.open()) {
        qDebug() << "Open database failed: " << db.lastError();
        return 1;
    }
    qDebug() << "QSqlDatabase init successed";
    mSqlDatabase = db;
    return initDbTable();
}

int PeopleCountDBStore::initDbTable() {
    if (mSqlDatabase.tables().contains("peoplecount",  Qt::CaseInsensitive)) {
        qDebug() << "peoplecounting table had be created, don't need create again.";
    } else {
        QSqlQuery sqlQuery(mSqlDatabase);
        QString createTableSql = "create table peoplecount (id integer primary key, time integer, count integer)";
        sqlQuery.prepare(createTableSql);
        if (!sqlQuery.exec()) {
            qDebug() << sqlQuery.lastError();
            return 1;
        }
    }

    qDebug() << "QSqlDatabase table init successed";
    qDebug() << "tables list:" << mSqlDatabase.tables();

    return 0;
}

int PeopleCountDBStore::insert(PeopleCount &item) {
    CHECK_STATUS(1);
    QSqlQuery sqlQuery(mSqlDatabase);
    QString insertSql = "insert into peoplecount(time, count) values (?, ?)";
    sqlQuery.prepare(insertSql);
    sqlQuery.addBindValue(item.mTimestamp);
    sqlQuery.addBindValue(item.mCount);
    if(!sqlQuery.exec()){
        qDebug() << sqlQuery.lastError();
        return -1;
    }
    return 0;
}

int PeopleCountDBStore::insert(PeopleCountList &peopleCountList) {
    CHECK_STATUS(1);
    QVariantList countList, timeStampList;
    for (auto iter = peopleCountList.cbegin(); iter != peopleCountList.cend(); iter++) {
        countList << iter->mCount;
        timeStampList << iter->mTimestamp;
    }

    QSqlQuery sqlQuery(mSqlDatabase);
    QString insertSql = "insert into peoplecount(time, count) values (?, ?)";
    sqlQuery.prepare(insertSql);
    sqlQuery.addBindValue(timeStampList);
    sqlQuery.addBindValue(countList);
    if(!sqlQuery.execBatch()){
        qDebug() << sqlQuery.lastError();
        return -1;
    }
    return 0;
}

int PeopleCountDBStore::query(unsigned int startTimestamp, unsigned int endTimeStamp, PeopleCountList &items) {
    CHECK_STATUS(1);
    QSqlQuery sqlQuery(mSqlDatabase);
    QString selectSql = QString("select time,count from peoplecount where time between %1 and %2").arg(startTimestamp).arg(endTimeStamp);
    if (!sqlQuery.exec(selectSql)) {
        qDebug() << sqlQuery.lastError();
        return 1;
    } else {
        while (sqlQuery.next()) {
            PeopleCount item = PeopleCount(sqlQuery.value(0).toUInt(), sqlQuery.value(1).toUInt());
            items.push_back(item);
        }
    }
    return 0;
}

int PeopleCountDBStore::count(unsigned int startTimestamp, unsigned int endTimeStamp) {
    int counts = 0;
    PeopleCountList items;
    query(startTimestamp, endTimeStamp, items);
    if (items.size() != 0) {
        std::for_each(items.cbegin(), items.cend(), [&](PeopleCount iter) {
            counts += iter.mCount;
        });
    }
    return counts;
}

int PeopleCountDBStore::average(unsigned int startTimestamp, unsigned int endTimeStamp) {
    int counts = 0;
    PeopleCountList items;
    query(startTimestamp, endTimeStamp, items);
    if (items.size() != 0) {
        std::for_each(items.cbegin(), items.cend(), [&](PeopleCount iter) {
            counts += iter.mCount;
        });
        counts = counts/static_cast<int>(items.size());
    }
    return counts;
}


