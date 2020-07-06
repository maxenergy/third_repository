#include "messagebox.h"
#include "ui_messagebox.h"
#include <QTextCodec>

MessageBox* MessageBox::mMessageBox = nullptr ;
MessageBox::MessageBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBox)
{
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));

    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal) ;
    this->setModal(true) ;
    mMainWidget = nullptr ;
    this->resize(640, 420) ;
    ui->frame->resize(this->width(), this->height()) ;
    this->setWindowFlags(Qt::FramelessWindowHint) ;
    setAttribute(Qt::WA_TranslucentBackground);
    ui->msgLab->setWordWrap(true) ;

    ui->rootLab->move(120, 140) ;
    ui->rootEdit->move(120, 200) ;
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(okOperate())) ;
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(cancelOperate())) ;
	ui->pushButton->setText(QString::fromUtf8("确定"));
	ui->pushButton_2->setText(QString::fromUtf8("取消"));
	ui->rootLab->setText(QString::fromUtf8("请输入管理员密码"));
	ui->msgLab->setText(QString::fromUtf8(""));

}

MessageBox::~MessageBox()
{
    delete ui;
}

void MessageBox::okOperate()
{
    this->accept() ;
    this->hide() ;
}
void MessageBox::cancelOperate()
{
    this->reject() ;
    this->hide() ;
}

void MessageBox::setMainWidget(QWidget *mainWidget)
{
    if(mMainWidget != mainWidget)
    {
        mMainWidget = mainWidget ;
    }
}

MessageBox* MessageBox::getInstance()
{
    if(mMessageBox==nullptr)
    {
        mMessageBox = new MessageBox ;
    }
    return mMessageBox ;
}

void MessageBox::setInfo(QString info, bool isHideRoot, bool isHideInfo)
{
    this->move(mMainWidget->x()+(mMainWidget->width()-640)/2, mMainWidget->y()+(mMainWidget->height()-420)/2) ;
    if(isHideInfo)  //闅愯棌msg鐨刲ab
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
        ui->rootEdit->setText("") ;
    }
}

QString MessageBox::getRootPassword()
{
    QString rootPassword = ui->rootEdit->text() ;
    if(rootPassword != "")
    {
        return rootPassword ;
    }
}
