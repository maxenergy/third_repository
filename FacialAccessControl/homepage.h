#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QProgressBar>
#include <QTimer>
#include <QDateTime>
#include <opencv2/opencv.hpp>
#include <messageboxframe.h>

namespace Ui {
class HomePage;
}

class HomePage : public QFrame
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();
public slots:
    void buttonGroupClicked(QAbstractButton *btn) ;
    void showTime() ;
    void updateUI(cv::Mat mat) ;
    void updateDB() ;
	void hidemasgbox();
	 void getPassword(QString) ;
signals:
    void switchUI(int num) ;

private:
    Ui::HomePage *ui;
    //头
    QFrame* mTitleFrame ;
    QLabel* mTitle ;
    QPushButton* mRebootBtn ;
    //video
    QLabel* mVideoLab ;
    QProgressBar* mProgressBar ;
    //bottom
    QPushButton* mDeviceSettingsBtn ;
    QPushButton* mDBUpdateBtn ;
    QPushButton* mFaceInputBtn ;
    QFrame* mBottomFrame ;
    //time
    QTimer mTimer ;
    //按钮组
    QButtonGroup* mButtonGroup ;
    void initUI() ;
	MessageBoxFrame* mMessageBoxFrame ;
};

#endif // HOMEPAGE_H
