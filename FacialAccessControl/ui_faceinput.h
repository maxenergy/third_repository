/********************************************************************************
** Form generated from reading UI file 'faceinput.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FACEINPUT_H
#define UI_FACEINPUT_H

#include <QtCore/QVariant>
#include <QApplication>
#include <QFrame>
#include <QLabel>

QT_BEGIN_NAMESPACE

class Ui_FaceInput
{
public:
    QLabel *picLab;

    void setupUi(QFrame *FaceInput)
    {
        if (FaceInput->objectName().isEmpty())
            FaceInput->setObjectName(QString::fromUtf8("FaceInput"));
        FaceInput->resize(400, 300);
        picLab = new QLabel(FaceInput);
        picLab->setObjectName(QString::fromUtf8("picLab"));
        picLab->setGeometry(QRect(100, 80, 241, 171));
        picLab->setStyleSheet(QString::fromUtf8(""));

        retranslateUi(FaceInput);

        QMetaObject::connectSlotsByName(FaceInput);
    } // setupUi

    void retranslateUi(QFrame *FaceInput)
    {
        FaceInput->setWindowTitle(QApplication::translate("FaceInput", "Frame", nullptr));
        picLab->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class FaceInput: public Ui_FaceInput {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FACEINPUT_H
