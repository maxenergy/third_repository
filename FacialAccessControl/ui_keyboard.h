/********************************************************************************
** Form generated from reading UI file 'keyboard.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYBOARD_H
#define UI_KEYBOARD_H

#include <QtCore/QVariant>
#include <QApplication>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

QT_BEGIN_NAMESPACE

class Ui_KeyBoard
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_51;
    QGridLayout *gridLayout;
    QPushButton *pushButton_2;
    QPushButton *pushButton_16;
    QPushButton *pushButton_14;
    QPushButton *pushButton_45;
    QPushButton *pushButton_57;
    QPushButton *pushButton_9;
    QPushButton *pushButton_4;
    QPushButton *pushButton_63;
    QPushButton *pushButton_54;
    QPushButton *pushButton_36;
    QPushButton *pushButton_29;
    QPushButton *pushButton_53;
    QPushButton *pushButton_5;
    QPushButton *pushButton_33;
    QPushButton *pushButton_52;
    QPushButton *pushButton_13;
    QPushButton *pushButton_17;
    QPushButton *pushButton_10;
    QPushButton *pushButton_12;
    QPushButton *pushButton_66;
    QPushButton *pushButton_61;
    QPushButton *pushButton_67;
    QPushButton *pushButton_56;
    QPushButton *pushButton_65;
    QPushButton *pushButton_62;
    QPushButton *pushButton_40;
    QPushButton *pushButton_35;
    QPushButton *pushButton_44;
    QPushButton *pushButton_39;
    QPushButton *pushButton_18;
    QPushButton *pushButton_48;
    QPushButton *pushButton_59;
    QPushButton *pushButton_31;
    QPushButton *pushButton_37;
    QPushButton *pushButton_38;
    QPushButton *pushButton_58;
    QPushButton *pushButton_41;
    QPushButton *pushButton_15;
    QPushButton *pushButton_34;
    QPushButton *pushButton_55;
    QPushButton *pushButton_19;
    QPushButton *pushButton_20;
    QPushButton *pushButton_30;
    QPushButton *pushButton_50;
    QPushButton *pushButton_8;
    QPushButton *pushButton_60;
    QPushButton *pushButton_21;
    QPushButton *pushButton_43;
    QPushButton *pushButton_68;
    QPushButton *pushButton_3;
    QPushButton *pushButton_24;
    QPushButton *pushButton_11;
    QPushButton *pushButton_25;
    QPushButton *pushButton_42;
    QPushButton *pushButton_22;
    QPushButton *pushButton_26;
    QPushButton *pushButton_27;
    QPushButton *pushButton_49;
    QPushButton *pushButton_46;
    QPushButton *pushButton_23;
    QPushButton *pushButton_28;
    QPushButton *pushButton_47;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_32;
    QButtonGroup *buttonGroup;
    QButtonGroup *buttonGroup_3;
    QButtonGroup *buttonGroup_2;

    void setupUi(QWidget *KeyBoard)
    {
        if (KeyBoard->objectName().isEmpty())
            KeyBoard->setObjectName(QString::fromUtf8("KeyBoard"));
        KeyBoard->resize(800, 317);
        KeyBoard->setStyleSheet(QString::fromUtf8("\n"
"QPushButton\n"
"{\n"
"background-color: rgb(0, 85, 127);\n"
"\n"
"	color: rgb(255, 255, 255);\n"
"}\n"
"QPushButton:hover:pressed\n"
"{\n"
"background-color: rgb(0, 85, 255);\n"
"	color: rgb(170, 85, 0);\n"
"}"));
        horizontalLayout_2 = new QHBoxLayout(KeyBoard);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton_51 = new QPushButton(KeyBoard);
        pushButton_51->setObjectName(QString::fromUtf8("pushButton_51"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_51->sizePolicy().hasHeightForWidth());
        pushButton_51->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pushButton_51);

        horizontalLayout->setStretch(0, 1);

        verticalLayout->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(3);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton_2 = new QPushButton(KeyBoard);
        buttonGroup_3 = new QButtonGroup(KeyBoard);
        buttonGroup_3->setObjectName(QString::fromUtf8("buttonGroup_3"));
        buttonGroup_3->addButton(pushButton_2);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy1);
        pushButton_2->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_2, 1, 1, 1, 1);

        pushButton_16 = new QPushButton(KeyBoard);
        buttonGroup = new QButtonGroup(KeyBoard);
        buttonGroup->setObjectName(QString::fromUtf8("buttonGroup"));
        buttonGroup->addButton(pushButton_16);
        pushButton_16->setObjectName(QString::fromUtf8("pushButton_16"));
        sizePolicy1.setHeightForWidth(pushButton_16->sizePolicy().hasHeightForWidth());
        pushButton_16->setSizePolicy(sizePolicy1);
        pushButton_16->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_16, 2, 3, 1, 1);

        pushButton_14 = new QPushButton(KeyBoard);
        buttonGroup_2 = new QButtonGroup(KeyBoard);
        buttonGroup_2->setObjectName(QString::fromUtf8("buttonGroup_2"));
        buttonGroup_2->addButton(pushButton_14);
        pushButton_14->setObjectName(QString::fromUtf8("pushButton_14"));
        sizePolicy1.setHeightForWidth(pushButton_14->sizePolicy().hasHeightForWidth());
        pushButton_14->setSizePolicy(sizePolicy1);
        pushButton_14->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_14, 1, 12, 1, 2);

        pushButton_45 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_45);
        pushButton_45->setObjectName(QString::fromUtf8("pushButton_45"));
        sizePolicy1.setHeightForWidth(pushButton_45->sizePolicy().hasHeightForWidth());
        pushButton_45->setSizePolicy(sizePolicy1);
        pushButton_45->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_45, 4, 8, 1, 1);

        pushButton_57 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_57);
        pushButton_57->setObjectName(QString::fromUtf8("pushButton_57"));
        sizePolicy1.setHeightForWidth(pushButton_57->sizePolicy().hasHeightForWidth());
        pushButton_57->setSizePolicy(sizePolicy1);
        pushButton_57->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_57, 0, 8, 1, 1);

        pushButton_9 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_9);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        sizePolicy1.setHeightForWidth(pushButton_9->sizePolicy().hasHeightForWidth());
        pushButton_9->setSizePolicy(sizePolicy1);
        pushButton_9->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_9, 1, 8, 1, 1);

        pushButton_4 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_4);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        sizePolicy1.setHeightForWidth(pushButton_4->sizePolicy().hasHeightForWidth());
        pushButton_4->setSizePolicy(sizePolicy1);
        pushButton_4->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_4, 1, 3, 1, 1);

        pushButton_63 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_63);
        pushButton_63->setObjectName(QString::fromUtf8("pushButton_63"));
        sizePolicy1.setHeightForWidth(pushButton_63->sizePolicy().hasHeightForWidth());
        pushButton_63->setSizePolicy(sizePolicy1);
        pushButton_63->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_63, 0, 3, 1, 1);

        pushButton_54 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_54);
        pushButton_54->setObjectName(QString::fromUtf8("pushButton_54"));
        sizePolicy1.setHeightForWidth(pushButton_54->sizePolicy().hasHeightForWidth());
        pushButton_54->setSizePolicy(sizePolicy1);
        pushButton_54->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_54, 4, 4, 1, 1);

        pushButton_36 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_36);
        pushButton_36->setObjectName(QString::fromUtf8("pushButton_36"));
        sizePolicy1.setHeightForWidth(pushButton_36->sizePolicy().hasHeightForWidth());
        pushButton_36->setSizePolicy(sizePolicy1);
        pushButton_36->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_36, 3, 5, 1, 1);

        pushButton_29 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_29);
        pushButton_29->setObjectName(QString::fromUtf8("pushButton_29"));
        sizePolicy1.setHeightForWidth(pushButton_29->sizePolicy().hasHeightForWidth());
        pushButton_29->setSizePolicy(sizePolicy1);
        pushButton_29->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_29, 3, 0, 1, 1);

        pushButton_53 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_53);
        pushButton_53->setObjectName(QString::fromUtf8("pushButton_53"));
        sizePolicy1.setHeightForWidth(pushButton_53->sizePolicy().hasHeightForWidth());
        pushButton_53->setSizePolicy(sizePolicy1);
        pushButton_53->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_53, 4, 2, 1, 1);

        pushButton_5 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_5);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        sizePolicy1.setHeightForWidth(pushButton_5->sizePolicy().hasHeightForWidth());
        pushButton_5->setSizePolicy(sizePolicy1);
        pushButton_5->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_5, 1, 4, 1, 1);

        pushButton_33 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_33);
        pushButton_33->setObjectName(QString::fromUtf8("pushButton_33"));
        sizePolicy1.setHeightForWidth(pushButton_33->sizePolicy().hasHeightForWidth());
        pushButton_33->setSizePolicy(sizePolicy1);
        pushButton_33->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_33, 3, 11, 1, 1);

        pushButton_52 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_52);
        pushButton_52->setObjectName(QString::fromUtf8("pushButton_52"));
        sizePolicy1.setHeightForWidth(pushButton_52->sizePolicy().hasHeightForWidth());
        pushButton_52->setSizePolicy(sizePolicy1);
        pushButton_52->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_52, 4, 6, 1, 1);

        pushButton_13 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_13);
        pushButton_13->setObjectName(QString::fromUtf8("pushButton_13"));
        sizePolicy1.setHeightForWidth(pushButton_13->sizePolicy().hasHeightForWidth());
        pushButton_13->setSizePolicy(sizePolicy1);
        pushButton_13->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_13, 1, 0, 1, 1);

        pushButton_17 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_17);
        pushButton_17->setObjectName(QString::fromUtf8("pushButton_17"));
        sizePolicy1.setHeightForWidth(pushButton_17->sizePolicy().hasHeightForWidth());
        pushButton_17->setSizePolicy(sizePolicy1);
        pushButton_17->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_17, 2, 7, 1, 1);

        pushButton_10 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_10);
        pushButton_10->setObjectName(QString::fromUtf8("pushButton_10"));
        sizePolicy1.setHeightForWidth(pushButton_10->sizePolicy().hasHeightForWidth());
        pushButton_10->setSizePolicy(sizePolicy1);
        pushButton_10->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_10, 1, 9, 1, 1);

        pushButton_12 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_12);
        pushButton_12->setObjectName(QString::fromUtf8("pushButton_12"));
        sizePolicy1.setHeightForWidth(pushButton_12->sizePolicy().hasHeightForWidth());
        pushButton_12->setSizePolicy(sizePolicy1);
        pushButton_12->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_12, 1, 11, 1, 1);

        pushButton_66 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_66);
        pushButton_66->setObjectName(QString::fromUtf8("pushButton_66"));
        sizePolicy1.setHeightForWidth(pushButton_66->sizePolicy().hasHeightForWidth());
        pushButton_66->setSizePolicy(sizePolicy1);
        pushButton_66->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_66, 0, 10, 1, 1);

        pushButton_61 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_61);
        pushButton_61->setObjectName(QString::fromUtf8("pushButton_61"));
        sizePolicy1.setHeightForWidth(pushButton_61->sizePolicy().hasHeightForWidth());
        pushButton_61->setSizePolicy(sizePolicy1);
        pushButton_61->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_61, 0, 9, 1, 1);

        pushButton_67 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_67);
        pushButton_67->setObjectName(QString::fromUtf8("pushButton_67"));
        sizePolicy1.setHeightForWidth(pushButton_67->sizePolicy().hasHeightForWidth());
        pushButton_67->setSizePolicy(sizePolicy1);
        pushButton_67->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_67, 0, 6, 1, 1);

        pushButton_56 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_56);
        pushButton_56->setObjectName(QString::fromUtf8("pushButton_56"));
        sizePolicy1.setHeightForWidth(pushButton_56->sizePolicy().hasHeightForWidth());
        pushButton_56->setSizePolicy(sizePolicy1);
        pushButton_56->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_56, 0, 7, 1, 1);

        pushButton_65 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_65);
        pushButton_65->setObjectName(QString::fromUtf8("pushButton_65"));
        sizePolicy1.setHeightForWidth(pushButton_65->sizePolicy().hasHeightForWidth());
        pushButton_65->setSizePolicy(sizePolicy1);
        pushButton_65->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_65, 0, 11, 1, 1);

        pushButton_62 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_62);
        pushButton_62->setObjectName(QString::fromUtf8("pushButton_62"));
        sizePolicy1.setHeightForWidth(pushButton_62->sizePolicy().hasHeightForWidth());
        pushButton_62->setSizePolicy(sizePolicy1);
        pushButton_62->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_62, 0, 0, 1, 2);

        pushButton_40 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_40);
        pushButton_40->setObjectName(QString::fromUtf8("pushButton_40"));
        sizePolicy1.setHeightForWidth(pushButton_40->sizePolicy().hasHeightForWidth());
        pushButton_40->setSizePolicy(sizePolicy1);
        pushButton_40->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_40, 3, 2, 1, 1);

        pushButton_35 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_35);
        pushButton_35->setObjectName(QString::fromUtf8("pushButton_35"));
        sizePolicy1.setHeightForWidth(pushButton_35->sizePolicy().hasHeightForWidth());
        pushButton_35->setSizePolicy(sizePolicy1);
        pushButton_35->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_35, 3, 1, 1, 1);

        pushButton_44 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_44);
        pushButton_44->setObjectName(QString::fromUtf8("pushButton_44"));
        sizePolicy1.setHeightForWidth(pushButton_44->sizePolicy().hasHeightForWidth());
        pushButton_44->setSizePolicy(sizePolicy1);
        pushButton_44->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_44, 4, 7, 1, 1);

        pushButton_39 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_39);
        pushButton_39->setObjectName(QString::fromUtf8("pushButton_39"));
        sizePolicy1.setHeightForWidth(pushButton_39->sizePolicy().hasHeightForWidth());
        pushButton_39->setSizePolicy(sizePolicy1);
        pushButton_39->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_39, 3, 6, 1, 1);

        pushButton_18 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_18);
        pushButton_18->setObjectName(QString::fromUtf8("pushButton_18"));
        sizePolicy1.setHeightForWidth(pushButton_18->sizePolicy().hasHeightForWidth());
        pushButton_18->setSizePolicy(sizePolicy1);
        pushButton_18->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_18, 2, 8, 1, 1);

        pushButton_48 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_48);
        pushButton_48->setObjectName(QString::fromUtf8("pushButton_48"));
        sizePolicy1.setHeightForWidth(pushButton_48->sizePolicy().hasHeightForWidth());
        pushButton_48->setSizePolicy(sizePolicy1);
        pushButton_48->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_48, 4, 1, 1, 1);

        pushButton_59 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_59);
        pushButton_59->setObjectName(QString::fromUtf8("pushButton_59"));
        sizePolicy1.setHeightForWidth(pushButton_59->sizePolicy().hasHeightForWidth());
        pushButton_59->setSizePolicy(sizePolicy1);
        pushButton_59->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_59, 0, 12, 1, 2);

        pushButton_31 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_31);
        pushButton_31->setObjectName(QString::fromUtf8("pushButton_31"));
        sizePolicy1.setHeightForWidth(pushButton_31->sizePolicy().hasHeightForWidth());
        pushButton_31->setSizePolicy(sizePolicy1);
        pushButton_31->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_31, 3, 7, 1, 1);

        pushButton_37 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_37);
        pushButton_37->setObjectName(QString::fromUtf8("pushButton_37"));
        sizePolicy1.setHeightForWidth(pushButton_37->sizePolicy().hasHeightForWidth());
        pushButton_37->setSizePolicy(sizePolicy1);
        pushButton_37->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_37, 3, 9, 1, 1);

        pushButton_38 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_38);
        pushButton_38->setObjectName(QString::fromUtf8("pushButton_38"));
        sizePolicy1.setHeightForWidth(pushButton_38->sizePolicy().hasHeightForWidth());
        pushButton_38->setSizePolicy(sizePolicy1);
        pushButton_38->setMinimumSize(QSize(1, 1));
        pushButton_38->setCheckable(false);

        gridLayout->addWidget(pushButton_38, 4, 0, 1, 1);

        pushButton_58 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_58);
        pushButton_58->setObjectName(QString::fromUtf8("pushButton_58"));
        sizePolicy1.setHeightForWidth(pushButton_58->sizePolicy().hasHeightForWidth());
        pushButton_58->setSizePolicy(sizePolicy1);
        pushButton_58->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_58, 0, 5, 1, 1);

        pushButton_41 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_41);
        pushButton_41->setObjectName(QString::fromUtf8("pushButton_41"));
        sizePolicy1.setHeightForWidth(pushButton_41->sizePolicy().hasHeightForWidth());
        pushButton_41->setSizePolicy(sizePolicy1);
        pushButton_41->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_41, 3, 4, 1, 1);

        pushButton_15 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_15);
        pushButton_15->setObjectName(QString::fromUtf8("pushButton_15"));
        sizePolicy1.setHeightForWidth(pushButton_15->sizePolicy().hasHeightForWidth());
        pushButton_15->setSizePolicy(sizePolicy1);
        pushButton_15->setMinimumSize(QSize(1, 1));
        pushButton_15->setCheckable(true);

        gridLayout->addWidget(pushButton_15, 2, 0, 1, 1);

        pushButton_34 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_34);
        pushButton_34->setObjectName(QString::fromUtf8("pushButton_34"));
        sizePolicy1.setHeightForWidth(pushButton_34->sizePolicy().hasHeightForWidth());
        pushButton_34->setSizePolicy(sizePolicy1);
        pushButton_34->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_34, 3, 10, 1, 1);

        pushButton_55 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_55);
        pushButton_55->setObjectName(QString::fromUtf8("pushButton_55"));
        sizePolicy1.setHeightForWidth(pushButton_55->sizePolicy().hasHeightForWidth());
        pushButton_55->setSizePolicy(sizePolicy1);
        pushButton_55->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_55, 4, 12, 1, 2);

        pushButton_19 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_19);
        pushButton_19->setObjectName(QString::fromUtf8("pushButton_19"));
        sizePolicy1.setHeightForWidth(pushButton_19->sizePolicy().hasHeightForWidth());
        pushButton_19->setSizePolicy(sizePolicy1);
        pushButton_19->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_19, 2, 11, 1, 1);

        pushButton_20 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_20);
        pushButton_20->setObjectName(QString::fromUtf8("pushButton_20"));
        sizePolicy1.setHeightForWidth(pushButton_20->sizePolicy().hasHeightForWidth());
        pushButton_20->setSizePolicy(sizePolicy1);
        pushButton_20->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_20, 2, 10, 1, 1);

        pushButton_30 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_30);
        pushButton_30->setObjectName(QString::fromUtf8("pushButton_30"));
        sizePolicy1.setHeightForWidth(pushButton_30->sizePolicy().hasHeightForWidth());
        pushButton_30->setSizePolicy(sizePolicy1);
        pushButton_30->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_30, 3, 3, 1, 1);

        pushButton_50 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_50);
        pushButton_50->setObjectName(QString::fromUtf8("pushButton_50"));
        sizePolicy1.setHeightForWidth(pushButton_50->sizePolicy().hasHeightForWidth());
        pushButton_50->setSizePolicy(sizePolicy1);
        pushButton_50->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_50, 4, 9, 1, 1);

        pushButton_8 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_8);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        sizePolicy1.setHeightForWidth(pushButton_8->sizePolicy().hasHeightForWidth());
        pushButton_8->setSizePolicy(sizePolicy1);
        pushButton_8->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_8, 1, 7, 1, 1);

        pushButton_60 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_60);
        pushButton_60->setObjectName(QString::fromUtf8("pushButton_60"));
        sizePolicy1.setHeightForWidth(pushButton_60->sizePolicy().hasHeightForWidth());
        pushButton_60->setSizePolicy(sizePolicy1);
        pushButton_60->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_60, 0, 2, 1, 1);

        pushButton_21 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_21);
        pushButton_21->setObjectName(QString::fromUtf8("pushButton_21"));
        sizePolicy1.setHeightForWidth(pushButton_21->sizePolicy().hasHeightForWidth());
        pushButton_21->setSizePolicy(sizePolicy1);
        pushButton_21->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_21, 2, 1, 1, 1);

        pushButton_43 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_43);
        pushButton_43->setObjectName(QString::fromUtf8("pushButton_43"));
        sizePolicy1.setHeightForWidth(pushButton_43->sizePolicy().hasHeightForWidth());
        pushButton_43->setSizePolicy(sizePolicy1);
        pushButton_43->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_43, 4, 3, 1, 1);

        pushButton_68 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_68);
        pushButton_68->setObjectName(QString::fromUtf8("pushButton_68"));
        sizePolicy1.setHeightForWidth(pushButton_68->sizePolicy().hasHeightForWidth());
        pushButton_68->setSizePolicy(sizePolicy1);
        pushButton_68->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_68, 0, 4, 1, 1);

        pushButton_3 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_3);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        sizePolicy1.setHeightForWidth(pushButton_3->sizePolicy().hasHeightForWidth());
        pushButton_3->setSizePolicy(sizePolicy1);
        pushButton_3->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_3, 1, 2, 1, 1);

        pushButton_24 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_24);
        pushButton_24->setObjectName(QString::fromUtf8("pushButton_24"));
        sizePolicy1.setHeightForWidth(pushButton_24->sizePolicy().hasHeightForWidth());
        pushButton_24->setSizePolicy(sizePolicy1);
        pushButton_24->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_24, 2, 13, 1, 1);

        pushButton_11 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_11);
        pushButton_11->setObjectName(QString::fromUtf8("pushButton_11"));
        sizePolicy1.setHeightForWidth(pushButton_11->sizePolicy().hasHeightForWidth());
        pushButton_11->setSizePolicy(sizePolicy1);
        pushButton_11->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_11, 1, 10, 1, 1);

        pushButton_25 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_25);
        pushButton_25->setObjectName(QString::fromUtf8("pushButton_25"));
        sizePolicy1.setHeightForWidth(pushButton_25->sizePolicy().hasHeightForWidth());
        pushButton_25->setSizePolicy(sizePolicy1);
        pushButton_25->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_25, 2, 6, 1, 1);

        pushButton_42 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_42);
        pushButton_42->setObjectName(QString::fromUtf8("pushButton_42"));
        sizePolicy1.setHeightForWidth(pushButton_42->sizePolicy().hasHeightForWidth());
        pushButton_42->setSizePolicy(sizePolicy1);
        pushButton_42->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_42, 3, 12, 1, 2);

        pushButton_22 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_22);
        pushButton_22->setObjectName(QString::fromUtf8("pushButton_22"));
        sizePolicy1.setHeightForWidth(pushButton_22->sizePolicy().hasHeightForWidth());
        pushButton_22->setSizePolicy(sizePolicy1);
        pushButton_22->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_22, 2, 5, 1, 1);

        pushButton_26 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_26);
        pushButton_26->setObjectName(QString::fromUtf8("pushButton_26"));
        sizePolicy1.setHeightForWidth(pushButton_26->sizePolicy().hasHeightForWidth());
        pushButton_26->setSizePolicy(sizePolicy1);
        pushButton_26->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_26, 2, 2, 1, 1);

        pushButton_27 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_27);
        pushButton_27->setObjectName(QString::fromUtf8("pushButton_27"));
        sizePolicy1.setHeightForWidth(pushButton_27->sizePolicy().hasHeightForWidth());
        pushButton_27->setSizePolicy(sizePolicy1);
        pushButton_27->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_27, 2, 4, 1, 1);

        pushButton_49 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_49);
        pushButton_49->setObjectName(QString::fromUtf8("pushButton_49"));
        sizePolicy1.setHeightForWidth(pushButton_49->sizePolicy().hasHeightForWidth());
        pushButton_49->setSizePolicy(sizePolicy1);
        pushButton_49->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_49, 4, 5, 1, 1);

        pushButton_46 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_46);
        pushButton_46->setObjectName(QString::fromUtf8("pushButton_46"));
        sizePolicy1.setHeightForWidth(pushButton_46->sizePolicy().hasHeightForWidth());
        pushButton_46->setSizePolicy(sizePolicy1);
        pushButton_46->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_46, 4, 11, 1, 1);

        pushButton_23 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_23);
        pushButton_23->setObjectName(QString::fromUtf8("pushButton_23"));
        sizePolicy1.setHeightForWidth(pushButton_23->sizePolicy().hasHeightForWidth());
        pushButton_23->setSizePolicy(sizePolicy1);
        pushButton_23->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_23, 2, 9, 1, 1);

        pushButton_28 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_28);
        pushButton_28->setObjectName(QString::fromUtf8("pushButton_28"));
        sizePolicy1.setHeightForWidth(pushButton_28->sizePolicy().hasHeightForWidth());
        pushButton_28->setSizePolicy(sizePolicy1);
        pushButton_28->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_28, 2, 12, 1, 1);

        pushButton_47 = new QPushButton(KeyBoard);
        buttonGroup_2->addButton(pushButton_47);
        pushButton_47->setObjectName(QString::fromUtf8("pushButton_47"));
        sizePolicy1.setHeightForWidth(pushButton_47->sizePolicy().hasHeightForWidth());
        pushButton_47->setSizePolicy(sizePolicy1);
        pushButton_47->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_47, 4, 10, 1, 1);

        pushButton_6 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_6);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        sizePolicy1.setHeightForWidth(pushButton_6->sizePolicy().hasHeightForWidth());
        pushButton_6->setSizePolicy(sizePolicy1);
        pushButton_6->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_6, 1, 5, 1, 1);

        pushButton_7 = new QPushButton(KeyBoard);
        buttonGroup_3->addButton(pushButton_7);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        sizePolicy1.setHeightForWidth(pushButton_7->sizePolicy().hasHeightForWidth());
        pushButton_7->setSizePolicy(sizePolicy1);
        pushButton_7->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_7, 1, 6, 1, 1);

        pushButton_32 = new QPushButton(KeyBoard);
        buttonGroup->addButton(pushButton_32);
        pushButton_32->setObjectName(QString::fromUtf8("pushButton_32"));
        sizePolicy1.setHeightForWidth(pushButton_32->sizePolicy().hasHeightForWidth());
        pushButton_32->setSizePolicy(sizePolicy1);
        pushButton_32->setMinimumSize(QSize(1, 1));

        gridLayout->addWidget(pushButton_32, 3, 8, 1, 1);


        verticalLayout->addLayout(gridLayout);

        verticalLayout->setStretch(0, 1);
        verticalLayout->setStretch(1, 5);

        horizontalLayout_2->addLayout(verticalLayout);


        retranslateUi(KeyBoard);

        QMetaObject::connectSlotsByName(KeyBoard);
    } // setupUi

    void retranslateUi(QWidget *KeyBoard)
    {
        KeyBoard->setWindowTitle(QApplication::translate("KeyBoard", "Form", nullptr));
        pushButton_51->setText(QString());
        pushButton_2->setText(QApplication::translate("KeyBoard", "1", nullptr));
        pushButton_16->setText(QApplication::translate("KeyBoard", "E", nullptr));
        pushButton_14->setText(QApplication::translate("KeyBoard", "BackSpace", nullptr));
        pushButton_45->setText(QApplication::translate("KeyBoard", "M", nullptr));
        pushButton_57->setText(QString());
        pushButton_9->setText(QApplication::translate("KeyBoard", "8", nullptr));
        pushButton_4->setText(QApplication::translate("KeyBoard", "3", nullptr));
        pushButton_63->setText(QString());
        pushButton_54->setText(QApplication::translate("KeyBoard", "C", nullptr));
        pushButton_36->setText(QApplication::translate("KeyBoard", "G", nullptr));
        pushButton_29->setText(QApplication::translate("KeyBoard", "Shift", nullptr));
        pushButton_53->setText(QApplication::translate("KeyBoard", "Z", nullptr));
        pushButton_5->setText(QApplication::translate("KeyBoard", "4", nullptr));
        pushButton_33->setText(QApplication::translate("KeyBoard", "\"\n"
"'", nullptr));
        pushButton_52->setText(QApplication::translate("KeyBoard", "B", nullptr));
        pushButton_13->setText(QApplication::translate("KeyBoard", "Tab", nullptr));
        pushButton_17->setText(QApplication::translate("KeyBoard", "U", nullptr));
        pushButton_10->setText(QApplication::translate("KeyBoard", "9", nullptr));
        pushButton_12->setText(QApplication::translate("KeyBoard", "_\n"
"-", nullptr));
        pushButton_66->setText(QString());
        pushButton_61->setText(QString());
        pushButton_67->setText(QString());
        pushButton_56->setText(QString());
        pushButton_65->setText(QString());
        pushButton_62->setText(QApplication::translate("KeyBoard", "<-", nullptr));
        pushButton_40->setText(QApplication::translate("KeyBoard", "S", nullptr));
        pushButton_35->setText(QApplication::translate("KeyBoard", "A", nullptr));
        pushButton_44->setText(QApplication::translate("KeyBoard", "N", nullptr));
        pushButton_39->setText(QApplication::translate("KeyBoard", "H", nullptr));
        pushButton_18->setText(QApplication::translate("KeyBoard", "I", nullptr));
        pushButton_48->setText(QApplication::translate("KeyBoard", "Ctrl", nullptr));
        pushButton_59->setText(QApplication::translate("KeyBoard", "->", nullptr));
        pushButton_31->setText(QApplication::translate("KeyBoard", "J", nullptr));
        pushButton_37->setText(QApplication::translate("KeyBoard", "L", nullptr));
        pushButton_38->setText(QApplication::translate("KeyBoard", "EN", nullptr));
        pushButton_58->setText(QString());
        pushButton_41->setText(QApplication::translate("KeyBoard", "F", nullptr));
        pushButton_15->setText(QApplication::translate("KeyBoard", "Cap", nullptr));
        pushButton_34->setText(QApplication::translate("KeyBoard", ":\n"
";", nullptr));
        pushButton_55->setText(QApplication::translate("KeyBoard", "\346\224\266\350\265\267", nullptr));
        pushButton_19->setText(QApplication::translate("KeyBoard", "{\n"
"[", nullptr));
        pushButton_20->setText(QApplication::translate("KeyBoard", "P", nullptr));
        pushButton_30->setText(QApplication::translate("KeyBoard", "D", nullptr));
        pushButton_50->setText(QApplication::translate("KeyBoard", "<\n"
",", nullptr));
        pushButton_8->setText(QApplication::translate("KeyBoard", "7", nullptr));
        pushButton_60->setText(QString());
        pushButton_21->setText(QApplication::translate("KeyBoard", "Q", nullptr));
        pushButton_43->setText(QApplication::translate("KeyBoard", "X", nullptr));
        pushButton_68->setText(QString());
        pushButton_3->setText(QApplication::translate("KeyBoard", "2", nullptr));
        pushButton_24->setText(QApplication::translate("KeyBoard", "|", nullptr));
        pushButton_11->setText(QApplication::translate("KeyBoard", "0", nullptr));
        pushButton_25->setText(QApplication::translate("KeyBoard", "Y", nullptr));
        pushButton_42->setText(QApplication::translate("KeyBoard", "Enter", nullptr));
        pushButton_22->setText(QApplication::translate("KeyBoard", "T", nullptr));
        pushButton_26->setText(QApplication::translate("KeyBoard", "W", nullptr));
        pushButton_27->setText(QApplication::translate("KeyBoard", "R", nullptr));
        pushButton_49->setText(QApplication::translate("KeyBoard", "V", nullptr));
        pushButton_46->setText(QApplication::translate("KeyBoard", "?\n"
"/", nullptr));
        pushButton_23->setText(QApplication::translate("KeyBoard", "O", nullptr));
        pushButton_28->setText(QApplication::translate("KeyBoard", "}\n"
"]", nullptr));
        pushButton_47->setText(QApplication::translate("KeyBoard", ">\n"
".", nullptr));
        pushButton_6->setText(QApplication::translate("KeyBoard", "5", nullptr));
        pushButton_7->setText(QApplication::translate("KeyBoard", "6", nullptr));
        pushButton_32->setText(QApplication::translate("KeyBoard", "K", nullptr));
    } // retranslateUi

};

namespace Ui {
    class KeyBoard: public Ui_KeyBoard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYBOARD_H
