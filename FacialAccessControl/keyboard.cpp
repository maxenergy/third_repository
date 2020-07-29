#include "keyboard.h"
#include "ui_keyboard.h"

KeyBoard::KeyBoard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KeyBoard)
{
    ui->setupUi(this);

    //按钮组重新标号(显示汉字的按钮组)
    ui->buttonGroup_3->setId(ui->pushButton_60 , 0);
    ui->buttonGroup_3->setId(ui->pushButton_63 , 1);
    ui->buttonGroup_3->setId(ui->pushButton_68 , 2);
    ui->buttonGroup_3->setId(ui->pushButton_58 , 3);
    ui->buttonGroup_3->setId(ui->pushButton_67 , 4);
    ui->buttonGroup_3->setId(ui->pushButton_56 , 5);
    ui->buttonGroup_3->setId(ui->pushButton_57 , 6);
    ui->buttonGroup_3->setId(ui->pushButton_61 , 7);
    ui->buttonGroup_3->setId(ui->pushButton_66 , 8);
    ui->buttonGroup_3->setId(ui->pushButton_65 , 9);


    ui->pushButton_59->setEnabled(false);
    ui->pushButton_62->setEnabled(false);

    Cap = true ;
    CH_EN = true ;
    str = "" ;
    curPageNum = 0 ;
    connect(ui->buttonGroup_2 , SIGNAL(buttonClicked(QAbstractButton*)) , this , SLOT(enter(QAbstractButton*))) ;
    connect(ui->buttonGroup , SIGNAL(buttonClicked(QAbstractButton*)) , this , SLOT(GetText(QAbstractButton*))) ;
    connect(ui->buttonGroup_3 , SIGNAL(buttonClicked(QAbstractButton*)) , this , SLOT(buttonGroup3(QAbstractButton*))) ;

    QFile file(":/ZK/utf8.txt") ;
    file.open(QIODevice::ReadOnly) ;
    if(file.isOpen())
    {
        QString str1(file.readAll()) ;
        file.close();
        HZK = str1.split("\r\n");
        //qDebug()<<HZK ;
    }
}

KeyBoard::~KeyBoard()
{
    delete ui;
}

//第二个按钮组（shif , 中英文。。。。。）
void KeyBoard::enter(QAbstractButton* btn)
{
    if(btn->text() == "Cap")
    {
        if(Cap == true)
        {
            btn->setStyleSheet("QPushButton{background-color:red}");
            Cap = false ;
        }
        else if(Cap == false)
        {
            btn->setStyleSheet("QPushButton{background-color: rgb(0, 85, 127)}");
            Cap = true ;
        }
    }
    else if(btn == ui->pushButton_38)
    {
        if(CH_EN == false)        //英文
         {
             btn->setText("EN");
             CH_EN = true ;

         }
         else if(CH_EN == true)
         {
              btn->setText("CH");
              CH_EN = false ;
         }
    }
    else if(btn->text() == "BackSpace")
    {
        if(ui->pushButton_51->text().isEmpty())      //显示拼音的button为空的话
        {
            //就清lineEdit里的内容  需要emit
            emit sendStr("BackSpace") ;
            //ui->lineEdit->backspace();

        }
        else                                          //不为空
        {
            QString str1 = ui->pushButton_51->text() ;
            str1.chop(1);                                   //一位一位地清除button里的内容
            ui->pushButton_51->setText(str1);

            //查找
            for(int i = 0 ; i < HZK.size() ; i++)
            {
                if(HZK.at(i).split(",").at(0) == ui->pushButton_51->text())     //比对拼音
                {
                    HZstr = HZK.at(i).split(",").at(1) ;   //获得汉字字符串
                    //qDebug()<<HZstr ;
                    for(int j = 0 ; j < 10 ; j++)             //先将上一页的清空
                    {
                        ui->buttonGroup_3->button(j)->setText("");
                    }
                    display() ;
                }
            }

        }

    }
    else if(btn->text() == "->")
    {
        curPageNum++ ;
        ui->pushButton_62->setEnabled(true);
        //qDebug()<<curPageNum ;
        if(HZstr.size() - curPageNum *10 <= 10)       //最后一页
        {
            //qDebug()<<HZstr.size() - curPageNum *10 ;
            for(int i = 0 ; i < 10 ; i++)             //先将上一页的清空
            {
                ui->buttonGroup_3->button(i)->setText("");
            }
            for(int i = 0 ; i < HZstr.size() - curPageNum *10 ; i++)         //显示最后 一页的内容
            {

                ui->buttonGroup_3->button(i)->setText(HZstr.at(curPageNum *10 + i));
            }
            ui->pushButton_59->setEnabled(false);               //失能->键

        }
        else
        {
            display() ;
        }
    }
    else if(btn->text() == "<-")
    {
        curPageNum-- ;
        ui->pushButton_59->setEnabled(true);
        //qDebug()<<curPageNum ;
        if(curPageNum == 0)            //翻到第0页
        {
            for(int i = 0 ; i < 10 ; i++)     //显示第0页的内容
            {
                ui->buttonGroup_3->button(i)->setText(HZstr.at(i));
            }
            ui->pushButton_62->setEnabled(false);      //最后失能<-
        }
        else
        {
            display() ;
        }
    }
    else if(btn->text() == "}\n]")
    {
        emit sendStr("]") ;
    }
    else if(btn->text() == "{\n[")
    {
        emit sendStr("[") ;
    }
    else if(btn->text() == ":\n;")
    {
	emit sendStr(";");
    }
    else if(btn->text() == "\"\n\'")
    {
	emit sendStr("\'");
    }
    else if(btn->text() == "<\n,")
    {
        emit sendStr(",");
    }
    else if(btn->text() == ">\n.")
    {
	emit sendStr(".");
    }
    else if(btn->text() == "?\n/")
    {
	emit sendStr("/");
    }

}

//第3个按钮组（选择汉字）
void KeyBoard::buttonGroup3(QAbstractButton* btn)
{
    if(btn == ui->pushButton_55)     //空格
    {
        this->hide();
        //emit sendStr(" ") ;
        //ui->lineEdit->setText(ui->lineEdit->text() + " ");
    }
    else
    {
        emit sendStr(btn->text()) ;
        //ui->lineEdit->setText(ui->lineEdit->text() + btn->text());
    }
    for(int i = 0 ; i < 10 ; i++)
    {
        ui->buttonGroup_3->button(i)->setText("");
    }
    ui->pushButton_51->setText("");
    curPageNum = 0 ;
}
//获得键盘输入 的字符串
void KeyBoard::GetText(QAbstractButton* btn)
{
    if(Cap == false)                     //大写
    {
        str = btn->text() ;
        emit sendStr(str) ;
        //ui->lineEdit->setText(ui->lineEdit->text() + str);

    }
    else if(Cap == true)                //小写
    {
        str = btn->text() ;
        str= btn->text().toLower() ;       //变小写
        if(CH_EN == false)       //中文
        {
            //查找
            Pystr = ui->pushButton_51->text() + str;
            ui->pushButton_51->setText(ui->pushButton_51->text() + str);
            for(int i = 0 ; i < 10 ; i++)
            {
                ui->buttonGroup_3->button(i)->setText("");
            }
            for(int i = 0 ; i < HZK.size() ; i++)
            {
                if(HZK.at(i).split(",").at(0) == ui->pushButton_51->text())     //比对拼音
                {
                    HZstr = HZK.at(i).split(",").at(1) ;   //获得汉字字符串
                    curPageNum = 0 ;
                    display() ;
                }
            }
        }
        else if(CH_EN == true)   //英文
        {
            emit sendStr(str) ;
            //ui->lineEdit->setText(ui->lineEdit->text() + str);
        }
    }

}


//显示中文
void KeyBoard::display()
{
    //qDebug()<<curPageNum ;
    if(HZstr.size() <= 10)                  //拼音对应的汉字数<=10个
    {
        for(int i = 0 ; i < HZstr.size() ; i++)
        {
            ui->buttonGroup_3->button(i)->setText(HZstr.at(i));
        }
        ui->pushButton_59->setEnabled(false);
        ui->pushButton_62->setEnabled(false);
    }
    else if(HZstr.size() > 10)               //拼音对应的汉字数>10个
    {
        if(curPageNum > 0)
        {
            ui->pushButton_62->setEnabled(true);
        }
        else
        {
           ui->pushButton_62->setEnabled(false);
        }
        ui->pushButton_59->setEnabled(true);
        for(int j = 0 ; j < 10 ; j++)
        {
            ui->buttonGroup_3->button(j)->setText(HZstr.at(curPageNum *10+j));
        }
    }

}
