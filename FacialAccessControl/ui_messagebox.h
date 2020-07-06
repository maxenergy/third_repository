/********************************************************************************
** Form generated from reading UI file 'messagebox.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEBOX_H
#define UI_MESSAGEBOX_H

#include <QtCore/QVariant>
#include <QApplication>
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MessageBox
{
public:
    QFrame *frame;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QLineEdit *rootEdit;
    QLabel *rootLab;
    QLabel *msgLab;

    void setupUi(QDialog *MessageBox)
    {
        if (MessageBox->objectName().isEmpty())
            MessageBox->setObjectName(QString::fromUtf8("MessageBox"));
        MessageBox->resize(643, 423);
        frame = new QFrame(MessageBox);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 640, 420));
        frame->setStyleSheet(QString::fromUtf8("QFrame{\n"
"background-color:#ffffff;\n"
"	background-color: rgb(250, 250, 250);\n"
"	border:none;\n"
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
"background-color: rgb(243, 243, 243);"));
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

        retranslateUi(MessageBox);

        QMetaObject::connectSlotsByName(MessageBox);
    } // setupUi

    void retranslateUi(QDialog *MessageBox)
    {
        MessageBox->setWindowTitle(QApplication::translate("MessageBox", "Dialog", nullptr));
        pushButton->setText(QString::fromUtf8("确定"));
        pushButton_2->setText(QString::fromUtf8("取消"));
        rootLab->setText(QString::fromUtf8("root lab"));
        msgLab->setText(QString::fromUtf8("msglab"));
    } // retranslateUi

};

namespace Ui {
    class MessageBox: public Ui_MessageBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEBOX_H
