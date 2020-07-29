#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "facerecognition_public.h"
#include "facedetect.h"
#include <QDebug>
#include <QMetaType>
extern "C" {
extern void sysexit(void);
extern int SetIRLedLight(unsigned int light);
}
extern void draw_box(MtcnnInterface::Out detect_box);
extern void draw_blank(void);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800, 1280-20) ;       //sub 200 for test,need to restore
    setWindowFlags(Qt::CustomizeWindowHint);

   // pixmap.load(":/img/bg.png") ;
    //加载qss文件
    QFile qssFile(":/QSS/base.qss") ;
    qssFile.open(QFile::ReadOnly);
    QString qss;
    qss = qssFile.readAll();
    qssFile.close() ;
    this->setStyleSheet(qss);

    //首页界面
    mHomePage = new HomePage(this) ;
    mHomePage->show() ;
    //connect(this, SIGNAL(frameAviable(cv::Mat)), mHomePage, SLOT(updateUI(cv::Mat))) ;
    connect(this, SIGNAL(frameAviable_box()), this, SLOT(update_box(void)));
    connect(mHomePage, SIGNAL(switchUI(int)), this, SLOT(getPage(int))) ;

    //设备配置
    mDeviceSettings = new DeviceSettings(this) ;
    connect(mDeviceSettings, SIGNAL(backHomePage(int)), this, SLOT(getPage(int))) ;
    mDeviceSettings->hide() ;
    //人脸录入
    mFaceInput = new FaceInput(this) ;
    mFaceInput->hide() ;
    connect(mFaceInput, SIGNAL(backHomePage(int)), this, SLOT(getPage(int))) ;
    connect(mFaceInput, SIGNAL(switchUI(int)), this, SLOT(getPage(int))) ;
    //connect(this, SIGNAL(frameAviable(cv::Mat)), mFaceInput, SLOT(updateUI(cv::Mat))) ;

    //信息编辑
     mInfoEdit = new InfoEdit(this) ;
     connect(this, SIGNAL(frameAviable(cv::Mat)), mInfoEdit, SLOT(updateUI(cv::Mat))) ;
     connect(mInfoEdit, SIGNAL(backFaceInputPage(int)), this, SLOT(getPage(int))) ;
     mInfoEdit->hide() ;

    qRegisterMetaType<cv::Mat>("cv::Mat");
	
    FaceRecognitionApi::getInstance().startCameraPreview();
    //connect(this, SIGNAL(frameAviable(cv::Mat)), this, SLOT(updateUI(cv::Mat)));
   // FaceRecognitionApi::getInstance().setCameraPreviewCallBack([&](cv::Mat frame){
   FaceRecognitionApi::getInstance().setCameraPreviewCallBack([&](FaceDetect::Msg bob){
       save_box(bob);
	   emit frameAviable_box();
    });
	
	setWindowOpacity(1);
	setAttribute(Qt::WA_TranslucentBackground);
   // FaceRecognitionApi::getInstance().startCameraPreview();
}

void MainWindow::save_box(FaceDetect::Msg bob){
	bob_current = bob;	
}

void MainWindow::update_box()
{
	if(bob_current.mMtcnnInterfaceOut.mOutList.size() > 0){
		SetIRLedLight(100);
		draw_box(bob_current.mMtcnnInterfaceOut);
	}
	else{
		SetIRLedLight(0);
		draw_blank();
	}
}

void MainWindow::paintEvent(QPaintEvent *)
{

}
MainWindow::~MainWindow()
{
    delete ui;
	sysexit();
}


void MainWindow::getPage(int pageNum)
{
	qDebug() << "getPage :"<<pageNum<<"\n";

    switch (pageNum) {
    case 1:
		disconnect(this, SIGNAL(frameAviable_box()), this, SLOT(update_box(void)));
        mHomePage->hide() ;
        mDeviceSettings->show() ;
        break ;
    case 0:
        //connect(this, SIGNAL(frameAviable(cv::Mat)), mHomePage, SLOT(updateUI(cv::Mat))) ;
        //disconnect(this, SIGNAL(frameAviable(cv::Mat)), mFaceInput, SLOT(updateUI(cv::Mat))) ;
        connect(this, SIGNAL(frameAviable_box()), this, SLOT(update_box(void)));
        mDeviceSettings->hide() ;
        mFaceInput->hide() ;
        mInfoEdit->hide() ;
        mHomePage->show() ;
        break ;
    case 31:
	disconnect(this, SIGNAL(frameAviable_box()), this, SLOT(update_box(void)));
        mHomePage->hide() ;
        mInfoEdit->hide() ;
        mFaceInput->show() ;
        mFaceInput->mCurrentUserId = mInfoEdit->mCurrentUserId;
        //disconnect(this, SIGNAL(frameAviable(cv::Mat)), mHomePage, SLOT(updateUI(cv::Mat))) ;
        connect(this, SIGNAL(frameAviable(cv::Mat)), mFaceInput, SLOT(updateUI(cv::Mat))) ;
        break ;
    case 32 :
	disconnect(this, SIGNAL(frameAviable_box()), this, SLOT(update_box(void)));
        mHomePage->hide() ;
        mFaceInput->hide() ;
        mInfoEdit->show() ;
		break;
	case 33:
		disconnect(this, SIGNAL(frameAviable_box()), this, SLOT(update_box(void)));
		break;
	case 34:
		connect(this, SIGNAL(frameAviable_box()), this, SLOT(update_box(void)));
        break ;
    }
}


