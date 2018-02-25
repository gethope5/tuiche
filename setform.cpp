#include "setform.h"
#include "ui_setform.h"
#include "rador/radorscannermodule.h"
#include <QDebug>
setForm::setForm(QWidget *parent) :
    QWidget(parent)
  ,ui(new Ui::setForm)
  ,m_bMeasureType(true)
  ,m_bpower(false)
  ,bFirstPower(true)
  ,bRadarPower(false)
  ,bUIExpand(false)
  ,bLedStatus(false)
  ,bWIFI(false)
{
    ui->setupUi(this);
    uiInitial();
    curAngleTcpStatus=false;
    getPar();
    //    ui->wdgSideConset->setCustomLabel(".");
    //    ui->wdgSideDis->setCustomLabel(".");

    connect(ui->rdbRealMeasure, SIGNAL(clicked()),this, SLOT(slot_measureType()));
    connect(ui->rdbSimulate, SIGNAL(clicked()),this, SLOT(slot_measureType()));
    if(ui->rdbRealMeasure->isChecked())
    {
        m_bMeasureType=true;
    }
    else
    {
        m_bMeasureType=false;
    }
//    ui->groupBox->setVisible(false);
    ui->grbNotVisible->setVisible(false);
    setPbtnUI(ui->pbtnPowerOn,false);//显示雷达关闭按钮
    setPbtnUI(ui->pbtnUIExpand,false);
    setPbtnUI(ui->pbtnCurveSyn,true);
    setPbtnUI(ui->pbtnOverAlarm,false);
    setPbtnUI(ui->pbtnDeviceConnect,false);
    setPbtnUI(ui->pbtnLED,false);
    ui->groupBox_3->setVisible(false);
    //    ui->pbtnDeviceConnect->initialPbtnUI("./images/radarcon-focus.png","./images/radarcon.png");
}
void setForm::slot_updateDeviceInfo(deviceInfo di )
{
    ui->lblDianlliang->setText(di.dianliang);
    ui->lblGPSJ->setText(di.GPSJ);
    ui->lblGPSW->setText(di.GPSW);
    ui->lblW->setText(di.wAndS);
}
//函数功能：用于雷达首次上电后，发送网络自动连接信号
void setForm::radorFirstPower(void)
{
    emit conDevice();
}
//函数功能：用于控制雷达指示按钮，flag=on,雷达已经打开；off,雷达已经关闭
void setForm::updateRadorStatus(bool flag)
{
    m_bpower=flag;
    if(flag)
    {
        ui->pbtnPowerOn->setToolTip(QTextCodec::codecForName("gbk")->toUnicode("单击关闭雷达..."));
        if(bFirstPower)
        {
            bFirstPower=false;
            QTimer::singleShot(1000,this,SLOT(radorFirstPower()));
        }
        setPbtnUI(ui->pbtnPowerOn,true);
    }
    else
    {
        ui->pbtnPowerOn->setToolTip(QTextCodec::codecForName("gbk")->toUnicode("单击打开雷达..."));
        bFirstPower=true;
        //        雷达电源关闭后，初始化
        setPbtnUI(ui->pbtnDeviceConnect,false);
        setPbtnUI(ui->pbtnPowerOn,false);
        MeasureDB::bRadarStatus=false;
//        MeasureDB::bSimDraw=true;
    }
}
void setForm::updateWIFIStatus(bool b)
{
    bWIFI=b;
    setPbtnUI(ui->pbtnCurveSyn,b);
}
void setForm::slot_deviceControl(void)
{
    QPushButton *pbtn=(QPushButton *)this->sender();
    //雷达供电控制按钮
    if(pbtn==ui->pbtnPowerOn)
    {
        if(m_bpower)
        {
            if(QMessageBox::critical(this,QTextCodec::codecForName("gbk")->toUnicode("设置..."),QTextCodec::codecForName("gbk")->toUnicode("关闭雷达电源，进入待机模式?"),QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)==QMessageBox::Ok)
            {
                emit radorAndWIFIControl(CLOSE_RADAR);
            }
            qDebug()<<"close radar";
            //            m_bpower=false;
        }
        else
        {
            qDebug()<<"open radar";
            emit radorAndWIFIControl(OPEN_RADAR);
            //            m_bpower=true;
        }
    }
}
void setForm::slot_measureType(void)
{
    QRadioButton *rdb=(QRadioButton *)this->sender();
    QString strMeasureType;
    if(ui->rdbRealMeasure->isChecked())
        strMeasureType=QTextCodec::codecForName("gbk")->toUnicode("实时测量");
    else if(ui->rdbSimulate->isChecked())
        strMeasureType=QTextCodec::codecForName("gbk")->toUnicode("模拟测量");
    //    information ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
    if(QMessageBox::critical(this,QTextCodec::codecForName("gbk")->toUnicode("设置测量模式..."),QTextCodec::codecForName("gbk")->toUnicode("确认将测量模式更改为")+strMeasureType+"?",QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)==QMessageBox::Ok)
    {
        //    bool bTmp;
        if(rdb==ui->rdbRealMeasure)
        {
            m_bMeasureType=true;
            //qDebug()<<"real measure ";
        }
        else if(rdb==ui->rdbSimulate)
        {
            m_bMeasureType=false;
            //        qDebug()<<"simulate";
        }
        QSettings  settings("Config.ini", QSettings::IniFormat);
        //1测量初始参数设置
        settings.setValue("system/measureType", m_bMeasureType);
        emit measureType(m_bMeasureType);
    }
    else
    {
        if(strMeasureType.contains(QTextCodec::codecForName("gbk")->toUnicode("实时")))
        {
            ui->rdbSimulate->setChecked(true);
        }
        else if(strMeasureType.contains(QTextCodec::codecForName("gbk")->toUnicode("模拟")))
        {
            ui->rdbRealMeasure->setChecked(true);
        }
    }
}
//函数功能：界面初始化
void setForm::uiInitial(void)
{
#if 0
    ui->wdgStdHeight->setLblName(QTextCodec::codecForName("gbk")->toUnicode("高度："));
    ui->wdgStdWidth->setLblName(QTextCodec::codecForName("gbk")->toUnicode("偏移："));
    ui->wdgInitialRailX->setLblName(QTextCodec::codecForName("gbk")->toUnicode("轨距："));
    ui->wdgInitialRailY->setLblName(QTextCodec::codecForName("gbk")->toUnicode("超高："));
#else
    ui->wdgGrade->setLblName(QTextCodec::codecForName("gbk")->toUnicode("坡度："));
//    ui->wdgInitialRailX->setLblName(QTextCodec::codecForName("gbk")->toUnicode("轨距："));
//    ui->wdgInitialRailY->setLblName(QTextCodec::codecForName("gbk")->toUnicode("超高："));
#endif
    ui->wdgMaxWireHeight->setLblName(QTextCodec::codecForName("gbk")->toUnicode("最大导高："));
    ui->wdgMinWireHeight->setLblName(QTextCodec::codecForName("gbk")->toUnicode("最小导高："));
    ui->wdgMaxWireWidth->setLblName(QTextCodec::codecForName("gbk")->toUnicode("最大拉出值："));
    ui->wdgMinWireWidth->setLblName(QTextCodec::codecForName("gbk")->toUnicode("最小拉出值："));

    ui->wdgStructureHeight->setLblName(QTextCodec::codecForName("gbk")->toUnicode("结构高度："));
    //    ui->wdgMinWireWidth->setLblName(QTextCodec::codecForName("gbk")->toUnicode("最小拉出值："));


    //    ui->wdgSideConset->setLblName(QTextCodec::codecForName("gbk")->toUnicode("初始位置:"));
    //    ui->wdgSideDis->setLblName(QTextCodec::codecForName("gbk")->toUnicode("测量距离"));
    //    ui->grbCurStatus->setStyleSheet("QLineEdit{background-color: rgb(85, 255, 255);}");

    //    ui->lneHeat->setEnabled(false);
    //    ui->lneRador->setEnabled(false);
    connect(ui->pbtnPowerOn,SIGNAL(clicked()),this,SLOT(slot_deviceControl()));
    //    ui->lneRador->setStyleSheet("color: rgb(0, 0, 0);font: 13pt 900 \"微软雅黑\";background-color: rgb(85, 255, 255);");
    //    this->setStyleSheet("QLineEdit{background-color: rgb(85, 255, 255)}");

}
void setForm::slot_meassureStauts(bool flag)
{
    m_bMeasureStatus=flag;
}
setForm::~setForm()
{
    delete ui;
}
//function：初始阶段，获取配置文件中的标准参数，并显示在对应的ui中
void setForm::getPar(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    //定点拍摄时，距离支柱的距离
    //载入超限默认参数
    ui->wdgMaxWireHeight->setValue(settings.value("OverRunPar/MaxWireHeight").toString());
    ui->wdgMinWireHeight->setValue(settings.value("OverRunPar/MinWireHeight").toString());

    ui->wdgMaxWireWidth->setValue(settings.value("OverRunPar/MaxWireWidth").toString());
    ui->wdgMinWireWidth->setValue(settings.value("OverRunPar/MinWireWidth").toString());
    ui->wdgGrade->setValue(settings.value("initialPar/alarmSlop").toString());
    //    m_nDirect=settings.value("system/directflag", 0).toInt();
    m_nExpand=settings.value("system/expandFlag", 0).toInt();

    //    qDebug()<<"initial direct flag="<<m_nDirect<<"expand direct flag="<<m_nExpand;
    //侧面界限
    //    ui->wdgSideDis->setValue(settings.value("initialPar/sideDisConst").toString());
    //    ui->wdgSideConset->setValue(settings.value("initialPar/sideWidthConst").toString());
    m_bMeasureType=settings.value("system/measureType").toBool();
    if(m_bMeasureType)
    {
        ui->rdbRealMeasure->setChecked(true);
        ui->rdbSimulate->setChecked(false);
    }
    else
    {
        ui->rdbRealMeasure->setChecked(false);
        ui->rdbSimulate->setChecked(true);
    }
    emit baseParChanged();
}
void setForm::slot_updateIPDeviceStatus(QString buffer)
{
    //    qDebug()<<"device status"<<buffer;
    if(buffer.contains( "Radar_Open"))
    {
        //        ui->lneRador->setText(QTextCodec::codecForName("gbk")->toUnicode("打开"));
        ui->pbtnPowerOn->setIconSize(ui->pbtnPowerOn->size());
        ui->pbtnPowerOn->setIcon(QIcon(QPixmap::fromImage(QImage("./images/poweron.png"))));
    }
    if(buffer.contains( "Radar_Close"))
    {
        //        ui->lneRador->setText(QTextCodec::codecForName("gbk")->toUnicode("关闭"));
        ui->pbtnPowerOn->setIconSize(ui->pbtnPowerOn->size());
        ui->pbtnPowerOn->setIcon(QIcon(QPixmap::fromImage(QImage("./images/poweroff.png"))));
    }
    if(buffer.contains( "All_Close"))
    {
        //        ui->lneRador->setText(QTextCodec::codecForName("gbk")->toUnicode("关闭"));
        //ui->lneHeat->setText(QTextCodec::codecForName("gbk")->toUnicode("关闭"));
    }
}
void setForm::on_pbtnDeviceConnect_clicked()
{
    setPbtnUI(ui->pbtnDeviceConnect,false);
    emit conDevice();
}
//函数功能：雷达指示按钮，指示当前雷达上电状态
void setForm::slot_radorConnect(DEVICE_CONNECT_STATUS flag)
{
    qDebug()<<"rador is ok++++++++++++++++++";
    if(flag==DISCONNECTED)
    {
        setPbtnUI(ui->pbtnDeviceConnect,false);
    }
    else if(flag==CONNECTED)
    {
        setPbtnUI(ui->pbtnDeviceConnect,true);

    }
}

void setForm::on_pbtnUIExpand_clicked()
{
    if(bUIExpand)
    {
        setPbtnUI(ui->pbtnUIExpand,false);
        emit  expandFlag(true);
    }
    else
    {
        setPbtnUI(ui->pbtnUIExpand,true);
        emit  expandFlag(false);
    }
    bUIExpand=!bUIExpand;
}
void setForm::setPbtnUI(QPushButton *pbtn,bool flag)
{
    pbtn->setText("");
    pbtn->setIconSize(pbtn->size());
    pbtn->setStyleSheet("border:0px;");

    if(flag)
    {
        QPixmap dd("./images/poweron.png");
        pbtn->setIcon(QIcon(dd.scaled(pbtn->size())));
    }
    else
    {
        QPixmap dd("./images/poweroff.png");
        pbtn->setIcon(QIcon(dd.scaled(pbtn->size())));
    }
}
void setForm::on_pbtnLED_clicked()
{
    if(bLedStatus)
    {
        setPbtnUI(ui->pbtnLED,true);
        emit railComControl(CLOSE_LED);
        qDebug()<<"close led command";
    }
    else
    {
        qDebug()<<"open led command";

        setPbtnUI(ui->pbtnLED,false);
        emit railComControl(OPEN_LED);
    }
}
void setForm::slot_comReplay(COM_REPLAY_TYPE dd)
{
//    qDebug()<<"see";
    switch (dd)
    {
    case LED_OPENED:
    {
        updateLEDStatus(true);
        break;
    }
    case LED_CLOSED:
    {
        updateLEDStatus(false);
        break;
    }
    case DATA_OPENED:
    {
        break;
    }
    case DATA_CLOSED:
    {
        break;
    }
    case WALK_CLEARED:
    {
        break;
    }
    case RADAR_OPENED:
    {
        updateRadorStatus(true);
        break;
    }
    case RADAR_CLOSED:
    {
        updateRadorStatus(false);
        emit radorPowerOff();
        break;
    }
    case WIFI_OPENED:
    {
        updateWIFIStatus(true);
        break;
    }
    case WIFI_CLOSED:
    {
        updateWIFIStatus(false);
        break;
    }

    }
}
void setForm::updateLEDStatus(bool f)
{
    qDebug()<<"led status"<<f;
    setPbtnUI(ui->pbtnLED,f);
    bLedStatus=f;
}
void setForm::on_pbtnCurveSyn_clicked()
{
    qDebug()<<"bWifi status"<<bWIFI;
    if(bWIFI)
    {
        emit radorAndWIFIControl(CLOSE_WIFI);
    }
    else
        emit radorAndWIFIControl(OPEN_WIFI);
}
void setForm::on_pbtnOverAlarm_clicked()
{
    if(MeasureDB::overAlarm)
    {
        setPbtnUI(ui->pbtnOverAlarm,false);
        MeasureDB::overAlarm=false;
    }
    else
    {
        setPbtnUI(ui->pbtnOverAlarm,true);
        MeasureDB::overAlarm=true;
    }
}
void setForm::on_pbtnOverUpdate_clicked()
{
    if(QMessageBox::question(this,QTextCodec::codecForName("gbk")->toUnicode("提示..."),QTextCodec::codecForName("gbk")->toUnicode("确定更新预警参数?"),QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok);
    {
        QSettings  settings("Config.ini", QSettings::IniFormat);
        //定点拍摄时，距离支柱的距离
        //载入超限默认参数
        settings.setValue("OverRunPar/MaxWireHeight",ui->wdgMaxWireHeight->getValue());
        settings.setValue("OverRunPar/MinWireHeight",ui->wdgMinWireHeight->getValue());
        settings.setValue("OverRunPar/MaxWireWidth",ui->wdgMaxWireWidth->getValue());
        settings.setValue("OverRunPar/MinWireWidth",ui->wdgMinWireWidth->getValue());
        settings.setValue("initialPar/alarmSlope",ui->wdgGrade->getValue());
        emit baseParChanged();
    }

}
