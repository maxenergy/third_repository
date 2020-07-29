#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

#include <QFrame>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMouseEvent>

#include "keyboard.h"

namespace Ui {
class DeviceSettings;
}

class DeviceSettings : public QFrame
{
    Q_OBJECT

public:
    explicit DeviceSettings(QWidget *parent = nullptr);
    ~DeviceSettings();

    //鼠标点击事件重载
    void mousePressEvent(QMouseEvent *event)  ;
    //过滤器
    bool eventFilter(QObject *obj, QEvent *e) ;

private slots:
    void backBtnClicked() ;
    void store() ;
    void startPreview();
    void stopPreview();
    void getStrFromKey(QString word) ;

signals:
    void backHomePage(int pageNum) ;

private:
    Ui::DeviceSettings *ui;
    //head
    QFrame* mTitleFrame ;
    QLabel* mTitle ;
    QPushButton* mBackBtn ;
    QPushButton* mSaveBtn ;
    //settings
    QLabel *mDeviceIpAddrSetLab;
    QLineEdit *mDeviceIpAddrEdit ;
    QLabel *mDeviceIpMaskSetLab;
    QLineEdit *mDeviceIpMaskEdit ;
    QLabel *mDeviceIpGatewaySetLab;
    QLineEdit *mDeviceIpGatewayEdit;
    QLabel *mDeviceMQTTServerIpAddrSetLab;
    QLineEdit *mDeviceMQTTServerIpAddrEdit;
    QLabel *mDeviceFtpIpAddrSetLab;
    QLineEdit *mDeviceFtpIpAddrEdit;
    QLabel *mDeviceIdSetLab;
    QLineEdit *mDeviceIdSetEdit;

    QFrame* mSettingFrame ;

    bool mIsStopPreview = false;
    KeyBoard *mKeyBoard ;
    QString str1 ;
    QFrame* mKeyFrame ;

    void initUI() ;
};

#endif // DEVICESETTINGS_H
