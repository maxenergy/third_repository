#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QFile>
#include <QPainter>

#include "homepage.h"
#include "devicesettings.h"
#include "faceinput.h"
#include "infoedit.h"
///
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void frameAviable(cv::Mat mat);
    void frameAviable_box(void);
public slots :
    void getPage(int pageNum) ;
	void update_box(void);

private:
    Ui::MainWindow *ui;
    QPixmap pixmap  ;

    HomePage* mHomePage ;
    DeviceSettings* mDeviceSettings ;
    FaceInput* mFaceInput ;
    InfoEdit* mInfoEdit ;
	FaceDetect::Msg bob_current;
	void save_box(FaceDetect::Msg bob);

	
    void paintEvent(QPaintEvent*) ;
};

#endif // MAINWINDOW_H
