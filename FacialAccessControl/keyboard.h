#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QAbstractButton>
#include <QFile>

namespace Ui {
class KeyBoard;
}

class KeyBoard : public QWidget
{
    Q_OBJECT

public:
    explicit KeyBoard(QWidget *parent = nullptr);
    ~KeyBoard();
    void display() ;
public slots:
    void enter(QAbstractButton*) ;
    void GetText(QAbstractButton*) ;
    void buttonGroup3(QAbstractButton*) ;
signals:
    void sendStr(QString str) ;     //回删信号

private:
    Ui::KeyBoard *ui;
    bool Cap ;
    bool CH_EN ;
    QString str ;
    QStringList HZK ;
    QString HZstr ;
    int curPageNum ;
    QString Pystr ;
};

#endif // KEYBOARD_H
