#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialog>

namespace Ui {
class MessageBox;
}

class MessageBox : public QDialog
{
    Q_OBJECT

private:
    explicit MessageBox(QWidget *parent = nullptr);
    ~MessageBox();
    static MessageBox* mMessageBox ;
private slots:
    void okOperate() ;
    void cancelOperate() ;
public:
    static MessageBox* getInstance() ;
    void setInfo(QString info, bool isHideroot, bool isHideInfo) ;
    void setMainWidget(QWidget* mainWidget) ;
    QString getRootPassword() ;
private:
    Ui::MessageBox *ui;
    QWidget* mMainWidget ;
};

#endif // MESSAGEBOX_H
