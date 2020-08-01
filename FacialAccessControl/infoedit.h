#ifndef INFOEDIT_H
#define INFOEDIT_H

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QScrollBar>
#include <QButtonGroup>
#include <QVector>
#include <QMap>
#include <QMouseEvent>
#include "keyboard.h"
#include "facerecognition_public.h"
#include "customerqlabel.h"


namespace Ui {
class InfoEdit;
}

class InfoEdit : public QFrame
{
    Q_OBJECT

public:
    explicit InfoEdit(QWidget *parent = nullptr);
    ~InfoEdit();

    //鼠标点击事件重载
    void mousePressEvent(QMouseEvent *event)  ;
    //过滤器
    bool eventFilter(QObject *obj, QEvent *e) ;

private slots:
    void backBtnClicked() ;
    void store() ;
    void capture() ;     //拍照保存
    void reCapture() ;    //重拍
    void faceInputClicked() ;        //显示拍照页面
    void updateUI(cv::Mat mat) ;
    void stopPreview();
    void startPreview();
    void getStrFromKey(QString word) ;

signals:
    void backFaceInputPage(int num) ;
private:
    Ui::InfoEdit *ui;
    //head
    QFrame* mTitleFrame ;
    QLabel* mTitle ;
    QPushButton* mBackBtn ;
    QPushButton* mSaveBtn ;

    QFrame* mInfoFrame ;
    QLabel* mNameLab ;
    QLabel* mUserIDLab ;
    QLineEdit* mNameEdit ;
    QLineEdit* mUserIDEdit ;
    //QPushButton* mFaceBtn ;     //录入人脸按钮
    customerqlabel* mFaceBtn;

    QFrame* mFaceInputFrame ;
    QLabel* mVideoLab ;
    QLabel* mPicLab ;
    QPushButton* mCapBtn ;
    QPushButton* mReCapBtn ;
    bool mIsCap ;
    VIFrame mBtnPhoto ;
    unsigned char *RGB_BUF;
    bool mIsStopPreview = false;

    KeyBoard *mKeyBoard ;
    QString str1 ;
    QFrame* mKeyFrame ;
    void initUI() ;
    void YVU420P_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height);

public:
    int mCurrentUserId = -1;
};

#endif // INFOEDIT_H
