#include "appconfig.h"

AppConfig::AppConfig() {
    mSettings = new QSettings("./config.ini", QSettings::IniFormat);
}

void AppConfig::set(QString qstrnodename, QString qstrkeyname, QVariant qvarvalue) {
    return mSettings->setValue(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname), qvarvalue);
}

QVariant AppConfig::get(QString qstrnodename, QString qstrkeyname) {
    return mSettings->value(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
}
