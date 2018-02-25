#include "softinputform.h"
#include "ui_softinputform.h"

softInputForm::softInputForm(QWidget *parent) :
    QWidget(parent),sCustomLabel("+/-"),
    ui(new Ui::softInputForm)
{
    ui->setupUi(this);
    ui->pbtnIconKey->setIcon(QIcon("./images/keyboard.png"));
    ui->pbtnIconKey->setIconSize(ui->pbtnIconKey->size());
    ui->lblName->setTextFormat(Qt::AutoText);
    softBoard=new softBoardDlg(ui->lblName);//(this);
    softBoard->setCustomLabel(sCustomLabel);
    softBoard->setWindowFlags(Qt::Popup);
    connect(softBoard,SIGNAL(keyNum(int)),this,SLOT(slot_setValue(int)));
}
void softInputForm::setCustomLabel(QString buffer)
{
    sCustomLabel=buffer;
    softBoard->setCustomLabel(sCustomLabel);
}
softInputForm::~softInputForm()
{
    delete ui;
}
void softInputForm::setLineEidtWidth(int width)
{
    QSize size=ui->lneValue->size();
    size.setWidth(width);
    ui->lneValue->resize(size);
}
void softInputForm::setLblName(QString name)
{
    ui->lblName->setText(name);
}
void softInputForm::setLineWidth(int maxWidth,int minWidth)
{
    ui->lneValue->setMaximumWidth(maxWidth);
    ui->lneValue->setMinimumWidth(minWidth);
}
void softInputForm::on_pbtnIconKey_clicked()
{

//处理软键盘超出屏幕位置的情况
    QPoint screenSize;
    QPoint cursorPos=QCursor::pos();
    QPoint movePoint;
    screenSize.setY(QApplication::desktop()->size().height());
    screenSize.setX(QApplication::desktop()->size().width());
    int x=cursorPos.x()+softBoard->width();
    int y=cursorPos.y()+softBoard->height();
    if(y>screenSize.y())
    {
        int i=cursorPos.y()-y+screenSize.y()-20;
        movePoint.setY(i);
    }
    else
        movePoint.setY(cursorPos.y());
    if(x>screenSize.x())
    {
        int i=cursorPos.x()-x+screenSize.x()-20;
        movePoint.setX(i);
    }
    else
        movePoint.setX(cursorPos.x());
    softBoard->move(movePoint);

    QString strBefore=ui->lneValue->text();
    if(!softBoard->exec())
    {
        ui->lneValue->setText(strBefore);
    }
}
//函数功能：
void softInputForm::setValue(QString strValue)
{
    ui->lneValue->setText(strValue);
}
QString softInputForm::getValue(void )
{
    return ui->lneValue->text();
}
//函数功能：实时更新输入框的值
void softInputForm::slot_setValue(int i)
{
    QString buffer=ui->lneValue->text();
//    qDebug()<<"buffer="<<buffer;

    if(i==10)
    {
        int nCount=ui->lneValue->text().count();
        //        qDebug()<<"text value="<<ui->lneValue->text().left(nCount-1);
        ui->lneValue->setText(ui->lneValue->text().left(nCount-1));
    }
    else if(i==-1)
    {
        if(softBoard->customLabel()==QString("+/-"))
        {
            if(buffer.count())
            {
                if(buffer.left(1)=="-")
                {
                    buffer= buffer.right(buffer.count()-1);
                }
                else
                {
                    buffer= QString("-").append(buffer);
                }
            }
        }
        else if(softBoard->customLabel()==QString("."))
        {
            if(buffer.count())
            {
                if(!buffer.contains("."))
                    buffer.append(".");
            }
            else
                buffer=QString("0.");
        }
        ui->lneValue->setText(buffer);
    }
    else
    {
        ui->lneValue->setText(ui->lneValue->text().append(QString::number(i)));
    }
}
