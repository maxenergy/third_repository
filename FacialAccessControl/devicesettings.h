#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

#include <QFrame>
#include <QLabel>
#include <QCheckBox>
#include <QTextEdit>
#include <QPushButton>

namespace Ui {
class DeviceSettings;
}

class DeviceSettings : public QFrame
{
    Q_OBJECT

public:
    explicit DeviceSettings(QWidget *parent = nullptr);
    ~DeviceSettings();
private slots:
    void backBtnClicked() ;
    void store() ;
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
    QFrame* mSettingFrame ;
    QLabel* mModeSelectionLab ;
    QCheckBox* mKaoQinCheck ;
    QCheckBox* mAccessCheck ;
    QLabel* mDeviceDescribeLab ;
    QTextEdit* mDeviceDescribeEdit ;

    void initUI() ;
};

#endif // DEVICESETTINGS_H
