#include "mainwindow.h"
#include "facerecognition_public.h"
#include <QApplication>
#include <QDebug>
#include <QWSServer>

#include<string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <dlfcn.h>

//#include "include/shared_quene.h"

using namespace std;
extern "C" {
extern int sysinit(int *fd_out,int mroute,int mflip);
#ifndef BUILD_FACTORY_TEST_APP
extern void log_level_set(int level);
extern void init_pola_sdk(int liveless_mode,float detect_threshold,int test_flag);
#endif
}

int camroute = 90;
int camflip = 1;

int main(int argc, char *argv[])
{
    int fd =0;
    std::cout << "pola sdk init!!! " << std::endl;
    sysinit(&fd,camroute,camflip);
#ifndef BUILD_FACTORY_TEST_APP
		log_level_set(1);
		init_pola_sdk(0,0.4,0);
#endif
    FaceRecognitionApi &app = FaceRecognitionApi::getInstance();
    if (!app.init()) {
        qDebug() << "app init failed";
        return 1;
    };
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
