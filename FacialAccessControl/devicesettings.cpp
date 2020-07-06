#include "devicesettings.h"
#include "ui_devicesettings.h"
#include <QDebug>
#include <QTextCodec>

DeviceSettings::DeviceSettings(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DeviceSettings)
{
    ui->setupUi(this);
    setFixedSize(800, 1280-20) ; //sub 200 for test,need to restore
    initUI() ;
    connect(mBackBtn, SIGNAL(clicked()), this, SLOT(backBtnClicked())) ;
    connect(mSaveBtn, SIGNAL(clicked()), this, SLOT(store())) ;
}

DeviceSettings::~DeviceSettings()
{
    delete ui;
}
void DeviceSettings::initUI()
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));

    //head
    mTitleFrame = new QFrame(this) ;
    mTitleFrame->setObjectName("mTitleFrame") ;
    mTitleFrame->setGeometry(0, 0, this->width(), 88) ;
    mBackBtn = new QPushButton(mTitleFrame) ;
    mBackBtn->setObjectName("mBackBtn") ;
    mBackBtn->setGeometry(40, 14, 60, 60) ;
    mSaveBtn = new QPushButton(mTitleFrame) ;
    mSaveBtn->setObjectName("mSaveBtn") ;
    mSaveBtn->setGeometry(760, 14, 60, 60) ;
    mTitle = new QLabel(mTitleFrame) ;
    mTitle->setObjectName("mTitle") ;
    mTitle->setGeometry((800-144)/2, 24, 144, 36) ;
    mTitle->setText(QString::fromUtf8("设备配置")) ;

    mSettingFrame = new QFrame(this) ;
    mSettingFrame->setObjectName("mSettingFrame") ;
    mSettingFrame->setGeometry(this->x()+20, this->y()+227 + 88, 760, 650) ;
    mModeSelectionLab = new QLabel(mSettingFrame) ;
    mModeSelectionLab->setObjectName("mModeSelectionLab") ;
    mModeSelectionLab->move(parentWidget()->x()+20, parentWidget()->y()+120) ;
    mModeSelectionLab->setText(QString::fromUtf8("模式选择")) ;
    mKaoQinCheck = new QCheckBox(mSettingFrame) ;
    mKaoQinCheck->setObjectName("mKaoQinCheck") ;
    mKaoQinCheck->move(parentWidget()->x()+20+120+40, parentWidget()->y()+120) ;
    mKaoQinCheck->setText(QString::fromUtf8("考勤")) ;
    mAccessCheck = new QCheckBox(mSettingFrame) ;
    mAccessCheck->setObjectName("mKaoQinCheck") ;
    mAccessCheck->move(parentWidget()->x()+20+120+40+30+20+20+110, parentWidget()->y()+120) ;
    mAccessCheck->setText(QString::fromUtf8("门禁")) ;
    mAccessCheck->setCheckState(Qt::Checked) ;
    mDeviceDescribeLab = new QLabel(mSettingFrame) ;
    mDeviceDescribeLab->setObjectName("mModeSelectionLab") ;
    mDeviceDescribeLab->move(parentWidget()->x()+20, parentWidget()->y() + 120 +30+120) ;
    mDeviceDescribeLab->setText(QString::fromUtf8("设备描述")) ;
    mDeviceDescribeEdit = new QTextEdit(mSettingFrame);
    mDeviceDescribeEdit->setObjectName("mDeviceDescribeEdit") ;
    mDeviceDescribeEdit->setGeometry(parentWidget()->x()+20+120+40, parentWidget()->y() + 120 +30+115,560,120) ;
//mDeviceDescribeEdit->setPlaceholderText("位置和功能") ;
    mDeviceDescribeEdit->setPlainText(QString::fromUtf8("位置和功能")) ;
}

void DeviceSettings::backBtnClicked()
{
    qDebug()<<"DeviceSettings" ;
    backHomePage(0) ;
}

void DeviceSettings::store()
{
    int flag ;
    QString describe ;
    if(mKaoQinCheck->checkState() == Qt::Checked)  //考勤
        flag = 1 ;
    else if (mAccessCheck->checkState() == Qt::Checked)   //menjin
        flag = 2 ;
    if(mKaoQinCheck->checkState() == Qt::Checked && mAccessCheck->checkState() == Qt::Checked)  //both
        flag = 3 ;
    describe = mDeviceDescribeEdit->toPlainText() ;
    qDebug()<<flag<<describe ;
}
