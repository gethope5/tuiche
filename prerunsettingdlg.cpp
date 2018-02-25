#include "prerunsettingdlg.h"
#include "ui_prerunsettingdlg.h"
#include "database.h"
#include "global.h"

static const char * defaultpath = "c:/tieren";


PreRunSettingDlg::PreRunSettingDlg(QString title,QWidget *parent):QDialog(parent),ui(new Ui::PreRunSettingDlg)
  ,sxTableName("")
  ,xxTableName("")
{
    ui->setupUi(this);
    profile.lineName=("");
    profile.tableName=("");
    setWindowTitle(title);
    readConfigure(); //set m_datapath
    uiMeasure();
    uiVerify();
    setPreRunSetting();
    fileName();
}
void PreRunSettingDlg::uiMeasure(void)
{
    /* modified 2012-05-11 */
    ui->lblFilePath->setText(m_datapath);
    //main ui
    connect(ui->rdbNewRail,SIGNAL(clicked(bool)),this,SLOT(slot_tabWidget(bool)));
    //老线路单选按钮
    connect(ui->rdbVerify,SIGNAL(clicked(bool)),this,SLOT(slot_tabWidget(bool)));
    //系统确定按钮
    connect(ui->pbtnEnter, SIGNAL(clicked()), this, SLOT(setBackData()));
    //系统取消按钮
    connect(ui->pbtnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    //系统设置数据库存放路径
    connect(ui->pbtnSelPath, SIGNAL(clicked()), this, SLOT(setSavePath()));

    ui->rdbNewRail->setChecked(true);
    ui->pbtnEnter->setText(QString::fromWCharArray(L"测量"));
    ui->pbtnSelPath->setVisible(true);
    setGroupEnable(false);
    //page 0
    ui->cmbDirection->addItem(QTextCodec::codecForName("gbk")->toUnicode("上行"));
    ui->cmbDirection->addItem(QTextCodec::codecForName("gbk")->toUnicode("下行"));
    ui->spbStationName->setPrefix(QTextCodec::codecForName("gbk")->toUnicode("站区"));
    ui->spbStationName->setMinimum(1);

    //page 2
    ui->wdgStartKm->setValue("20.0");
    ui->wdgPoleNum->setValue("138");
    ui->wdgStartKm->setLblName(QString::fromWCharArray(L"公里标"));
    ui->wdgStartKm->setCustomLabel(".");
    ui->wdgPoleNum->setLblName(QString::fromWCharArray(L"杆  号"));//    ui->lneName->setText(QTextCodec::codecForName("gbk")->toUnicode("临时1"));
    //    ui->wdgStartKm->setLineWidth(70,70);
    //    ui->wdgPoleNum->setLineWidth(60,60);
    //将page0的cmbDirection、文件名路径移植到page2上
    ui->cmbDirection->setParent(ui->stackedWidget->widget(2));
    ui->cmbDirection->resize(101,30);
    ui->cmbDirection->move(310,10);
    connect(ui->cmbDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_railDirectionChanged(int)));
    connect(ui->lwStation, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_railStationChanged(QListWidgetItem*)));
    connect(ui->lwPole, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_railPoleChanged(QListWidgetItem*)));
    //    connect(ui->pbtnAddMore, SIGNAL(clicked()), this, SLOT(adjustPos()));
    //    connect(ui->pbtnAddLess, SIGNAL(clicked()), this, SLOT(adjustPos()));
    //    connect(ui->pbtnSubMore, SIGNAL(clicked()), this, SLOT(adjustPos()));
    //    connect(ui->pbtnSubLess, SIGNAL(clicked()), this, SLOT(adjustPos()));


    //    ui->label_7->setParent(ui->stackedWidget->widget(2));
    //    ui->label_7->resize(41,28);
    //    ui->label_7->move(20,250);

    //    ui->lblFileName->setParent(ui->stackedWidget->widget(2));
    //    ui->lblFileName->resize(401,41);
    //    ui->lblFileName->move(60,250);

    //    ui->label_9->setParent(ui->stackedWidget->widget(2));
    //    ui->label_9->resize(41,28);
    //    ui->label_9->move(20,280);

    //    ui->lblFilePath->setParent(ui->stackedWidget->widget(2));
    //    ui->lblFilePath->resize(401,25);
    //    ui->lblFilePath->move(60,290);

    ui->stackedWidget->setCurrentIndex(2);
#ifdef GUIYANG_TUICHE
#else
    this->resize(491,403);

    ui->stackedWidget->resize(481,311);
    ui->stackedWidget->move(10,25);

    //轨距、超高校验
    ui->grbRail->setParent(ui->stackedWidget->widget(1));
    ui->grbRail->resize(471,301);
    ui->grbRail->move(11,5);

    ui->label_3->setParent(ui->stackedWidget->widget(0));
    ui->label_3->resize(80,30);
    ui->label_3->move(31,31);
#if 0
    ui->cmbDirection->setParent(ui->stackedWidget->widget(0));
    ui->cmbDirection->resize(171,35);
    ui->cmbDirection->move(110,30);
    ui->wdgStartKm->setParent(ui->stackedWidget->widget(0));
    ui->wdgStartKm->resize(261,41);
    ui->wdgStartKm->move(20,170);
    ui->lblFileName->setParent(ui->stackedWidget->widget(0));
    ui->lblFileName->resize(401,41);
    ui->lblFileName->move(50,220);
    ui->label_9->setParent(ui->stackedWidget->widget(0));
    ui->label_9->resize(41,28);
    ui->label_9->move(10,250);
    ui->label_7->setParent(ui->stackedWidget->widget(0));
    ui->label_7->resize(41,28);
    ui->label_7->move(10,220);
    ui->lblFilePath->setParent(ui->stackedWidget->widget(0));
    ui->lblFilePath->resize(401,25);
    ui->lblFilePath->move(50,260);
    ui->spbStationName->setParent(ui->stackedWidget->widget(0));
    ui->spbStationName->resize(171,35);
    ui->spbStationName->move(110,80);
#endif

    ui->label_4->setParent(ui->stackedWidget->widget(0));
    ui->label_4->resize(71,41);
    ui->label_4->move(30,70);


    initialNewRailPole();

    ui->chbDirect->setParent(ui->stackedWidget->widget(0));
    ui->chbDirect->resize(111,31);
    ui->chbDirect->move(310,140);

    ui->checkBox->setParent(ui->stackedWidget->widget(0));
    ui->checkBox->resize(101,51);
    ui->checkBox->move(310,40);

    ui->pbtnSelPath->setParent(this);
    ui->pbtnSelPath->resize(91,41);
    ui->pbtnSelPath->move(20,340);

    ui->pbtnCancel->setParent(this);
    ui->pbtnCancel->resize(91,41);
    ui->pbtnCancel->move(400,340);

    ui->pbtnEnter->setParent(this);
    ui->pbtnEnter->resize(91,41);
    ui->pbtnEnter->move(290,340);
#endif
}
void PreRunSettingDlg::initialNewRailPole(void)
{
    //    ui->wdgPoleNum->setParent(ui->stackedWidget->widget(0));
    //    ui->wdgPoleNum->resize(261,41);
    //    ui->wdgPoleNum->move(20,120);
}
void PreRunSettingDlg::uiVerify(void)
{
    ui->wdgStdHeight->setLblName(QTextCodec::codecForName("gbk")->toUnicode("导高值"));
    ui->wdgStdWidth->setLblName(QTextCodec::codecForName("gbk")->toUnicode("拉出值"));
    ui->wdgRailHeight->setLblName(QTextCodec::codecForName("gbk")->toUnicode("超高"));
    ui->wdgRailWidth->setLblName(QTextCodec::codecForName("gbk")->toUnicode("轨距"));
    ui->wdgStdSide->setLblName(QTextCodec::codecForName("gbk")->toUnicode("界限值"));
    ui->wdgRailHeight->setCustomLabel(".");
    ui->wdgRailWidth->setCustomLabel(".");
    //    ui->wdgGrade->setLblName(QTextCodec::codecForName("gbk")->toUnicode("坡度："));
    //    ui->chbSideLimt->setChecked(true);
    ui->grbWire->setChecked(true);
    ui->grbWire->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    ui->grbRail->setChecked(false);
    ui->grbRail->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    ui->grbSide->setChecked(false);
    ui->grbSide->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(ui->grbSide,SIGNAL(toggled(bool)),this,SLOT(slot_verifyType(bool)));
    connect(ui->grbWire,SIGNAL(toggled(bool)),this,SLOT(slot_verifyType(bool)));
    connect(ui->grbRail,SIGNAL(toggled(bool)),this,SLOT(slot_verifyType(bool)));

#if GUIYANG_TUICHE
    //检测车拉出值导高标定UI初始化
    ui->wdgStdHeight->resize(370,50);
    ui->wdgStdWidth->resize(370,50);
    ui->wdgStdHeight->setParent(ui->stackedWidget->widget(1));
    ui->wdgStdWidth->setParent(ui->stackedWidget->widget(1));
    ui->wdgStdWidth->move(40,60);
    ui->wdgStdHeight->move(40,150);
    ui->grbRail->setVisible(false);
    ui->grbSide->setVisible(false);
    ui->grbWire->setVisible(false);

    ui->lblLatest_2->move(50,200);
    ui->lblLatest->move(140,220);
#else
    //推车标定UI初始化（拉出值、导高、轨距、超高及侧面界限）
    ui->grbRail->setVisible(true);
    ui->grbSide->setVisible(true);
    ui->grbWire->setVisible(true);
    ui->wdgStdHeight->resize(201,50);
    //1拉出值导高标定初始设置
    ui->grbWire->setParent(ui->stackedWidget->widget(1));
    ui->grbWire->resize(211,131);
    ui->grbWire->move(20,20);

    ui->wdgStdWidth->resize(201,50);
    ui->wdgStdHeight->resize(201,50);
    ui->wdgStdHeight->move(10,30);
    ui->wdgStdWidth->move(10,80);
    ui->wdgStdHeight->setParent(ui->grbWire);
    ui->wdgStdWidth->setParent(ui->grbWire);

    //2轨距超高标定初始设置
    ui->grbRail->setParent(ui->stackedWidget->widget(1));
    ui->grbRail->resize(211,131);
    ui->grbRail->move(250,20);
    ui->wdgRailHeight->setParent(ui->grbRail);
    ui->wdgRailWidth->setParent(ui->grbRail);
    ui->wdgRailWidth->resize(191,50);
    ui->wdgRailHeight->resize(191,50);
    ui->wdgRailHeight->move(10,30);
    ui->wdgRailWidth->move(10,80);

    //3侧面界限标定初始设置
    ui->grbSide->setParent(ui->stackedWidget->widget(1));
    ui->grbSide->resize(441,80);
    ui->grbSide->move(20,150);

    ui->wdgStdSide->setParent(ui->grbSide);
    ui->wdgStdSide->resize(421,50);
    ui->wdgStdSide->move(10,30);

    ui->lblLatest_2->move(10,240);
    ui->lblLatest->move(100,260);
#endif

    getPar();
}
//函数功能:根据单击按钮,选择新线路或者老线路设置界面
void PreRunSettingDlg::slot_tabWidget(bool e)
{
    QRadioButton *rdb=(QRadioButton *)this->sender();

    if(rdb==ui->rdbNewRail)
    {
        ui->pbtnEnter->setText(QString::fromWCharArray(L"测量"));
        ui->pbtnSelPath->setVisible(true);
        setGroupEnable(false);
    }
    else if(rdb==ui->rdbVerify)
    {
        ui->pbtnEnter->setText(QString::fromWCharArray(L"标定"));
        ui->pbtnSelPath->setVisible(false);
        setGroupEnable(true);
    }
}
//函数功能：获取标准参数,同时进行标定二选一，即进行拉出值导高的标定或进行侧面界限的标定
wireStandardValues PreRunSettingDlg::getStandardValue(void)
{
    wireStandardValues tmp;
    tmp.nVerifyWireHeight= ui->wdgStdHeight->getValue().toInt();
    tmp.nVerifyWireWidth=ui->wdgStdWidth->getValue().toInt();
    tmp.nVerifyRailHeight= ui->wdgRailHeight->getValue().toInt();
    tmp.nVerifyRailWidth=ui->wdgRailWidth->getValue().toInt();
    tmp.nVerifySide=ui->wdgStdSide->getValue().toInt();
    if(ui->grbSide->isChecked())
    {
        tmp.strVerifyType="side";
    }
    else if(ui->grbWire->isChecked())
    {
        tmp.strVerifyType=   "wire";
    }
    else if(ui->grbRail->isChecked())
    {
        tmp.strVerifyType=   "rail";
    }
    return tmp;
}
//函数功能：true，测试状态；false，标定状态
bool PreRunSettingDlg::isVerify(void)
{

    if(ui->rdbNewRail->isChecked())
    {
        return true;
    }
    else
    {
        return false;
    }
}
void PreRunSettingDlg::setGroupEnable(bool flag)
{
    //    ui->grbVerify->setEnabled(flag);
    //    ui->wdgSelectRail->setEnabled(!flag);
    if(flag)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(2);
    }
}
QString PreRunSettingDlg::fileName(void)
{
    QString dbName;
    if(ui->cmbLine->currentText().contains(QString::fromWCharArray(L"临时")))
    {
        dbName=ui->spbStationName->prefix()+QString::number(ui->spbStationName->value());
        dbName += " "+ui->cmbDirection->currentText();
    }
    else
    {
        //    qDebug()<<"cur station"<<ui->lwStation->currentIndex().row()<<ui->lwStation->count();
        if(ui->lwStation->currentIndex().column()!=-1)
        {
            dbName=ui->lwStation->currentItem()->text();
            dbName += " "+ui->cmbDirection->currentText();
        }
        else
        {
            if(ui->lwStation->count())
            {
                dbName=ui->lwStation->item(0)->text();
                dbName += " "+ui->cmbDirection->currentText();
            }
            //        qDebug()<<"station is null";
        }
    }
    dbName += QDateTime::currentDateTime().toString(" yyyy-MM-dd-hh-mm-ss");
    dbName += ".db";
    //    m_dbName = dbName;

    ui->lblFileName->setText(dbName);
    return dbName;
}
//enter
void PreRunSettingDlg::setBackData()
{
    if(ui->cmbLine->currentText().contains(QString::fromWCharArray(L"临时")))
    {
        profile.prevPole=ui->wdgPoleNum->getValue().toInt();
        MeasureDB::nPoleNum=ui->wdgPoleNum->getValue();
        profile.bNewRail=true;
        profile.station=ui->spbStationName->text();
        emit setMeasureType(true,ui->wdgPoleNum->getValue());
    }
    else
    {
        qDebug()<<"ddd"<<profile.nPoleTableID;
        emit setMeasureType(false,QString::number(profile.nPoleTableID));
        profile.bNewRail=false;
    }

    m_datapath.replace("\\","/");
    m_datapath=ui->lblFilePath->text().replace("\\","/");
    //    qDebug()<<"-----file paths"<<profile.dbpath <<m_dbName;
    if(ui->checkBox->checkState()==Qt::Checked)
    {
        qDebug()<<"clear distance";
        bClearDist=true;
        emit clearDistanc(CLEAR_WALK);
    }
    else
    {
        bClearDist=false;
    }
    accept();
}
void PreRunSettingDlg::getProfile(RunProfile & toprofile)
{
    //        newRail->newRailInfo(profile);
    profile.direction=ui->cmbDirection->currentIndex();//->currentText();
    profile.startKm=ui->wdgStartKm->getValue().toDouble();
    profile.dbpath=ui->lblFilePath->text()+"/"+ui->lblFileName->text();
    if(ui->cmbLine->currentText().contains(QString::fromWCharArray(L"临时")))
    {
        profile.station=ui->spbStationName->text();
        profile.poleStr=ui->wdgPoleNum->getValue();
    }
    else
    {
        if(ui->lwStation->currentRow()==-1&&ui->lwStation->count())
            profile.station=ui->lwStation->item(0)->text();
        else
            profile.station=ui->lwStation->currentItem()->text();
        if(ui->lwPole->currentRow()==-1&&ui->lwPole->count())
            profile.poleStr=ui->lwPole->item(0)->text();
        else
            profile.poleStr=ui->lwPole->currentItem()->text();
    }
    qDebug()<<"profile,start km="<<profile.startKm;//<<profile.prevStationIdx;

    toprofile = profile;
}
void PreRunSettingDlg::readConfigure()
{
    QSettings settings("Config.ini", QSettings::IniFormat);
    m_datapath = settings.value("system/DataPaths", QString(defaultpath)).toString();
    m_datapath.append("\\data");
    m_datapath.replace("\\","/");
    m_nDirect=settings.value("system/directflag", 0).toInt();
    //    qDebug()<<"qsetting"<<m_datapath;
    QDir ndir;
    ndir.mkpath(m_datapath);
    readSettingParameters( m_lineInfo);
}
void PreRunSettingDlg::writeSetting()
{
    QSettings settings("Config.ini", QSettings::IniFormat);
    settings.setValue("system/DataPaths", m_datapath);
}
void PreRunSettingDlg::setSavePath()
{

    m_datapath = QFileDialog::getExistingDirectory(this, QTextCodec::codecForName("gbk")->toUnicode("设置保存路径"),
                                                   "",
                                                   QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
    if (!m_datapath.isEmpty())
    {
        //ui->dataPathEdit->setText(m_datapath + "/" + m_dbName);
        writeSetting();
    }
    else
        m_datapath = defaultpath;
    ui->lblFilePath->setText(m_datapath);
}
QString PreRunSettingDlg::updatePole(void)
{
    return ui->wdgPoleNum->getValue();
}

PreRunSettingDlg::~PreRunSettingDlg()
{
    delete ui;
}
void PreRunSettingDlg::slot_startKmChanged(QString tmp,QString poleStr)
{
    float cc=tmp.toFloat();
    ui->wdgStartKm->setValue(QString::number(cc,'f',3));
    ui->wdgPoleNum->setValue(poleStr);
}

void PreRunSettingDlg::on_chbDirect_pressed()
{
    if(QMessageBox::information(this,QTextCodec::codecForName("gbk")->toUnicode("提示..."),QTextCodec::codecForName("gbk")->toUnicode("确定修改此选项?"),QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
    {
        if(ui->chbDirect->checkState()==Qt::Checked)
        {
            ui->chbDirect->setCheckState(Qt::Unchecked);
            qDebug()<<"check box status=";
            emit  directFlag(1);
        }
        else if(ui->chbDirect->checkState()==Qt::Unchecked)
        {
            ui->chbDirect->setCheckState(Qt::Checked);
            qDebug()<<"check box unstatus=";
            emit  directFlag(-1);
        }
        saveDirectFlag();
    }
    else
    {
        return;
    }
}
void PreRunSettingDlg::saveDirectFlag(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    settings.setValue("system/directflag", ui->chbDirect->checkState());
    settings.setValue("system/expandFlag", ui->chbDirect->checkState());
}
void PreRunSettingDlg::on_cmbDirection_activated(int index)
{
    fileName();
}
//函数功能：根据配置文件的参数情况，更新UI界面对象的显示
void PreRunSettingDlg::getPar(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    QString strBuffer;
    //1导高拉出值
    strBuffer=settings.value("initialPar/stdWireHeight","").toString();
    ui->wdgStdHeight->setValue(strBuffer);
    strBuffer=settings.value("initialPar/stdWireWidht", "").toString();
    ui->wdgStdWidth->setValue(strBuffer);
    //2轨距及超高
    strBuffer=settings.value("initialPar/stdRailHeight","").toString();
    ui->wdgRailHeight->setValue(strBuffer);
    strBuffer=settings.value("initialPar/stdRailWidth", "").toString();
    ui->wdgRailWidth->setValue(strBuffer);
    //3侧面界限
    strBuffer=settings.value("initialPar/stdSide", "").toString();
    ui->wdgStdSide->setValue(strBuffer);
    //4上次标定时间
    strBuffer=settings.value("initialPar/lastVerify", "").toString();
    ui->lblLatest->setText(strBuffer);
    //    emit baseParChanged();
}
void PreRunSettingDlg::on_spbStationName_valueChanged(const QString &arg1)
{
    fileName();
}
void PreRunSettingDlg::saveInitialPar(sensorInitialValues sensorIntialValue)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    //1测量初始参数设置
    double nBuffer;
    //1.1 超高标定
    if(sensorIntialValue.nInitialRailHeight)
    {
        nBuffer=sensorIntialValue.nInitialRailHeight;
        qDebug()<<"nbuffer "<<nBuffer;
        settings.setValue("initialPar/RailHeightC", nBuffer);

        nBuffer=sensorIntialValue.nInitialRailWidth;
        settings.setValue("initialPar/RailWidthC",nBuffer);
    }

    nBuffer=sensorIntialValue.nInitialWireHeight;
    if(sensorIntialValue.nInitialWireHeight)
    {
        //保存标定后的导高及拉出值
        qDebug()<<"verify wire height ok"<<nBuffer;
        settings.setValue("initialPar/m_nDaogaoC", nBuffer);

        nBuffer=sensorIntialValue.nInitialWireWidth;
        qDebug()<<"verify wire width ok"<<nBuffer;
        settings.setValue("initialPar/m_nLachuzhiC", nBuffer);
    }
    //1.5 侧面界限标定值保存
    nBuffer=sensorIntialValue.nSide;
    //    qDebug()<<"verify side ok"<<nBuffer;
    if(nBuffer)
    {
        settings.setValue("initialPar/sideC", nBuffer);
    }

    QString str=QString("verify finished,wd=%1,wh%2,side=%3").arg(sensorIntialValue.nInitialWireWidth).arg(sensorIntialValue.nInitialWireHeight).arg(sensorIntialValue.nSide);
    //    emit showMessage(str);
    //4侧面界限参数
    //        nBuffer=ui->wdgSideConset->getValue().toInt();
    //        if(nBuffer)
    //        {
    //            settings.setValue("initialPar/sideWidthConst", nBuffer);
    //        }
    //        nBuffer=ui->wdgSideDis->getValue().toInt();
    //        if(nBuffer)
    //        {
    //            settings.setValue("initialPar/sideDisConst", nBuffer);
    //        }


    nBuffer=ui->wdgStdHeight->getValue().toInt();
    settings.setValue("initialPar/stdWireHeight", nBuffer);
    nBuffer=ui->wdgStdWidth->getValue().toInt();
    settings.setValue("initialPar/stdWireWidht", nBuffer);

    nBuffer=ui->wdgRailHeight->getValue().toDouble();
    settings.setValue("initialPar/stdRailHeight", nBuffer);
    nBuffer=ui->wdgRailWidth->getValue().toDouble();
    settings.setValue("initialPar/stdRailWidth", nBuffer);

    //    qDebug()<<"verify is ok";
    nBuffer=ui->wdgStdSide->getValue().toInt();
    settings.setValue("initialPar/stdSide", nBuffer);
    settings.setValue("initialPar/lastVerify", QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"));
    getPar();
}
void PreRunSettingDlg::slot_verifyType(bool b)
{
    QGroupBox *gpb=(QGroupBox *)this->sender();
    if(b)
    {
        if(gpb==ui->grbSide)
        {
            ui->grbSide->setChecked(true);
            ui->grbRail->setChecked(false);
            ui->grbWire->setChecked(false);
        }
        else if(gpb==ui->grbWire)
        {
            ui->grbWire->setChecked(true);
            ui->grbRail->setChecked(false);
            ui->grbRail->setChecked(false);
        }
        else if(gpb==ui->grbRail)
        {
            ui->grbRail->setChecked(true);
            ui->grbSide->setChecked(false);
            ui->grbWire->setChecked(false);
        }
    }
}
void PreRunSettingDlg::setPreRunSetting()
{
    //获取已有的线路信息,即获取数据库中的线路名称
    QStringList list=Global::getInstance()->gbase.getRailList();
    qDebug()<<"select res: " <<list;
    if (list.isEmpty())
    {
        QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("没有任何铁路路线，请先进基本数据中导入路线！"));
        return;
    }
    m_strLineList = list;
    QStringList   listTemp;
    for( int i=0; i<list.length(); i++)
    {
        QString   strName = list.at(i);//.left(3);
        listTemp.append(strName);
    }
    ui->cmbLine->addItems(listTemp);//添加线路名称到线路信息下拉列表框
    ui->cmbLine->setCurrentIndex(0);
    setRailType(false,0);
    profile.lineIdx=0;
    setDirection(0);
    connect(ui->cmbLine, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_railNameChanged(int)));
}
void PreRunSettingDlg::setRailType(bool flag,int idx)
{
    if(ui->cmbLine->currentText().contains(QString::fromWCharArray(L"临时")))
    {
        ui->stwRails->setCurrentIndex(1);
        fileName();
    }
    else
    {
        ui->stwRails->setCurrentIndex(0);
        if(flag)
        {
            profile.lineIdx =idx;
            setDirection(idx);
            emit railInfoChanged(false);
        }
    }
}
void PreRunSettingDlg::initialUI(void)
{
    ui->cmbDirection->clear();
    ui->cmbLine->clear();
    ui->lwStation->clear();
    ui->lwPole->clear();
}
QString PreRunSettingDlg::railName(void)
{
    return ui->cmbLine->currentText();
}
QString PreRunSettingDlg::railDirection(void)
{
    return ui->cmbDirection->currentText();
}
void PreRunSettingDlg::protectEnv(void)
{
    m_lineIndex = profile.lineIdx;

    m_strLinename = profile.lineName;
    m_directionIndex = profile.direction;
    m_strStationname = profile.station;
    m_fPosition = profile.position;
    m_strPolename = profile.poleStr;
}
void PreRunSettingDlg::restoreEnv(void)
{
    profile.lineIdx = m_lineIndex;
    profile.lineName = m_strLinename;
    profile.direction = m_directionIndex;
    profile.station = m_strStationname;
    profile.position = m_fPosition;
    profile.poleStr = m_strPolename;
}
void PreRunSettingDlg::currentKM(int curStationIdx,int curPoleIdx)
{
    static bool first=true;
    int poleTableIndex;
    if(curStationIdx==-1)curStationIdx=0;
    if(curPoleIdx==-1)curPoleIdx=0;
    //    qDebug()<<"cur line station counts"<<railPoleClassify.count()<<curStationIdx<<curPoleIdx<<railPoleClassify.at(curStationIdx).sStation<<railPoleClassify.at(curStationIdx).onepole.at(curPoleIdx).sPoleId<<railPoleClassify.at(curStationIdx).onepole.at(curPoleIdx).nPoleIndex;
    //在进行QVector读取时,最好先判断其容量,如果超出范围,程序有直接死掉的风险.
    if(curStationIdx<railPoleClassify.count())
    {
        if(curPoleIdx<railPoleClassify.at(curStationIdx).onepole.count())
        {
            if(first)
            {
                first=false;
#if 1
                QString cc=railPoleClassify.at(0).onepole.at(0).sPoleId;//
                qDebug()<<"cc"<<cc<<cc.remove(QRegExp("[A-Z]"))<<cc.remove(QRegExp("[A-Z]")).toInt();
                int fristPole=cc.remove(QRegExp("[A-Z]")).toInt();
                QString cc1=railPoleClassify.at(0).onepole.at(1).sPoleId;//
                int nextPole=cc1.remove(QRegExp("[A-Z]")).toInt();
#else

#endif
                qDebug()<<"frist pole"<<fristPole<<nextPole;
                if(fristPole<nextPole)
                    profile.bTablePoleInc=true;
                else
                    profile.bTablePoleInc=false;
            }
            profile.nPoleSum=railPoleClassify.at(railPoleClassify.count()-1).onepole.at(railPoleClassify.at(railPoleClassify.count()-1).onepole.count()-1).nPoleIndex;

            poleTableIndex=railPoleClassify.at(curStationIdx).onepole.at(curPoleIdx).nPoleIndex;
            QSqlQuery query(Global::getInstance()->gbase.currDatabase());
            QString strQuery="SELECT distFlag FROM " + profile.tableName + " where ID="+ QString::number(poleTableIndex);

            //    qDebug()<<"query string ="<<strQuery<<"current poleTableIndex="<<poleTableIndex;
            if (!query.exec(strQuery))
            {
                QSqlError err;
                err = query.lastError();

                if (err.type() != QSqlError::NoError)
                {
                    qDebug()<<"get current km sql error="<<strQuery<<err.text();
                }
                return ;
            }
            else
            {
                while (query.next())
                {
                    emit startKm(query.value(0).toString(),profile.poleStr);
                }
            }
        }
    }
    return ;
}
void PreRunSettingDlg::readSettingParameters( LineInfoSelect &mLineData )
{
    QSettings  settings("lineFile", QSettings::IniFormat);

    mLineData.nLineIndex = settings.value("line", 1).toInt();
    mLineData.nDirection = settings.value("direction", 0).toInt();
    mLineData.nStationIndex = settings.value( "station", 1).toInt();
    mLineData.nPoleIndex = settings.value("pole", 0).toInt();
}
void PreRunSettingDlg::saveSettingParameters( LineInfoSelect  data )
{
    QSettings  settings("lineFile", QSettings::IniFormat);
    settings.setValue("line", data.nLineIndex);
    settings.setValue("direction", data.nDirection);
    settings.setValue("station", data.nStationIndex);
    settings.setValue("pole", data.nPoleIndex);
}
//函数功能:对listwidget的选中进行操作,选中某一项后,其他为非选中状态
void PreRunSettingDlg::setListWidgetCheckable(QListWidget *widget,int curIdx)
{
    for(int i=0;i<widget->count();i++)
    {
        if(i==curIdx)
        {
            widget->item(i)->setCheckState(Qt::Checked);
        }
        else
        {
            widget->item(i)->setCheckState(Qt::Unchecked);
        }
    }

}
//函数功能:在listwidth中显示对应站区的杆号,sidx表示对应的站区index
void PreRunSettingDlg::setPole(int sidx)
{
    qDebug()<<"set sidex"<<sidx<<railPoleClassify.count()<<railPoleClassify.at(sidx).onepole.count();
    if(sidx<railPoleClassify.count()&&railPoleClassify.at(sidx).onepole.count())
    {
        strPoleidlist.clear();
        ui->lwPole->clear();
        ui->lwStation->item(sidx)->setCheckState(Qt::Checked);
        profile.station = ui->lwStation->item(sidx)->text();
        qDebug()<<"sub pole count"<<railPoleClassify.at(sidx).onepole.count();
        //        if(railPoleClassify.at(sidx).onepole.count())
        for(int i=0;i<railPoleClassify.at(sidx).onepole.count();i++)
        {
            ui->lwPole->addItem(railPoleClassify.at(sidx).onepole.at(i).sPoleId);
            ui->lwPole->item(ui->lwPole->count()-1)->setCheckState(Qt::Unchecked);
            //            ui->lwPole->item(ui->lwStation->count()-1)->setCheckState(Qt::Unchecked);
            strPoleidlist.append(railPoleClassify.at(sidx).onepole.at(i).sPoleId);
        }
        ui->lwPole->item(0)->setCheckState(Qt::Checked);
        ui->lblTunnel->setText(railPoleClassify.at(sidx).onepole.at(0).sTunnel);
        profile.position = 0.0f;
        profile.prevPole = 0;
        profile.poleStr =ui->lwPole->item(0)->text();
        profile.nPoleTableID=railPoleClassify.at(sidx).onepole.at(0).nPoleIndex;
        qDebug()<<"npole index"<<profile.nPoleTableID<<railPoleClassify.at(sidx).onepole.at(0).dSumDist;
        //        profile.m_currIdx=;
        //ui->currSelPole->setText(profile.poleStr);
        //        ui->dirPosition->setText(QString::number(0.0f));
        fileName();
    }
    else
    {
        qDebug()<<"side is error or cur sub pole is null";
    }
}
//函数功能:设置线路,id表示线路index
void PreRunSettingDlg::setDirection(int id)
{
    if(id==(-1))
    {
        qDebug()<<"linde id =-1";
        return ;
    }
    else
    {
        qDebug()<<"here line id= "<<id;
        ui->cmbDirection->clear();
        profile.lineIdx = id;
        CDataBase *pdb = &Global::getInstance()->gbase;
        if( !pdb )
        {
            qDebug()<<"database is null";
            return;
        }
        QSqlQuery query(pdb->currDatabase());
        qDebug()<<"line name"<<ui->cmbLine->currentText()<<m_strLineList.at(id);
        profile.lineName = ui->cmbLine->currentText();
        if(id<m_strLineList.count())
            m_strLineName = m_strLineList.at(id);
        else
            return;
        qDebug() <<" line name: " <<  profile.lineName;

        QString cmd = "SELECT * FROM railList where railabbr = \"" +  m_strLineName + "\"";
        qDebug()<<"the cmd 1: " << cmd;
        query.exec(cmd);
        if (query.next())
        {
            sxTableName=query.value(1).toString();
            xxTableName=query.value(2).toString();
        }
        query.finish();
        qDebug()<<"rail table"<<sxTableName<<xxTableName;
        if(!sxTableName.isEmpty())
            ui->cmbDirection->addItem(QTextCodec::codecForName("gbk")->toUnicode("上行"));
        if(!xxTableName.isEmpty())
            ui->cmbDirection->addItem(QTextCodec::codecForName("gbk")->toUnicode("下行"));
        if(ui->cmbDirection->count())
            ui->cmbDirection->setCurrentIndex(0);
        setTableName();
        ui->rdbPoleReduce->setChecked(true);
        ui->rdbPoleIncrease->setChecked(false);
        qDebug()<<"the profile.tal" <<  profile.tableName;
        setStation(profile.tableName);
    }
}
void PreRunSettingDlg::setStation(QString sTableName)
{    
    //    QString
    if(sTableName.isEmpty())
    {
        qDebug()<<" set station,table name is null";
        return ;
    }
    ui->lwStation->clear();
    QSqlQuery query(Global::getInstance()->gbase.currDatabase());
    QString strQuery="select id,station,poleid from " + sTableName + " group by station order by ID";
    //    QVector <stationSubPole> railPoleClassify;

    //    qDebug()<<"station sql="<<strQuery;
    QStringList subStation;
    if (query.exec(strQuery))
    {
        while (query.next())
        {
            //            qDebug()<<"id="<< query.value(0).toInt()<<"station"<< query.value(1).toString()<<"poleid"<< query.value(2).toString();
            subStation<<query.value(1).toString();
        }
    }
    else
    {
        QSqlError err;
        err = query.lastError();

        if (err.type() != QSqlError::NoError)
        {
            qDebug()<<"set station error ,string sql="<<strQuery<<err.text();
        }
        return ;
    }
    query.finish();
    railPoleClassify.clear();
    for(int i=0;i<subStation.count();i++)
    {
        QString strQuery="SELECT id,station,poleid,tunnel,distflag FROM " + profile.tableName + " where station=" + "\"" + subStation.at(i) + "\"";
        stationSubPole oneStation;
        oneStation.sStation=subStation.at(i);
        //        qDebug()<<"add station item"<<i<<subStation.at(i);
        ui->lwStation->addItem(subStation.at(i));
        onePole cc;

        ui->lwStation->item(ui->lwStation->count()-1)->setCheckState(Qt::Unchecked);
        //        addItems(ui->lwStation, subStation.at(i));
        if (query.exec(strQuery))
        {
            while (query.next())
            {
                cc.nPoleIndex=query.value(0).toInt();
                cc.sPoleId=query.value(2).toString();
                cc.sTunnel=query.value(3).toString();
                cc.dSumDist=query.value(4).toDouble();
                //添加杆号及对应的id信息index
                oneStation.onepole<<cc;
                //qDebug()<<"station count:"<<oneStation.onepole.count();
            }
        }
        else
        {
            qDebug()<<"not get the sub station pole";
        }
        railPoleClassify<<oneStation;
    }
    if(ui->lwStation->count())
    {
        setPole(0);
        currentKM(ui->lwStation->currentRow(),0);
    }
}
//函数功能:向对象QListWidget中添加选项text,并设置为非选中状态
void PreRunSettingDlg::addItems(QListWidget* plw, QString text)
{
    //qDebug()<<"the items " << text;
    //    plw->clear();
    QListWidgetItem *pitem = new QListWidgetItem(plw);
    if(pitem )
    {
        //pitem->setSelected(true);
        pitem->setCheckState(Qt::Unchecked);
        pitem->setText(text);
        plw->addItem(pitem);
    }
}
//函数功能：线路名发生变化,idx表示线路名对应的index号
void PreRunSettingDlg::slot_railNameChanged(int idx)
{
    setRailType(true,idx);
}
//函数功能：上下行选择的槽函数,idx，表示上行或下行选择
void PreRunSettingDlg::slot_railDirectionChanged(int idx)
{
    if(idx==(-1))
    {
        qDebug()<<"error,idx = -1";
        return ;
    }
    qDebug()<<"------------------raild Direction changed";
    qDebug()<<"sd"<<profile.tableName<<profile.lineName<<idx;
    setTableName();
    /* 2012-05-11 */
    qDebug()<<"after table name"<<profile.tableName<<"idx="<<idx;
    //    m_lineInfo.nStationIndex = 0;
    setStation(profile.tableName);
    if(idx)//下行
    {
        ui->rdbPoleIncrease->setChecked(true);
        ui->rdbPoleReduce->setChecked(false);
    }
    else
    {
        ui->rdbPoleIncrease->setChecked(false);
        ui->rdbPoleReduce->setChecked(true);
    }
    emit railInfoChanged(false);
}
void PreRunSettingDlg::setTableName(void)
{
    if(ui->cmbDirection->currentText().contains(QString::fromWCharArray(L"下行")))
    {
        profile.direction=1;
        profile.tableName= xxTableName;
    }
    else if(ui->cmbDirection->currentText().contains(QString::fromWCharArray(L"上行")))
    {
        profile.direction=0;
        profile.tableName= sxTableName;
    }
}
void PreRunSettingDlg::slot_railStationChanged(QListWidgetItem* item)
{
    qDebug()<<"station is changing";

    int curRow=ui->lwStation->row(item);
    setPole(curRow);
    setListWidgetCheckable(ui->lwStation,curRow);
    currentKM(curRow,0);
    fileName();
}
void PreRunSettingDlg::adjustPos()
{
    float adjust = 0.0f;
    QPushButton *pbtn = dynamic_cast<QPushButton*>(sender());
    if(pbtn)
    {
        //        if (pbtn == ui->pbtnAddMore)
        //            adjust = 1.0f;
        //        else if (pbtn == ui->pbtnAddLess)
        //            adjust = 0.1f;
        //        else if (pbtn == ui->pbtnSubMore)
        //            adjust = -1.0f;
        //        else
        //            adjust = -0.1f;
        //        profile.position += adjust;
        //        if (fabs(profile.position) < 1e-6)
        //            profile.position = 0.0f;
        //        ui->dirPosition->setText(QString::number(profile.position));
    }
}
void PreRunSettingDlg::slot_railPoleChanged(QListWidgetItem* item)
{
    setListWidgetCheckable(ui->lwPole,ui->lwPole->row(item));
    int i=0;
    for(;i<railPoleClassify.count();i++)
    {
        if(railPoleClassify.at(i).sStation==profile.station)
            break;
    }
    profile.poleStr = item->text();//string,杆号
    profile.nPoleTableID=railPoleClassify.at(i).onepole.at(ui->lwPole->row(item)).nPoleIndex;//int,当前站区的杆号在整个线路中的index
    qDebug()<<"pole number is changing"<<profile.nPoleTableID<<profile.station<<profile.lineName<<profile.poleStr;

    ui->lblTunnel->setText(railPoleClassify.at(i).onepole.at(ui->lwPole->row(item)).sTunnel);
    item->setToolTip(railPoleClassify.at(i).onepole.at(ui->lwPole->row(item)).sTunnel);
    //    qDebug()<<"cur station idx="<<ui->lwStation->currentIndex().row()<<"cur pole idx="<<;
    currentKM(ui->lwStation->currentIndex().row(),ui->lwPole->currentIndex().row());
    fileName();
}
QStringList PreRunSettingDlg::get_poleidlist()
{
    return strPoleidlist;
}
//函数功能:
void PreRunSettingDlg::showPoleId(int sidx)
{
    strPoleidlist.clear();
    //profile.m_currIdx
    profile.prevStationIdx = sidx;
    qDebug()<<"sidx"<<sidx<<ui->lwStation->count();
    if(!(sidx<ui->lwStation->count()))
        sidx=0;
    ui->lwStation->item(sidx)->setCheckState(Qt::Checked);
    profile.station = ui->lwStation->item(sidx)->text();
    //m_strStationName = ui->lwStation->item(sidx)->text();
    CDataBase *pdb = &Global::getInstance()->gbase;
    QSqlQuery query(pdb->currDatabase());
    query.exec("SELECT * FROM " + profile.tableName + " where station=" + "\"" + profile.station + "\"");
    while (query.next())
    {
        //ui->lwPole->addItem(query.value(3).toString());
        addItems(ui->lwPole, query.value(3).toString());
        strPoleidlist.append(query.value(3).toString());
    }
    //qDebug()<<"pole string="<<strPoleidlist;
    profile.position = 0.0f;
    profile.prevPole = 0;
    profile.poleStr = ui->lwPole->item(0)->text();
    ui->lwPole->item(0)->setCheckState(Qt::Checked);
    //     ui->currSelPole->setText(profile.poleStr);
    //    ui->dirPosition->setText(QString::number(0.0f));
}
