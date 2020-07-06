#include "maskwidget.h"
#include <QApplication>
MaskWidget* MaskWidget::mMask = nullptr ;
pthread_mutex_t MaskWidget::mMutex ;
MaskWidget::MaskWidget()
{
    mMainWidget = nullptr ;
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint) ;
    setOpacity(0.3) ;
    setBGColor(QColor(0,0,0)) ;
    qApp->installEventFilter(this) ;
}

MaskWidget* MaskWidget::getInstance()
{
    if(mMask == nullptr)
    {
        pthread_mutex_lock(&mMutex) ;
        if(mMask == nullptr)
        {
            mMask = new MaskWidget ;
        }
        pthread_mutex_unlock(&mMutex) ;
    }
    return mMask ;
}

void MaskWidget::setMainWidget(QWidget *mainWidget)
{
    if(this->mMainWidget != mainWidget)
    {
        this->mMainWidget = mainWidget ;
    }
}

void MaskWidget::setBGColor(const QColor &bgcolor)
{
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Background, bgcolor);
    this->setPalette(palette);
}

void MaskWidget::setOpacity(double opacity)
{
    this->setWindowOpacity(opacity) ;
}

void MaskWidget::showEvent(QShowEvent *)
{
    if(mMainWidget != nullptr)
        this->setGeometry(mMainWidget->geometry()) ;
}

bool MaskWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::Show){
        if(obj->objectName() == "MessageBox")
        {
            this->show() ;

        }
    }else if(event->type() == QEvent::Hide) {
        if(obj->objectName() == "MessageBox")
        {
            this->hide() ;
        }
    }
    return QObject::eventFilter(obj, event);
}
