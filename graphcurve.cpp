#include <QtGui>
#include <cmath>
#include "graphcurve.h"

#define PI 3.1415926
bool graphcurve::fristDraw=false;
graphcurve::graphcurve(QWidget *parent):QPushButton(parent)
{
    drawData.clear();
    dataCount=drawData.count();
    readParamter();
    penInitial();
    refreshPixmap();
    constZoom=1;
}
void graphcurve::penInitial(void)
{
    //绘制点划线
    gridPenAxis.setStyle(Qt::SolidLine);
    gridPenAxis.setWidth(1);
    //设置背景为黑色
    gridPenAxis.setColor(uiColor.axisColor);
    //绘制点划线
    if(dotFlag)
        gridPenCurves.setStyle(Qt::DotLine);
    else
        gridPenCurves.setStyle(Qt::SolidLine);
    gridPenCurves.setWidth(1);
    //    gridPen.setColor(Qt::white);
    //设置背景为黑色
    gridPenCurves.setColor(uiColor.scanWireColor);
    gridPenCurves.setWidth(2);
}
void graphcurve::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
}
//函数功能：改变绘图中显示的颜色
void graphcurve::slot_setDisplayColor(int type,QColor curColor)
{
    qDebug()<<"color changed";
    if(type==1)
        uiColor.scanWireColor=curColor;
    else if(type==2)
        uiColor.axisColor=curColor;
    else if(type==3)
        uiColor.backGroundColor=curColor;
    else if(type==4)//恢复默认
    {
        uiColor.scanWireColor=Qt::red;
        uiColor.axisColor=Qt::white;
        uiColor.backGroundColor=Qt::black;
    }
    penInitial();
    refreshPixmap();
}
//函数功能：用于绘制坐标轴
void graphcurve::drawAxies(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(gridPenAxis);
    painter->drawLine(0,displayRect.height,displayRect.width,displayRect.height);

    //    painter->drawLine(0,HEIGH,WIDTH,HEIGH);
    //    painter->drawLine(0,400,400,400);
    painter->drawLine(this->width()/2,0,this->width()/2,this->height());
    painter->drawText(0,10,QString::number(this->width())+" hei"+QString::number(this->height()));
//    painter->drawArc(200,200,100,100,0,360*16);
}
void  graphcurve::drawCurves(QPainter *painter)
{
    //    qDebug()<<"draw hie"<<this->minimumHeight()<<this->maximumHeight()<<this->minimumWidth();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(gridPenCurves);
    //    qDebug()<<"size "<<this->width()<<this->height();
    //painter->    QRect rect(QPoint(-200, 200),QPoint( 200,-10));

    if(!dataCount)
        return;
    float DrawHeight=this->height();
    float DrawWidth=this->width();
    float  prevRx=-50;
    float  prevRy=-50;


    for(int i=0;i<dataCount;i++)
    {
        //        float ry=(400-(1/constZoom)*drawData.at(i).y());
        float ry=(displayRect.height-(constZoom*0.1)*drawData.at(i).y());
        float rx=(constZoom*drawData.at(i).x()+displayRect.width/2);
        //        QString labletemp=QString::number(drawData.at(i).x());
        //        qDebug()<<i<<"ry="<<ry<<drawData.at(i).y()<<drawDatabuffer->at(i).toInt();
        if(prevRx>-50)
        {
            if(dotFlag)
                painter->drawPoint(rx,ry);
            else
            {
                painter->drawLine(prevRx,prevRy,rx,ry);
//                qDebug()<<"point = "<<drawData.count()<<drawData.at(0).x();
            }
        }
        prevRx=rx;
        prevRy=ry;
    }
}
void graphcurve::enqueue(QStringList *drawDatabuffer,float AngleResolution,float starAngle)
{
    //    qDebug()<<"enqueue"<<drawDatabuffer->count();
    dataCount=drawDatabuffer->count();
    float buffer1=startAngle.toFloat();
    float buffer2=angleResolution.toFloat();
    float offsetAngle2=95.0;
    drawData.clear();
    for(int i=0;i<dataCount;i++)
    {
        float angle=i* AngleResolution / 180 *3.1415;
        qreal rx=drawDatabuffer->at(i).toInt()* sin(angle - offsetAngle2 / 180 *3.1415);
        qreal ry=drawDatabuffer->at(i).toInt()* cos(angle - offsetAngle2 / 180 *3.1415);
        drawData<<QPointF(rx,ry);
    }
    refreshPixmap();
}
void graphcurve::refreshPixmap()
{
    pixmap = QPixmap(size());
    pixmap.fill(uiColor.backGroundColor);
    QPainter painter(&pixmap);
    painter.initFrom(this);
    displayRect.width=this->width();
    displayRect.height=this->height()-10;
    if(fristDraw)
    {
        drawAxies(&painter);
        drawCurves(&painter);
        update();
    }
    else
    {
        fristDraw=true;
        painter.drawText(200,200,50,50,Qt::AlignLeft| Qt::AlignTop,"zbw");
        update();
    }
}
void graphcurve::resizeEvent(QResizeEvent * /* event */)
{
    refreshPixmap();
}
void graphcurve::readParamter(void)
{
    QSettings  settings("SystemConfigInfo.ini", QSettings::IniFormat);
    settings.beginGroup("ScanDiaplayColor");
    uiColor.scanWireColor=Qt::GlobalColor(setting::strColorToInt(settings.value("scanWireColor","red").toString()));
    uiColor.axisColor=(Qt::GlobalColor)(setting::strColorToInt(settings.value("axisColor","white").toString()));
    uiColor.backGroundColor=(Qt::GlobalColor)(setting::strColorToInt(settings.value("backGroundColor","black").toString()));
}

