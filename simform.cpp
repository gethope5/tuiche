#include "simform.h"
#include "ui_simform.h"

simForm::simForm(QWidget *parent) :
    QWidget(parent),
    packageIndex(0),
    widthCurveIndex(100),
    bIndexIncrease(false),

    m_wireStartAngle(75),
    m_wireEndAngle(100),
    indexDirection(true),
    ui(new Ui::simForm),
    m_virtualWalk(0),
    wireRangeMarkIndex(0),
    curDrawData("")
{
    ui->setupUi(this);
    m_drawSide=ui->widget_2;
    connect(m_drawSide->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(AxisChanging1(QCPRange)));
    m_drawSide->setInteractions(QCP::iRangeDrag| QCP::iRangeZoom| QCP::iRangeZoom);
    m_drawSide->axisRect()->setupFullAxesBox(true);
    m_drawSide->addGraph();

    m_drawSingle=ui->widget;
    connect(m_drawSingle->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(AxisChanging1(QCPRange)));
    m_drawSingle->setInteractions(QCP::iRangeDrag| QCP::iRangeZoom| QCP::iRangeZoom);
    m_drawSingle->axisRect()->setupFullAxesBox(true);
    //connect(m_drawSingle,SIGNAL(mouseDoubleClick (QMouseEvent *)), this, SLOT(defaultSize1(QMouseEvent*)));

    m_drawSingle->xAxis->setLabel(QString::fromWCharArray(L"Y"));
    m_drawSingle->yAxis->setLabel(QString::fromWCharArray(L"X"));
    //connect(m_drawSingle->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(AxisChanging1(QCPRange)));

    m_drawSingle->addGraph();
    m_drawSingle->addGraph();
    m_drawSingle->addGraph();
    setAxisRange();
    timer=new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(slot_timerOnePackage()));

    drawSideDataRange(Side_Start_Angle,Side_End_Angle);
    drawWireDataRange();
    drawRailCurve(0,0);
    //    zeroCounts.swap();//
    setWindowFlags(Qt::Window);//->setWindowRole();
    //qDebug()<<"verify data counts="<<ui->lneIndexs->text().split(",").count();
    wireMarkIndex.resize(4);
    for(int i=0;i<wireMarkIndex.count();i++)
    {
        QPair<int ,int > dd;
        dd.first=m_drawSingle->graphCount();
        m_drawSingle->addGraph();
        dd.second=m_drawSingle->graphCount();
        m_drawSingle->addGraph();
        wireMarkIndex.replace(i,dd);
        //qDebug()<<"wire mark graph index"<<i<<wireMarkIndex.at(i).first<<wireMarkIndex.at(i).second;
    }
    readConfigure();
    ui->lneWireLower->setText("75");
    ui->lneWireUp->setText("100");
    ui->lneAngle->setText("0");
    ui->lneSideLower->setText(QString::number(Side_Start_Angle));
    ui->lneSideUp->setText(QString::number(Side_End_Angle));
    connect(ui->chbAdd,SIGNAL(pressed()),this,SLOT(slot_railAngle()));
    connect(ui->chbRailAngle,SIGNAL(pressed()),this,SLOT(slot_railAngle()));
    ui->chbRailAngle->setChecked(true);
}
void simForm::slot_getVerify(wireStandardValues d)
{

}
simForm::~simForm()
{
    delete ui;
}

void  simForm::AxisChanging1(QCPRange)
{

    m_drawSingle->replot();
    m_drawSide->replot();
    //    setAxisRange();

}
void simForm::defaultSize1(QMouseEvent*)
{
    setAxisRange();
}
void simForm::setAxisRange(void)
{
    //    QSettings  settings("Config.ini", QSettings::IniFormat);
    int WireWidthMax=5000;//settings.value("axisPar/WireWidthMax", 600).toInt();
    int WireWidthMin=-5000;//settings.value("axisPar/WireWidthMin", -600).toInt();
    int WireHeightMax=10000;//settings.value("axisPar/WireHeightMax", 7500).toInt();
    int WireHeightMin=-200;//settings.value("axisPar/WireHeightMin", 5300).toInt();
    //    int WalkMax=;//settings.value("axisPar/WalkMax", -5).toInt();
    //    int WalkMin=;//settings.value("axisPar/WalkMin", -5).toInt();

    m_drawSingle->yAxis->setRange(WireHeightMin,WireHeightMax);
    m_drawSingle->xAxis->setRange(WireWidthMin,WireWidthMax);
    m_drawSide->yAxis->setRange(WireHeightMin,WireHeightMax);
    m_drawSide->xAxis->setRange(WireWidthMin,WireWidthMax);

    QPen pen(Qt::green, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
    m_drawSide->graph(0)->setPen(pen);

    //    bFirstAddPole=true;//坐标系统发生改变,重新设置添加杆号的偏移坐标
    //    qDebug()<<"walk"<<WalkMax<<WalkMin;
}
void simForm::slot_updateInitialPar(sensorInitialValues)
{
    MeasureDB::bVerify=false;
}
void simForm::slot_debug(QString str)
{
    ui->lneIndex->setText(str);
}
void simForm::slot_timerOnePackage(void)
{
#if 0
    if(bIndexIncrease)
    {
        QString cc=ui->lneIndexs->text();

        int index=0;
        if((verifyIndex)<cc.split(",").count())
        {
            index=cc.split(",").at(verifyIndex++).toInt();
        }
        else
        {
            qDebug()<<"simulation index is out";

            verifyIndex=0;
        }
        ui->lblIndex->setText(QString::number(index));
        ui->lineEdit->setText(ui->lblIndex->text());
        emit oneStrPackage(strPackages.at(index).split(0x20),m_virtualWalk);

    }
    else
#endif
    {
        if(packageIndex>=strPackages.count()||packageIndex<0)
        {
            packageIndex=strPackages.count()-1;
            qDebug()<<"index i> package count";
            return;
        }

        ui->lblIndex->setText(QString::number(packageIndex));
        ui->lineEdit->setText(ui->lblIndex->text());
        //    drawSinglePackage(strPackages.at(packageIndex));
        //        qDebug()<<"++++++++++++++++++  index="<<packageIndex;
        //        qDebug()<<"walk"<<m_virtualWalk;
        m_virtualWalk+=(rand()%10)*0.01;
        emit oneStrPackage(strPackages.at(packageIndex).split(0x20),m_virtualWalk);
        //        qDebug()<<packageIndex;
        if(bIndexIncrease)
        {
            if(indexDirection)
            {
                packageIndex++;
            }
            else
                packageIndex--;
        }
    }
}

void simForm::radorData(QString str)
{
    curDrawData=str;
    drawSinglePackage(str,0);
    drawRailCurve(0,0);
}
//函数功能：将一帧完整的数据绘制曲线,并将数据存放在onePackage中。
void simForm::drawSinglePackage(QString strMsg,float baseAngle)
{
    bool ok;
    QVector<double> WireXs,WireYs;
    updateIndex();
    QTime time;
    time.start();
    QStringList list = strMsg.split(0x20);

    if (list.size() < 26)
    {
        return;
    }
    onePackage.clear();
    QString strSize = list.at(25);
    int   dataSize=0;
    sscanf(strSize.toLatin1().constData(), "%X", &dataSize);

    QStringList originalData=list.mid(26,dataSize);
    int zeroCount=0;    //统计一帧数据中的零值
    int sideZeroCount=0;//统计侧面界限对应区域的零值
    int notZeroCount=0;
    for(int i=0;i<originalData.count();i++)
    {
        int nData=(originalData.at(i)).toInt(&ok,16);
        QPair<int,QPair<int,int> > dd;
        dd.first=i;

        if(nData)
        {
            double angle=(i*0.1667-5-baseAngle)*3.1415926/180;
            notZeroCount++;
            dd.second.first=nData*cos(angle);
            dd.second.second=nData*sin(angle);
            //            qDebug()<<angle<<"x"<<dd.second.first<<"y"<<dd.second.second;
            WireYs<<dd.second.second;
            WireXs<<dd.second.first;
        }
        else
        {
            zeroCount++;
            dd.second.first=0;
            dd.second.second=0;
            if((i>startIndex)&&(i<endIndex))
            {
                sideZeroCount++;
            }
        }
        onePackage<<dd;
    }
    //    if(flag)
    {
        //qDebug()<<"analyze time="<<time.elapsed()<<"0 count="<<zeroCount<<"not 0 count="<<notZeroCount;
        m_drawSingle->graph(0)->setData(WireXs,WireYs);
        m_drawSingle->graph(1)->setData(startX,startY);
        m_drawSingle->graph(2)->setData(endX,endY);
        m_drawSingle->replot();
    }
    //    else
    //    {
    //        //        if((sideZeroCount<10)&&(zeroCount>10))
    //        //        {
    //        //            timer->start(1000);
    //        //            qDebug()<<"package idnex"<<packageIndex<<sideZeroCount<<zeroCount;//<<strPackages.at(packageIndex-1);
    //        //            double a,b;//y=a+bx;
    //        //            analyze_sideConst(onePackage.mid(startIndex,endIndex-startIndex),a,b);
    //        //            //qDebug()<<"sim object,side value ax+b,"<<a<<b;
    //        //            if(b)
    //        //            {
    //        //                int n=ui->lneBase1->text().toInt();
    //        //                if(n)
    //        //                    ui->lneSideValue->setText(QString::number(-b/(a*n)));
    //        //                else
    //        //                    ui->lneSideValue->setText(QString::number(-b/(a)));
    //        //            }
    //        //        }
    //        //        else
    //        //        {
    //        //            m_drawSide->graph(0)->clearData();
    //        //            m_drawSide->replot();
    //        //            timer->start(10);
    //        //        }
    //    }

}
void simForm::slot_updateSideCuve(QVector<QPair<int,QPair<int,int> > > side)
{
    QVector<double> sidex;
    QVector<double> sidey;
    for(int i=0;i<side.count();i++)
    {
        sidex<<side.at(i).second.first;
        sidey<<side.at(i).second.second;
    }
    m_drawSide->graph(0)->setData(sidex,sidey);
    m_drawSide->replot();
    //    timer->start(SIM_TIMER_INTERVAL);
    //    qDebug()<<"++++++++++++++++++++++++++love";
}
void simForm::analyze_sideConst( QVector<QPair<int,QPair<int,int> > > side,double &a,double &b)
{
    double x1=0,x2=0,y1=0,y2=0;
    long sumx=0;
    long sumy=0;
    long sumxx=0;
    long sumxy=0;
    //    QString str=QDir::currentPath().append("/").append(QDateTime::currentDateTime().toString("hh-mm-ss")).append(".csv");
    //    QFile file(str);
    //    file.open(QIODevice::WriteOnly|QIODevice::Text);

    //    qDebug()<<"file paths="<<str;
    //1曲线拟合方式获取支柱对应的直线方程,f(x)=a+bx
    //nA+Bsumx=sumy;Asumx+Bsumxx=sumxy;y=A+Bx;
    QVector<double> sidex;
    QVector<double> sidey;
    for(int i=0;i<side.count();i++)
    {
        //        if(file.isOpen())
        //        {
        //            QTextStream dd(&file);
        //            QString cc=QString ("%1,%2,%3").arg(i).arg(side.at(i).second.first).arg(side.at(i).second.second);
        //            dd<<cc<<endl;
        //        }
        sumx+=side.at(i).second.first;
        sumy+=side.at(i).second.second;
        sumxx+=side.at(i).second.first*side.at(i).second.first;
        sumxy+=side.at(i).second.second*side.at(i).second.first;
        sidex<<side.at(i).second.first;
        sidey<<side.at(i).second.second;
    }
    //    qDebug()<<"x,y,xx,xy"<<sumx<<sumy<<sumxx<<sumxy;
    m_drawSide->graph(0)->setData(sidex,sidey);
    m_drawSide->replot();
    int n=endIndex-startIndex;
    int b1=(n*sumxy-sumy*sumx)/(n*sumxx-sumx*sumx);
    int a1=(sumy-b1*sumx)/n;
    int ex=sumx/n;
    unsigned long qx=0;
    //
    for(int i=0;i<side.count();i++)
    {
        qx+=pow(ex-side.at(i).second.first,2);
    }
    qDebug()<<"a1,b1"<<a1<<b1<<-a1/b1<<"qx"<<ex<</*sqrt*/(qx/n);//<<sumxx<<sumxy;
    //    ui->lneSideValue1->setText(QString::number(-a1/b1));
    //    ui->lbQx->setText(QString::number(qx/n));
    //2简单方式计算支柱对应的直线方程,f(x)=ax+b
    for(int i=0;i<side.count();i++)
    {
        for(int j=i+1;j<side.count();j++)
        {
            if(side.at(i).second.second>side.at(j).second.second)
            {
                QPair<int,QPair<int,int> > tmp;
                tmp=side.at(i);
                side.replace(i,side.at(j));
                side.replace(j,tmp);
            }
        }
    }

    for(int i=0;i<10;i++)
    {
        x1+=side.at(i).second.first;
        y1+=side.at(i).second.second;

        x2+=side.at(side.count()-i-1).second.first;
        y2+=side.at(side.count()-i-1).second.second;
        //            qDebug()<<"cc"<<i<<side.at(i).second.first<<side.at(i).second.second<<side.at(side.count()-i-1).second.first<<side.at(side.count()-i-1).second.second<<side.count()-i;
    }
    x1=x1/10.0;x2=x2/10.0;
    y2=y2/10.0;y1=y1/10.0;
    //    qDebug()<<QString("x1=%1,x2=%2,y1=%3,y2=%4").arg(x1).arg(x2).arg(y1).arg(y2);
    if(x2-x1)
    {
        a=double((y2-y1)/(x2-x1));
        b=double(y1-a*x1);
    }
    else
    {
        a=0;
        b=0;
    }
    qDebug()<<"a,b"<<a<<b<<-b/a;
    //    file.close();
}
void simForm::on_pbtnStart_clicked()
{
    packageIndex=ui->lineEdit->text().toInt();
    updateIndex();
    if(packageIndex>=strPackages.count())
    {
        qDebug()<<"index i> package count";
        return;
    }
    //    qDebug()<<"cur index"<<startIndex<<endIndex;
    //    drawSideDataRange();
    timer->start(50);
    emit controlTimer(true,50);
}
void  simForm::simStart(bool flag)
{
    bIndexIncrease=flag;
    on_pbtnStart_clicked();
}
void simForm::updateIndex(void )
{
    startIndex=(Side_Start_Angle+5)/0.1667;
    endIndex=(Side_End_Angle+5)/0.1667;
    //    qDebug()<<"sim object,side index info,start="<<startIndex<<"end="<<endIndex;
}
void simForm::drawWireDataRange(void)
{
    if(!wireRangeMarkIndex)
    {
        wireRangeMarkIndex=m_drawSingle->graphCount();
        m_drawSingle->addGraph();
        m_drawSingle->addGraph();
    }
    QVector<double> x1,x2,y1,y2;
    x1<<0;
    x1<<+6000;
    y1<<0;
    y1<<(x1.at(1)*tan((m_wireStartAngle)*3.1415926/180));

    x2<<0;
    x2<<-6000;
    y2<<0;
    y2<<abs(x2.at(1)*tan((m_wireEndAngle)*3.1415926/180));

    //    qDebug()<<m_wireStartAngle<<m_wireEndAngle<<x1<<y1<<x2<<y2<<"wire range mark";
    m_drawSingle->graph(wireRangeMarkIndex)->setData(x1,y1);
    m_drawSingle->graph(wireRangeMarkIndex)->setPen(QPen(Qt::darkCyan));
    m_drawSingle->graph(wireRangeMarkIndex+1)->setData(x2,y2);
    m_drawSingle->graph(wireRangeMarkIndex+1)->setPen(QPen(Qt::darkCyan));
    m_drawSingle->replot();
}
void simForm::drawSideDataRange(int startAngle,int endAngle)
{
    m_drawSingle->graph(1)->setPen(QPen(Qt::red));
    m_drawSingle->graph(2)->setPen(QPen(Qt::green));
    startX.clear();startY.clear();

    endX.clear();
    endY.clear();

    startX<<0;
    startX<<-6000;

    endX<<0;
    endX<<-6000;
    startY<<0;
    //    startY<<startX.at(1)*tan((180-ui->lneStartAngle->text().toInt())*3.1415926/180);
    startY<<startX.at(1)*tan(startAngle*3.1415926/180);

    endY<<0;
    endY<<endX.at(1)*tan(endAngle*3.1415926/180);
    //    qDebug()<<"start y"<<startY.at(1)<<endY.at(1);
    m_drawSingle->graph(1)->setData(startX,startY);
    m_drawSingle->graph(2)->setData(endX,endY);
    m_drawSingle->replot();
}
void simForm::on_pbtnStop_clicked()
{
    simStop(false);
}

void simForm::simStop(bool f)
{
    timer->stop();
    //    emit startMeausre();
    emit controlTimer(false);
    if(f)
    {
        m_virtualWalk=0;
    }
}
void simForm::on_pbtnAnalyze_clicked()
{
    if(timer->isActive())
        timer->stop();
    qDebug()<<"index"<<packageIndex;
    if(packageIndex>strPackages.count())
        return ;
    drawSinglePackage(strPackages.at(packageIndex++),0);
    updateIndex();
    bool ok;
    QTime time;
    time.start();
    for(int idx=0;idx<strPackages.count();idx++)
    {
        QPair<int ,int> oneZero;
        oneZero.first=idx;
        QString strMsg=strPackages.at(idx);

        QStringList list = strMsg.split(0x20);
        if (list.size() < 26)
        {
            continue;
        }
        int dataSize = QString(list.at(25)).toInt(&ok,16);

        QStringList originalData=list.mid(26,dataSize);
        int zeroCount=0;
        if(endIndex>originalData.count())
            continue;
        for(int i=startIndex;i<endIndex;i++)
        {
            int nData=(originalData.at(i)).toInt(&ok,16);
            if(!nData)
            {
                zeroCount++;
                oneZero.second++;
            }
        }
        zeroCounts<<oneZero;
    }
    //    qDebug()<<"hex value before"<<zeroCounts.count();
    qDebug()<<"hex value after";//<<zeroCounts;
    QString cc;
    for(int i=5;i<zeroCounts.count()-5;i++)
    {


        if((zeroCounts.at(i).second-zeroCounts.at(i+1).second)>50)
        {
            qDebug()<<"abs"<<(zeroCounts.at(i).second-zeroCounts.at(i+1).second);
            //            for(int j=0;j<10;j++)
            //            {
            //                qDebug()<<"i,idx="<<zeroCounts.at(j+i).first<<" "<<zeroCounts.at(j+i).second;
            //            }
            cc.append(QString::number(i)).append(",");
        }
    }
    ui->lneIndex->setText(cc);
}
void simForm::on_pbtnOpen_clicked()
{
#if 0
    QString filePaths=QDir::currentPath();
    filePaths.append("/data04-19/6 2017-04-28 11.38.57.txt");
#else
    QStringList txtFile;
    txtFile<<"D:/guiyangTuiche/originalData/jy23-47-01.txt";              //0 3354
    txtFile<<"D:/guiyangTuiche/originalData/jy23-51-48.txt";              //1
    txtFile<<"D:/guiyangTuiche/originalData/jy23-54-36.txt";              //2
    txtFile<<"D:/guiyangTuiche/originalData/jy00-06-40.txt";              //3
    txtFile<<"D:/guiyangTuiche/originalData/jy00-09-39.txt";              //4
    //无交叉线岔
    txtFile<<"D:/guiyangTuiche/originalData/jy14-44-16.txt";              //5 1117
    txtFile<<"D:/guiyangTuiche/originalData/jy14-52-29.txt";              //6 1774.error
    txtFile<<"D:/guiyangTuiche/originalData/jy15-03-33.txt";              //7 2713,无交叉线岔
    txtFile<<"D:/guiyangTuiche/originalData/jy15-12-00.txt";              //8

    txtFile<<"D:/guiyangTuiche/originalData/jy10-41-42.txt";              //9 9681，有两垮，经过第二跨后就不动了,4800附近有异常,拉出值跳变,可能的原因是设备动了.
    txtFile<<"D:/guiyangTuiche/originalData/jy10-53-10.txt";              //10 9030,运行过程中有bug,程序死掉(由于数据中没有)
    txtFile<<"D:/guiyangTuiche/originalData/jy11-00-02.txt";              //11 4826,ok
    txtFile<<"D:/guiyangTuiche/originalData/jy11-05-17.txt";              //12 30498,ok,包含锚段关节
    txtFile<<"D:/guiyangTuiche/originalData/jy11-41-11.txt";              //13 16121 ,7000左右线岔
    txtFile<<"D:/guiyangTuiche/originalData/jy11-53.01.txt";              //14
    txtFile<<"D:/guiyangTuiche/originalData/gy6 2017-04-28 11.38.57.txt"; //15 1500左右,锚段开始
    //    496,507,512,515,1449,2398,3313,3314,4209,5179,5184,5195,6137,7047,7964,7975,7980,7983,8917,9866,10781,10782,11677,12647,12652,12663,13605,14515,

    txtFile<<"D:/guiyangTuiche/originalData/0610-zy/zy14-02-41.txt";              //16 9000,3700
    txtFile<<"D:/guiyangTuiche/originalData/0610-zy/zy14-00-00.txt";              //17
    txtFile<<"D:/guiyangTuiche/originalData/0610-zy/zy15-00-00.txt";              //18 38891
    txtFile<<"D:/guiyangTuiche/originalData/0610-zy/zy0610-15-26-33.txt";         //19 35419
    txtFile<<"D:/guiyangTuiche/originalData/0610-zy/zy06-11-10-58-13.txt";         //20 1410

    txtFile<<"D:/guiyangTuiche/originalData/mgy6 2017-04-28 11.38.57.txt";//21 14936 包含两个锚段,1start:1200 ,1end:4600;2start:8600,2end:12100;2600,工作支切换之前的最近序号;4000左右2导线交叉,wire2中断
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-15-54-19.txt";       //22,error,delete
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-16-00-00.txt";       //23,error,delete
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-16-03-00.txt";       //24
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-16-14-43.txt";       //25
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-16-18-45.txt";       //26
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-16-25-53.txt";       //27    12506,
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-16-37-32.txt";       //28    24551，error，数据无效,delete
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-16-55-21.txt";       //29    5751,error,，delete
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-16-59-10.txt";       //30    895，error，delete
    txtFile<<"D:/guiyangTuiche/originalData/bj0726-17-00-00.txt";       //31   16329，好像数据是逆向推着设备再走。
    txtFile<<"D:/measure data/11-16 nanning/1115-10-32-39.txt";         //32    33335,
    txtFile<<"D:/measure data/11-16 nanning/1115-11-00-00.txt";         //33    43934,
    txtFile<<"D:/measure data/11-16 nanning/1115-10-00-18.txt";         //34    42166,5000左右有一个杆号，同时在杆号附近有一个遮挡物

    txtFile<<"D:/measure data/0924-yangyuan/1 0924-09-46-44.txt";       //35   16329，好像数据是逆向推着设备再走。
    txtFile<<"D:/measure data/0924-yangyuan/2 0924-09-59-06.txt";       //36    33335,
    txtFile<<"D:/measure data/0924-yangyuan/3 0924-10-00-00.txt";       //37    43934,
    txtFile<<"D:/measure data/0924-yangyuan/4 0924-10-27-48.txt";       //38    42166,5000左右有一个杆号，同时在杆号附近有一个遮挡物
    txtFile<<"D:/measure data/11-27 lianbingchang/2017-11-22-12-27-20.txt"; //39
    txtFile<<"D:/measure data/11-27 lianbingchang/2017-11-22-12-37-00.txt"; //40    1212,   680
    txtFile<<"D:/measure data/11-27 lianbingchang/2017-11-22-13-00-26.txt"; //41
    txtFile<<"D:/measure data/11-27 lianbingchang/2017-11-22-13-06-34.txt"; //42

    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-00-12-30.txt";     //43    2891,ok
    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-00-32-44.txt";     //44    781,
    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-00-43-43.txt";     //45    3325,
    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-01-21-21.txt";     //46    3325,
    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-01-39-24.txt";     //47    3325,
    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-01-43-24.txt";     //48    3325,
    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-01-51-31.txt";     //49    3325,
    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-01-56-57.txt";     //50    3325,
    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-02-08-36.txt";     //51    3325,
    txtFile<<"D:/measure data/11-25 sanliqiao/2017-11-25-02-09-55.txt";     //52    3325,


    //    QString filePaths=QString("D:/measure data/11-16 nanning/nanning1115-10-32-39.txt");
    QString filePaths=txtFile.at(21);
#endif
    //filePaths.append("/data04-19/05-07 12-27-30.txt");
    MeasureDB::bSimDraw=true;
    strPackages.clear();
    packageIndex=0;
    readTxtOriginalData(filePaths,strPackages);
    ui->lblFrames->setText(QString::number(strPackages.count()));
    qDebug()<<QString("open file paths=%1,record counts=%2").arg(filePaths).arg(strPackages.count());
    updateIndex();
    emit startMeausre();
}
void simForm::slot_startinitialed (void)
{
    if(strPackages.count())
        //        timer->start(SIM_TIMER_INTERVAL);
        ;
}
void simForm::readTxtOriginalData(QString filePaths,QStringList &data)
{
    QFile file(filePaths);

    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream data_in(&file);
        while(!data_in.atEnd())
            data<<data_in.readLine();
    }
    else
    {
        qDebug()<<"file is not read";
    }
//    qDebug()<<"original data read ,row count="<<data.count();
    file.close();
}
void simForm::on_pbtnVerify_clicked()
{
    MeasureDB::bMeasure=false;
    bIndexIncrease=true;
    verifyIndex=0;
    if(ui->lineEdit->text().isEmpty())
        packageIndex=0;
    else
        packageIndex=ui->lineEdit->text().toInt();
    if(!timer->isActive())
        timer->start(SIM_TIMER_INTERVAL);
    emit simulationVerify();
    emit controlTimer(true);
}

void simForm::on_pbtnSpeed_clicked()
{
    timer->start(SIM_TIMER_INTERVAL_FAST);
}
void simForm::changeBaseAngle(float angle)
{
    ui->lneWireLower->setText(QString::number(ui->lneWireLower->text().toInt()-angle));
    ui->lneWireUp->setText(QString::number(ui->lneWireUp->text().toInt()-angle));
    m_wireStartAngle=ui->lneWireLower->text().toInt();
    m_wireEndAngle=ui->lneWireUp->text().toInt();
    drawWireDataRange();
    emit updateWireRange(m_wireStartAngle,m_wireEndAngle);
}
void simForm::drawRailCurve(int x,int y)
{
    if(widthCurveIndex)
    {
        widthCurveIndex=m_drawSingle->graphCount()-1;
    }

    QVector<double> x1,y1;
    x1.clear();y1.clear();
    x1<<-m_nLachuzhiC;
    x1<<-m_nLachuzhiC;
    y1<<m_drawSingle->yAxis->range().upper;
    y1<<m_drawSingle->yAxis->range().lower;
    m_drawSingle->graph(widthCurveIndex)->setLineStyle(QCPGraph::lsLine);
    m_drawSingle->graph(widthCurveIndex)->setPen(QPen(Qt::red));
    m_drawSingle->graph(widthCurveIndex)->setData(x1,y1);
    m_drawSingle->replot();
}
void simForm::drawWireMark(int x,int y,int n)
{
    QVector<double> x1,y1,x2,y2;
    x1.clear();y1.clear();
    x2.clear();y2.clear();

    if(y)
    {
        x1<<x-100;
        x1<<x+100;
        y1<<y;
        y1<<y;
        x2<<x;
        x2<<x;
        y2<<y-100;
        y2<<y+100;
        QColor color;
        switch (n)
        {
        case 0:
        {
            color=Qt::red;
            break;
        }
        case 1:
        {
            color=Qt::darkRed;
            break;
        }
        case 2:
        {
            color=Qt::darkGreen;
            break;
        }
        case 3:
        {
            color=Qt::darkBlue;
            break;
        }
        }
        m_drawSingle->graph(wireMarkIndex.at(n).first)->setPen(QPen(color));
        m_drawSingle->graph(wireMarkIndex.at(n).second)->setPen(QPen(color));
    }
    m_drawSingle->graph(wireMarkIndex.at(n).first)->setData(x1,y1);
    m_drawSingle->graph(wireMarkIndex.at(n).second)->setData(x2,y2);
    m_drawSingle->replot();
}
//函数功能：通过ini文件获得Rador设置的基本信息
void simForm::readConfigure(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    m_nLachuzhiC = settings.value("initialPar/m_nLachuzhiC", 0).toInt();
    m_nDaogaoC = settings.value("initialPar/m_nDaogaoC", 0).toInt();
    //    m_nSide=settings.value("initialPar/sideC", 0).toInt();
}
void simForm::slot_allwire(QVector<singleWireInfo> tmp,bool f)
{
    static QPoint cc=QPoint(0,0);
    if(f)
    {
        drawWireDataRange();
        if(tmp.count())
        {
            for(int i=0;i<tmp.count();i++)
            {
                if(i>3)
                    break;
                drawWireMark(tmp.at(i).nWidth,tmp.at(i).nHeight,i);
                float angle=(tmp.at(i).dataIndex.toInt()*0.1667-5)*3.1415/180.0;
                QStringList mm=tmp.at(i).originalWireData.split(" ");
                //                qDebug()<<"width"<<i<<mm.at(2).toInt()*cos(angle)<<mm.at(2).toInt()*sin(angle);//<<tmp.at(0).nWidth<<tmp.at(0).nHeight;
            }
        }
        //        qDebug()<<QString("%1 %2").arg(tmp.at(0).nWidth-cc.x(),4).arg(tmp.at(0).nHeight-cc.y(),-4);
        //    for(int i=0;i<tmp.count();i++)
        //        qDebug()<<"tmp"<<i<<tmp.at(i).nWidth<<tmp.at(i).nHeight;
    }
    else
    {
        drawWireMark(0,0,0);
    }
}
void simForm::on_pbtnDirection_clicked()
{
    indexDirection=!indexDirection;
    if(indexDirection)
        ui->pbtnDirection->setText("increase");
    else
        ui->pbtnDirection->setText("decrease");
}

void simForm::resizeEvent(QResizeEvent *event)
{
    ui->groupBox_2->resize(ui->groupBox_2->width(),this->height()-ui->groupBox->height());
}

void simForm::on_pbtnChangeSideAngle_clicked()
{
    int start=ui->lneSideLower->text().toInt();
    int end=ui->lneSideUp->text().toInt();
    drawSideDataRange(start,end);
    emit updateBaseAngle(start,end,false);
}

void simForm::on_pbtnChangeWireAngle_clicked()
{
    float angle=ui->lneAngle->text().toFloat();
    drawSinglePackage(curDrawData,angle);
    changeBaseAngle(angle);
    emit updateBaseAngle(ui->lneWireLower->text().toInt(),ui->lneWireUp->text().toInt(),true);
}
void simForm::slot_updateRailAngle(float & f)
{
    ui->lneRailAngle->setText(QString::number(f));
}
void simForm::slot_railAngle(void)
{
    QPair<bool,bool > cc;
    if(ui->chbAdd->checkState()==Qt::Checked)
    {
        cc.second=true;
    }
    else
    {
        cc.second=false;

    }
    if(ui->chbRailAngle->checkState()==Qt::Checked)
    {
        cc.first=true;
    }
    else
    {
        cc.first=false;
    }
    qDebug()<<"fff"<<cc;
    emit updateRailAngle(cc);
}

void simForm::on_pbtnUpdateInitialWidth_clicked()
{
    readConfigure();
}
