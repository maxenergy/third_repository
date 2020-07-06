#ifndef MASKWIDGET_H
#define MASKWIDGET_H

#include <QWidget>

class MaskWidget : public QWidget
{
    Q_OBJECT
protected:
    MaskWidget();

    void showEvent(QShowEvent* ev) ;
    bool eventFilter(QObject* obj, QEvent* event) ;

    static MaskWidget* mMask ;
    QWidget* mMainWidget ;
public:
    static MaskWidget* getInstance() ;
    static pthread_mutex_t mMutex  ;

    void setMainWidget(QWidget* mainWidget) ;
    void setBGColor(const QColor &bgcolor) ;
    void setOpacity(double opacity) ;
};

#endif // MASKWIDGET_H
