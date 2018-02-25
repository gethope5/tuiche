#include "preoldrailform.h"
#include "global.h"
#include "ui_preoldrailform.h"
static const char * defaultpath = "c:/tieren";

preOldRailForm::preOldRailForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::preOldRailForm)
  ,sxTableName("")
  ,xxTableName("")
{
    ui->setupUi(this);
    profile.lineName.clear();//="";
    profile.tableName.clear();//="";
    profile.station.clear();
    readSettingParameters( m_lineInfo);
    connect(ui->cmbDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_railDirectionChanged(int)));
    connect(ui->lwStation, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_railStationChanged(QListWidgetItem*)));
    connect(ui->lwPole, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_railPoleChanged(QListWidgetItem*)));
    connect(ui->pbtnAddMore_3, SIGNAL(clicked()), this, SLOT(adjustPos()));
    connect(ui->pbtnAddLess_3, SIGNAL(clicked()), this, SLOT(adjustPos()));
    connect(ui->pbtnSubMore_3, SIGNAL(clicked()), this, SLOT(adjustPos()));
    connect(ui->pbtnSubLess_3, SIGNAL(clicked()), this, SLOT(adjustPos()));
    //线路趋势单选按钮
//    connect(ui->rdbPoleIncrease,SIGNAL(clicked(bool)),this,SLOT(slot_poleSortFlag(bool)));
//    connect(ui->rdbPoleReduce,SIGNAL(clicked(bool)),this,SLOT(slot_poleSortFlag(bool)));
}

void preOldRailForm::adjustPos()
{
    float adjust = 0.0f;
    QPushButton *pbtn = dynamic_cast<QPushButton*>(sender());
    if (pbtn)
    {
        if (pbtn == ui->pbtnAddMore_3)
            adjust = 1.0f;
        else if (pbtn == ui->pbtnAddLess_3)
            adjust = 0.1f;
        else if (pbtn == ui->pbtnSubMore_3)
            adjust = -1.0f;
        else
            adjust = -0.1f;

        profile.position += adjust;
        if (fabs(profile.position) < 1e-6)
            profile.position = 0.0f;
        ui->dirPosition_3->setText(QString::number(profile.position));
    }

}
preOldRailForm::~preOldRailForm()
{
    delete ui;
}
void preOldRailForm::addItems(QListWidget* plw, QString text)
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

void preOldRailForm::setPreRunSetting()
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
    profile.lineIdx=0;
    setDirection(0);
    connect(ui->cmbLine, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_railNameChanged(int)));
}
//函数功能:设置线路,id表示线路index
void preOldRailForm::setDirection(int id)
{
    if(id==(-1))
    {
        return ;
    }
    else
    {
        ui->cmbDirection->clear();
        profile.lineIdx = id;
        CDataBase *pdb = &Global::getInstance()->gbase;
        if( !pdb )
            return;
        QSqlQuery query(pdb->currDatabase());
        profile.lineName = ui->cmbLine->currentText();
        if(id<m_strLineList.count())
            m_strLineName = m_strLineList.at(id);
        else
            return;
        qDebug() <<" line name: " <<  profile.lineName;

        QString cmd = "SELECT * FROM railList where name = \"" +  m_strLineName + "\"";
        qDebug()<<"the cmd 1: " << cmd;
        query.exec(cmd);
        if (query.next())
        {
            sxTableName=query.value(1).toString();
            xxTableName=query.value(2).toString();
        }
        query.finish();
        if(!sxTableName.isEmpty())
            ui->cmbDirection->addItem(QTextCodec::codecForName("gbk")->toUnicode("上行"));
        if(!xxTableName.isEmpty())
            ui->cmbDirection->addItem(QTextCodec::codecForName("gbk")->toUnicode("下行"));
        if(ui->cmbDirection->count())
            ui->cmbDirection->setCurrentIndex(0);

        profile.tableName = sxTableName;
        profile.direction =0;
        ui->rdbPoleReduce->setChecked(true);
        ui->rdbPoleIncrease->setChecked(false);
        qDebug()<<"the profile.tal" <<  profile.tableName;
        setStation(profile.tableName);
    }
}
void preOldRailForm::setStation(QString sTableName)
{
    //    QString
    ui->lwStation->clear();
    QSqlQuery query(Global::getInstance()->gbase.currDatabase());
    QString strQuery="select id,station,poleid from " + sTableName + " group by station order by ID";
    //    QVector <stationSubPole> railPoleClassify;
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
            qDebug()<<"result："<<err.text();
        }
        return ;
    }
    query.finish();
    railPoleClassify.clear();
    for(int i=0;i<subStation.count();i++)
    {

        QString strQuery="SELECT id,station,poleid,tunnel FROM " + profile.tableName + " where station=" + "\"" + subStation.at(i) + "\"";
        stationSubPole oneStation;
        oneStation.sStation=subStation.at(i);
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
                //添加杆号及对应的id信息index
                oneStation.onepole<<cc;
            }
        }
        railPoleClassify<<oneStation;
    }
    if(ui->lwStation->count())
    {
        setPole(0);
        currentKM(ui->lwStation->currentRow(),0);
    }
}
//函数功能:在listwidth中显示对应站区的杆号,sidx表示对应的站区index
void preOldRailForm::setPole(int sidx)
{
    if(sidx<railPoleClassify.count())
    {
        ui->lwPole->clear();
        ui->lwStation->item(0)->setCheckState(Qt::Checked);
        profile.station = ui->lwStation->item(sidx)->text();
        qDebug()<<"sub pole count"<<railPoleClassify.at(sidx).onepole.count();
        for(int i=0;i<railPoleClassify.at(sidx).onepole.count();i++)
        {
            ui->lwPole->addItem(railPoleClassify.at(sidx).onepole.at(i).sPoleId);
            ui->lwPole->item(ui->lwPole->count()-1)->setCheckState(Qt::Unchecked);
            //            ui->lwPole->item(ui->lwStation->count()-1)->setCheckState(Qt::Unchecked);

        }
        ui->lwPole->item(0)->setCheckState(Qt::Checked);
        ui->lblTunnel->setText(railPoleClassify.at(sidx).onepole.at(0).sTunnel);
        profile.poleStr =ui->lwPole->item(0)->text();
        profile.nPoleTableID=railPoleClassify.at(sidx).onepole.at(0).nPoleIndex;
    }
}

void preOldRailForm::getProfile(RunProfile& toprofile)
{
    if (ui->dirPosition_3->text().isEmpty())
        profile.position = 0;
    else
        profile.position = ui->dirPosition_3->text().toDouble();
    if(ui->rdbPoleIncrease->isChecked())
    {
        profile.bDoPoleInc=true;
    }
    else if(ui->rdbPoleReduce->isChecked())
    {
        profile.bDoPoleInc=false;
    }
    qDebug()<<"pole increase sort"<<profile.bDoPoleInc<<profile.bTablePoleInc;
    toprofile = profile;
}
void preOldRailForm::initialUI(void)
{
    ui->cmbDirection->clear();
    ui->cmbLine->clear();
    ui->lwStation->clear();
    ui->lwPole->clear();
}
QString preOldRailForm::railName(void)
{
    return ui->cmbLine->currentText();
}
QString preOldRailForm::railDirection(void)
{
    return ui->cmbDirection->currentText();
}
void preOldRailForm::slot_railNameChanged(int idx)
{
    profile.lineIdx =idx;
    setDirection(idx);
    emit railInfoChanged(false);
}

void preOldRailForm::slot_railDirectionChanged(int idx)
{
    qDebug()<<"------------------raild Direction changed";

    qDebug()<<"sd"<<profile.tableName<<profile.lineName;
    if(idx)
        profile.tableName= xxTableName;
    else
        profile.tableName= sxTableName;
    profile.direction =idx;
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
void preOldRailForm::slot_railStationChanged(QListWidgetItem* item)
{
    qDebug()<<"station is changing";

    int curRow=ui->lwStation->row(item);
    setPole(curRow);
    setListWidgetCheckable(ui->lwStation,curRow);
    currentKM(curRow,0);
}
//函数功能:对listwidget的选中进行操作,选中某一项后,其他为非选中状态
void preOldRailForm::setListWidgetCheckable(QListWidget *widget,int curIdx)
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
void preOldRailForm::slot_railPoleChanged(QListWidgetItem* item)
{
    qDebug()<<"pole number is changing";
    setListWidgetCheckable(ui->lwPole,ui->lwPole->row(item));
    profile.poleStr = item->text();
    int i=0;
    for(;i<railPoleClassify.count();i++)
    {
        if(railPoleClassify.at(i).sStation==profile.station)
            break;
    }
    profile.nPoleTableID=railPoleClassify.at(i).onepole.at(ui->lwPole->row(item)).nPoleIndex;
    ui->lblTunnel->setText(railPoleClassify.at(i).onepole.at(ui->lwPole->row(item)).sTunnel);
    item->setToolTip(railPoleClassify.at(i).onepole.at(ui->lwPole->row(item)).sTunnel);
    //    qDebug()<<"cur station idx="<<ui->lwStation->currentIndex().row()<<"cur pole idx="<<;
    currentKM(ui->lwStation->currentIndex().row(),ui->lwPole->currentIndex().row());
}

//function：获取当前杆号对应的公里标
//QString preOldRailForm::currentKM()
void preOldRailForm::currentKM(int curStationIdx,int curPoleIdx)
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
                    qDebug()<<"result："<<err.text();
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
void preOldRailForm::readSettingParameters( LineInfoSelect &mLineData )
{
    QSettings  settings("lineFile", QSettings::IniFormat);

    mLineData.nLineIndex = settings.value("line", 1).toInt();
    mLineData.nDirection = settings.value("direction", 0).toInt();
    mLineData.nStationIndex = settings.value( "station", 1).toInt();
    mLineData.nPoleIndex = settings.value("pole", 0).toInt();
}
void preOldRailForm::saveSettingParameters( LineInfoSelect  data )
{
    QSettings  settings("lineFile", QSettings::IniFormat);
    settings.setValue("line", data.nLineIndex);
    settings.setValue("direction", data.nDirection);
    settings.setValue("station", data.nStationIndex);
    settings.setValue("pole", data.nPoleIndex);
}
void preOldRailForm::protectEnv(void)
{
    m_lineIndex = profile.lineIdx;

    m_strLinename = profile.lineName;
    m_directionIndex = profile.direction;
    m_strStationname = profile.station;
    m_fPosition = profile.position;
    m_strPolename = profile.poleStr;
}
void preOldRailForm::restoreEnv(void)
{
    profile.lineIdx = m_lineIndex;
    profile.lineName = m_strLinename;
    profile.direction = m_directionIndex;
    profile.station = m_strStationname;
    profile.position = m_fPosition;
    profile.poleStr = m_strPolename;
}
