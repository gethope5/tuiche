#include "recordmeasureform.h"
#include "ui_recordmeasureform.h"

recordMeasureForm::recordMeasureForm(QString filePrefix,QWidget *parent) :
    QWidget(parent),ui(new Ui::recordMeasureForm)
  ,m_bUIVisible(false)
  ,dataFilePaths("")
  ,xlsTitleName("")
  ,detailTbFilter("")
  ,strFileSavePath("")
  ,indexInterval(0)
{
    ui->setupUi(this);
    tableView=ui->tableView;
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->rdbDetailData,SIGNAL(clicked()),this,SLOT(slot_DisplayTable()));
    connect(ui->rdbSimpleData,SIGNAL(clicked()),this,SLOT(slot_DisplayTable()));
    connect(ui->rdbRangelData,SIGNAL(clicked()),this,SLOT(slot_DisplayTable()));

    model=NULL;

    connect(&subCSVFile,SIGNAL(saveOK(bool)),this,SLOT(slot_csvSaveStatus(bool)));
    ui->pbtnDataReplay->setEnabled(false);

    openDbUiInitial();
    ui->pbtnDataReplay->initialPbtnUI("./images/replayData-focus.png","./images/replayData.png");
    ui->pbtnDataOutput->initialPbtnUI("./images/exportData-focus.png","./images/exportData.png");
    ui->pbtnOpenFile->initialPbtnUI("./images/openFile-focus.png","./images/openFile.png");
    ui->pbtnOverdata->initialPbtnUI("./images/overPar-focus.png","./images/overPar.png");
    ui->pbtnDataCompare->initialPbtnUI("./images/dataCompare-focus.png","./images/dataCompare.png");
    ui->pbtnUISet->initialPbtnUI("./images/uiSet-focus.png","./images/uiSet.png");
    ui->pbtnModifyPole->initialPbtnUI("./images/modifyPole.png","./images/modifyPole-focus.png");
    ui->pbtnUpdate->initialPbtnUI("./images/updateData.png","./images/updateData-focus.png");
    connect(ui->chbDropper,SIGNAL(clicked()),this,SLOT(slot_updateSimpleTable()));
    connect(ui->chbMidwire,SIGNAL(clicked()),this,SLOT(slot_updateSimpleTable()));
    connect(ui->chbWireWidth,SIGNAL(clicked()),this,SLOT(slot_updateSimpleTable()));
    readConfig();
    ui->stackedWidget->setCurrentIndex(1);
    ui->rdbAll->setChecked(true);
    connect(ui->rdbAll,SIGNAL(clicked()),this,SLOT(slot_DisplayData()));
    connect(ui->rdbLimiteWire,SIGNAL(clicked()),this,SLOT(slot_DisplayData()));
    connect(ui->rdbMidWire,SIGNAL(clicked()),this,SLOT(slot_DisplayData()));
    connect(ui->rdbOverPar,SIGNAL(clicked()),this,SLOT(slot_DisplayData()));
    //    this->installEventFilter(this);
    //    ui->tableView->installEventFilter(this);
    sPathPrefix=filePrefix;
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}   
void recordMeasureForm::openDbUiInitial(void)
{
    ui->rdbDetailData->setChecked(true);
    ui->chbDropper->setChecked(true);
    ui->chbMidwire->setChecked(true);
    ui->chbWireWidth->setChecked(true);
    setSimpleTableCheck(false);
    slot_updateSimpleTable();
    prePoleDist.first=true;
}
void recordMeasureForm::readConfig(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    nFirstDropperStep=settings.value("initialPar/firstDropper", 4).toInt();
    minDropperStep=settings.value("initialPar/minRange", 4).toInt();
    maxDropperStep=settings.value("initialPar/maxRang", 4).toInt();
}
recordMeasureForm::~recordMeasureForm()
{
    delete ui;
}
void recordMeasureForm::on_pbtnDataOutput_clicked()
{

#if 0
    if(model)
    {
        //0426删除了isMeasure
        if(MeasureDB::bMeasure)
        {
            QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("请暂停测量后，再导出数据..."));
        }
        else
        {
            strFileSavePath = QFileDialog::getSaveFileName(this, QTextCodec::codecForName("gbk")->toUnicode("加载xls文档"), (dataFilePaths), QTextCodec::codecForName("gbk")->toUnicode("csv文档 (*.csv)"));
            exportCSVFile(strFileSavePath);
        }
    }
    else
    {
        QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("警告"),QTextCodec::codecForName("gbk")->toUnicode("请加载测量数据后再导出数据..."));
        //        qDebug()<<""
    }
#else
    //    QFile cc;
    //    Sqlite2Xls(L"D:\measure data\临时0 上行 2017-06-11-08-31-37.db", L"debugmeasure", L"D:/measure data/cc.xls");
    ExcelParse cc;
    qDebug()<<"cur db filePath="<<dataFilePaths<<dataFilePaths.mid(dataFilePaths.lastIndexOf("/")+1,dataFilePaths.count()-dataFilePaths.lastIndexOf("/")-1);
    if(model&&(!dataFilePaths.isNull()))
    {
        if(dataFilePaths.isEmpty())
            return;
        //         strFileSavePath = QFileDialog::getSaveFileName(0,QString::fromWCharArray(L"另存为"),dataFilePaths.left(dataFilePaths.indexOf(".")),QString::fromWCharArray(L"Excel工作簿(*.xls)"));

        QString fileTmp=dataFilePaths.left(dataFilePaths.indexOf("."));
        if(ui->rdbMidWire->isChecked())
        {
            fileTmp.append(QString::fromWCharArray(L"-跨中"));
        }
        else if(ui->rdbLimiteWire->isChecked())
        {
            fileTmp.append(QString::fromWCharArray(L"-定位点"));
        }
        else if(ui->rdbOverPar->isChecked())
        {
            fileTmp.append(QString::fromWCharArray(L"-超限"));
        }
        if(this->size().height()<500)
        {
            strFileSavePath=QString(fileTmp).append(".xls");
            if(cc.outputXLS(model,xlsTitleName,strFileSavePath))
            {
                QMessageBox::about(this,QString::fromWCharArray(L"提示"),QString::fromWCharArray(L"文件保存成功:\n")+strFileSavePath);
            }
        }
        else
        {
            QFileDialog mm(ui->tableView);
            //        mm.resize(100,100);
            //            qDebug()<<"data paths="<<dataFilePaths.left(dataFilePaths.indexOf("."))<<this->size();
            strFileSavePath =  mm.getSaveFileName(ui->tableView,QString::fromWCharArray(L"另存为"),fileTmp,QString::fromWCharArray(L"Excel工作簿(*.xls)"));
            //            qDebug()<<"fuck updata export ui"<<mm.size();
            cc.outputXLS(model,xlsTitleName,strFileSavePath);
        }
    }
#endif
}

void recordMeasureForm::exportCSVFile(QString filePaths)
{
    qDebug()<<"dataFilePaths="<<dataFilePaths;
    int indexBuffer=dataFilePaths.lastIndexOf('/');

    if(indexBuffer!=-1)
    {
        qDebug()<<"love you"<<dataFilePaths.right(dataFilePaths.count()-indexBuffer-1);
        dataFilePaths.chop(3);//removes 3 characters from the end of the string
    }

#if 1
    if(!filePaths.isEmpty())
    {
        //    qDebug()<<"filepat"<<filepath<<selectShowTable;

        //    subCSVFile.tableTitle=QString(QTextCodec::codecForName("gbk")->toUnicode("编号,站区名称,隧道名称,支柱号,跨距,结构标志,曲线标志,公里标,供电段名称,车间名称,工区名称,导高标准值,拉出标准值"));

        //    qDebug()<<"love fuck"<<subCSVFile.dataBase.isValid();
        subCSVFile.sSavePath=filePaths;
        subCSVFile.start();
    }
#else

    QElapsedTimer timer;
    timer.start();
    //    filepath=QString("c:/6.xls");
    if (filepath.isEmpty())
    {
        return;
    }
    //打开文件，取得工作簿
    excelControl j(filepath );
    qDebug()<<"filepath="<<filepath;

    //    const QString xlsTitle(QTextCodec::codecForName("gbk")->toUnicode("编号,站区名称,隧道名称,支柱号,跨距,结构标志,曲线标志,公里标,供电段名称,车间名称,工区名称,导高标准值,拉出标准值"));
    //    j.exportXLS(Global::getInstance()->gbase.currDatabase(),(model->tableName()),filepath,xlsTitleName);
    j.exportXLS(m_db,(model->tableName()),filepath,xlsTitleName);
    qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";
#endif
}
void recordMeasureForm::slot_csvSaveStatus(bool bFlag)
{
    if(bFlag)
    {
        QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("文件保存成功,文件保存位置：\n")+strFileSavePath);
    }
    else
    {
        QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("请加载测量数据后再导出数据..."));
    }
}
void recordMeasureForm::modifyXLSFile()
{
    //    setXLSFile.setCellValue(13,2,"zbwsfd");
}
//函数功能：用于在打开数据文件及测量时，进行数据库的关联设置
int recordMeasureForm::setCurDatabase(QSqlDatabase *curDatabase,QString filepath)
{
#if 1
    if(!curDatabase->isOpen())
    {
        if(curDatabase->open())
            qDebug()<<"database is ok";
        else
            return 0;
    }
    //    qDebug()<<"recored database is ok"<<curDatabase->tables();
    bool ok1=curDatabase->tables().contains(MeasureDB::detailDataTb,Qt::CaseInsensitive);
    bool ok2=curDatabase->tables().contains(MeasureDB::simpleDataTb,Qt::CaseInsensitive);
    if(!(ok1&&ok2))
    {
        QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("该文件未包含测量数据信息..."));
        return 0;
    }
    //    qDebug()<<"database is 1";

    subCSVFile.dataBase=*curDatabase;

    if(model)
    {
        delete model;
    }
    model = new RailLineSqlModel(tableView,*curDatabase);
    QString tmp=filepath;
#if 0
    if(tmp.count()>25)
        tmp.insert(25,"\n");
#endif
    ui->lblFileInfo->setText(tmp);
    dataFilePaths=filepath;
    qDebug()<<"get database"<<filepath;
    getDbType();
    showDataTable(MeasureDB::detailDataTb);
    //    qDebug()<<"show data table";
    return 1;
#else
    Opendatabase(filepath);
#endif
}
void recordMeasureForm::updateShowTable(bool flag)
{
    //    qDebug()<<"1"<<ui->rdbDetailData->isChecked()<<"2"<<ui->rdbRangelData->isChecked()<<"3"<<ui->rdbSimpleData->isChecked();
    if(model)
    {
        if(flag)
        {
            if(ui->rdbDetailData->isChecked())
            {
                showDataTable(MeasureDB::detailDataTb);
            }
            else if(ui->rdbRangelData->isChecked())
            {
                showDataTable(MeasureDB::overDataTb);
            }
            else if(ui->rdbSimpleData->isChecked())
            {
                showDataTable(MeasureDB::simpleDataTb);
            }
        }
        else
        {
            showDataTable(MeasureDB::verifyTb);
        }
    }
}
//函数功能：中途进行了一次数据结构的调整，该函数用于对不同的函数调整表格detailtable的不同字段，包括显示字段名及表格英文字段名
void recordMeasureForm::getDbType(void)
{
    QSqlQuery query(model->database());
#if 1
    if(query.exec(QString("select *from %1").arg(MeasureDB::detailDataTb)))
    {
        if(query.record().indexOf("fXangle")!=(-1))
        {
            MeasureDB::detailDataTitle=MeasureDB::detailOldTitle;//QString::fromWCharArray(L"ID1,时间1,里程(米)1,公里标(Km)1,站区0,杆号1,轨距1,超高1,拉出值1,导高1,拉出值11,导高11,承力索1,结构高度1,原始数据0");
            MeasureDB::detailTableFields=MeasureDB::detailOldFields;//QString("id,tm,dist,km,station,PoleID,railWidth,railHeight,wireWidth,wireHeight,wireWidth1,wireHeight1,fXangle,fYangle,orignalData");
        }
        else if(query.record().indexOf("carrier")!=(-1))
        {
//            qDebug()<<"before title "<<MeasureDB::detailNewTitle
//                   <<MeasureDB::detailNewFields;
            detectRecord(query,QString::fromWCharArray(L"承拉"),"carrierWidth");
            detectRecord(query,QString::fromWCharArray(L"速度(Km/h)"),"speed");
//            qDebug()<<"after title "<<MeasureDB::detailNewTitle
//                   <<MeasureDB::detailNewFields;
            MeasureDB::detailDataTitle=MeasureDB::detailNewTitle;
            MeasureDB::detailTableFields=MeasureDB::detailNewFields;
        }
    }
#else
    if(query.exec(QString("select fXangle from %1").arg(MeasureDB::detailDataTb)))
    {
        MeasureDB::detailDataTitle=MeasureDB::detailOldTitle;//QString::fromWCharArray(L"ID1,时间1,里程(米)1,公里标(Km)1,站区0,杆号1,轨距1,超高1,拉出值1,导高1,拉出值11,导高11,承力索1,结构高度1,原始数据0");
        MeasureDB::detailTableFields=MeasureDB::detailOldFields;//QString("id,tm,dist,km,station,PoleID,railWidth,railHeight,wireWidth,wireHeight,wireWidth1,wireHeight1,fXangle,fYangle,orignalData");
    }
    else if(query.exec(QString("select carrier from %1").arg(MeasureDB::detailDataTb)))
    {
        MeasureDB::detailDataTitle=MeasureDB::detailNewTitle;
        MeasureDB::detailTableFields=MeasureDB::detailNewFields;
    }
#endif
}
//函数功能:根据表格中字段的情况，对对象MeasureDB::detailNewTitle、MeasureDB::detailNewFields中的内容进行调整
//前面进行了数据结构的调整，本函数用于判别表格中是否存在某个字段，并调整对应中文字段等信息
void recordMeasureForm::detectRecord(QSqlQuery &query,QString title,QString name)
{
    //未包含字段name，在从MeasureDB::detailNewTitle、MeasureDB::detailNewFields中删除对应字段的表格名称及中文名称
    int nameIndex=query.record().indexOf(name);
    if(nameIndex==(-1))//
    {
        QString title1=",";
        title1.append(title).append("1,");
        qDebug()<<"not include "<<title<<name;
        QString title2=",";
        QString nameTmp=QString(",%1,").arg(name);
        title2.append(title).append("0,");

        int tmpIndex1=MeasureDB::detailNewTitle.indexOf(title1);
        int tmpIndex2=MeasureDB::detailNewTitle.indexOf(title2);
        int nameIndex=MeasureDB::detailNewFields.indexOf(nameTmp);
        if(nameIndex!=(-1))
            MeasureDB::detailNewFields.replace(nameIndex,nameTmp.count(),",");
        if(tmpIndex1!=(-1))
        {
            MeasureDB::detailNewTitle.replace(tmpIndex1,title1.count(),",");
        }
        else if(tmpIndex2!=(-1))
        {
            MeasureDB::detailNewTitle.replace(tmpIndex2,title2.count(),",");
        }
        //        qDebug()<<"index"<<tmpIndex1<<tmpIndex2<<nameIndex<<nameTmp;
    }
    //表格中包含了对应字段，首先MeasureDB::detailNewTitle、MeasureDB::detailNewFields查看是否包含相应字段，如未包含，则进行添加。
    else
    {
        if(MeasureDB::detailNewTitle.contains(title))
        {
            return;
        }
        QString nameTmp=QString("%1,").arg(name);
        QString title1=title;
        title1.append("1,");
        qDebug()<<"before insert title"<<MeasureDB::detailNewTitle<<nameIndex;
        qDebug()<<nameTmp<<title1;
        MeasureDB::detailNewTitle=MeasureDB::detailNewTitle.insert(MeasureDB::detailNewTitle.indexOf(MeasureDB::detailNewTitle.split(",").at(nameIndex-indexInterval)),title1);
        MeasureDB::detailNewFields=MeasureDB::detailNewFields.insert(MeasureDB::detailNewFields.indexOf(MeasureDB::detailNewFields.split(",").at(nameIndex-indexInterval++)),nameTmp);

        qDebug()<<"before insert title"<<MeasureDB::detailNewTitle<<MeasureDB::detailNewFields<<nameIndex;
    }
}
void recordMeasureForm::showDataTable(const QString &tableName)
{
    if(model)
    {
        QString strFilter=QString("id>0");
        QString fieldsSelect;
        if(tableName.contains(MeasureDB::detailDataTb))
        {
            xlsTitleName=MeasureDB::detailDataTitle;//QTextCodec::codecForName("gbk")->toUnicode(sDebugTitle.toLatin1().data());
            fieldsSelect=MeasureDB::detailTableFields;
//            qDebug()<<"title"<<xlsTitleName;
//            qDebug()<<"field select "<<fieldsSelect;
            if(!detailTbFilter.isEmpty())
            {
                strFilter.append(" and ").append(detailTbFilter);
            }
        }
        else if(tableName.contains(MeasureDB::simpleDataTb))
        {
            xlsTitleName=MeasureDB::simpleDataTitle;//QTextCodec::codecForName("gbk")->toUnicode(.toLatin1().data());
            strFilter.append(" and ").append(simpleTbFilter);
            fieldsSelect=MeasureDB::simpleTableFields;
            //            strFilter= simpleTbFilter;
            //                        qDebug()<<"simple filter = "<<strFilter;
        }
        else if(tableName.contains(MeasureDB::overDataTb))
        {
            xlsTitleName=MeasureDB::overDataTitle;
            fieldsSelect=MeasureDB::overTableField;
        }
        else if(tableName.contains(MeasureDB::verifyTb))
        {
            xlsTitleName=MeasureDB::verifyTitle;
            fieldsSelect=MeasureDB::verifyFields;
        }
        subCSVFile.tableName=tableName;
        subCSVFile.tableTitle=xlsTitleName;

        model->setTable(tableName);
        if(!model->database().isOpen())
        {
            model->database().open();
        }
        model->setEditStrategy(QSqlTableModel::OnRowChange);
        //        int distIndex=getDistIndex(tableName);
        //        if(distIndex)
        //        {
        //            model->setSort(distIndex,Qt::AscendingOrder);
        //        }
#if DEBUG_TITLE
        QSqlQuery query(model->database());
        QString strTmp;
        for(int i=0;i<xlsTitleName.split(',').count();i++)
        {
            QString tmp=xlsTitleName.split(',').at(i);
            if(tmp.right(1).toInt())
            {
                strTmp.append(fieldsSelect.split(',').at(i)).append(",");
            }
        }
        //        qDebug()<<"string filter"<<strFilter;
        model->setFilter(strFilter);
        strTmp=strTmp.left(strTmp.count()-1);
        QString cc=QString("select %1 from %2").arg(strTmp).arg(tableName);
        query.exec(cc);
        model->setquery(query);
        int j=0;
        model->setSort(0,Qt::AscendingOrder);
#endif
        model->select();
        //qDebug()<<"xls title"<<model->tableName()<<tableName<<xlsTitleName<<ui->rdbDetailData->isChecked()<<ui->rdbSimpleData->isChecked()<<ui->rdbRangelData->isChecked();


        while (model->canFetchMore())
        {
            model->fetchMore();
        }
        tableView->setModel(model);
        tableView->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
        tableView->setColumnWidth(model->record().indexOf("poleid"),40);
        tableView->setColumnWidth(model->record().indexOf("id"),40);
        tableView->setColumnWidth(model->record().indexOf("tm"),80);
        tableView->setColumnWidth(model->record().indexOf("dist"),60);
        tableView->setColumnWidth(model->record().indexOf("km"),80);
        tableView->setColumnWidth(model->record().indexOf("station"),80);
        tableView->setColumnWidth(-1,80);
        tableView->selectRow(model->rowCount()-1);

        for(int i=0;i<xlsTitleName.split(',').count();i++)
        {
            QString tmp=xlsTitleName.split(',').at(i);
#if DEBUG_TITLE
            if(tmp.right(1).toInt())
            {
                model->setHeaderData(j++,Qt::Horizontal, QString(tmp.left(tmp.length()-1)));
            }
        }
#else
            if(tmp.right(1).toInt())
            {
                model->setHeaderData(i,Qt::Horizontal, QString(tmp.left(tmp.length()-1)));
            }
            else
            {
                //                        qDebug()<<"display flag"<<i<<tmp.right(1).toInt()<<tmp<<model->record().indexOf(fieldsSelect.split(',').at(i));
                tableView->setColumnHidden(model->record().indexOf(fieldsSelect.split(',').at(i)),true);//x倾角
            }
        }
#endif
    }
}
//函数功能：获取字段dist所对应的列序号
int recordMeasureForm::getDistIndex(QString tableName)
{
    QSqlQuery query(model->database());

    QString strSql=QString("select * from %1").arg(tableName);

    if((query.exec(strSql)))
    {
        int nTmp=query.record().indexOf("dist");
        //        qDebug()<<"dist index"<<nTmp;
        if(nTmp==-1)
            return   0;
        else
            return nTmp;
    }
    return 0;
}

void recordMeasureForm::setSimpleTableCheck(bool flag)
{
    ui->chbDropper->setEnabled(flag);
    ui->chbMidwire->setEnabled(flag);
    ui->chbWireWidth->setEnabled(flag);
}
bool recordMeasureForm::Opendatabase(const QString  &filepath)
{
    bool ReturnFlag;
    m_db.close();
    QSqlError err;
    m_db = QSqlDatabase::addDatabase("QSQLITE", "vapor_connection");
    qDebug()<<"filepath"<<filepath;
    m_db.setDatabaseName(filepath);
    ReturnFlag=m_db.open();
    if (!ReturnFlag)
    {
        err = m_db.lastError();
        m_db = QSqlDatabase();
    }

    if (err.type() != QSqlError::NoError)
    {
        QMessageBox::warning(NULL, QTextCodec::codecForName("gbk")->toUnicode("无法打开数据库"), QTextCodec::codecForName("gbk")->toUnicode("数据库操作错误: ") + err.text());
    }
    return ReturnFlag;
}
void recordMeasureForm::slot_DisplayTable()
{
    QRadioButton *rdb=(QRadioButton*)this->sender();
    if(rdb==ui->rdbDetailData)
    {
        setSimpleTableCheck(false);
        if(model)
            showDataTable(MeasureDB::detailDataTb);
    }
    else if(rdb==ui->rdbSimpleData)
    {
        setSimpleTableCheck(true);
        if(model)
            showDataTable(MeasureDB::simpleDataTb);

    }
    else if(rdb==ui->rdbRangelData)
    {
        setSimpleTableCheck(false);
        //        selectShowTable=2;
        if(model)
            showDataTable(MeasureDB::overDataTb);
    }
}
void recordMeasureForm::on_pbtnOpenFile_clicked()
{
    if(MeasureDB::bMeasure)
    {
        QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("测量中,请测试完毕后再打开其他数据库..."));
    }
    else
    {
        //        dataFilePaths = QFileDialog::getOpenFileName(this, QTextCodec::codecForName("gbk")->toUnicode("加载数据文档"), "D://guiyangTuiche//GYData", QTextCodec::codecForName("gbk")->toUnicode("db文档 (*.db)"));
        QString tmp= QFileDialog::getOpenFileName(this, QTextCodec::codecForName("gbk")->toUnicode("加载数据文档"), sPathPrefix, QTextCodec::codecForName("gbk")->toUnicode("db文档 (*.db)"));
        //qDebug()<<"open data file"<<dataFilePaths;
        if(!tmp.isEmpty())
        {
            dataFilePaths =tmp;
        }

        dDistance.clear();      //1
        wires1.clear();         //2
        wires2.clear();         //3
        carriers.clear(); //4
        poles.clear();
        indexInterval=0;
        getFileData(dataFilePaths,wires1,wires2,carriers,poles);
        //        qDebug()<<"+++++++++++++++all pole"<<poles;
        emit  dataReplay(dDistance,wires1,wires2,carriers,poles);//0118

        openDbUiInitial();
        ui->pbtnDataReplay->setEnabled(true);
    }
}
void recordMeasureForm::getFileData(QString filePath,QVector<QPoint> &wire1,QVector<QPoint> &wire2,QVector<QPoint> &carrierHeight, QVector<QPair<double,int> > &poles)
{
    if(Opendatabase(filePath))
    {
        setCurDatabase(&m_db,filePath);
    }
    else
        return;
    if(m_db.isValid())
    {
        QSqlQuery query(m_db);
        //debugMeasure(id INT, tm varchar(30),dist int,station varchar(20),PoleID varchar(20),railWidth int,railHeight int,wireWidth int,wireHeight int,carrier float,fYangle float,orignalData varchar(100))"); /*rev float*/
        sOriginalDatas.clear();
        dDistance.clear();
        carriers.clear();
        allDist.clear();

        if(query.exec("select * from debugmeasure"))
        {
            getDataFromDb(query,wire1,wire2);
        }
        else
            qDebug()<<"not get file data";
        getFilePoles(poles);
    }
}
void recordMeasureForm::getDataFromDb(QSqlQuery &query,QVector<QPoint> &wire1,QVector<QPoint> &wire2)
{
    //    "fXangle"
    //    "carrier"
    int distIndex=query.record().indexOf("dist");
    int wireWidthIndex=query.record().indexOf("wireWidth");
    int wireHeightIndex=query.record().indexOf("wireHeight");
    int wireWidthIndex1=query.record().indexOf("wireWidth1");
    int wireHeightIndex1=query.record().indexOf("wireHeight1");

    int carrierHeightIndex=query.record().indexOf("carrier");
    int carrierWidthIndex=query.record().indexOf("carrierWidth");
    //    qDebug()<<"record index"<<wireWidthIndex<<wireWidthIndex1<<wireHeightIndex<<wireHeightIndex1<<carrierHeightIndex;
    double tmpDist;

    while(query.next())
    {
        tmpDist=query.value(distIndex).toDouble();
        //                qDebug()<<"value dist="<<tmpDist<<distIndex;

        QPoint tmpWire1,tmpWire2;

        dDistance<<tmpDist;

        tmpWire1.setX(query.value(wireWidthIndex).toInt());
        tmpWire1.setY(query.value(wireHeightIndex).toInt());

        tmpWire2.setX(query.value(wireWidthIndex1).toInt());
        tmpWire2.setY(query.value(wireHeightIndex1).toInt());


        wire1<<tmpWire1;
        wire2<<tmpWire2;
        allDist<<QPair<int,double> (query.value(0).toInt(),tmpDist);
        if(carrierHeightIndex!=(-1))
        {
            if(carrierWidthIndex!=(-1))
            {
                carriers<<QPoint(query.value(carrierWidthIndex).toInt(),query.value(carrierHeightIndex).toInt());
            }
            else
            {
                carriers<<QPoint(0,query.value(carrierHeightIndex).toInt());
            }
        }
        else
        {
            carriers<<QPoint(0,0);
        }
    }
}
//函数功能：从表格measuredb::simpleDataTb中获取当前的定位点里程及杆号信息
void recordMeasureForm::getFilePoles(QVector<QPair<double,int> > &poles)
{
    QSqlQuery query(model->database());
    //获取杆号及对应的里程信息,并存入
    QPair<double,int> tmpPoleInfo;
    QString strSql=QString("select id,dist,poleid from %1 group by poleid locationRemark ='%2'").arg(MeasureDB::simpleDataTb).arg(QString::fromWCharArray(L"定位点"));
    if(query.exec(strSql))
    {
        while(query.next())
        {
            tmpPoleInfo.first=query.value(1).toDouble();
            tmpPoleInfo.second=query.value(2).toInt();
            //            qDebug()<<"dist"<<tmpPoleInfo.first<<"pole "<<tmpPoleInfo.second;
            poles<<tmpPoleInfo;
        }
    }
    else if(query.exec(QString("select id,dist,poleid from %1 where datatype='%2'").arg(MeasureDB::detailDataTb).arg(QString::fromWCharArray(L"定位点"))))
    {
        while(query.next())
        {
            tmpPoleInfo.first=query.value(1).toDouble();
            tmpPoleInfo.second=query.value(2).toInt();
            //            qDebug()<<"dist"<<tmpPoleInfo.first<<"pole "<<tmpPoleInfo.second;
            poles<<tmpPoleInfo;
        }
    }
}
void recordMeasureForm::on_pbtnDataReplay_clicked()
{
    poles.clear();
    getFilePoles(poles);
    emit  dataReplay(dDistance,wires1,wires2,carriers,poles);
    //        emit poleReplay(poles);
}

void recordMeasureForm::on_pbtnUIExpand_clicked()
{
    setExpand(m_bUIVisible);
    m_bUIVisible=!m_bUIVisible;
}
void recordMeasureForm::slot_expandUI(bool b)
{
    setExpand(b);
    m_bUIVisible=b;
}
void recordMeasureForm::setExpand(bool flag)
{
    if(flag)
    {
        ui->groupBox->setVisible(false);
        ui->pbtnUIExpand->setText("<<");
    }
    else
    {
        ui->groupBox->setVisible(true);
        ui->pbtnUIExpand->setText(">>");
    }
}

void recordMeasureForm::on_pbtnModifyPole_clicked()
{
    if(model)
    {
        qDebug()<<"cur model is not null"  <<model->tableName() ;
        modelPoleDlg dd;

        if(model->tableName()==MeasureDB::detailDataTb)
        {
            dd.initialUI(false);
            if(dd.exec())
            {
                int start=0;
                int end=0;
                int pole=0;
                dd.modifyPoleInfo(start,end,pole);
                modelPole(start, end,pole);
            }
        }
        else if(model->tableName()==MeasureDB::simpleDataTb)
        {
            dd.initialUI(true);
            if(dd.exec())
            {
                QSqlQuery query(model->database());
                QString strSql=QString("delete from %1 where id=%2").arg(MeasureDB::simpleDataTb).arg(dd.getDeleteID());
                if(query.exec(strSql))
                {
                    QMessageBox::about(this,QString::fromWCharArray(L"提示"),QString::fromWCharArray(L"杆号删除成功!!!"));
                    showDataTable(MeasureDB::detailDataTb);
                }
                else
                {
                    QMessageBox::about(this,QString::fromWCharArray(L"提示"),QString::fromWCharArray(L"杆号删除失败,%1").arg(query.lastError().text()));
                }
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        qDebug()<<"cur model is null"   ;
    }
}
//函数功能：利用sql语句修改杆号
void recordMeasureForm::modelPole(int startId,int endId,int pole)
{
    if(model)
    {
        QSqlQuery query(model->database());

        QString sqlBuffer=QString("update %1 set poleid= '%2' where id> %3 and id<%4" ).arg(MeasureDB::detailDataTb).arg(pole).arg(startId).arg(endId);
        if(query.exec(sqlBuffer))
        {
            QMessageBox::about(this,QString::fromWCharArray(L"提示"),QString::fromWCharArray(L"杆号修改成功!!!"));
            showDataTable(MeasureDB::detailDataTb);
        }
        else
        {
            QMessageBox::about(this,QString::fromWCharArray(L"提示"),QString::fromWCharArray(L"杆号修改失败,%1").arg(query.lastError().text()));
        }
    }
}
bool recordMeasureForm::insertRecord(  MeasureIdEx_t  &dval)
{
    QSqlQuery query(model->database());
    if(dval.poleid.isNull())
    {
        qDebug()<<"insert mid wirewidth,pole is null";
        return false;
    }
    else
    {
        QString strSql = QString("INSERT INTO MeasureEx VALUES(:id1, :id2, :id3, :id4, :id5, :id6, :id7,:id8, :id9,:id10,:id11,:id12,:id13,:id14,:id15,:id16,:id17,:id18,:id19,:id20,:id121,:id22,:id23,:id24)");
        query.prepare(strSql);
        //    query.bindValue(":id1",dval.id);
        //ID, 2 时间, 3 里程(米), 4公里标(千米),  5站区,   6杆号,  7轨距,       8超高,
        //id, tm,    dist       ,km ,         station, poleid,railwidth ,railheight
        //9侧面界限,  10 红线 11 结构高度   12定拉       13定高,       14坡度,      高差,    16中高,
        //sideWidth,Height,structHeight ,dWireWidth ,dWireHeight ,slope float,gaocha, midWireHeight,
        //17中拉,       18非支拉出值,19非支导高1, 20 水平距离,21垂直距离,   22 500高差,      23 定位器描述,   24 巡视图片;
        //midWireWidht,wirewidth1,wireheight1,wiresWidth,wiresHeight,wire500Height,LocationRemark,picPath");

        query.bindValue(":id2",dval.tm);
        query.bindValue(":id3",dval.nWalkDistance);
        query.bindValue(":id4",QString::number(dval.dKm,'f',3).toFloat());
        //    query.bindValue(":id4",dval.dKm);

        query.bindValue(":id5", dval.station);
        query.bindValue(":id6", dval.poleid);
        query.bindValue(":id7", dval.onePolePar.nRailWidth);
        query.bindValue(":id8", dval.onePolePar.nRailHeight);
        query.bindValue(":id9", dval.sideLimit);
        query.bindValue(":id10",dval.onePolePar.sideHeight);
        query.bindValue(":id11",dval.onePolePar.structHeight);
        query.bindValue(":id12",dval.onePolePar.dWireWidth);
        query.bindValue(":id13",dval.onePolePar.dWireHeight);
        query.bindValue(":id14",dval.onePolePar.slope);
        query.bindValue(":id15",dval.onePolePar.gaocha);
        query.bindValue(":id16",dval.onePolePar.midWireHeight);
        query.bindValue(":id17",dval.onePolePar.midWireWidth);
        query.bindValue(":id18", dval.onePolePar.wireWidth1);
        query.bindValue(":id19", dval.onePolePar.wireHeight1);
        query.bindValue(":id20", dval.onePolePar.wiresWidth);
        query.bindValue(":id21", dval.onePolePar.wiresHeight);
        query.bindValue(":id22",dval.onePolePar.wire500Height);
        query.bindValue(":id23",dval.locationRemark);
        query.bindValue(":id24",dval.PicPath);
        return query.exec();
    }
}
void recordMeasureForm::update1WireWidth(MeasureIdEx_t &dval)
{
    QSqlQuery query(model->database());
    QString strSql;
    if(indexPoleNum(dval.poleid))
    {
        strSql=QString( "update %1 set dwirewidth =%2,dwireHeight =%3,tm='%4',dist=%5,km=%6 where poleid ='%7' and locationremark='%8'")
                .arg(MeasureDB::simpleDataTb)       //1
                .arg(dval.onePolePar.dWireWidth)    //2
                .arg(dval.onePolePar.dWireHeight)   //3
                .arg(dval.tm)                       //4
                .arg(dval.nWalkDistance)            //5
                .arg(dval.dKm)                      //6
                .arg(dval.poleid)                   //7
                .arg(QString::fromWCharArray(L"定位点"));//8
        //        qDebug()<<"update wire widht sql"<<strSql<<
        query.exec(strSql);
    }
    else
    {
        //        MeasureIdEx_t  dval;
        //        memset(&dval.onePolePar,0,sizeof(OnePolePar));
        //        dval.dKm=dval.d;
        //        dval.nWalkDistance=dist;
        //        dval.poleid=poleNum;
        //        dval.onePolePar.dWireHeight=p.y();
        //        dval.onePolePar.dWireWidth=p.x();
        dval.locationRemark=QString::fromWCharArray(L"定位点");
        qDebug()<<"insert not aready pole,pole= "<<dval.poleid<<insertRecord(dval);
    }
}
//函数功能：搜索是否存在包含了杆号polesNum的定位点信息,并判断表格中是否包含了多条某一杆号的定位点数据的记录，若存在进行处理。
bool recordMeasureForm::indexPoleNum(QString poleNum)
{
    QSqlQuery query(model->database());
    if(poleNum.isNull())
    {
        qDebug()<<"wirewidth,poleNum is null";
        return false;
    }

    QString strSql=QString("select id from %1 where poleid='%2' and locationRemark='%3' ").arg(MeasureDB::simpleDataTb).arg(poleNum).arg(QString::fromWCharArray(L"定位点"));
    //    qDebug()<<"sql"<<strSql;
    QVector<int> ids;
    if(query.exec(strSql))
    {
        int id=-1;
        while(query.next())
        {
            id=query.value(0).toInt();
            ids<<id;
        }
        if(id==-1)
        {
            return false;
        }
        else
        {
            //MeasureDB::simpleDataTb表格中一个杆号可能存在多条其定位点的数据，判别，并删除多余的,留下最后一条
            if(ids.count()>1)
            {
                for(int i=0;i<ids.count();i++)
                {
                    qDebug()<<QString("delete simple multiple record,id=%1,pole =%2,").arg(ids.at(i)).arg(poleNum)<<deleteSimpleRecord(ids.at(i));
                }
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    else
        return false;
}
bool recordMeasureForm::deleteSimpleRecord(int id)
{
    QSqlQuery query(model->database());
    QString strSql=QString( "delete from %1 where id= %2").arg(MeasureDB::simpleDataTb).arg(id);
    return query.exec(strSql);
}
void recordMeasureForm::updateAllWireWidth(void)
{
    QSqlQuery query(model->database());
    QString strSql=QString( "select id,poleid,dist from %1 group by poleId order by id").arg(MeasureDB::detailDataTb);
    tbPoles.clear();
    //    qDebug()<<"dist"<<strSql;
    if(query.exec(strSql))
    {
        while(query.next())
        {
            QPair<int,QPair<double,QString> > tmp;
            tmp.first=0;
            tmp.second=QPair<double,QString> (0,query.value(1).toString());
            tbPoles<<tmp;

            //            qDebug()<<"group by data"<<query.value(0).toInt()<<query.value(1).toInt()<<query.value(2).toDouble();
        }
    }
    for(int i=0;i<tbPoles.count();i++)
    {
        //获取极值
        //                      0                   1   2   3   4       5
        strSql=QString("select max(abs(wirewidth)),id,dist,km,wirewidth,wireheight from %1 where poleid='%2' ").arg(MeasureDB::detailDataTb).arg(tbPoles.at(i).second.second);
        //        qDebug()<<"limit sql"<<strSql;
        if(query.exec(strSql))
        {
            while(query.next())
            {
                //tbPoles<<query.value(1).toInt();
                float dist=query.value(2).toFloat();
                QPair<int,QPair<double,QString> > tmp=tbPoles.at(i);
                tmp.first=query.value(1).toInt();//id
                tmp.second.first=dist;//dist
                tbPoles.replace(i,tmp);
                MeasureIdEx_t dval;
                if(getDetailData(tmp.first,dval))
                {
                    update1WireWidth(dval);
                }
                else
                    return ;
            }
        }
        else
        {
            qDebug()  <<"query is false";
        }

    }
}
void recordMeasureForm::on_pbtnOverdata_clicked()
{
    qDebug()<< indexPoleNum("1");
}
void recordMeasureForm::slot_updateSimpleTable()
{
    QStringList tmp("");
    QString tmp1("");

    if(ui->chbDropper->checkState()==Qt::Checked)
    {
        tmp<<QString("LocationRemark = '%1'").arg(QString::fromWCharArray(L"吊弦"));
    }
    else
    {

        tmp<<"false";
    }
    if(ui->chbMidwire->checkState()==Qt::Checked)
    {
        tmp<<QString("LocationRemark = '%1' ").arg(QString::fromWCharArray(L"跨中"));
    }
    else
    {
        tmp<<"false";
    }
    if(ui->chbWireWidth->checkState()==Qt::Checked)
    {
        tmp<<QString("LocationRemark ='%1' ").arg(QString::fromWCharArray(L"定位点"));
    }
    else
    {
        tmp<<"false";
    }
    for(int i=0;i<tmp.count();i++)
    {
        if(!QString( tmp.at(i)).contains("false"))
        {
            if(QString(tmp.at(i)).isEmpty())
                continue;
            //qDebug()<<i;
            tmp1.append(tmp.at(i)).append(" or ");
        }
    }
    tmp1=tmp1.trimmed();
    if(tmp1.endsWith("or"))
    {
        //qDebug()<<"see "<<tmp1.lastIndexOf("or")<<tmp1.count();
        tmp1.replace(tmp1.lastIndexOf("or"),tmp1.count(),"");
    }
    if(tmp1.isEmpty())
    {
        tmp1=QString("LocationRemark =' ' ");
    }
    simpleTbFilter=tmp1;
    //    qDebug()<<"filter string"<<simpleTbFilter;
    //    simpleTbFilter=QString("LocationRemark ='%1' ").arg(QString::fromWCharArray(L"定位点"));
    if(ui->rdbSimpleData->isChecked())
    {
        showDataTable(MeasureDB::simpleDataTb);
    }
    //    qDebug()<<"filter string="<<tmp1;
}
//函数功能：将某一条记录转换为MeasureIdEx_t对象，返回值为记录获取的状态，true获取成功，false获取失败
bool recordMeasureForm::getDetailData(int id, MeasureIdEx_t  &dval)
{
    memset(&dval.onePolePar,0,sizeof(OnePolePar));
    QSqlQuery query(model->database());
    //                               0 1    2   3           4
    QString  strSql=QString("select id,dist,km,wirewidth,wireheight,tm,poleid from %1 where id=%2 ").arg(MeasureDB::detailDataTb).arg(id);
    if(query.exec(strSql))
    {
        while(query.next())
        {
            dval.id=query.value(0).toInt();
            dval.nWalkDistance=query.value(1).toDouble();
            dval.dKm=query.value(2).toDouble();
            dval.onePolePar.dWireWidth=query.value(3).toInt();
            dval.onePolePar.dWireHeight=query.value(4).toInt();
            dval.tm=query.value(5).toString();
            dval.poleid=query.value(6).toString();
        }
        return true;
    }
    else
        return false;
}
//函数功能：搜索是否存在包含了杆号polesNum的跨中信息
bool recordMeasureForm::indexMidWireWidth(QString poleNum)
{
    QSqlQuery query(model->database());
    if(poleNum.isNull())
    {
        qDebug()<<"mid wireWidth,pole is null";
        return false;
    }
    else
    {
        QString strSql=QString("select id from %1 where poleid='%2' and locationRemark='%3' ").arg(MeasureDB::simpleDataTb).arg(poleNum).arg(QString::fromWCharArray(L"跨中"));
        //        qDebug()<<"found mid wire sql"<<strSql;
        QVector<int> ids;
        if(query.exec(strSql))
        {
            int id=-1;
            while(query.next())
            {
                id=query.value(0).toInt();
                ids<<id;
            }
            if(id==-1)
            {
                return false;
            }
            else
            {
                //MeasureDB::simpleDataTb表格中一个杆号可能存在多条其定位点的数据，判别，并删除多余的,留下最后一条
                if(ids.count()>1)
                {
                    for(int i=0;i<ids.count();i++)
                    {
                        qDebug()<<QString("delete simple multiple record,id=%1,pole =%2,").arg(ids.at(i)).arg(poleNum)<<deleteSimpleRecord(ids.at(i));
                    }
                    return false;
                }
                else
                    return true;
            }
        }
        else
            return false;
    }
}
//函数功能：更新跨中数据
void recordMeasureForm::updateAllMidWireWidth(void)
{
    if(tbPoles.count())
    {
        for(int i=0;i<tbPoles.count();i++)
        {
            //            qDebug()<<"mid wire width "<<i<<tbPoles.at(i).first<<tbPoles.at(i).second.first<<tbPoles.at(i).second.second<<getMidWire(tbPoles.at(i).second.first);

            int idIndex=getMidWire(tbPoles.at(i).second.first);
            MeasureIdEx_t  dval;
            memset(&dval.onePolePar,0,sizeof(OnePolePar));
            if(idIndex!=-1)
            {
                if(!getDetailData(idIndex,dval))
                {
                    return ;
                }
            }
            else
            {
                return ;
            }
            dval.poleid=tbPoles.at(i).second.second;
            //            qDebug()<<"get mid wire info"<<i<<dval.poleid<<dval.nWalkDistance<<idIndex;
            if(indexMidWireWidth(tbPoles.at(i).second.second))
            {
                QSqlQuery query(model->database());
                QString strSql=QString( "update %1 set dwirewidth =%2,dwireHeight =%3,tm='%4',dist=%5,km=%6 where poleid ='%7' and locationremark='%8'")
                        .arg(MeasureDB::simpleDataTb)       //1 table name
                        .arg(dval.onePolePar.dWireWidth)    //2 wire width
                        .arg(dval.onePolePar.dWireHeight)   //3 wire height
                        .arg(dval.tm)                       //4 tm
                        .arg(dval.nWalkDistance)            //5 dist
                        .arg(dval.dKm)                      //6 km
                        .arg(dval.poleid)                   //6 poleid
                        .arg(QString::fromWCharArray(L"跨中"));//
                //                            qDebug()<<"update wire widht sql"<<strSql<<
                //                    qDebug()<<"update mid wire "<<strSql<<
                query.exec(strSql);
            }
            else
            {
                dval.locationRemark=QString::fromWCharArray(L"跨中");
                //                qDebug()<<"insert mid wire "<<dval.poleid<<
                insertRecord(dval);
            }
        }
    }
}
//函数功能：根据当前定位点的里程值，返回对应跨中测量中在detail表各种所对应的记录的id号
int recordMeasureForm::getMidWire(double curDist)
{
    double midDistance=0.0;
    if(prePoleDist.first)
    {
        QSqlQuery query(model->database());
        QString strSql=QString("select dist from %1 order by dist").arg(MeasureDB::detailDataTb);
        //        qDebug()<<"get first distance ,str sql"<<strSql;
        if(query.exec(strSql))
        {
            while (query.next())
            {
                prePoleDist.second=query.value(0).toDouble();
                prePoleDist.first=false;
                break;
            }
        }
    }
    midDistance=abs(curDist-prePoleDist.second)/2.0+prePoleDist.second;
    //    qDebug()<<"mid distan"<<midDistance;
    if(allDist.count())
    {
        for(int i=0;i<allDist.count()-1;i++)
        {
            if((midDistance>allDist.at(i).second)&&((midDistance<allDist.at(i+1).second)))
            {
                prePoleDist.second=curDist;
                if(abs(midDistance-allDist.at(i).second)>abs(midDistance-allDist.at(i+1).second))
                {
                    return allDist.at(i+1).first;
                }
                else
                {
                    return allDist.at(i).first;
                }
            }
        }
    }
    return -1;
}
//函数功能：刷新定位点、跨中及吊弦数据
void recordMeasureForm::on_pbtnUpdate_clicked()
{
    if(model)
    {
        updateAllWireWidth();//1更新所有定位点参数
        updateAllMidWireWidth();//2更新跨中数据
        updateSimpleTableId();
        emit showMessage(QString::fromWCharArray(L"数据已刷新!!!") );
    }
    else
    {
        emit showMessage(QString::fromWCharArray(L"数据未刷新!!!") );
    }
}

int recordMeasureForm::getRecordCount(QString tb)
{
    QSqlQuery query(model->database());
    QString strSql=QString("select count(id) from %1").arg(tb);
    int count=0;
    if(query.exec(strSql))
    {
        while(query.next())
        {
            count=query.value(0).toInt();
        }
    }
    return count;
}
void recordMeasureForm::updateSimpleTableId(void)
{
    int count=getRecordCount(MeasureDB::simpleDataTb);
    if(count)
    {
        QSqlQuery query(model->database());

        QString strSql=QString("select id,dist from  %1 order by dist").arg(MeasureDB::simpleDataTb);
        QVector<double> ids;
        if(query.exec(strSql))
        {
            while(query.next())
            {
                ids<<query.value(1).toDouble();
                //                qDebug()<<"cur id"<<query.value(0).toInt()<<query.value(1).toDouble();
            }
        }
        query.finish();
        for(int i=0;i<ids.count();i++)
        {
            strSql=QString("update %1 set id=%2 where dist= %3").arg(MeasureDB::simpleDataTb).arg(i+100).arg(ids.at(i));
            query.exec(strSql);
        }
        for(int i=0;i<ids.count();i++)
        {
            strSql=QString("update %1 set id=%2 where dist= %3").arg(MeasureDB::simpleDataTb).arg(i+1).arg(ids.at(i));
            //            qDebug()<<"update id"<<i<<strSql<<
            query.exec(strSql);
        }
    }
    else
    {
        qDebug()<<"simple table is null";
    }
}
void  recordMeasureForm:: clearTable(void)
{
    ui->pbtnDataReplay->setEnabled(false);
    tableView->setModel(NULL);
    tableView->repaint();
    //    model=NULL;

}
void recordMeasureForm::slot_DisplayData(void)
{
    QRadioButton *chb=(QRadioButton *)this->sender();
    //    if(chb==ui->rdbAll)
    //    {

    //    }
    //    else if(chb==ui->rdbLimiteWire)
    //    {

    //    }
    //    else if(chb==ui->rdbMidWire)
    //    {

    //    }
    //    else if(chb==ui->rdbOverPar)
    //    {

    //    }

    if(ui->rdbAll->isChecked())
    {
        detailTbFilter="";//<<QString("LocationRemark = '%1'").arg(QString::fromWCharArray(L"吊弦"));
    }
    else if(ui->rdbLimiteWire->isChecked())
    {
        detailTbFilter=QString("dataType = '%1' ").arg(QString::fromWCharArray(L"定位点"));
    }
    else if(ui->rdbMidWire->isChecked())
    {
        detailTbFilter=QString("dataType ='%1' ").arg(QString::fromWCharArray(L"跨中"));
    }
    else if(ui->rdbOverPar->isChecked())
    {
        detailTbFilter=QString("dataType like'%%1' ").arg(QString::fromWCharArray(L"超限"));
    }

    showDataTable(MeasureDB::detailDataTb);
    qDebug()<<"filter value"<<detailTbFilter;
}
bool  recordMeasureForm::eventFilter(QObject *o, QEvent *e )
{
    if((e->type()==QEvent::Resize))
    {

        int reH=ui->grpTable->size().height()-ui->lblFile->size().height();
        qDebug()<<"record ui size"<<this->size()<<ui->grpTable->geometry()<<ui->tableView->geometry()<<"re height"<<reH;
        if(reH>0)
            ui->tableView->resize(QSize(ui->tableView->size().width(),reH));
        qDebug()<<"replace record ui size"<<this->size()<<ui->grpTable->geometry()<<ui->tableView->geometry();
    }
    return true;
}
