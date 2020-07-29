#include "faceinput.h"
#include "ui_faceinput.h"
#include "facerecognition_public.h"
#include <QTextCodec>

#include <QDebug>
FaceInput::FaceInput(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FaceInput)
{
    ui->setupUi(this);
    setFixedSize(800, 1280-20) ; //sub 200 for test,need to restore
    mIsCap = false ;
    //人脸录入
    initUI() ;
    connect(mBackBtn, SIGNAL(clicked()), this, SLOT(backBtnClicked())) ;
    connect(mCapBtn, SIGNAL(clicked()), this, SLOT(capBtnClicked())) ;
    connect(mInfoBtn, SIGNAL(clicked()), this, SLOT(infoBtnClicked())) ;
    connect(mInputBtn, SIGNAL(clicked()), this, SLOT(inputBtnClicked())) ;
}

FaceInput::~FaceInput()
{
    delete ui;
}

void FaceInput::initUI()
{
    //head
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
	
    mTitleFrame = new QFrame(this) ;
    mTitleFrame->setObjectName("mTitleFrame");
    mTitleFrame->setGeometry(0, 0, this->width(), 88);
    mBackBtn = new QPushButton(mTitleFrame);
    mBackBtn->setObjectName("mBackBtn");
    mBackBtn->setGeometry(40, 14, 60, 60);
    mTitle = new QLabel(mTitleFrame);
    mTitle->setObjectName("mTitle");
    mTitle->setGeometry((800-144)/2, 26, 144, 36);
    mTitle->setText(QString::fromUtf8("����¼��"));

    //show video
    mPicLab = new QLabel(this) ;
    mPicLab->setObjectName("mPicLab") ;
    mPicLab->setGeometry(80, this->y()+332 + 88, 640, 480) ;
    mPicLab->setAlignment(Qt::AlignCenter) ;
    mPicLab->setText("starting...") ;
    mPicLab->hide();

    //bottom
    mBottomFrame = new QFrame(this) ;
    mBottomFrame->setObjectName("mBottomFrame") ;
    mBottomFrame->setGeometry(0,this->height()-88, this->width(), 88) ;
	
    mCapBtn = new QPushButton(mBottomFrame) ;
    mCapBtn->setObjectName("mCapBtn");
    mCapBtn->setGeometry(125, 0, 100, 90);
    mInfoBtn = new QPushButton(mBottomFrame);
    mInfoBtn->setObjectName("mInfoBtn");
    mInfoBtn->setGeometry(350, 0, 100, 90);
    mInputBtn = new QPushButton(mBottomFrame);
    mInputBtn->setObjectName("mInputBtn");
    mInputBtn->setGeometry(575, 0, 100, 90);
}

void FaceInput::capBtnClicked()
{
/*
    //拍照按钮按下
    mIsCap = !mIsCap ;
    if(mIsCap)
    {
        FaceRecognitionApi::getInstance().capture(mPhoto) ;
        if (mPhoto.empty()) {
            return;
        }
        QImage image(mPhoto.data ,mPhoto.cols, mPhoto.rows, mPhoto.step, QImage::Format_RGB888) ;
        ui->picLab->setGeometry(mPicLab->geometry()) ;
        ui->picLab->setPixmap(QPixmap::fromImage(image.rgbSwapped()));
        ui->picLab->show() ;
        ui->picLab->raise() ;
        mCapBtn->setStyleSheet("border-image: url(:/img/recap_blue.png);") ;
    }else {
        ui->picLab->hide() ;
        mCapBtn->setStyleSheet("border-image: url(:/img/cap_blue.png);") ;
    }*/
}

void FaceInput::infoBtnClicked()
{
    //信息编辑按钮按下
    emit switchUI(32) ;
    qDebug()<<"信息编辑按钮按下" ;
}

void FaceInput::inputBtnClicked()
{
/*
    //录入按钮按下
    qDebug()<<"录入按钮按下" ;
    if (!FaceRecognitionApi::getInstance().updateFaceInfo(mCurrentUserId, mPhoto)) {
        std::cout << "face info update failed" << std::endl;
        return;
    } else {
        std::cout << "face info update success" << std::endl;
    }*/
}

void FaceInput::backBtnClicked()
{
    mIsCap = false ;
    ui->picLab->hide() ;
    mCapBtn->setStyleSheet("border-image: url(:/img/cap_blue.png);") ;
    backHomePage(0) ;
}

void FaceInput::updateUI(cv::Mat mat) {
   // QImage image(mat.data,mat.cols, mat.rows, mat.step, QImage::Format_RGB888) ;
   // mPicLab->setPixmap(QPixmap::fromImage(image.rgbSwapped()));
}
