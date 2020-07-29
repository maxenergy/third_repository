#include "devicesettings.h"
#include "ui_devicesettings.h"
#include "minIni.h"
#include <QDebug>
#include <QTextCodec>
#include <string>

using namespace std;

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

#define MAX_PARAM_SIZE 20
extern int param_check(const char* str);

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

    /* Set Device Manually */
    /* show current settings from InitConfig.ini */
    char camera_ip_addr[MAX_PARAM_SIZE];
    char camera_ip_mask[MAX_PARAM_SIZE];
    char camera_ip_gateway[MAX_PARAM_SIZE];
    char ftp_server_ip[MAX_PARAM_SIZE];
    char mqtt_server_ip[MAX_PARAM_SIZE];
    char g_device_sn[MAX_PARAM_SIZE];
    string temp;

    minIni ini("InitConfig.ini");

    memset(camera_ip_addr,0,MAX_PARAM_SIZE);
    temp = ini.gets("CAMERA","camera_ip_addr","222.191.244.203");
    if(param_check(temp.c_str())){
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(camera_ip_addr,"%s",temp.c_str());

    memset(camera_ip_mask,0,MAX_PARAM_SIZE);
    temp = ini.gets("CAMERA","camera_ip_mask","255.255.255.0");
    if(param_check(temp.c_str())){
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(camera_ip_mask,"%s",temp.c_str());

    memset(camera_ip_gateway,0,MAX_PARAM_SIZE);
    temp = ini.gets("CAMERA","camera_ip_gateway","222.191.244.203");
    if(param_check(temp.c_str())){
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(camera_ip_gateway,"%s",temp.c_str());

    memset(ftp_server_ip,0,MAX_PARAM_SIZE);
    temp = ini.gets("FTP","ftp_server_ip","222.191.244.203");
    if(param_check(temp.c_str())){
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(ftp_server_ip,"%s",temp.c_str());

    memset(mqtt_server_ip,0,MAX_PARAM_SIZE);
    temp = ini.gets("MQTT","mqtt_server_ip","222.191.244.203");
    if(param_check(temp.c_str())){
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(mqtt_server_ip,"%s",temp.c_str());

    memset(g_device_sn,0,MAX_PARAM_SIZE);
    temp = ini.gets("CAMERA","g_device_sn","SN00001");
    if(param_check(temp.c_str())){
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(g_device_sn,"%s",temp.c_str());

    /* IP Address */
    mDeviceIpAddrSetLab = new QLabel(mSettingFrame);
    mDeviceIpAddrSetLab->setObjectName("mDeviceIpAddrSetLab");
    mDeviceIpAddrSetLab->move(parentWidget()->x()+20, parentWidget()->y()+60);
    mDeviceIpAddrSetLab->setText(QString::fromUtf8("IP地址:"));

    mDeviceIpAddrEdit = new QLineEdit(mSettingFrame);
    mDeviceIpAddrEdit->setObjectName("mDeviceIpAddrEdit") ;
    mDeviceIpAddrEdit->setGeometry(parentWidget()->x()+20+260, parentWidget()->y()+60,400,60) ;
    mDeviceIpAddrEdit->setText(QString::fromUtf8(camera_ip_addr)) ;

    /* IP Mask */
    mDeviceIpMaskSetLab = new QLabel(mSettingFrame);
    mDeviceIpMaskSetLab->setObjectName("mDeviceIpMaskSetLab");
    mDeviceIpMaskSetLab->move(parentWidget()->x()+20,parentWidget()->y()+120);
    mDeviceIpMaskSetLab->setText(QString::fromUtf8("子网掩码:"));

    mDeviceIpMaskEdit = new QLineEdit(mSettingFrame);
    mDeviceIpMaskEdit->setObjectName("mDeviceIpMaskEdit") ;
    mDeviceIpMaskEdit->setGeometry(parentWidget()->x()+20+260, parentWidget()->y()+120,400,60) ;
    mDeviceIpMaskEdit->setText(QString::fromUtf8(camera_ip_mask)) ;

    /* Gateway */
    mDeviceIpGatewaySetLab = new QLabel(mSettingFrame);
    mDeviceIpGatewaySetLab->setObjectName("mDeviceIpGatewaySetLab");
    mDeviceIpGatewaySetLab->move(parentWidget()->x()+20,parentWidget()->y()+180);
    mDeviceIpGatewaySetLab->setText(QString::fromUtf8("默认网关:"));

    mDeviceIpGatewayEdit = new QLineEdit(mSettingFrame);
    mDeviceIpGatewayEdit->setObjectName("mDeviceIpGatewayEdit") ;
    mDeviceIpGatewayEdit->setGeometry(parentWidget()->x()+20+260, parentWidget()->y()+180,400,60) ;
    mDeviceIpGatewayEdit->setText(QString::fromUtf8(camera_ip_gateway)) ;

    /* mqtt server ip address */
    mDeviceMQTTServerIpAddrSetLab = new QLabel(mSettingFrame);
    mDeviceMQTTServerIpAddrSetLab->setObjectName("mDeviceMQTTServerIpAddrSetLab");
    mDeviceMQTTServerIpAddrSetLab->move(parentWidget()->x()+20,parentWidget()->y()+240);
    mDeviceMQTTServerIpAddrSetLab->setText(QString::fromUtf8("MQTT服务器地址:"));

    mDeviceMQTTServerIpAddrEdit = new QLineEdit(mSettingFrame);
    mDeviceMQTTServerIpAddrEdit->setObjectName("mDeviceMQTTServerIpAddrEdit") ;
    mDeviceMQTTServerIpAddrEdit->setGeometry(parentWidget()->x()+20+260, parentWidget()->y()+240,400,60) ;
    mDeviceMQTTServerIpAddrEdit->setText(QString::fromUtf8(mqtt_server_ip)) ;

    /* ftp ip address */
    mDeviceFtpIpAddrSetLab = new QLabel(mSettingFrame);
    mDeviceFtpIpAddrSetLab->setObjectName("mDeviceFtpIpAddrSetLab");
    mDeviceFtpIpAddrSetLab->move(parentWidget()->x()+20,parentWidget()->y()+300);
    mDeviceFtpIpAddrSetLab->setText(QString::fromUtf8("FTP服务器地址:"));

    mDeviceFtpIpAddrEdit = new QLineEdit(mSettingFrame);
    mDeviceFtpIpAddrEdit->setObjectName("mDeviceFtpIpAddrEdit") ;
    mDeviceFtpIpAddrEdit->setGeometry(parentWidget()->x()+20+260, parentWidget()->y()+300,400,60) ;
    mDeviceFtpIpAddrEdit->setText(QString::fromUtf8(ftp_server_ip)) ;

    /* Device ID */
    mDeviceIdSetLab = new QLabel(mSettingFrame);
    mDeviceIdSetLab->setObjectName("mDeviceIdSetLab");
    mDeviceIdSetLab->move(parentWidget()->x()+20,parentWidget()->y()+360);
    mDeviceIdSetLab->setText(QString::fromUtf8("设备ID:"));

    mDeviceIdSetEdit = new QLineEdit(mSettingFrame);
    mDeviceIdSetEdit->setObjectName("mDeviceIdSetEdit") ;
    mDeviceIdSetEdit->setGeometry(parentWidget()->x()+20+260, parentWidget()->y()+360,400,60) ;
    mDeviceIdSetEdit->setText(QString::fromUtf8(g_device_sn)) ;

    /* vitual keyboard */
    mKeyBoard = new KeyBoard(this) ;
    connect(mKeyBoard, SIGNAL(sendStr(QString)), this, SLOT(getStrFromKey(QString))) ;
    mKeyBoard->move(this->x(), this->height()-mKeyBoard->height()-30) ;
    mDeviceIpAddrEdit->installEventFilter(this);
    mDeviceIpMaskEdit->installEventFilter(this);
    mDeviceIpGatewayEdit->installEventFilter(this);
    mDeviceIpMaskEdit->installEventFilter(this);
    mDeviceMQTTServerIpAddrEdit->installEventFilter(this);
    mDeviceFtpIpAddrEdit->installEventFilter(this);
    mDeviceIdSetEdit->installEventFilter(this);

}

void DeviceSettings::backBtnClicked()
{
    qDebug()<<"DeviceSettings" ;
    backHomePage(0) ;
}

void DeviceSettings::store()
{
    bool is_success;

    char camera_ip_addr[MAX_PARAM_SIZE];
    char camera_ip_mask[MAX_PARAM_SIZE];
    char camera_ip_gateway[MAX_PARAM_SIZE];
    char ftp_server_ip[MAX_PARAM_SIZE];
    char mqtt_server_ip[MAX_PARAM_SIZE];
    char g_device_sn[MAX_PARAM_SIZE];

    minIni ini("InitConfig.ini");

    memset(camera_ip_addr,0,MAX_PARAM_SIZE);
    memset(camera_ip_mask,0,MAX_PARAM_SIZE);
    memset(camera_ip_gateway,0,MAX_PARAM_SIZE);
    memset(ftp_server_ip,0,MAX_PARAM_SIZE);
    memset(mqtt_server_ip,0,MAX_PARAM_SIZE);
    memset(g_device_sn,0,MAX_PARAM_SIZE);

    if(param_check(mDeviceIpAddrEdit->text().toUtf8().constData()))
    {
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(camera_ip_addr, "%s", mDeviceIpAddrEdit->text().toUtf8().constData());

    if(param_check(mDeviceIpMaskEdit->text().toUtf8().constData()))
    {
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(camera_ip_mask, "%s", mDeviceIpMaskEdit->text().toUtf8().constData());

    if(param_check(mDeviceIpGatewayEdit->text().toUtf8().constData()))
    {
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(camera_ip_gateway, "%s", mDeviceIpGatewayEdit->text().toUtf8().constData());

    if(param_check(mDeviceFtpIpAddrEdit->text().toUtf8().constData()))
    {
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(ftp_server_ip, "%s", mDeviceFtpIpAddrEdit->text().toUtf8().constData());

    if(param_check(mDeviceMQTTServerIpAddrEdit->text().toUtf8().constData()))
    {
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(mqtt_server_ip, "%s", mDeviceMQTTServerIpAddrEdit->text().toUtf8().constData());

    if(param_check(mDeviceIdSetEdit->text().toUtf8().constData()))
    {
        printf("error str len > maxsize \n",MAX_PARAM_SIZE);
        return;
    }
    sprintf(g_device_sn, "%s", mDeviceIdSetEdit->text().toUtf8().constData());

    qDebug()<<camera_ip_addr;
    qDebug()<<camera_ip_mask;
    qDebug()<<camera_ip_gateway;
    qDebug()<<ftp_server_ip;
    qDebug()<<mqtt_server_ip;
    qDebug()<<g_device_sn;

    is_success = ini.put("CAMERA","camera_ip_addr",camera_ip_addr);
    if(!is_success)
    {
        printf("ERROR: ini.put CAMERA camera_ip_addr failed.\n ");
        return;
    }

    is_success = ini.put("CAMERA","camera_ip_mask",camera_ip_mask);
    if(!is_success)
    {
        printf("ERROR: ini.put CAMERA camera_ip_mask failed.\n ");
        return;
    }

    is_success = ini.put("CAMERA","camera_ip_gateway",camera_ip_gateway);
    if(!is_success)
    {
        printf("ERROR: ini.put CAMERA camera_ip_gateway failed.\n ");
        return;
    }

    is_success = ini.put("FTP","ftp_server_ip",ftp_server_ip);
    if(!is_success)
    {
        printf("ERROR: ini.put FTP ftp_server_ip failed.\n ");
        return;
    }

    is_success = ini.put("MQTT","mqtt_server_ip",mqtt_server_ip);
    if(!is_success)
    {
        printf("ERROR: ini.put MQTT mqtt_server_ip failed.\n ");
        return;
    }

    is_success = ini.put("CAMERA","g_device_sn",g_device_sn);
    if(!is_success)
    {
        printf("ERROR: ini.put CAMERA g_device_sn failed.\n ");
        return;
    }

    printf("device has configed,nned reboot!\n");
    system("reboot");
}

void DeviceSettings::getStrFromKey(QString word)
{
    //qDebug()<<word<<str1 ;
    if(str1 == "mDeviceIpAddrEdit")     //不能重objectName，要改qss
    {
        if(word=="BackSpace"){
            QString str = mDeviceIpAddrEdit->text() ;
            str.chop(1) ;
            mDeviceIpAddrEdit->setText(str) ;
        }else {
            QString str = mDeviceIpAddrEdit->text() + word ;
            mDeviceIpAddrEdit->setText(str) ;
        }
    }else if (str1 == "mDeviceIpMaskEdit") {
        if(word=="BackSpace"){
            QString str = mDeviceIpMaskEdit->text() ;
            str.chop(1) ;
            mDeviceIpMaskEdit->setText(str) ;
        }else {
            QString str = mDeviceIpMaskEdit->text() + word ;
            mDeviceIpMaskEdit->setText(str) ;
        }
    }else if (str1 == "mDeviceIpGatewayEdit"){
        if(word=="BackSpace"){
            QString str = mDeviceIpGatewayEdit->text() ;
            str.chop(1) ;
            mDeviceIpGatewayEdit->setText(str) ;
        }else {
            QString str = mDeviceIpGatewayEdit->text() + word ;
            mDeviceIpGatewayEdit->setText(str) ;
        }
    }else if (str1 == "mDeviceMQTTServerIpAddrEdit"){
	if(word=="BackSpace"){
            QString str = mDeviceMQTTServerIpAddrEdit->text() ;
            str.chop(1) ;
            mDeviceMQTTServerIpAddrEdit->setText(str) ;
        }else {
            QString str = mDeviceMQTTServerIpAddrEdit->text() + word ;
            mDeviceMQTTServerIpAddrEdit->setText(str) ;
        }
    }else if (str1 == "mDeviceFtpIpAddrEdit"){
	if(word=="BackSpace"){
            QString str = mDeviceFtpIpAddrEdit->text() ;
            str.chop(1) ;
            mDeviceFtpIpAddrEdit->setText(str) ;
        }else {
            QString str = mDeviceFtpIpAddrEdit->text() + word ;
            mDeviceFtpIpAddrEdit->setText(str) ;
        }
    }else if (str1 == "mDeviceIdSetEdit"){
	if(word=="BackSpace"){
            QString str = mDeviceIdSetEdit->text() ;
            str.chop(1) ;
            mDeviceIdSetEdit->setText(str) ;
        }else {
            QString str = mDeviceIdSetEdit->text() + word ;
            mDeviceIdSetEdit->setText(str) ;
        }
    }
}

//鼠标点击事件，单击使键盘消失
void DeviceSettings::mousePressEvent(QMouseEvent *event)
{
    //左击事件
   if(event->button() == Qt::LeftButton)
   {
       //mKeyFrame->hide();
       //qDebug()<<"DDDD" ;
   }

}

bool DeviceSettings::eventFilter(QObject *obj, QEvent *e)
{
    if(obj==mDeviceIpAddrEdit||obj==mDeviceIpMaskEdit||obj==mDeviceIpGatewayEdit||
       obj==mDeviceFtpIpAddrEdit||obj||mDeviceIdSetEdit)
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
        return DeviceSettings::eventFilter(obj , e) ;
    }
}

void DeviceSettings::startPreview() {
    mIsStopPreview = false;
}

void DeviceSettings::stopPreview() {
    mIsStopPreview = true;
}

