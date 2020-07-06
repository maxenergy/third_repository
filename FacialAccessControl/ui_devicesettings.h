/********************************************************************************
** Form generated from reading UI file 'devicesettings.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICESETTINGS_H
#define UI_DEVICESETTINGS_H

#include <QtCore/QVariant>
#include <QApplication>
#include <QFrame>

QT_BEGIN_NAMESPACE

class Ui_DeviceSettings
{
public:

    void setupUi(QFrame *DeviceSettings)
    {
        if (DeviceSettings->objectName().isEmpty())
            DeviceSettings->setObjectName(QString::fromUtf8("DeviceSettings"));
        DeviceSettings->resize(400, 300);

        retranslateUi(DeviceSettings);

        QMetaObject::connectSlotsByName(DeviceSettings);
    } // setupUi

    void retranslateUi(QFrame *DeviceSettings)
    {
        DeviceSettings->setWindowTitle(QApplication::translate("DeviceSettings", "Frame", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DeviceSettings: public Ui_DeviceSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICESETTINGS_H
