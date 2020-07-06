#ifndef FACEINPUT_H
#define FACEINPUT_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <opencv2/opencv.hpp>
namespace Ui {
class FaceInput;
}

class FaceInput : public QFrame
{
    Q_OBJECT

public:
    explicit FaceInput(QWidget *parent = nullptr);
    ~FaceInput();

private slots:
    void backBtnClicked() ;
    void capBtnClicked() ;
    void inputBtnClicked() ;
    void infoBtnClicked() ;
    void updateUI(cv::Mat mat) ;

signals:
    void switchUI(int num) ;
    void backHomePage(int pageNum) ;
private:
    Ui::FaceInput *ui;
    //head
    QFrame* mTitleFrame ;
    QLabel* mTitle ;
    QPushButton* mBackBtn ;
    //video
    QLabel* mPicLab ;
    //bottom
    QFrame* mBottomFrame ;
    QPushButton* mCapBtn ;
    QPushButton* mInfoBtn ;
    QPushButton* mInputBtn ;

    bool mIsCap ;
    QImage img ;
    cv::Mat mPhoto;

    void initUI() ;

public:
    int mCurrentUserId = -1;
};

#endif // FACEINPUT_H
