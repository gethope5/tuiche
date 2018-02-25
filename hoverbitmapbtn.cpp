#include "hoverbitmapbtn.h"
#include <QGraphicsDropShadowEffect>
//static HoverBitmapBtn *prePbtn=;
HoverBitmapBtn *HoverBitmapBtn::prePbtn=NULL;
HoverBitmapBtn *HoverBitmapBtn::curPbtn=NULL;
HoverBitmapBtn::HoverBitmapBtn(const QPixmap& normal, const QPixmap& highmap, QWidget *parent):
    QPushButton(parent), bHover(false), nmap(normal), hmap(highmap),clickflag(false),hoverFontColor(""),backColor("")/*,prePbtn(NULL),curPbtn(NULL)*/
{
    initialButton();
}
HoverBitmapBtn::HoverBitmapBtn(QWidget *parent) : QPushButton(parent),bHover(false),clickflag(false),hoverFontColor(""),backColor("")/*,prePbtn(NULL),curPbtn(NULL)*/
{
    initialButton();
}
void HoverBitmapBtn::initialButton(void)
{
    alignFlag = Qt::AlignCenter;
    bHold = false;
    beforeCSS=this->styleSheet();
    beforeCSS.append("border:0px;");
    getConfigInfo();
    afterCSS=QString("background-color:%1;border:0px;color:%2;").arg(backColor).arg(hoverFontColor);
    //    qDebug()<<"css after"<<afterCSS;
    //    afterCSS="background-color:rgb(216, 246, 255);border:0px;color:blue;";
    this->setIconSize(QSize(131,71));
    this->setStyleSheet(beforeCSS);

    //未控件添加阴影效果

    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(-5, 5);
    shadow_effect->setColor(Qt::gray);
    shadow_effect->setBlurRadius(8);
    this->setGraphicsEffect(shadow_effect);
    bHover=true;
}
void HoverBitmapBtn::initialPbtnUI(QString path,QString hoverPath)
{
    this->setText("");
#if 1
    this->setBitmap(QPixmap::fromImage(QImage(path)),QPixmap::fromImage(QImage(hoverPath)));
#else
    this->setIcon(QIcon(path));
    this->setIconSize(this->size());
#endif
    this->setStyleSheet("border:0px;");
}
void HoverBitmapBtn::setBitmap(const QPixmap& normal, const QPixmap& highmap)
{
    nmap = highmap;
    hmap = normal;
    this->setIconSize(this->size());
#if 1
    this->setIcon(nmap.scaled(this->iconSize()));
#else
//    this->setIcon(QIcon(this->iconSize()));
#endif
}
void HoverBitmapBtn::getConfigInfo( void )
{
    //系统通过设置文件对串口号、波特率、数据位等参数进行设置
    QSettings  settings("Config.ini", QSettings::IniFormat);
    backColor=settings.value("initialPar/hoverBackColor", "blue").toString();
    hoverFontColor=settings.value("initialPar/hoverFontColor", "red").toString();
}
void HoverBitmapBtn::noHovedDisplay( )
{
    this->setIcon(nmap.scaled(this->iconSize()));
    this->setStyleSheet(beforeCSS);
}
void HoverBitmapBtn::enterEvent(QEvent *)
{
    //    bHover = true;
    if(bHover)
    {
        this->setIcon(hmap.scaled(this->iconSize()));
        this->setStyleSheet(afterCSS);
        update();
        emit getHover();
    }
}
void HoverBitmapBtn::leaveEvent(QEvent *)
{
    if(bHover)
    {
        this->setIcon(nmap.scaled(this->iconSize()));
        //    this->setPalette(QPalette(QColor(Qt::blue)));
        this->setStyleSheet(beforeCSS);
        //    this->setPalette(afterPalette);
        update();
    }
}
void HoverBitmapBtn::SetClickFlag(HoverBitmapBtn *pBtn,int pBtnLength,int CurrentTrueClick)
{
    for(int i=0;i<pBtnLength;i++)
    {
        if(i==(CurrentTrueClick))
            (pBtn+i)->clickflag=true;
        else
            (pBtn+i)->clickflag=false;
    }
}
void HoverBitmapBtn::slot_changeStatic(void)
{

}
