#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "singleton.h"
#include <QSettings>

class AppConfig : public Singleton<AppConfig>
{
public:
    QVariant get(QString qstrnodename, QString qstrkeyname);
    void set(QString qstrnodename, QString qstrkeyname, QVariant qvarvalue);

private:
    QSettings *mSettings;
    AppConfig();

    friend Singleton;
};


#endif // APPCONFIG_H
