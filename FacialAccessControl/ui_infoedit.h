/********************************************************************************
** Form generated from reading UI file 'infoedit.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INFOEDIT_H
#define UI_INFOEDIT_H

#include <QtCore/QVariant>
#include <QApplication>
#include <QFrame>
#include <QHeaderView>
#include <QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_InfoEdit
{
public:
    QTableWidget *infotab;

    void setupUi(QFrame *InfoEdit)
    {
        if (InfoEdit->objectName().isEmpty())
            InfoEdit->setObjectName(QString::fromUtf8("InfoEdit"));
        InfoEdit->resize(400, 300);
        infotab = new QTableWidget(InfoEdit);
        infotab->setObjectName(QString::fromUtf8("infotab"));
        infotab->setGeometry(QRect(80, 40, 256, 192));

        retranslateUi(InfoEdit);

        QMetaObject::connectSlotsByName(InfoEdit);
    } // setupUi

    void retranslateUi(QFrame *InfoEdit)
    {
        InfoEdit->setWindowTitle(QApplication::translate("InfoEdit", "Frame", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InfoEdit: public Ui_InfoEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INFOEDIT_H
