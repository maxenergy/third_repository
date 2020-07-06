#include "homepage.h"
#include "ui_homepage.h"
#include "messagebox.h"
#include <QDebug>
#include <QTextCodec>

HomePage::HomePage(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::HomePage)
{
    ui->setupUi(this);
    setFixedSize(800, 1280-20) ; //sub 200 for test,need to restore
    initUI() ;
    MessageBox::getInstance()->setMainWidget(this) ;
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(showTime())) ;
    mMessageBoxFrame = new MessageBoxFrame(this) ;
    mMessageBoxFrame->move(0,0) ;

    connect(mMessageBoxFrame, SIGNAL(sendOperate(QString)), this, SLOT(getPassword(QString))) ;
	connect(mMessageBoxFrame, SIGNAL(hide_window(void)), this, SLOT(hidemasgbox(void))) ;

	mMessageBoxFrame->hide() ;
    mTimer.start(1000) ;
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::hidemasgbox(void)
{
	emit switchUI(34) ;
}


void HomePage::getPassword(QString str)
{
    qDebug()<<str ;
    mMessageBoxFrame->mKeyBoard->hide() ;
    if(str == "123")
    {
        mMessageBoxFrame->hide() ;
        mMessageBoxFrame->mKeyBoard->hide();
        emit switchUI(32) ;
    }
    else {
        mMessageBoxFrame->setInfo("wrong", true, false) ;
    }
}
//send signals to switchUI
void HomePage::buttonGroupClicked(QAbstractButton* btn)
{
    qDebug()<<btn->objectName() ;
    if(btn->objectName() == "mDeviceSettingsBtn"){
        emit switchUI(1) ;
    }else if (btn->objectName() == "mFaceInputBtn") {
        emit switchUI(33);
        mMessageBoxFrame->setInfo("", false, true) ;
        mMessageBoxFrame->show() ;
    }

}

void HomePage::initUI()
{
    //head
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
	
    mTitleFrame = new QFrame(this) ;
    mTitleFrame->setObjectName("mTitleFrame") ;
    mTitleFrame->setGeometry(0, 0, this->width(), 88) ;
    mRebootBtn = new QPushButton(mTitleFrame) ;
    mRebootBtn->setObjectName("mRebootBtn") ;
    mRebootBtn->setGeometry(700, 14, 60, 60) ;
    mTitle = new QLabel(mTitleFrame) ;
    mTitle->setObjectName("mTitle") ;
    mTitle->setGeometry((800-350)/2, 24, 350, 36) ;
    //video show
    mVideoLab = new QLabel(this) ;
    mVideoLab->setObjectName("mVideoLab") ;
    mVideoLab->setGeometry(this->x()+10, 88+10, 800-20, 1280-88-10-100-20) ;
    mVideoLab->setAlignment(Qt::AlignCenter) ;
    mVideoLab->setText(QString::fromUtf8("��ҳ������...")) ;
    mVideoLab->setScaledContents(true);
	mVideoLab->hide();

    mProgressBar = new QProgressBar(this) ;
    mProgressBar->setObjectName("mProgressBar") ;
    mProgressBar->move(20, 88+20) ;
    mProgressBar->setMaximum(0) ;
    mProgressBar->setMinimum(0) ;
    mProgressBar->hide() ;

    //bottom
    mBottomFrame = new QFrame(this) ;
    mBottomFrame->setObjectName("mBottomFrame") ;
    mBottomFrame->setGeometry(0,this->height()-120, this->width(), 120) ;
    mDeviceSettingsBtn = new QPushButton(mBottomFrame) ;
    mDeviceSettingsBtn->setObjectName("mDeviceSettingsBtn") ;
    mDeviceSettingsBtn->setGeometry(125, 0, 100, 90) ;
    mDBUpdateBtn = new QPushButton(mBottomFrame) ;
    mDBUpdateBtn->setObjectName("mDBUpdateBtn") ;
    mDBUpdateBtn->setGeometry(350, 0, 100, 90) ;
    //connect(mDBUpdateBtn, SIGNAL(clicked()) , this, SLOT(updateDB())) ;
    mFaceInputBtn = new QPushButton(mBottomFrame) ;
    mFaceInputBtn->setObjectName("mFaceInputBtn") ;
    mFaceInputBtn->setGeometry(575, 0, 100, 90) ;

    //按钮组
    mButtonGroup = new QButtonGroup(this) ;
    mButtonGroup->addButton(mDeviceSettingsBtn, 0) ;
    mButtonGroup->addButton(mDBUpdateBtn, 1) ;
    mButtonGroup->addButton(mFaceInputBtn, 2) ;
    connect(mButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)) , this , SLOT(buttonGroupClicked(QAbstractButton*))) ;
}
void HomePage::showTime()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy/MM/dd hh:mm:ss");
    mTitle->setText(current_date) ;
}

void HomePage::updateUI(cv::Mat frame) {
    return;
}

void HomePage::updateDB()
{
    qDebug()<<"更新数据库" ;
    mProgressBar->show() ;
    mProgressBar->setRange(0,100000) ;      //需要获得总的文件数量以及当前已更新的文件数量
    for(int i=0; i<100000+1; i++)
    {
        mProgressBar->setValue(i) ;
    }
}





