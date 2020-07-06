#ifndef CUSTOMERQLABEL_H
#define CUSTOMERQLABEL_H
#include <QLabel>
namespace Ui {
class customerqlabel;
}

class customerqlabel : public QLabel
{
    Q_OBJECT

public:
    explicit customerqlabel(QWidget *parent=0, Qt::WindowFlags f=0);
    explicit customerqlabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);
protected:
    /*!
      ��Qt�Լ����õ�����¼�������,�ڴ˷���clicked�ź�
    */
    virtual void mouseReleaseEvent(QMouseEvent * ev);
signals:
    void clicked(void);
};
#endif // CUSTOMERQLABEL_H

