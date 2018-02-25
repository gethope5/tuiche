#include "twoplotform.h"
#include "ui_twoplotform.h"

#if DEBUG_51UI

twoPlotForm::twoPlotForm(QWidget *parent) :
    QWidget(parent),QThread(parent)
  ,ui(new Ui::twoPlotForm)
  ,curDistance(0)
  ,bFirstAddPole(true)
  ,addWireWidthOffsetY(0)
  ,addWireHeightOffsetY(0)

  #else
twoPlotForm::twoPlotForm(QWidget *parent) :
    QWidget(parent)
  ,ui(new Ui::twoPlotForm)
  ,curDistance(0)
  ,preDropperWalk(-1000)
  ,curWire2Index(0)
  ,curWire1Index(0)
  ,curCarrierIndex(0)
  ,preWire1(QPoint(0,0))
  #endif
{
    ui->setupUi(this);
    readConfig();
    uiInitial();
    //    qDebug()<<"struct initial1"<<m_heightDropperCurve.curveIndex<<m_heightDropperCurve.x.count()<<m_heightDropperCurve.y.count();
}
void twoPlotForm::uiInitial(void)
{
    curvesInitial();
    displayTypeInitial();
    //    getDefaultDisplay();
    setDrawPar=new setDrawParDlg(this);
    connect(setDrawPar,SIGNAL(colorChanged(curveType,QColor)),this,SLOT(slot_setDisplayStyle(curveType,QColor)));
    connect(setDrawPar,SIGNAL(curveUIParChanged()),this,SLOT(slot_setAxisRange()));
    connect(setDrawPar,SIGNAL(curveVisiable(curveType,bool)),this,SLOT(slot_curveVisiable(curveType,bool)));

#ifdef GUIYANG_TUICHE
    ui->stackPoleUI->setCurrentIndex(1);
#else
    ui->stackPoleUI->setCurrentIndex(0);
#endif

    //forgeCurve();
    ui->pbtnMeasures->initialPbtnUI("./images/startMeasure-focus.png","./images/startMeasure.png");
    ui->pbtnStop->initialPbtnUI("./images/stopMeasure-focus.png","./images/stopMeasure.png");
    ui->pbtnUISet->initialPbtnUI("./images/UIset-focus.png","./images/uiset.png");
    ui->pbtnSpeech->initialPbtnUI("./images/parSpeech-focus.png","./images/parSpeech.png");
    //    qDebug()<<"twoplotform initial ok";
    rBars.clear();
    //    addData(100,5000,1,100);
    //    addData(+100,5200,10,200);
    widthPole.clear();
    heightPole.clear();
    //    qDebug()<<"struct initial"<<m_heightDropperCurve.curveIndex<<m_heightDropperCurve.x.count()<<m_heightDropperCurve.y.count();//<<wire2Curves.count()<<poleCurves.count();
    updateOverPar();
    wire2Curves.clear();
    poleCurves.clear();
    wire2Curves<<addWireCurve(wire2Color);

    wire1Curves<<addWireCurve(wire1Color);
    carrierCurves<<addWireCurve(carrierColor);
    poleCurves<<addWireCurve(poleColor);
    //    qDebug()<<"wire2 curve count="<<wire2Curves.count()<<"pole curve count="<<poleCurves.count();
    //    this->installEventFilter(this);
    //    this->installEventFilter(this);
    ui->wdgCurveParent->installEventFilter(this);
    ui->stackPoleUI->installEventFilter(this);
    connect(ui->listViewPolelist, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slots_railPoleClicked(QListWidgetItem*)));
#ifdef GUIYANG_TUICHE
    ui->stackPoleUI->resize(230,300);
    ui->stackPoleUI->move(10,95);
    ui->groupBox->setStyleSheet("border:0px;");
    ui->label_12->setVisible(false);
    ui->lineSide->setVisible(false);
    QPoint tmp=ui->stackPoleUI->pos();
    ui->groupBox->move(tmp.x(),tmp.y()+ui->stackPoleUI->height());
//    ui->pbtnMeasures->move(20,600);
//    ui->pbtnUISet->move(20,650);
//    ui->pbtnStop->move(130,600);
//    ui->pbtnSpeech->move(130,650);
//    ui->chkErrorPole->move(20,690);
#else
    ui->stackPoleUI->resize(230,100);
    ui->stackPoleUI->move(10,95);
#endif
    ui->wdgPoleNum->setLblName(QTextCodec::codecForName("gbk")->toUnicode("杆号:"));
    connect(ui->rdbAdd,SIGNAL(clicked(bool)),this,SLOT(slot_poleDirection(bool)));
    connect(ui->rdbReduce,SIGNAL(clicked(bool)),this,SLOT(slot_poleDirection(bool)));
//    slot_initialPole(bMeasureType,"0");
    ui->rdbAdd->setChecked(true);
}
//函数功能：根据杆号和里程信息，在曲线中绘制杆号，包括杆号文本和杆号提示线。
void twoPlotForm::setPole(QString poleNumber,double sDistance)
{
    addPoleText(m_drawWireWidth,poleNumber,sDistance);
    addPoleText(m_drawWireHeight,poleNumber,sDistance);
    if(!poleCurves.count())
    {
        poleCurves<<addWireCurve(poleColor);
    }
    //qDebug()<<"replay draw pole number,"<<sDistance<<poleNumber<<poleCurves.count();
    drawPoleCurve(sDistance,true,poleCurves.count()-1);//更新导高曲线对应的杆号提示线
    drawPoleCurve(sDistance,false,poleCurves.count()-1);//更新拉出值曲线对应的杆号提示线
    poleCurves<<addWireCurve(poleColor);
}
void twoPlotForm::updateALlPoleCurve(bool flag)
{
    for(int i=0;i<poleCurves.count();i++)
    {
        if(poleCurves.at(i).first.x.count())
        {
            double tmpX=poleCurves.at(i).first.x.at(0);
            drawPoleCurve(tmpX,flag,i);
        }
    }
}
//函数功能：在导高和拉出值曲线的对应位置，绘制杆号提示线。flag=false,在拉出值曲线是绘制提示线；flag=true,在导高曲线上绘制提示线.
void twoPlotForm::drawPoleCurve(double dist,bool flag,int curIndex)
{
    if(poleCurves.count())
    {
        if(flag)
        {
            //qDebug()<<"pole curve"<<poleCurves.count()<<curIndex;
            curveInfo tmp=poleCurves.at(curIndex).first;
            tmp.x.clear();
            tmp.y.clear();
            tmp.x<<dist;
            tmp.x<<dist;
            tmp.y<<m_drawWireWidth->yAxis->range().upper;
            tmp.y<<m_drawWireWidth->yAxis->range().lower;

            poleCurves.replace(curIndex,QPair<curveInfo,curveInfo>(tmp,poleCurves.at(curIndex).second));


            //                        qDebug()<<"width pole curve data,x"<<tmp.x<<tmp.y;
            m_drawWireWidth->graph(tmp.curveIndex)->setData(tmp.x,tmp.y);
        }
        else
        {
            curveInfo tmp=poleCurves.at(curIndex).second;
            tmp.x.clear();
            tmp.y.clear();
            tmp.x<<dist;
            tmp.x<<dist;
            tmp.y<<m_drawWireHeight->yAxis->range().upper;
            tmp.y<<m_drawWireHeight->yAxis->range().lower;
            poleCurves.replace(curIndex,QPair<curveInfo,curveInfo>(poleCurves.at(curIndex).first,tmp));

            //                        qDebug()<<"height pole curve data,x"<<tmp.x<<tmp.y;
            m_drawWireHeight->graph(tmp.curveIndex)->setData(tmp.x,tmp.y);
        }
    }
}
int twoPlotForm::updateRangY(QCustomPlot *textParent)
{
    unsigned int absRange=abs(textParent->yAxis->range().upper-textParent->yAxis->range().lower);
    return textParent->yAxis->range().lower+absRange*0.1;
}
//函数功能：添加杆号文本到对应的拉出值和导高曲线
void twoPlotForm::addPoleText(QCustomPlot *textParent,QString poleNumber,unsigned long sDistance,bool flag)
{
    QCPItemText *addPole =new QCPItemText (textParent);
    //        qDebug()<<"abs"<<absRange<<absRange*0.1;
    //    qDebug()<<"draw pole"<<offsetY<<sDistance<<poleNumber;
    addPole ->position->setCoords(sDistance, updateRangY(textParent));
    addPole ->setFont(QFont(font().family(), 10));
    //    addPole ->setText(QTextCodec::codecForName("gbk")->toUnicode(poleNumber.toLatin1().data()));
    addPole ->setText(QTextCodec::codecForName("gbk")->toUnicode(poleNumber.toLatin1().data()));
    addPole->setColor(poleColor);
    if(textParent==m_drawWireWidth)
    {
        widthPole<<addPole;
    }
    if(textParent==m_drawWireHeight)
    {
        heightPole<<addPole;
    }
    textParent->replot();
}
void twoPlotForm::updatePolePosition(void)
{
    for(int i=0;i<widthPole.count();i++)
    {
        double x=widthPole.at(i)->position->coords().x();
        widthPole.at(i)->position->setCoords(x,updateRangY(m_drawWireWidth));
    }

    for(int i=0;i<heightPole.count();i++)
    {
        double x=heightPole.at(i)->position->coords().x();
        heightPole.at(i)->position->setCoords(x,updateRangY(m_drawWireHeight));
    }
}
//函数功能：用于初始化操作时，获取初始化杆号.true,新线;false,老线界面
void twoPlotForm::slot_initialPole(bool b,QString initialPole)
{
    qDebug()<<"i am initial ";
    //b=true,新线;=false,老线
    if(b)
    {
        newRailInitialUI(true);
        updateNumber(initialPole);
    }
    else
    {
        fillPoleSelList(oldRail_idx);
    }
    setRailType(b);
}
twoPlotForm::~twoPlotForm()
{
    delete ui;
}
void twoPlotForm::curvesInitial(void)
{
    //--------------------------拉出值相关Ui初始化--------------------------------
    m_drawWireWidth=ui->wdgWidth;
    //    m_drawWireWidth->yAxis->setRangeLower(-1000);
    //    m_drawWireWidth->yAxis->setRangeUpper(1000);
    m_drawWireWidth->xAxis->setLabel(QTextCodec::codecForName("gbk")->toUnicode("里程"));
    m_drawWireWidth->yAxis->setLabel(QTextCodec::codecForName("gbk")->toUnicode("拉出值"));

    m_drawWireWidth->setInteractions(QCP::iRangeDrag| QCP::iSelectAxes| QCP::iRangeZoom/**/);
    m_drawWireWidth->axisRect(0)->setBackground(backgroundColor);
    m_drawWireWidth->axisRect()->setupFullAxesBox(true);
    connect(m_drawWireWidth->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(AxisChangingWX(QCPRange)));
    connect(m_drawWireWidth->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(AxisChangingWH(QCPRange)));

    connect(m_drawWireWidth,SIGNAL(mouseDoubleClick (QMouseEvent *)), this, SLOT(defaultSize(QMouseEvent*)));

    //width 1
    m_widthOver1Up=add1Curve(m_drawWireWidth,m_widthOver1Up.color);
    //width 2
    m_widthOver2Up=add1Curve(m_drawWireWidth,m_widthOver2Up.color);
    //width 3
    m_widthOver1Down=add1Curve(m_drawWireWidth,m_widthOver1Down.color);
    //width 4
    m_widthOver2Down=add1Curve(m_drawWireWidth,m_widthOver2Down.color);
    //width 5
    m_widthToolTip=add1Curve(m_drawWireWidth,m_widthToolTip.color);
    //--------------------------导高相关Ui初始化--------------------------------
    m_drawWireHeight=ui->wdgHegiht;
    m_drawWireHeight->xAxis->setLabel(QTextCodec::codecForName("gbk")->toUnicode("里程"));
    m_drawWireHeight->yAxis->setLabel(QTextCodec::codecForName("gbk")->toUnicode("导高"));
    //1导高曲线索引
    m_drawWireHeight->setInteractions(QCP::iRangeDrag| QCP::iSelectAxes| QCP::iRangeZoom/**/);
    m_drawWireHeight->axisRect(0)->setBackground(backgroundColor);
    m_drawWireHeight->axisRect()->setupFullAxesBox(true);
    connect(m_drawWireHeight->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(AxisChangingHX(QCPRange)));
    connect(m_drawWireHeight->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(AxisChangingHY(QCPRange)));
    //    connect(m_drawWireHeight,SIGNAL(mouseDoubleClick (QMouseEvent *)), this, SLOT(defaultSize1(QMouseEvent*)));
    connect(m_drawWireHeight,SIGNAL(mouseDoubleClick (QMouseEvent *)), this, SLOT(defaultSize1(QMouseEvent*)));


    //2 导高吊弦曲线索引
    if(!m_heightDropperCurve.curveIndex)
    {
        //height 1
        m_drawWireHeight->addGraph();
        m_heightDropperCurve.curveIndex=m_drawWireHeight->graphCount()-1;

        m_drawWireHeight->graph(m_heightDropperCurve.curveIndex)->setPen(QPen(dropperColor));
        m_drawWireHeight->graph(m_heightDropperCurve.curveIndex)->setLineStyle(QCPGraph::lsNone);
        m_drawWireHeight->graph(m_heightDropperCurve.curveIndex)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));

        QPen dropperPen;
        dropperPen.setColor(Qt::red);
        dropperPen.setWidth(2);

        errorBars = new QCPErrorBars(m_drawWireHeight->xAxis, m_drawWireHeight->yAxis);
        errorBars->removeFromLegend();
        errorBars->setAntialiased(false);
        errorBars->setDataPlottable(m_drawWireHeight->graph(m_heightDropperCurve.curveIndex));
        errorBars->setPen(dropperPen);
    }
    //3导高警示值下限曲线索引,height 2
    m_heightOver1Down=m_heightOver2Up=add1Curve(m_drawWireHeight,m_heightOver1Down.color);
    //4导高界限值下限曲线索引,height 3
    m_heightOver2Down=m_heightOver2Up=add1Curve(m_drawWireHeight,m_heightOver2Down.color);
    //5导高警示值上限曲线索引,height 4
    m_heightOver1Up=m_heightOver2Up=add1Curve(m_drawWireHeight,m_heightOver1Up.color);
    //5导高界限值上限曲线索引,height 5
    m_heightOver2Up=add1Curve(m_drawWireHeight,m_heightOver2Up.color);
    //height 6
    m_HeightToolTip=add1Curve(m_drawWireHeight,m_HeightToolTip.color);

    //承力索,height 7
//    heightCarriers<<add1Curve(m_drawWireHeight,carrierColor);

    m_drawWireWidth->setMouseTracking(true);
    m_drawWireHeight->setMouseTracking(true);
    connect(m_drawWireWidth,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(slot_mouseMove(QMouseEvent *)));
    connect(m_drawWireHeight,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(slot_mouseMove(QMouseEvent *)));
    connect(m_drawWireWidth,SIGNAL(mouseLeave(QEvent*)),this,SLOT(slot_mouseRelease(QEvent *)));
    connect(m_drawWireHeight,SIGNAL(mouseLeave(QEvent*)),this,SLOT(slot_mouseRelease(QEvent *)));

    setAxisColor(axisColor);
    slot_setAxisRange();
    updateOverPar();
    updateDrawUI();
}
//函数功能：滚动鼠标时，图片放大或缩小
void twoPlotForm::AxisChangingWH(QCPRange cc)
{
#if 0
    //    drawAlarmCurve(m_drawWireWidth->xAxis->range().lower);
    //    drawAlarmCurve(m_drawWireWidth->xAxis->range().upper);
    //    drawAlarmCurve();
    addWireWidthOffsetY= updateRangY(m_drawWireWidth);
    //    addWireHeightOffsetY= updateRangY(m_drawWireHeight);
    for(int i=0;i<widthPole.count();i++)
    {
        QCPItemText * tmp=new QCPItemText (m_drawWireWidth);//widthPole.at(i);
        qDebug()<<"before y"<<widthPole.value(i)->position->coords().y()<<addWireWidthOffsetY;
        tmp->setColor(widthPole.at(i)->color());
        QPoint cc( widthPole.at(i)->position->coords().x(),addWireHeightOffsetY);
        tmp->position->setCoords(cc);//coords().setY(addWireWidthOffsetY);
        widthPole.replace(i,tmp);
        qDebug()<<"after y"<<widthPole.value(i)->position->coords().y()<< tmp->position->coords().y();
    }

    //if(textParent==m_drawWireHeight)
    //    heightPole<<addPoleDisplay;
#endif
    updateALlPoleCurve(true);
    updatePolePosition();
    m_drawWireWidth->replot();
    //    qDebug()<<"uper"<<m_drawWireWidth->yAxis->range()<<cc.upper<<cc.lower;
}
void twoPlotForm::AxisChangingWX(QCPRange cc)
{
    drawAlarmCurve(cc.lower,false);
    drawAlarmCurve(cc.upper,false);
    m_drawWireWidth->replot();
    //    qDebug()<<"uper"<<m_drawWireWidth->xAxis->range().lower<<m_drawWireWidth->xAxis->range().upper;
}
void  twoPlotForm::AxisChangingHX(QCPRange x)
{
    drawAlarmCurve(x.lower,true);
    drawAlarmCurve(x.upper,true);
    m_drawWireHeight->replot();
}
void  twoPlotForm::AxisChangingHY(QCPRange)
{
    updatePolePosition();
    updateALlPoleCurve(false);
    m_drawWireHeight->replot();
}
void twoPlotForm::defaultSize(QMouseEvent*)
{
    //    qDebug()<<"x=w"<<m_XRange<<m_YRange;
    //    m_drawWireWidth->xAxis->setRange(m_drawWireWidth->xAxis->range().lower,m_drawWireWidth->xAxis->range().lower+Default_X_Value);
    m_drawWireWidth->yAxis->setRange(-450,450);
    m_drawWireWidth->xAxis->setRange(0,Default_X_Value);

}
void twoPlotForm::defaultSize1(QMouseEvent*)
{
    //    qDebug()<<"x=h"<<m_XRange<<m_YRange;
    //    m_drawWireHeight->xAxis->setRange(m_drawWireHeight->xAxis->range().lower,m_drawWireHeight->xAxis->range().lower+Default_X_Value);
    m_drawWireHeight->xAxis->setRange(0,Default_X_Value);

    m_drawWireHeight->yAxis->setRange(5300,6800);
}
QPair<curveInfo,curveInfo > twoPlotForm::addWireCurve(QColor color)
{
    curveInfo widthTmp;
    curveInfo heightTmp;
    widthTmp=add1Curve(m_drawWireWidth,color);
    heightTmp=add1Curve(m_drawWireHeight,color);
    return QPair<curveInfo,curveInfo>(widthTmp,heightTmp);
}
curveInfo twoPlotForm::add1Curve(QCustomPlot *drawWire,QColor color)
{
    curveInfo tmp;
    if(!tmp.curveIndex)
    {
        drawWire->addGraph();
        //        qDebug()<<"cur graph count="<<drawWire->graphCount();
        tmp.curveIndex=drawWire->graphCount()-1;
        drawWire->graph(tmp.curveIndex)->setPen(QPen(color));
    }
    return tmp;
}
void twoPlotForm::addWire2Point(MeasureDebugData &databuffer )
{
    if(wire2Curves.count())
    {
        if(abs(curWire2Index-wire2Curves.count())!=1)
        {
            qDebug()<<"Wire2 Index error,wire2 curve count="<<wire2Curves.count()<<"cur index="<<curWire2Index;
            return ;
        }
    }
    else
        return ;
    if(databuffer.wire2.y())
    {
        curveInfo widthCurve=wire2Curves.at(curWire2Index).first;
        curveInfo heightCurve=wire2Curves.at(curWire2Index).second;
        heightCurve.y<<databuffer.wire2.y();

        widthCurve.y<<databuffer.wire2.x();
        heightCurve.x<<databuffer.nWalkDistance;
        widthCurve.x<<databuffer.nWalkDistance;
        wire2Curves.replace(curWire2Index,QPair<curveInfo,curveInfo>(widthCurve,heightCurve));
    }
    //    qDebug()<<"count"<<wire2Curves.count()<<curWire2Index;
    m_drawWireWidth->graph(wire2Curves.at(curWire2Index).first.curveIndex)->setData(wire2Curves.at(curWire2Index).first.x,wire2Curves.at(curWire2Index).first.y);
    m_drawWireHeight->graph(wire2Curves.at(curWire2Index).second.curveIndex)->setData(wire2Curves.at(curWire2Index).second.x,wire2Curves.at(curWire2Index).second.y);
}
void twoPlotForm::addOneData( MeasureDebugData &databuffer)
{
    if(databuffer.wire1.y()==0||databuffer.wire1.x()>1000||databuffer.wire1.x()<-1000)
        return ;
    //    qDebug()<<"dropper data"<<nWalkDistance-preDropperWalk<<nWalkDistance;
    //3绘制拉出值及导高曲线
    if(databuffer.wire1.y())
    {
        QPair<curveInfo,curveInfo> tmp=wire1Curves.at(curWire1Index);

        if(preWire1.y())
        {
            //            if(abs(preWire1.y()-databuffer.wire1.y())<10)
            {
                tmp.first.x<<databuffer.nWalkDistance;
                tmp.first.y<<databuffer.wire1.x();
                tmp.second.y<<databuffer.wire1.y();
                tmp.second.x<<databuffer.nWalkDistance;
                //                qDebug()<<"add a point ,array count="<<curWire1Index<<wire1Curves.at(curWire1Index).first.x.count()<<wire1Curves.at(curWire1Index).first.y.count()<<wire1Curves.at(curWire1Index).first.x<<wire1Curves.at(curWire1Index).first.y;
                wire1Curves.replace(curWire1Index,tmp);
                preWire1=databuffer.wire1;
            }
            //            else
            //            {
            //                qDebug()<<"error wire1"<<preWire1<<databuffer.wire1;
            //            }
        }
        else
        {
            preWire1=databuffer.wire1;
        }
    }

    //    qDebug()<<"before x"<<wire1Curves.at(curWire1Index).first.x.count()<<wire1Curves.at(curWire1Index).second.x.count();
    //        qDebug()<<"wire 1 info x"<<wire1Curves.at(curWire1Index).first.y.count()<<wire1Curves.at(curWire1Index).second.y.count()<<wire1Curves.at(curWire1Index).second.y<<m_WidthCurve1.y;
    m_drawWireWidth->graph(wire1Curves.at(curWire1Index).first.curveIndex)->setData(wire1Curves.at(curWire1Index).first.x,wire1Curves.at(curWire1Index).first.y);
    m_drawWireHeight->graph(wire1Curves.at(curWire1Index).second.curveIndex)->setData(wire1Curves.at(curWire1Index).second.x,wire1Curves.at(curWire1Index).second.y);
    //    qDebug()<<"wire1 par"<<databuffer.wire1.first;//databuffer.nWireHeight1<<databuffer.nWireWidth1;
    //添加wire2,用于线岔及锚段关节绘制
    if(databuffer.wire2Valid)//wire2 起始判别
    {
        addWire2Point(databuffer);
    }

    //    qDebug()<<"start judge+++++"<<databuffer.wire2Valid.first<<databuffer.wire2Valid.second<<databuffer.wire1<<wire2Curves.count();//.startWire2.first<<wire2Judge.startWire2.second<<"end"<<wire2Judge.endWire2.first<<wire2Judge.endWire2.second<<wire2Curves.count()<<wire2Curves.at(curWire2Index).first.y.count();
    //4绘制承力索曲线
    drawCarrier(databuffer.carrier,databuffer.nWalkDistance);
    //坐标调整
    axisChange(databuffer.nWalkDistance);
    m_drawWireWidth->replot();
    m_drawWireHeight->replot();
}
void twoPlotForm::drawCarrier(QPoint carrier,double walk)
{
    //    qDebug()<<"carrier"<<carrier<<walk;
    if(carrier.y())
    {
        QPair<curveInfo,curveInfo> tmpCarrier=carrierCurves.at(curCarrierIndex);
        bool cc=true;
        if(tmpCarrier.first.x.count())
        {
            if(tmpCarrier.first.x.at(tmpCarrier.first.x.count()-1)==walk)
            {
                cc=false;
                //                qDebug()<<"error carrier data"<<carrier<<walk<<tmpCarrier.x.at(tmpCarrier.x.count()-1);
            }
        }
        if(cc)
        {
            tmpCarrier.second.y<<carrier.y();
            tmpCarrier.second.x<<walk;
            tmpCarrier.first.y<<carrier.x();
            tmpCarrier.first.x<<walk;
            carrierCurves.replace(curCarrierIndex,tmpCarrier);
            m_drawWireHeight->graph(tmpCarrier.second.curveIndex)->setData(tmpCarrier.second.x,tmpCarrier.second.y);
            m_drawWireWidth->graph(tmpCarrier.first.curveIndex)->setData(tmpCarrier.first.x,tmpCarrier.first.y);
            //              qDebug()<<"carrier draw"<<tmpCarrier.x.count()<<tmpCarrier.curveIndex<<walk<<carrier;
        }
    }
}
void twoPlotForm::axisChange(const int walk)
{
    //判断当前里程是否在坐标范围之内
    //    qDebug()<<"y range="<<m_drawWireHeight->xAxis->range().lower<<m_drawWireHeight->xAxis->range().upper;
    double xUpper=m_drawWireHeight->xAxis->range().upper;
    double xLower=m_drawWireHeight->xAxis->range().lower;
    double dUpperBuffer=walk-xUpper;
    double dLowerBuffer=xLower-walk;
    if(dUpperBuffer>0)
    {
        m_drawWireHeight->xAxis->setRange(xLower+dUpperBuffer,xUpper+dUpperBuffer);
        m_drawWireWidth->xAxis->setRange(xLower+dUpperBuffer,xUpper+dUpperBuffer);

    }
    else if(dLowerBuffer>0)
    {
        m_drawWireHeight->xAxis->setRange(xLower-dLowerBuffer,xUpper-dLowerBuffer);
        m_drawWireWidth->xAxis->setRange(xLower-dLowerBuffer,xUpper-dLowerBuffer);
    }
}
//函数功能：绘制警示值曲线
void twoPlotForm::drawAlarmCurve(const int x,bool flag)
{
    //    qDebug()<<"alarm over"<<x;
    if(flag)
    {
        m_heightOver1Up.y<<overHeightMax;
        m_heightOver1Down.y<<overHeightMin;
        m_heightOver1Down.x<<x;
        m_heightOver2Up.y<<heightOver2Up;
        m_heightOver2Down.y<<heightOver2Down;
        m_heightOver2Down.x<<x;
        if(bAlarm1Display)
        {

            m_drawWireHeight->graph(m_heightOver1Down.curveIndex)->setData(m_heightOver1Down.x,m_heightOver1Down.y);
            m_drawWireHeight->graph(m_heightOver1Up.curveIndex)->setData(m_heightOver1Down.x,m_heightOver1Up.y);
        }
        if(bAlarm2Display)
        {
            m_drawWireHeight->graph(m_heightOver2Up.curveIndex)->setData(m_heightOver2Down.x,m_heightOver2Up.y);
            m_drawWireHeight->graph(m_heightOver2Down.curveIndex)->setData(m_heightOver2Down.x,m_heightOver2Down.y);
        }
    }
    else
    {
        m_widthOver1Up.y<<overWidthMax;
        m_widthOver2Up.y<<widthOver2;

        m_widthOver1Down.y<<overWidthMin;
        m_widthOver2Down.y<<-widthOver2;
        m_widthOver2Down.x<<x;
        m_widthOver1Down.x<<x;
        if(bAlarm1Display)
        {
            m_drawWireWidth->graph(m_widthOver1Up.curveIndex)->setData(m_widthOver1Down.x,m_widthOver1Up.y);
            m_drawWireWidth->graph(m_widthOver1Down.curveIndex)->setData(m_widthOver1Down.x,m_widthOver1Down.y);
        }
        if(bAlarm2Display)
        {
            m_drawWireWidth->graph(m_widthOver2Up.curveIndex)->setData(m_widthOver2Down.x,m_widthOver2Up.y);
            m_drawWireWidth->graph(m_widthOver2Down.curveIndex)->setData(m_widthOver2Down.x,m_widthOver2Down.y);
        }
    }
    m_drawWireWidth->replot();
    m_drawWireHeight->replot();
}
//函数功能：清楚曲线数据及对应的图像信息
void twoPlotForm::clearCurves(void)
{
    //    //    wire1Curves.at(curWire1Index).first.x.clear();
    //    //    wire1Curves.at(curWire1Index).first.y.clear();
    //    //    wire1Curves.at(curWire1Index).second.y.clear();
    //    //    wire1Curves.at(curWire1Index).second.x.clear();

    //    heightPole.clear();
    //    widthPole.clear();

    for(int i=0;i<widthPole.count();i++)
    {
        widthPole.at(i)->setText("");
    }
    for(int i=0;i<heightPole.count();i++)
    {
        heightPole.at(i)->setText("");
    }
    widthPole.clear();
    heightPole.clear();

    for(int i=0;i<wire1Curves.count();i++)
    {
        QPair<curveInfo,curveInfo> tmp=wire1Curves.at(i);
        //        qDebug()<<"wire count"<<tmp.first.x.count()<<tmp.first.y.count();
        tmp.first.x.clear();
        tmp.first.y.clear();
        tmp.second.y.clear();
        tmp.second.x.clear();
        wire1Curves.replace(i,tmp);
        //        qDebug()<<"wire count+"<<wire1Curves.at(i).first.x.count()<<wire1Curves.at(i).first.y.count();
        m_drawWireWidth->graph(wire1Curves.at(i).first.curveIndex)->setData(wire1Curves.at(i).first.x,wire1Curves.at(i).first.y);
        m_drawWireHeight->graph(wire1Curves.at(i).second.curveIndex)->setData(wire1Curves.at(i).second.x,wire1Curves.at(i).second.y);
    }
    for(int i=0;i<wire2Curves.count();i++)
    {
        QPair<curveInfo,curveInfo> tmp=wire2Curves.at(i);
        tmp.first.x.clear();
        tmp.first.y.clear();
        tmp.second.y.clear();
        tmp.second.x.clear();
        wire2Curves.replace(i,tmp);
        m_drawWireWidth->graph(wire2Curves.at(i).first.curveIndex)->setData(wire2Curves.at(i).first.x,wire2Curves.at(i).first.y);
        m_drawWireHeight->graph(wire2Curves.at(i).second.curveIndex)->setData(wire2Curves.at(i).second.x,wire2Curves.at(i).second.y);
    }
    for(int i=0;i<poleCurves.count();i++)
    {
        QPair<curveInfo,curveInfo> tmp=poleCurves.at(i);
        tmp.first.x.clear();
        tmp.first.y.clear();
        tmp.second.y.clear();
        tmp.second.x.clear();
        poleCurves.replace(i,tmp);
        m_drawWireHeight->graph(poleCurves.at(i).second.curveIndex)->setData(poleCurves.at(i).second.x,poleCurves.at(i).second.y);
        m_drawWireWidth->graph(poleCurves.at(i).first.curveIndex)->setData(poleCurves.at(i).first.x,poleCurves.at(i).first.y);
    }
    poleCurves.clear();
    for (int i=0;i<heightPole.count();i++)
    {
        heightPole.at(i)->position->setCoords(0,0);
    }
    heightPole.clear();
    for (int i=0;i<widthPole.count();i++)
    {
        widthPole.at(i)->position->setCoords(0,0);
    }
    widthPole.clear();
    for(int i=0;i<carrierCurves.count();i++)
    {
        QPair<curveInfo,curveInfo>  tmp=carrierCurves.at(i);
        tmp.first.x.clear();
        tmp.first.y.clear();
        tmp.second.x.clear();
        tmp.second.y.clear();
        carrierCurves.replace(i,tmp);
        m_drawWireHeight->graph(carrierCurves.at(i).second.curveIndex)->setData(carrierCurves.at(i).second.x,carrierCurves.at(i).second.y);
        m_drawWireWidth->graph(carrierCurves.at(i).first.curveIndex)->setData(carrierCurves.at(i).first.x,carrierCurves.at(i).first.y);

    }
    curWire2Index=0;
    curWire1Index=0;
    curCarrierIndex=0;
    m_drawWireHeight->replot();
    m_drawWireWidth->replot();
    //    m_drawWireHeight->clearGraphs();
    //    m_drawWireWidth->clearGraphs();
}
void twoPlotForm::slot_dataReplay(QVector<double> dDistance,QVector<QPoint> wire1,QVector<QPoint> wire2,QVector<QPoint> dCarrierHeight,QVector<QPair<double,int> > &poles)
{
    qDebug()<<"i get the replay data";
    //    for(int i=0;i<dWireHeight.count()-1;i++)
    //    {
    //        qDebug()<<i<<dWireHeight.at(i)-dWireHeight.at(i+1);
    //    }
    qDebug()<<"replay data count,distance="<<dDistance.count()<<"wire1="<<wire1.count()<<"wire2="<<wire2.count()<<"carrier="<<dCarrierHeight.count();
    clearCurves();
    for(int i=0;i<wire1.count();i++)
    {
        if(wire1.at(i).y())
        {
            QPair<curveInfo,curveInfo> tmp=wire1Curves.at(curWire1Index);

            tmp.first.x<<dDistance.at(i);
            tmp.first.y<<wire1.at(i).x();
            tmp.second.y<<wire1.at(i).y();
            tmp.second.x<<dDistance.at(i);
            wire1Curves.replace(curWire1Index,tmp);
        }
    }
    processWire2Replay(wire2,dDistance);
    processCarrierReplay(dCarrierHeight,dDistance);

    for(int i=0;i<wire2Curves.count();i++)
    {
        //        qDebug()<<"wire2 ,index"<<i<<"data counts="<<wire2Curves.at(i).first.x.count();
        m_drawWireWidth->graph(wire2Curves.at(i).first.curveIndex)->setData(wire2Curves.at(i).first.x,wire2Curves.at(i).first.y);
        m_drawWireHeight->graph(wire2Curves.at(i).second.curveIndex)->setData(wire2Curves.at(i).second.x,wire2Curves.at(i).second.y);
    }
    for(int i=0;i<wire1Curves.count();i++)
    {
        m_drawWireWidth->graph(wire1Curves.at(i).first.curveIndex)->setData(wire1Curves.at(i).first.x,wire1Curves.at(i).first.y);
        m_drawWireHeight->graph(wire1Curves.at(i).second.curveIndex)->setData(wire1Curves.at(i).second.x,wire1Curves.at(i).second.y);
    }
    for(int i=0;i<carrierCurves.count();i++)
    {
        m_drawWireHeight->graph(carrierCurves.at(i).second.curveIndex)->setData(carrierCurves.at(i).second.x,carrierCurves.at(i).second.y);
        m_drawWireWidth->graph(carrierCurves.at(i).first.curveIndex)->setData(carrierCurves.at(i).first.x,carrierCurves.at(i).first.y);
    }
    m_drawWireWidth->replot();
    m_drawWireHeight->replot();
    qDebug()<<"curve counts,wire1="<<wire1Curves.count()<<"wire2="<<wire2Curves.count()<<"carrier ="<<carrierCurves.count();

    //    qDebug()<<"before,wire1 w="<< wire1Curves.at(curWire1Index).first.x.count()<<wire1Curves.at(curWire1Index).first.y.count()<< m_WidthCurve2.x.count()<<m_WidthCurve2.y.count()<< wire1Curves.at(curWire1Index).second.x.count()<<wire1Curves.at(curWire1Index).second.y.count()<< m_heightCurve2.x.count()<<m_heightCurve2.y.count();
    for(int i=0;i<poles.count();i++)
    {
        setPole(QString::number(poles.at(i).second),poles.at(i).first);
        //        addPoleText(m_drawWireHeight,QString::number(poles.at(i).second),poles.at(i).first);
        //        addPoleText(m_drawWireWidth,QString::number(poles.at(i).second),poles.at(i).first);
    }
    //    qDebug()<<"after data count,distance="<<    m_WidthCurve2.x.count();
}
void twoPlotForm::processCarrierReplay(QVector<QPoint> &carriers,QVector<double> &dDistance)
{
    QVector<int> carrierJudage;
    for(int i=0;i<carriers.count();i++)
    {
        if(carriers.at(i).y())
        {
            QPair<curveInfo,curveInfo>tmp =carrierCurves.at(curCarrierIndex);
            tmp.second.x<<dDistance.at(i);
            tmp.second.y<<carriers.at(i).y();
            tmp.first.x<<dDistance.at(i);
            tmp.first.y<<carriers.at(i).x();
            carrierCurves.replace(curCarrierIndex,tmp);
            carrierJudage.push_back(0);
        }
        else
        {
            carrierJudage.push_back(1);
        }
        if(carrierJudage.count()==(Another_Curve_STD+1))
        {
            carrierJudage.pop_front();
            int sumTmp=0;
            for(int index=0;index<carrierJudage.count();index++)
            {
                sumTmp+=carrierJudage.at(index);
            }
            if(sumTmp==carrierJudage.count())
            {
                if(carrierCurves.at(curCarrierIndex).first.x.count())
                {
                    slot_createCarrier();
                }
            }
        }
    }
}
//函数功能：根据wire2的回放数据，绘制wire2的曲线
void twoPlotForm::processWire2Replay(QVector<QPoint> &wire2,QVector<double> &dDistance)
{
    QVector<int> addCurve;
    for(int i=0;i<wire2.count();i++)
    {
        if(wire2.at(i).y())
        {
            QPair<curveInfo,curveInfo> tmp=wire2Curves.at(curWire2Index);

            tmp.first.x<<dDistance.at(i);
            tmp.first.y<<wire2.at(i).x();
            tmp.second.y<<wire2.at(i).y();
            tmp.second.x<<dDistance.at(i);
            wire2Curves.replace(curWire2Index,tmp);
            addCurve.push_back(0);
        }
        else
        {
            addCurve.push_back(1);
        }
        //判别是否新增导线2
        if(addCurve.count()==(Another_Curve_STD+1))
        {
            addCurve.pop_front();
            int sumTmp=0;
            for(int index=0;index<addCurve.count();index++)
            {
                sumTmp+=addCurve.at(index);
            }
            //            qDebug()<<"wire2 judage sum"<<sumTmp<<addCurve;
            if(sumTmp==addCurve.count())
            {
                if(wire2Curves.at(curWire2Index).first.x.count())
                {
                    addWire2Curve();
                }
                //                else
                //                {
                //                    qDebug()<<"error add wire2 curve";
                //                }
            }
        }
    }

}
void twoPlotForm::setAxisColor(QColor color)
{
    m_drawWireWidth->xAxis->setTickLabelColor(color);
    m_drawWireWidth->xAxis2->setTickLabelColor(color);
    m_drawWireWidth->yAxis->setTickLabelColor(color);
    m_drawWireWidth->yAxis2->setTickLabelColor(color);

    m_drawWireHeight->xAxis->setTickLabelColor(color);
    m_drawWireHeight->xAxis2->setTickLabelColor(color);
    m_drawWireHeight->yAxis->setTickLabelColor(color);
    m_drawWireHeight->yAxis2->setTickLabelColor(color);
    m_drawWireWidth->replot();
    m_drawWireHeight->replot();
}

void twoPlotForm::slot_setDisplayStyle(curveType type,QColor color)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);

    QString strColor;
    strColor=colorTostr(color);
    if(type==curveType::wire1Color)//接触线1曲线颜色
    {
//        QPen  myPen= m_drawWireWidth->graph(wire1Curves.at(curWire1Index).first.curveIndex)->pen();
//        myPen.setColor(color);
//        m_drawWireWidth->graph(wire1Curves.at(curWire1Index).first.curveIndex)->setPen(myPen);
//        m_drawWireHeight->graph(wire1Curves.at(curWire1Index).second.curveIndex)->setPen(myPen);
        wire1Color=color;
        updatePairWireColor(wire1Curves,color);

        settings.setValue("ScanDiaplayColor/scanWireColor",strColor);
        setLableColor(ui->lblWire,color);
    }

    else if(type==curveType::wire2Color)//接触线2曲线颜色
    {
        wire2Color=color;
        updatePairWireColor(wire2Curves,color);
        settings.setValue("ScanDiaplayColor/wire2Color",strColor);
        setLableColor(ui->lblWire2,color);
    }
    else if(type==curveType::poleColor)//杆号颜色
    {
        poleColor=color;
        updatePairWireColor(poleCurves,color);
        updatePoleTextColor(color);
        settings.setValue("ScanDiaplayColor/poleColor",strColor);
        setLableColor(ui->lblPole,color);
    }
    else if(type==curveType::dropperColor)//吊弦颜色
    {
        dropperColor=color;
        updateDropperColor(color);
        settings.setValue("ScanDiaplayColor/dropperColor",strColor);
        setLableColor(ui->lblDrop,color);
        dropperColor=color;
    }
    else if(type==curveType::carrierColor)//承力索颜色
    {
        carrierColor=color;
        updatePairWireColor(carrierCurves,color);
//        updateCarrierColor(color);
        setLableColor(ui->lblCarrier,color);
        settings.setValue("ScanDiaplayColor/carrierColor",strColor);
    }
    else if(type==curveType::axisColor)//坐标字符颜色
    {
        setAxisColor(color);
        settings.setValue("ScanDiaplayColor/scanWireColor",strColor);
    }
    else if(type==curveType::backgroundColor)//背景颜色
    {
        m_drawWireWidth->axisRect(0)->setBackground(color);
        m_drawWireHeight->axisRect(0)->setBackground(color);
        settings.setValue("ScanDiaplayColor/backGroundColor",strColor);
    }
    else if(type==curveType::DisplayStyle)
    {
        getDefaultDisplay();
    }
    else if(type==curveType::Alarm1)
    {
        QPen tmpPen=m_drawWireWidth->graph(m_widthOver1Up.curveIndex)->pen();
        tmpPen.setColor(color);
        m_drawWireWidth->graph(m_widthOver1Up.curveIndex)->setPen(tmpPen);

        tmpPen=m_drawWireWidth->graph(m_widthOver1Down.curveIndex)->pen();
        tmpPen.setColor(color);
        m_drawWireWidth->graph(m_widthOver1Down.curveIndex)->setPen(tmpPen);

        tmpPen=m_drawWireHeight->graph(m_heightOver1Down.curveIndex)->pen();
        tmpPen.setColor(color);
        m_drawWireHeight->graph(m_heightOver1Down.curveIndex)->setPen(tmpPen);

        tmpPen=m_drawWireHeight->graph(m_heightOver1Up.curveIndex)->pen();
        tmpPen.setColor(color);
        m_drawWireHeight->graph(m_heightOver1Up.curveIndex)->setPen(tmpPen);
        settings.setValue("ScanDiaplayColor/alarm1",strColor);
    }
    else if(type==curveType::Alarm2)
    {
        QPen tmpPen=m_drawWireWidth->graph(m_widthOver2Up.curveIndex)->pen();
        tmpPen.setColor(color);
        m_drawWireWidth->graph(m_widthOver2Up.curveIndex)->setPen(tmpPen);

        tmpPen=m_drawWireWidth->graph(m_widthOver2Down.curveIndex)->pen();
        tmpPen.setColor(color);
        m_drawWireWidth->graph(m_widthOver2Down.curveIndex)->setPen(tmpPen);

        tmpPen=m_drawWireHeight->graph(m_heightOver2Down.curveIndex)->pen();
        tmpPen.setColor(color);
        m_drawWireHeight->graph(m_heightOver2Down.curveIndex)->setPen(tmpPen);

        tmpPen=m_drawWireHeight->graph(m_heightOver2Up.curveIndex)->pen();
        tmpPen.setColor(color);
        m_drawWireHeight->graph(m_heightOver2Up.curveIndex)->setPen(tmpPen);
        settings.setValue("ScanDiaplayColor/alarm2",strColor);
    }

    m_drawWireWidth->replot();
    m_drawWireHeight->replot();
}
QString twoPlotForm::colorTostr(QColor &color)
{
    int nRed=color.red();
    int nGreen=color.green();
    int nBlue=color.blue();
    QString strColor=QString::number(nRed).append(",").append(QString::number(nBlue)).append(",").append(QString::number(nGreen));
    return strColor;
}
void twoPlotForm::getDefaultDisplay()
{
    slot_setDisplayStyle(curveType::wire1Color,defCurve);
    slot_setDisplayStyle(curveType::axisColor,defAxis);
    slot_setDisplayStyle(curveType::backgroundColor,defBackground);
    setDrawStyle(defaultDrawStyle);
}
void twoPlotForm::setDrawStyle(bool bLine)
{
    if(bLine)
    {
        m_drawWireWidth->graph(wire1Curves.at(curWire1Index).first.curveIndex)->setLineStyle(QCPGraph::lsNone);
        m_drawWireHeight->graph(wire1Curves.at(curWire1Index).second.curveIndex)->setLineStyle(QCPGraph::lsNone);
    }
    else
    {
        m_drawWireWidth->graph(wire1Curves.at(curWire1Index).first.curveIndex)->setLineStyle(QCPGraph::lsLine);
        m_drawWireHeight->graph(wire1Curves.at(curWire1Index).second.curveIndex)->setLineStyle(QCPGraph::lsLine);
    }
    m_drawWireWidth->replot();
    m_drawWireHeight->replot();
}
void twoPlotForm::updateConfigColor(void)
{

}
void twoPlotForm::readConfig(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    //获取默认配置信息
    defCurve=settings.value("defaultDiaplayStyle/defaultscanWireColor","red").toString();
    defAxis=settings.value("defaultDiaplayStyle/defaultaxisLabelColor","white").toString();
    defBackground=settings.value("defaultDiaplayStyle/defaultbackGroundColor","black").toString();
    defaultDrawStyle=settings.value("defaultDiaplayStyle/defaultDrawStyle",false).toBool();
    //获取最近显示配置信息    
    strToColor(settings.value("ScanDiaplayColor/scanWireColor","0,255,0").toString(),wire1Color);   //1 wire1
    strToColor(settings.value("ScanDiaplayColor/wire2Color","0,255,0").toString(),wire2Color);      //2 wire2
    strToColor(settings.value("ScanDiaplayColor/poleColor","0,255,0").toString(),poleColor);        //3 pole
    strToColor(settings.value("ScanDiaplayColor/dropperColor","0,255,0").toString(),dropperColor);  //4 dropper
    strToColor(settings.value("ScanDiaplayColor/carrierColor","blue").toString(),carrierColor);     //5 carrier

    setLableColor(ui->lblWire,wire1Color);
    setLableColor(ui->lblWire2,wire2Color);
    setLableColor(ui->lblPole,poleColor);
    setLableColor(ui->lblDrop,dropperColor);
    setLableColor(ui->lblCarrier,carrierColor);

    strToColor(settings.value("ScanDiaplayColor/axisLabelColor","red").toString(),axisColor);
    strToColor(settings.value("ScanDiaplayColor/backGroundColor","red").toString(),backgroundColor);
    drawStyle=settings.value("ScanDiaplayColor/DrawStyle","red").toBool();
    strToColor(settings.value("ScanDiaplayColor/alarm1","red").toString(),m_heightOver1Up.color);

    m_heightOver1Down.color=m_heightOver1Up.color;
    m_widthOver1Up.color=m_heightOver1Up.color;
    m_widthOver1Down.color=m_heightOver1Up.color;

    strToColor(settings.value("ScanDiaplayColor/alarm2","red").toString(),m_heightOver2Up.color);
    m_heightOver2Down.color=m_heightOver2Up.color;
    m_widthOver2Up.color=m_heightOver2Up.color;
    m_widthOver2Down.color=m_heightOver2Up.color;

    bAlarm1Display=settings.value("ScanDiaplayColor/alarm1Display",1).toBool();
    bAlarm2Display=settings.value("ScanDiaplayColor/alarm2Display",1).toBool();
    bDropperDisplay=settings.value("ScanDiaplayColor/dropperDisplay",1).toBool();
    bPoleDisplay=settings.value("ScanDiaplayColor/poleDisplay",1).toBool();
    bCarrierDisplay=settings.value("ScanDiaplayColor/carrierDisplay",1).toBool();

    //    qDebug()<<"draw style"<<drawStyle<<defaultDrawStyle<<curveColor<<axisColor;

    if(settings.value("system/poleType", "true").toBool())
    {
        ui->pbtnAddPole->setEnabled(true);
    }
    else
    {
        ui->pbtnAddPole->setEnabled(false);
    }
}
void twoPlotForm::displayTypeInitial(void)
{
    setAlarm1Visible(bAlarm1Display);
    setAlarm2Visible(bAlarm2Display);
    setPoleVisible(bPoleDisplay);
    setCarrierVisible(bCarrierDisplay);
    setDropperVisible(bDropperDisplay);
}
void twoPlotForm::strToColor(QString strColor,QColor &color)
{
    if(strColor.split(",").count()==3)
    {
        int nRed=strColor.split(",").at(0).toInt();
        int nBlue=strColor.split(",").at(1).toInt();
        int nGreen=strColor.split(",").at(2).toInt();
        color=QColor(nRed,nBlue,nGreen);
        //        qDebug()<<"color getted";
    }
}
void twoPlotForm::slot_setAxisRange(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    int WireWidthMax=settings.value("axisPar/WireWidthMax", 600).toInt();
    int WireWidthMin=settings.value("axisPar/WireWidthMin", -600).toInt();
    int WireHeightMax=settings.value("axisPar/WireHeightMax", 7500).toInt();
    int WireHeightMin=settings.value("axisPar/WireHeightMin", 5300).toInt();
    int WalkMax=settings.value("axisPar/WalkMax", -5).toInt();
    int WalkMin=settings.value("axisPar/WalkMin", -5).toInt();

    m_drawWireWidth->yAxis->setRange(WireWidthMin,WireWidthMax);
    //    m_drawWireWidth->defYRangeLower=WireWidthMin;
    //    m_drawWireWidth->defYRangeUpper=WireWidthMax;
    m_drawWireWidth->xAxis->setRange(WalkMin,WalkMax);

    m_drawWireHeight->yAxis->setRange(WireHeightMin,WireHeightMax);
    //    m_drawWireHeight->defYRangeLower=WireHeightMin;
    //    m_drawWireHeight->defYRangeUpper=WireHeightMax;
    m_drawWireHeight->xAxis->setRange(WalkMin,WalkMax);
    //    bFirstAddPole=true;//坐标系统发生改变,重新设置添加杆号的偏移坐标
    //    qDebug()<<"walk"<<WalkMax<<WalkMin;
    m_drawWireWidth->replot();
    m_drawWireHeight->replot();
}
void twoPlotForm::on_pbtnMeasures_clicked()
{    
    //    MeasureDB::bSimDraw=false;
    emit MeasuresSatus();
}
void twoPlotForm::setMeasureStatus(bool flag )
{
    qDebug()<<"meaure stauts"<<flag;
    if(flag)
    {
        ui->pbtnMeasures->initialPbtnUI("./images/startMeasure-focus.png","./images/startMeasure.png");
    }
    else
    {
        ui->pbtnMeasures->initialPbtnUI("./images/pasueMeasure-focus.png","./images/pasueMeasure.png");
    }
}
//函数功能：更新绘制曲线界面中，拉出值、导高、里程、公里标及侧面界限值
void twoPlotForm::updateMeasureData(MeasureDebugData &databuffer,int nSide,bool flag)
{

    //更新坐标系拉出值-里程，导高-里程
    if(flag)
    {
        ui->lineWireHeight->setText(QString::number(databuffer.wire1.y()));
        ui->lineWireWidth->setText(QString::number(databuffer.wire1.x()));

        //        ui->lineWireHeight1->setText(QString::number(databuffer.nWireHeight1));
        //        ui->lineWireWidth1->setText(QString::number(databuffer.nWireWidth1));
        //        qDebug()<<""
        if(( nSide>0)&&(nSide<100000))
        {
            //            qDebug()<<"++++++++++++++++++here get side";
            ui->lineSide->setText(QString::number(nSide));
        }
        else
        {
            ui->lineSide->setText("0");
        }
    }
    ui->lineDis->setText(QString::number(databuffer.nWalkDistance));
    ui->lineKm->setText(QString::number(databuffer.dKm,'f',3));
    if(!bNewRailType)
    {
        updateNumber(databuffer.poleID);
    }
    //    qDebug()<<"other wire 2 valid="<<databuffer.wire1.first;
    addOneData(databuffer);
}
void twoPlotForm::setRailType(bool flag)
{
    bNewRailType=flag;
    if(flag)
    {
        ui->stackPoleUI->setCurrentIndex(0);//临时线路
    }
    else
    {
        ui->stackPoleUI->setCurrentIndex(1);//既有线路
    }
}

void twoPlotForm::setDisplayPar(void)
{
    setDrawPar->exec();
}


void twoPlotForm::on_pbtnUISet_clicked()
{
    setDisplayPar();
}
void twoPlotForm::on_pbtnUIExpand_clicked()
{
    setExpand(m_bUIVisible);
    m_bUIVisible=!m_bUIVisible;
}
void twoPlotForm::slot_expandUI(bool b)
{
    qDebug()<<"expang pbtn is change";
    setExpand(b);
    m_bUIVisible=b;
}

void twoPlotForm::setExpand(bool flag)
{
    if(flag)
    {
        ui->wdgControlUI->setVisible(false);
        ui->pbtnUIExpand->setText("<<");
        //        qDebug()<<"size false"<<ui->wdgCurveParent->geometry();
        //        ui->wdgCurveParent->resize(QSize(this->size().width()-ui->pbtnUIExpand->width(),ui->wdgCurveParent->size().height()));
    }
    else
    {
        ui->wdgControlUI->setVisible(true);
        ui->pbtnUIExpand->setText(">>");
        //        qDebug()<<"size true"<<ui->wdgCurveParent->geometry();
        //        ui->wdgCurveParent->resize(QSize(this->size().width()-ui->pbtnUIExpand->width()-ui->wdgControlUI->size().width(),ui->wdgCurveParent->size().height()));
    }
}

void twoPlotForm::on_pbtnStop_clicked()
{
    emit stopMeasure();
}

void twoPlotForm::forgeCurve(void)
{

    QString constDistance("0,4.3,8.58,8.32,7.93,8.71,3.47,4.7,8.62,8.08,8.34,8.39,3.92,8.3,8.27,8.05,9.33,4.23,8.3,8.27,8.05,9.33,4.23,8.3,8.27,8.05,9.33,3.38,8.12,8.69,8.45,9.97,4.55,8.44,8.19,8.5,8.8,4.87,8.76,9.4,9.58,8.15,2.95");
    QString constWireHeights1("6000,6000,6000,6000,6000,6000,6000,6000,6000,6000,6000,6000,6050,6100,6200,6300,6500,6400,6300,6200,6100,6000,6000,6000,6000,6000,6000,6000,6000,6000,6000,6000,6050,6100,6200,6300,6500,6600,6700,6800,6900,7000,7100");
    QString constWireWidth1("200,200,200,200,200,200,200,200,100,0,-100,-250,-100,0,0,100,250,200,100,0,-100,-250,-200,-100,0,100,250,-200,-100,0,100,250,300,400,450,500,700,1000,1500,2000,2500,3000,3500");

    QString constWireHeights("7100,7000,6900,6800,6700,6600,6500,6400,6300,6200,6100,6000,6000,6000,6000,6000,6000,6000,6000,6000,6000,6000,6050,6100,6200,6300,6500,6400,6300,6200,6100,6000,6000,6000,6000,6000,6000,6000,6000,6000,6000,6000,6000");
    QString constWireWidths("3500,3000,2500,2000,1500,1000,700,600,500,400,300,250,200,100,0,-100,-250,-200,-100,0,100,250,200,100,0,-100,-250,-200,-100,0,100,-250,-200,-100,0,100,200,150,100,0,-100,-200,-300");


    int count=constWireHeights.split(",").count();
    double sumDistance=0;
    QVector<double> tmpWireWidth,tmpWireWidth1,tmpWireHeight,tmpWireHeight1,tmpDistance;
    qDebug()<<"tmp count"<<constDistance.split(",").count()<<constWireWidth1.split(",").count()<<constWireWidths.split(",").count();
    for(int i=0;i<count;i++)
    {
        tmpWireHeight<<constWireHeights.split(",").at(i).toDouble();
        tmpWireHeight1<<constWireHeights1.split(",").at(i).toDouble();
        tmpWireWidth<<constWireWidths.split(",").at(i).toDouble();
        tmpWireWidth1<<constWireWidth1.split(",").at(i).toDouble();

        //        tmpDistance<<i*20;
        sumDistance+=constDistance.split(",").at(i).toDouble();
        tmpDistance<<sumDistance;
        //        qDebug()<<"tmp value"<<i<<constDistance.split(",").at(i)<<constWireHeights.split(",").at(i)<<constWireHeights1.split(",").at(i);
    }
    m_drawWireWidth->graph(0)->addData(tmpDistance,tmpWireWidth);
    m_drawWireWidth->addGraph();
    m_drawWireWidth->graph(1)->addData(tmpDistance,tmpWireWidth1);
    m_drawWireWidth->replot();

    m_drawWireHeight->graph(0)->addData(tmpDistance,tmpWireHeight);
    m_drawWireHeight->addGraph();
    m_drawWireHeight->graph(1)->addData(tmpDistance,tmpWireHeight1);
    m_drawWireHeight->replot();
    //    qDebug()<<"space split"<<tmpDistance.count()<<tmpDistance<<constWireHeights<<tmpWireWidth;;
}

void twoPlotForm::on_pbtnSpeech_clicked()
{
    QString L=ui->lineWireWidth->text();
    QString H=ui->lineWireHeight->text();
    if(L.isEmpty())
        L="0";
    if(H.isEmpty())
        H="0";
    if(alarmSpeak.isFinished())
    {
        alarmSpeak.strSpeak=QString::fromWCharArray(L"拉出值%1导高%2").arg(L).arg(H);
        alarmSpeak.start();
    }
    else
    {
        qDebug()<<"speak is not finished,waiting...";
    }
}
void twoPlotForm::setAlarm1Visible(bool flag)
{
    if(m_widthOver1Up.curveIndex)
        m_drawWireWidth->graph(m_widthOver1Up.curveIndex)->setVisible(flag);
    if(m_heightOver1Up.curveIndex)
        m_drawWireHeight->graph(m_heightOver1Up.curveIndex)->setVisible(flag);
    if(m_widthOver1Down.curveIndex)
        m_drawWireWidth->graph(m_widthOver1Down.curveIndex)->setVisible(flag);
    if(m_heightOver1Down.curveIndex)
        m_drawWireHeight->graph(m_heightOver1Down.curveIndex)->setVisible(flag);
}
void twoPlotForm::setAlarm2Visible(bool flag)
{
    if(m_widthOver2Up.curveIndex)
        m_drawWireWidth->graph(m_widthOver2Up.curveIndex)->setVisible(flag);
    if(m_heightOver2Up.curveIndex)
        m_drawWireHeight->graph(m_heightOver2Up.curveIndex)->setVisible(flag);
    if(m_widthOver2Down.curveIndex)
        m_drawWireWidth->graph(m_widthOver2Down.curveIndex)->setVisible(flag);
    if(m_heightOver2Down.curveIndex)
        m_drawWireHeight->graph(m_heightOver2Down.curveIndex)->setVisible(flag);
}
//函数功能：设置杆号的显示或者隐藏
void twoPlotForm::setPoleVisible(bool flag)
{
    for(int i=0;i<widthPole.count();i++)
        widthPole.at(i)->setVisible(flag);
    for(int i=0;i<heightPole.count();i++)
        heightPole.at(i)->setVisible(flag);

    for(int i=0;i<poleCurves.count();i++)
    {
        m_drawWireHeight->graph(poleCurves.at(i).second.curveIndex)->setVisible(flag);
        m_drawWireWidth->graph(poleCurves.at(i).first.curveIndex)->setVisible(flag);
    }
}
//函数功能：设置接触线2的显示或者隐藏
void twoPlotForm::setWire2Visible(bool flag)
{
    //    qDebug()<<"wire2 counts="<<wire2Curves.count();
    for(int i=0;i<wire2Curves.count();i++)
    {
        m_drawWireHeight->graph(wire2Curves.at(i).second.curveIndex)->setVisible(flag);
        m_drawWireWidth->graph(wire2Curves.at(i).first.curveIndex)->setVisible(flag);
    }
}
void twoPlotForm::setCarrierVisible(bool flag)
{
    for(int i=0;i<carrierCurves.count();i++)
    {
        if(carrierCurves.at(i).first.curveIndex)
        {
            m_drawWireWidth->graph(carrierCurves.at(i).first.curveIndex)->setVisible(flag);
        }
        if(carrierCurves.at(i).second.curveIndex)
        {
            m_drawWireHeight->graph(carrierCurves.at(i).second.curveIndex)->setVisible(flag);
        }
    }
}
void twoPlotForm::setDropperVisible(bool flag)
{
    if( m_heightDropperCurve.curveIndex)
    {
        m_drawWireHeight->graph(m_heightDropperCurve.curveIndex)->setVisible(flag);
        errorBars->setVisible(flag);
    }
}
void twoPlotForm::slot_curveVisiable(curveType type,bool flag)
{
    switch (type)
    {
    //警示值曲线
    case curveType::Alarm1:
    {
        bAlarm1Display=flag;
        setAlarm1Visible(flag);
        break;
    }
        //界限值曲线
    case curveType::Alarm2:
    {
        setAlarm2Visible(flag);
        break;
    }
        //吊弦
    case curveType::Dropper:
    {
        setDropperVisible(flag);
        break;
    }
        //承力索
    case curveType::Carrier:
    {
        setCarrierVisible(flag);

        break;
    }
        //杆号
    case curveType::pole:
    {
        setPoleVisible( flag);
        break;
    }
    case curveType::wire2:
    {
        setWire2Visible( flag);
        break;
    }
    }
    m_drawWireHeight->replot();
    m_drawWireWidth->replot();
}
void twoPlotForm::slot_overParChange(void)
{
    updateOverPar();
}
void twoPlotForm::updateOverPar(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    //    qDebug()<<"over chanage";
    //定点拍摄时，距离支柱的距离
    //载入超限默认参数
    overHeightMax=settings.value("OverRunPar/MaxWireHeight").toInt();
    overHeightMin=settings.value("OverRunPar/MinWireHeight").toInt();
    overWidthMax=settings.value("OverRunPar/MaxWireWidth").toInt();
    overWidthMin=settings.value("OverRunPar/MinWireWidth").toInt();
    m_heightOver1Up.y.clear();
    m_heightOver1Down.y.clear();
    m_heightOver1Down.x.clear();
    //    m_heightOver1Down.y.fill(overHeightMin,m_heightOver1Up.x.count());
    //    m_heightOver1Up.y.fill(overHeightMax,m_heightOver1Up.x.count());

    m_widthOver1Up.y.clear();
    m_widthOver1Down.y.clear();
    m_widthOver1Down.x.clear();
    //    m_widthOver1Down.y.fill(overWidthMax,m_widthOver1Down.x.count());
    //    m_widthOver2Down.y.fill(overWidthMin,m_widthOver1Down.x.count());

    drawAlarmCurve(m_drawWireHeight->xAxis->range().upper,true);
    drawAlarmCurve(m_drawWireHeight->xAxis->range().lower,true);
    drawAlarmCurve(m_drawWireHeight->xAxis->range().upper,false);
    drawAlarmCurve(m_drawWireHeight->xAxis->range().lower,false);
}
void twoPlotForm::slot_dropperData(QPair<int ,QVector<double> > dropper)
{
    //1吊弦绘制
    for(int i=0;i<dropper.second.count();i++)
    {
        m_heightDropperCurve.x<<dropper.second.at(i);
        m_heightDropperCurve.y<<dropper.first;//.nWalkDistance;
        rBars<<100;
        errorBars->setData(rBars);
    }
    qDebug()<<"dropper value"<<dropper.first<<dropper.second;
    m_drawWireHeight->graph(m_heightDropperCurve.curveIndex)->setData(m_heightDropperCurve.x,m_heightDropperCurve.y);
}
void twoPlotForm::slot_mouseMove(QMouseEvent *e)
{
    QCustomPlot *plot=(QCustomPlot *)this->sender();
    if(plot==m_drawWireHeight)
    {
        updateTipCurve(m_drawWireHeight,m_HeightToolTip,e);
    }
    else if(plot==m_drawWireWidth)
    {
        updateTipCurve(m_drawWireWidth,m_widthToolTip,e);
    }
}
//函数功能:用于鼠标停留时,显示当前的里程,导高或拉出值
void twoPlotForm::updateTipCurve(QCustomPlot *plot,curveInfo &curve,QMouseEvent *e)
{
    double x=plot->xAxis->pixelToCoord(e->pos().x());
    double y=plot->yAxis->pixelToCoord(e->pos().y());
    plot->setToolTip(QString("%1 %2").arg(x).arg(y));
    curve.clear();
    curve.x<<x;
    curve.x<<x;
    curve.y<<plot->yAxis->range().lower;
    curve.y<<plot->yAxis->range().upper;
    plot->graph(curve.curveIndex)->setData(curve.x,curve.y);
    plot->replot();
}
void twoPlotForm::clearTipCurve(QCustomPlot *plot,curveInfo &curve)
{
    curve.clear();
    plot->graph(curve.curveIndex)->setData(curve.x,curve.y);
    plot->replot();
}
void twoPlotForm::slot_mouseRelease(QEvent *e)
{
    if(e->type()==QEvent::Leave)
    {
        //        qDebug()<<"mouse release";
        QCustomPlot *plot=(QCustomPlot *)this->sender();
        if(plot==m_drawWireHeight)
        {
            clearTipCurve(m_drawWireHeight,m_HeightToolTip);
        }
        else if(plot==m_drawWireWidth)
        {
            clearTipCurve(m_drawWireWidth,m_widthToolTip);
        }
    }
}
void twoPlotForm::slot_endWire2(void)
{
    emit endWire2();
    addWire2Curve();
}
void twoPlotForm::addWire2Curve(void)
{
    wire2Curves<<addWireCurve(wire2Color);
    //    qDebug()<<"create another wire2 curve,cur curve count="<<wire2Curves.count();
    if(wire2Curves.count())
        curWire2Index=wire2Curves.count()-1;
}
void twoPlotForm::slot_createWire1Curve(void)
{
    wire1Curves<<addWireCurve(wire1Color);
    qDebug()<<"create another wire1 curve,cur curve count="<<wire1Curves.count();
    if(wire1Curves.count())
        curWire1Index=wire1Curves.count()-1;
}
void twoPlotForm::slot_createCarrier(void)
{
    carrierCurves<<addWireCurve(carrierColor);
    if(carrierCurves.count())
        curCarrierIndex=carrierCurves.count()-1;
    //    qDebug()<<"created another curve,cur carrier count="<<carrierCurves.count();
}

void twoPlotForm::on_chkErrorPole_clicked()
{
    if(ui->chkErrorPole->checkState()==Qt::Checked)
    {
        //        qDebug()<<"checked";
        emit errorPole(true);
    }
    else if(ui->chkErrorPole->checkState()==Qt::Unchecked)
    {
        //        qDebug()<<"unchecked";
        emit errorPole(false);
    }
}
void twoPlotForm::slot_clearCurve(void)
{
    clearCurves();
    curWire1Index=0;
}
void twoPlotForm::updatePoleNumber(QString tmp)
{
    updateNumber(tmp);
}
void twoPlotForm::updateDrawUI(void)
{
    int pH=ui->wdgCurveParent->size().height();
    int pW=ui->wdgCurveParent->size().width();
    ui->wdgHegiht->move(0,0);
    ui->wdgHegiht->resize(QSize(pW,pH*0.5));
    ui->wdgWidth->resize(QSize(pW,pH*0.5));
    ui->wdgWidth->move(0,pH/2);
    //    qDebug()<<"curve size"<<this->size()<<"curve parent="<<ui->wdgCurveParent->size()<<"height="<<ui->wdgHegiht->geometry()<<"width="<<ui->wdgWidth->geometry();
    update();
}
bool  twoPlotForm::eventFilter(QObject *o, QEvent *e )
{
    if(e->type()==QEvent::Resize)
    {
        if(o==ui->wdgCurveParent)
        {
            updateDrawUI();
        }
        if(o==ui->stackPoleUI)
        {
            qDebug()<<"pole control ui size"<<ui->stackPoleUI->size()<<ui->stackPoleUI->pos();
        }
    }
    return true;
}
void twoPlotForm::fillPoleSelList(int id )
{
    qDebug()<<"cur idx= "<<id;
    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
    m_currIdx=id;
    int cnt = 0;
    clearOldPoleDipslay();
    //    qDebug()<<"`````````````the id = "<<id;
#ifdef GUIYANG_TUICHE
    for (int i = id-5; i < id+6 && i < polesets.size(); i++)
#else
    for (int i = id-2; i < id+3 && i < polesets.size(); i++)
#endif
    {
        if (i < 0)
            continue;
        curDisplayPoles.append(polesets[i].poleID);
        ui->listViewPolelist->addItem(polesets[i].poleID);
        if (id == i)
        {
            ui->listViewPolelist->item(cnt)->setBackground(QBrush(QColor(0,255,0)));
            curDisplayPole=ui->listViewPolelist->item(cnt)->text();
//            qDebug()<<"the cnt = "<<cnt<<curDisplayPole;
        }
        cnt++;
    }
//    emit passedPoleId(polesets[m_currIdx].poleID);
}
void twoPlotForm::slots_railPoleClicked(QListWidgetItem*item)
{
    QString toPole = item->text();
    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
    //labelPoleInfo->setText(item->text());
    ConFirmPoleChangeDialog dlg(polesets[m_currIdx].poleID, toPole, this);
//    connect(this, SIGNAL(passedPoleId(QString)), &dlg, SLOT(changePole(QString)));
    if (dlg.exec())
    {
//        qDebug()<<" I should change !"<<m_currIdx;
        slot_changePoleId( toPole);
        emit changingPole(QString::number(m_currIdx));
    }
}
void twoPlotForm::slot_changePoleId(QString toPole)
{
    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
    int minval = (m_currIdx-20) > 0 ? (m_currIdx-20) : 0;
    qDebug()<<"before currid"<<m_currIdx;
    for (int i = minval; i < m_currIdx+20 && i < polesets.size(); i++)
    {
        if (polesets[i].poleID == toPole)
        {
            if (i != m_currIdx)
            {
                m_currIdx = i;
                Global::getInstance()->changedPos.push_back(walkDistance );
                qDebug()<<"changed pos: "<<walkDistance <<m_currIdx;
                //stationChanged();
                fillPoleSelList(m_currIdx);
                return;
            }
            break;
        }
    }
}
void twoPlotForm::clearOldPoleDipslay(void)
{
    curDisplayPoles.clear();
    ui->listViewPolelist->clear();
}
void twoPlotForm::updateNumber(QString sPoleNumber)
{
     ui->wdgPoleNum->setValue(sPoleNumber);
//     MeasureDB::nPoleNum=sPoleNumber.toInt();
}
void twoPlotForm::newRailInitialUI(bool b)
{
    ui->wdgPoleNum->setEnabled(b);
    ui->pbtnAddPole->setEnabled(b);
    ui->rdbAdd->setEnabled(b);
    ui->rdbReduce->setEnabled(b);
//    ui->label->setEnabled(b);
}
void twoPlotForm::on_pbtnAddPole_clicked()
{
    poleChanged();
}
void twoPlotForm::slot_newPole(void)
{
    poleChanged();
}
void twoPlotForm::poleChanged(void)
{
//    int tmp=ui->wdgPoleNum->getValue().toInt();
//    if(ui->rdbAdd->isChecked())
//    {
//        tmp+=2;
//    }
//    else
//    {
//        if(tmp>1)
//            tmp-=2;
//        else
//            tmp=0;
//    }
//    updateNumber(QString::number(tmp));
    //    addDisplayPoleNum(QString::number(tmp),(curDistance),true);

    emit newRailPole(ui->wdgPoleNum->getValue());
}
void twoPlotForm::slot_poleDirection(bool b)
{
    if(ui->rdbAdd->isChecked())
    {
        MeasureDB::bPoleDir=true;
    }
    else
    {
        MeasureDB::bPoleDir=false;
    }
}
//函数功能:用于修改接触线1,接触线2及杆号的颜色
void twoPlotForm::updatePairWireColor(QVector<QPair<curveInfo,curveInfo> >  &wireCurves,QColor color)
{
    for(int i=0;i<wireCurves.count();i++)
    {
        QPair<curveInfo,curveInfo> tmp=wireCurves.at(i);
        tmp.first.color=color;
        tmp.second.color=color;
        wireCurves.replace(i,tmp);
        //        qDebug()<<"graph count"<<m_drawWireWidth->graphCount()<<m_drawWireHeight->graphCount()<<i<<wireCurves.at(i).second.curveIndex<<wireCurves.at(i).first.curveIndex;
        QPen  myPen= m_drawWireWidth->graph(wireCurves.at(i).first.curveIndex)->pen();
        myPen.setColor(tmp.second.color);
        m_drawWireWidth->graph(wireCurves.at(i).first.curveIndex)->setPen(myPen);
        m_drawWireHeight->graph(wireCurves.at(i).second.curveIndex)->setPen(myPen);
    }
}
void twoPlotForm::updatePoleTextColor(QColor color)
{
    for(int i=0;i<widthPole.count();i++)
    {
        QCPItemText *tmp=widthPole.at(i);
        tmp->setColor(color);
        widthPole.replace(i,tmp);//
    }
    for(int i=0;i<heightPole.count();i++)
    {
        QCPItemText *tmp=heightPole.at(i);
        tmp->setColor(color);
        heightPole.replace(i,tmp);//
    }
}
void twoPlotForm::updateDropperColor(QColor color)
{
    QPen tmpPen=m_drawWireHeight->graph(m_heightDropperCurve.curveIndex)->pen();
    tmpPen.setColor(color);
    m_drawWireHeight->graph(m_heightDropperCurve.curveIndex)->setPen(tmpPen);
}
void twoPlotForm::setLableColor(QLabel *lbl,QColor color)
{
    QString cc=QString("background:rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue());
    lbl->setStyleSheet(cc);
}
