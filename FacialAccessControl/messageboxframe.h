#ifndef MESSAGEBOXFRAME_H
#define MESSAGEBOXFRAME_H

#include <QFrame>
#include <QMouseEvent>
#include "keyboard.h"
namespace Ui {
class MessageBoxFrame;
}

class MessageBoxFrame : public QFrame
{
    Q_OBJECT

public:
    explicit MessageBoxFrame(QWidget *parent = nullptr);
    ~MessageBoxFrame();
    KeyBoard* mKeyBoard ;
    void mousePressEvent(QMouseEvent *event)  ;
    bool eventFilter(QObject *obj, QEvent *e) ;

    void setInfo(QString info, bool isHideRoot, bool isHideInfo) ;
public slots:
    void getStrFromKey(QString str) ;
    void okOperate() ;
    void cancelOperate() ;
signals:
    void sendOperate(QString) ;
	void hide_window(void) ;
private:
    Ui::MessageBoxFrame *ui;

};

#endif // MESSAGEBOXFRAME_H
