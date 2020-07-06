/********************************************************************************
** Form generated from reading UI file 'messageboxframe.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEBOXFRAME_H
#define UI_MESSAGEBOXFRAME_H

#include <QtCore/QVariant>
#include <QApplication>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MessageBoxFrame
{
public:
    QFrame *frame;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QLineEdit *rootEdit;
    QLabel *rootLab;
    QLabel *msgLab;

    void setupUi(QFrame *MessageBoxFrame)
    {
        if (MessageBoxFrame->objectName().isEmpty())
            MessageBoxFrame->setObjectName(QString::fromUtf8("MessageBoxFrame"));
        MessageBoxFrame->resize(800, 1280);
        frame = new QFrame(MessageBoxFrame);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(30, 160, 640, 420));
        frame->setStyleSheet(QString::fromUtf8("QFrame{\n"
"background-color:#ffffff;\n"
"	background-color: rgb(245, 245, 245);\n"
"	border:1px;\n"
"border-color:rgb(40, 147, 255) ;\n"
"	border-radius:10px;\n"
"}"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        pushButton = new QPushButton(frame);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(370, 340, 121, 51));
        QFont font;
        font.setPointSize(17);
        pushButton->setFont(font);
        pushButton->setStyleSheet(QString::fromUtf8("background-color: rgb(40, 147, 255);\n"
"border:none ;\n"
"border-radius:3px;\n"
"color:white ;"));
        pushButton_2 = new QPushButton(frame);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(120, 340, 121, 51));
        pushButton_2->setFont(font);
        pushButton_2->setStyleSheet(QString::fromUtf8("background-color: rgb(40, 147, 255);\n"
"border:none ;\n"
"border-radius:3px;\n"
"color:white ;"));
        rootEdit = new QLineEdit(frame);
        rootEdit->setObjectName(QString::fromUtf8("rootEdit"));
        rootEdit->setGeometry(QRect(500, 60, 381, 51));
        rootEdit->setStyleSheet(QString::fromUtf8("border:2px solid rgb(40, 147, 255) ;\n"
"	border-radius:5px ;\n"
"	font-size:25px ;\n"
"	background-color: rgb(245, 245, 245);\n"
""));
        rootLab = new QLabel(frame);
        rootLab->setObjectName(QString::fromUtf8("rootLab"));
        rootLab->setGeometry(QRect(500, 0, 221, 41));
        QFont font1;
        font1.setPointSize(19);
        rootLab->setFont(font1);
        msgLab = new QLabel(frame);
        msgLab->setObjectName(QString::fromUtf8("msgLab"));
        msgLab->setGeometry(QRect(120, 180, 381, 41));
        QFont font2;
        font2.setPointSize(23);
        msgLab->setFont(font2);
        msgLab->setAlignment(Qt::AlignCenter);

        retranslateUi(MessageBoxFrame);

        QMetaObject::connectSlotsByName(MessageBoxFrame);
    } // setupUi

    void retranslateUi(QFrame *MessageBoxFrame)
    {
        MessageBoxFrame->setWindowTitle(QApplication::translate("MessageBoxFrame", "Frame", nullptr));
        pushButton->setText(QApplication::translate("MessageBoxFrame", "\347\241\256\345\256\232", nullptr));
        pushButton_2->setText(QApplication::translate("MessageBoxFrame", "\345\217\226\346\266\210", nullptr));
        rootLab->setText(QApplication::translate("MessageBoxFrame", "\350\257\267\350\276\223\345\205\245root\345\257\206\347\240\201", nullptr));
        msgLab->setText(QApplication::translate("MessageBoxFrame", "120,140,120,200,370,340\347\241\256\345\256\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MessageBoxFrame: public Ui_MessageBoxFrame {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEBOXFRAME_H
