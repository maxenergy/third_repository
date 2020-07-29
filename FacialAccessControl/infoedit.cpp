#include "infoedit.h"
#include "ui_infoedit.h"
#include <QDebug>

InfoEdit::InfoEdit(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::InfoEdit)
{
    ui->setupUi(this);
    setFixedSize(800, 1280-20) ; //sub 200 for test,need to restore
    mCheckBoxGroup = new QButtonGroup(this) ;
    connect(mCheckBoxGroup, SIGNAL(buttonClicked(int)), this, SLOT(checkBoxClicked(int))) ;
    initUI() ;
    connect(mBackBtn, SIGNAL(clicked()), this, SLOT(backBtnClicked())) ;
    connect(mSaveBtn, SIGNAL(clicked()), this, SLOT(store())) ;
    drawTable(ui->infotab, 6) ;
    mIsCap = false ;
    RGB_BUF = 0;
}

InfoEdit::~InfoEdit()
{
    delete ui;
}

void InfoEdit::initUI()
{
    //head
    mTitleFrame = new QFrame(this) ;
    mTitleFrame->setObjectName("mTitleFrame") ;
    mTitleFrame->setGeometry(0, 0, this->width(), 88) ;
    mBackBtn = new QPushButton(mTitleFrame) ;
    mBackBtn->setObjectName("mBackBtn") ;
    mBackBtn->setGeometry(40, 14, 60, 60) ;
    mSaveBtn = new QPushButton(mTitleFrame) ;
    mSaveBtn->setObjectName("mSaveBtn") ;
    mSaveBtn->setGeometry(700, 14, 60, 60) ;
    mTitle = new QLabel(mTitleFrame) ;
    mTitle->setObjectName("mTitle") ;
    mTitle->setGeometry((800-144)/2, 24, 144, 36) ;
    mTitle->setText(QString::fromUtf8("信息编辑")) ;

    mInfoFrame = new QFrame(this) ;
    mInfoFrame->setObjectName("mInfoFrame") ;
    mInfoFrame->setGeometry(20, 52 + 88, 760, 1000) ;
    mNameLab = new QLabel(mInfoFrame) ;
    mNameLab->setObjectName("mModeSelectionLab") ;
    mNameLab->move(20,47) ;
    mNameLab->setText(QString::fromUtf8("姓名")) ;
    mNameEdit = new QLineEdit(mInfoFrame) ;
    mNameEdit->setObjectName("mNameEdit") ;
    mNameEdit->setGeometry(100, 40,380, 60) ;
    mNameEdit->setFocus() ;
    mUserIDLab = new QLabel(mInfoFrame) ;
    mUserIDLab->setObjectName("mModeSelectionLab") ;
    mUserIDLab->move(20, 147) ;
    mUserIDLab->setText(QString::fromUtf8("工号")) ;
    mUserIDEdit = new QLineEdit(mInfoFrame) ;
    mUserIDEdit->setObjectName("mUserIDEdit") ;
    mUserIDEdit->setGeometry(100, 140, 380, 60) ;

    mFaceBtn = new customerqlabel(mInfoFrame);
    mFaceBtn->setObjectName("mFaceBtn");
    QPixmap pmix;// = new QPixmap();
    pmix.load(":/img/bg.png");
    mFaceBtn->setPixmap(pmix);
    mFaceBtn->setText(QString::fromUtf8("录入人脸")) ;
    mFaceBtn->setGeometry(575, 20, 160, 210);

    //mFaceBtn->move(580, 40);
	
    connect(mFaceBtn, SIGNAL(clicked()), this, SLOT(faceInputClicked())) ;
	
    mLitleLab = new QLabel(mInfoFrame) ;
    mLitleLab->setObjectName("mLitleLab") ;
    mLitleLab->setGeometry(20, 240, 6, 36) ;
    mLimitsLab = new QLabel(mInfoFrame) ;
    mLimitsLab->setObjectName("mModeSelectionLab") ;
    mLimitsLab->move(42, 234) ;
    mLimitsLab->setText(QString::fromUtf8("权限")) ;
    ui->infotab->setParent(mInfoFrame) ;
    ui->infotab->setGeometry(20,314, 720, 464) ;
    ui->infotab->setColumnCount(2) ;
    QStringList headerLabels  ;
    headerLabels << QString::fromUtf8("描述") << QString::fromUtf8("权限") ;
    ui->infotab->setHorizontalHeaderLabels(headerLabels) ;
    ui->infotab->verticalHeader()->setMinimumWidth(80) ;
    ui->infotab->verticalHeader()->setDefaultAlignment(Qt::AlignCenter) ;
    ui->infotab->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter) ;
    ui->infotab->horizontalHeader()->setMinimumHeight(104) ;
    ui->infotab->horizontalScrollBar()->hide() ;
    ui->infotab->horizontalScrollBar()->setEnabled(false) ;
    ui->infotab->setColumnWidth(0, 452);
    ui->infotab->setColumnWidth(1, 188) ;
#if 0
	QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor(0x00,0x00,0x00,0x00));

    mFaceInputFrame = new QFrame(this) ;
    mFaceInputFrame->setObjectName("mFaceInputFrame") ;
    //mFaceInputFrame->setGeometry(60, 233, 680, 540) ;
    mFaceInputFrame->setGeometry(0, 80, 800, 1100) ;
    mFaceInputFrame->hide() ;
	mFaceInputFrame->setPalette(pal);

    mVideoLab = new QLabel(mFaceInputFrame) ;
    mVideoLab->setObjectName("mPicLab") ;
   // mVideoLab->setGeometry(20, 30, 640, 480) ;
    mVideoLab->setAlignment(Qt::AlignCenter);
    mVideoLab->setText("starting...") ;
	mVideoLab->hide();
#endif
    mPicLab = new QLabel(this) ;
    mPicLab->setObjectName("mPicLab");
   // mPicLab->setGeometry(20, 30, 640, 480) ;
    mPicLab->setGeometry(0, 88, 800, 1280-88-100-20-20) ;
    mPicLab->setAlignment(Qt::AlignCenter) ;
    mPicLab->setText("starting...") ;
    mPicLab->hide() ;

    mCapBtn = new QPushButton(this) ;
    mCapBtn->setObjectName("info_cap_btn") ;
    mCapBtn->setGeometry(10+800/2-30,30+1100-10-60,60,60) ;
	mCapBtn->hide();
    connect(mCapBtn, SIGNAL(clicked()), this, SLOT(capture())) ;

    mReCapBtn = new QPushButton(this) ;
    mReCapBtn->setObjectName("info_back_btn") ;
    mReCapBtn->setText(QString::fromUtf8("返回")) ;
    mReCapBtn->setGeometry(50,30+1100-10-60,60,60) ;
    mCapBtn->hide();
    connect(mReCapBtn, SIGNAL(clicked()), this, SLOT(reCapture())) ;

//    mKeyFrame = new QFrame(this) ;
//    mKeyFrame->hide() ;
//    mKeyFrame->setStyleSheet("background-color: rgb(255, 255, 255);") ;
    mKeyBoard = new KeyBoard(this) ;
    connect(mKeyBoard, SIGNAL(sendStr(QString)), this, SLOT(getStrFromKey(QString))) ;
    mKeyBoard->move(this->x(), this->height()-mKeyBoard->height()-30) ;
    mNameEdit->installEventFilter(this);
    mUserIDEdit->installEventFilter(this);
}

void InfoEdit::drawTable(QTableWidget *table, int Row)
{
    table->setRowCount(Row);
    for(int i = 0 ; i < Row ; i++){
        ui->infotab->setRowHeight(i, 60) ;
        //describe
        QTableWidgetItem *item = new QTableWidgetItem ;
        item->setText(QString::fromUtf8("233333 门禁 B栋1楼101"));
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        item->setTextAlignment(Qt::AlignCenter);
        ui->infotab->setItem(i, 0, item) ;
        //checkbox
        QCheckBox* box = new QCheckBox(mInfoFrame) ;
        box->setObjectName("limtBox") ;
        box->setText("enable") ;
        mCheckBoxGroup->addButton(box, i) ;
        mCheckBoxGroup->setExclusive(false);
       // box->setCheckState(Qt::Checked) ;
        ui->infotab->setCellWidget(i, 1, box) ;
    }
}

void InfoEdit::backBtnClicked()
{
	QPixmap pmix;// = new QPixmap();
    pmix.load(":/img/bg.png");
	mFaceBtn->setPixmap(pmix);
	mFaceBtn->setText(QString::fromUtf8("录入人脸"));
	mNameEdit->setText("");
	mUserIDEdit->setText("");
    emit backFaceInputPage(0) ;
}

void InfoEdit::checkBoxClicked(int id)
{
    QAbstractButton *pButton =mCheckBoxGroup->button(id);
       QCheckBox *pBox =(QCheckBox*)pButton;
       if(pBox->checkState()==Qt::Checked)
       {
           QString str;
           int  nNum =id + 1;
           str.setNum(nNum);
           mInfoMap[id] = "233333";
           //ui->tableWidget->setItem(id,0,new QTableWidgetItem(str));
       }
       else
       {
            mInfoMap.remove(id) ;
           //ui->tableWidget->takeItem(id,0);
       }
}

void InfoEdit::store()
{
   // std::string userName = mNameEdit->text().toStdString();
    std::string userName = mNameEdit->text().toUtf8().constData();
    int userId = mUserIDEdit->text().toInt() ;
    if(userName !="" && userId != -1)
    {
        if (!mBtnPhoto.empty()) {
            if (!FaceRecognitionApi::getInstance().updateFaceInfo(userId, mBtnPhoto)) {
                std::cout << "face info update failed" << std::endl;
                return;
            } else {
                std::cout << "face info update success" << std::endl;
            }
        }
		mBtnPhoto.release();
        //保存用户姓名ID
        UserInfo user;
        user.mUserName = userName;
        user.mUserID = userId ;
        if (FaceRecognitionApi::getInstance().updateUserInfo(user) == false) {
            return;
        }
        std::cout << "add {" << userId << ", "<< userName << "} ok" << std::endl;
        mCurrentUserId = userId;

        //保存用户权限
        PermissionInfo permission ;
        permission.mUserID = userId ;
        if(!mInfoMap.empty())
        {
            QMap<int, QString>::Iterator iter;
            for(iter = mInfoMap.begin(); iter != mInfoMap.end(); iter++)
            {
                //qDebug()<<iter.key()<<"  "<<iter.value();
                permission.mDeviceID = iter.value().toInt() ;
                permission.mPermission = 1 ;
                FaceRecognitionApi::getInstance().updatePermissionInfo(permission) ;
            }
        }
        else {
            //未选择设备权限，确定保存？
        }
    }
    else {
        //用户名工号不能为空
    }
}

void InfoEdit::faceInputClicked()
{
  //  mFaceInputFrame->show() ;
	mCapBtn->show();
	mReCapBtn->show();
	mInfoFrame->hide();
}

void InfoEdit::YVU420P_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height) {
	int index = 0;
	unsigned char *ybase = data;
	unsigned char *uvbase = &data[width * height];
	unsigned char *ubase = &data[width * height+width * height/4];
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
		//YYYYVU
			 unsigned char Y = ybase[x + y * width];
			 unsigned char V = uvbase[y / 2 * width + (x / 2)*2];
			 unsigned char U = uvbase[y / 2 * width + (x / 2)*2+1];

			
			int r = (float)Y + 1.4075 * ((float)V - 128);
			if(r >255)
				r = 255;
			else if(r <0)
				r =0;

			int g = (float)Y - 0.3455 * ((float)U - 128) - 0.7169 * ((float)V - 128); //G
			if(g >255)
				g = 255;
			else if(g <0)
				g =0;

			
			int b = (float)Y + 1.779 * ((float)U - 128); //B
			if(b >255)
				b = 255;
			else if(b <0)
				b =0;
			
			rgb[index++] = r;
			rgb[index++] = g; //G
			rgb[index++] = b; //B
		}
	}
}

void InfoEdit::capture()
{
    mIsCap = !mIsCap ;
    if(mIsCap)
    {
        stopPreview();
        FaceRecognitionApi::getInstance().capture(2,mBtnPhoto);
		if(RGB_BUF == 0){
			RGB_BUF = (unsigned char*)malloc(mBtnPhoto.mHeiht*mBtnPhoto.mWidth*3);
		}
		cv::Mat yuvFrame = cv::Mat(mBtnPhoto.mHeiht*3/2, mBtnPhoto.mWidth, CV_8UC1, mBtnPhoto.mData);
		cv::Mat dstImage;
		cv::cvtColor(yuvFrame, dstImage, cv::COLOR_YUV420sp2RGB);
		memcpy(RGB_BUF,(unsigned char*)dstImage.data,mBtnPhoto.mHeiht*mBtnPhoto.mWidth*3);
		QImage image = QImage((unsigned char*)dstImage.data,dstImage.cols, dstImage.rows,dstImage.step,QImage::Format_RGB888);
        QImage scaledImage = image.copy(0, 88,mPicLab->width(), mPicLab->height());
        mPicLab->setPixmap(QPixmap::fromImage(scaledImage));
		mPicLab->show();
        mCapBtn->setText("OK");
    }else{
        startPreview();
		mCapBtn->hide();
		mReCapBtn->hide();
		mPicLab->hide();
		mInfoFrame->show();
        mCapBtn->setText("");
		QImage image(RGB_BUF, mBtnPhoto.mWidth,  mBtnPhoto.mHeiht,QImage::Format_RGB888);
		QImage scaledImage = image.copy(0, 88,mPicLab->width(), mPicLab->height());
		QImage scaledImage2 = scaledImage.scaled(mFaceBtn->width(), mFaceBtn->height(), Qt::IgnoreAspectRatio);
		mFaceBtn->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        mFaceBtn->setPixmap(QPixmap::fromImage(scaledImage2));
    }
}

void InfoEdit::reCapture()
{
    mIsCap = false ;
    mCapBtn->setText("") ;
    if (mIsStopPreview == false) {
		mCapBtn->hide();
		mReCapBtn->hide();
		mInfoFrame->show();
		mPicLab->hide();
		QPixmap pmix;// = new QPixmap();
        pmix.load(":/img/bg.png");
		mFaceBtn->setPixmap(pmix);
		mFaceBtn->setText(QString::fromUtf8("录入人脸")) ;
    } else {
        startPreview();
    }
}

void InfoEdit::updateUI(cv::Mat mat) {
    if (mIsStopPreview == true) {
        return;
    }
	mPicLab->hide();
   //  QImage image(mat.data,mat.cols, mat.rows, mat.step, QImage::Format_RGB888) ;
   // mVideoLab->setPixmap(QPixmap::fromImage(image.rgbSwapped()));
}

void InfoEdit::getStrFromKey(QString word)
{
    //qDebug()<<word<<str1 ;
    if(str1 == "mNameEdit")     //不能重objectName，要改qss
    {
        if(word=="BackSpace"){
            QString str = mNameEdit->text() ;
            str.chop(1) ;
            mNameEdit->setText(str) ;
        }else {
            QString str = mNameEdit->text() + word ;
            mNameEdit->setText(str) ;
        }
    }else if (str1 == "mUserIDEdit") {
        if(word=="BackSpace"){
            QString str = mUserIDEdit->text() ;
            str.chop(1) ;
            mUserIDEdit->setText(str) ;
        }else {
            QString str = mUserIDEdit->text() + word ;
            mUserIDEdit->setText(str) ;
        }
    }

}
//鼠标点击事件，单击使键盘消失
void InfoEdit::mousePressEvent(QMouseEvent *event)
{
    //左击事件
   if(event->button() == Qt::LeftButton)
   {
       //mKeyFrame->hide();
       //qDebug()<<"DDDD" ;
   }

}

bool InfoEdit::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == mNameEdit || obj == mUserIDEdit)
    {
        if(e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *event = static_cast<QMouseEvent*>(e) ;
              if(event->button() == Qt::LeftButton)
            {
                mKeyBoard->show() ;
                qDebug()<<obj->objectName() ;
                str1 = obj->objectName() ;        //记录下选中的lineEdit
            }

        }
        else
            return false ;
    }
    else
    {
        return InfoEdit::eventFilter(obj , e) ;
    }
}

void InfoEdit::startPreview() {
    mIsStopPreview = false;
	mPicLab->hide();
}

void InfoEdit::stopPreview() {
    mIsStopPreview = true;
}
