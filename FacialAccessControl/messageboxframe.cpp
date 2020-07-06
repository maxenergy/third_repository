#include "messageboxframe.h"
#include "ui_messageboxframe.h"
#include <QDebug>

MessageBoxFrame::MessageBoxFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MessageBoxFrame)
{
    ui->setupUi(this);
    mKeyBoard = new KeyBoard(this) ;
    connect(mKeyBoard, SIGNAL(sendStr(QString)), this, SLOT(getStrFromKey(QString))) ;
    mKeyBoard->hide() ;
    ui->rootLab->move(120, 140) ;
    ui->rootEdit->move(120, 200) ;
    ui->frame->move(80,290) ;
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(okOperate())) ;
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(cancelOperate())) ;
    ui->rootEdit->installEventFilter(this);

}

MessageBoxFrame::~MessageBoxFrame()
{
    delete ui;
}

void MessageBoxFrame::getStrFromKey(QString word)
{
    qDebug()<<word ;

    QString txt = ui->rootEdit->text() ;
        if(word == "BackSpace")
        {
            txt.chop(1) ;
        }
        else {
            txt = txt+word ;
        }
        ui->rootEdit->setText(txt) ;
}
void MessageBoxFrame::okOperate()
{
    //this->hide() ;
    if(!ui->rootEdit->isVisible()){
        this->hide() ;
    }
    else {
        QString password = ui->rootEdit->text() ;
        ui->rootEdit->setText("") ;
        mKeyBoard->hide();
        emit sendOperate(password) ;
    }

}

void MessageBoxFrame::cancelOperate()
{
	emit hide_window();
    this->hide();
}
void MessageBoxFrame::setInfo(QString info, bool isHideRoot, bool isHideInfo)
{
    if(isHideInfo)  //隐藏msg的lab
    {
        ui->msgLab->hide() ;
    }else {
        ui->msgLab->setText(info) ;
        ui->msgLab->show() ;
    }
    if(isHideRoot)
    {
        ui->rootLab->hide() ;
        ui->rootEdit->hide() ;
    }else {
        ui->rootLab->show() ;
        ui->rootEdit->show() ;
        //ui->rootEdit->text() ;
        ui->rootEdit->setText("") ;
    }
}

//鼠标点击事件，单击使键盘消失
void MessageBoxFrame::mousePressEvent(QMouseEvent *event)
{
    //左击事件
   if(event->button() == Qt::LeftButton)
   {
       //mKeyBoard->hide();       ////////////////////////
       //qDebug()<<"DDDD" ;
   }

}

bool MessageBoxFrame::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == ui->rootEdit)
    {
        if(e->type() == QEvent::MouseButtonPress)
        {

            QMouseEvent *event = static_cast<QMouseEvent*>(e) ;

              if(event->button() == Qt::LeftButton)
            {
                mKeyBoard->move(0, this->height()-mKeyBoard->height()) ;
                mKeyBoard->show() ;
            }

        }
        else
            return false ;
    }
    else
    {
        return MessageBoxFrame::eventFilter(obj , e) ;
    }
}


