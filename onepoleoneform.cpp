#include "onepoleoneform.h"
#include "ui_onepoleoneform.h"

onePoleOneForm::onePoleOneForm(QString labelName,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::onePoleOneForm)
{
    ui->setupUi(this);
    isMeasure=false;
    bOpenFile=false;
    connect(ui->pbtnFirst,SIGNAL(clicked()),this,SLOT(slot_indexChanged()));
    connect(ui->pbtnNext,SIGNAL(clicked()),this,SLOT(slot_indexChanged()));
    connect(ui->pbtnLast,SIGNAL(clicked()),this,SLOT(slot_indexChanged()));
    connect(ui->pbtnBefore,SIGNAL(clicked()),this,SLOT(slot_indexChanged()));
    ui->lneABSHeight->setEnabled(false);
    ui->lneRailName->setEnabled(false);
    ui->lneStation->setEnabled(false);
    ui->lnePole->setEnabled(false);
    ui->lneMaxWireHeight->setEnabled(false);
    ui->lneMinWireHeigh->setEnabled(false);
    ui->lneWireWidth->setEnabled(false);
    ui->lneDirection->setEnabled(false);
    ui->lblPicture->setText(labelName);
//    ui->lneDirection->setPalette();
    this->setStyleSheet("QPushButton{font: 900 13pt \"微软雅黑\"}QLabel{font: 900 12pt \"微软雅黑\"}");
}
onePoleOneForm::~onePoleOneForm()
{
    delete ui;
}
void onePoleOneForm::slot_indexChanged(void)
{
    if(onePoleInfo.count())
    {
        QPushButton *getActSender=(QPushButton*)this->sender();
        if(getActSender==ui->pbtnBefore)
        {
            curIndex--;
        }
        else if(getActSender==ui->pbtnFirst)
        {
            curIndex=0;
        }
        else if(getActSender==ui->pbtnNext)
        {
            curIndex++;
        }
        else if(getActSender==ui->pbtnLast)
        {
            curIndex=onePoleInfo.count()-1;
        }
        if(curIndex<0)
        {
            QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("第一个支柱"));
            curIndex=0;
        }
        if(curIndex>=onePoleInfo.count())
        {
            curIndex=onePoleInfo.count()-1;
            QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("最后一个支柱"));
        }
        update_display(onePoleInfo.at(curIndex),true);
    }
    else
        QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("未包含测量信息..."));
}
void onePoleOneForm::update_display(MeasureIdEx_t curMeaExData,bool bOpenFile)
{
    if(!bOpenFile)
    {
        qDebug()<<"rail name"<<curMeaExData.railwayName<<curMeaExData.PicPath;
        ui->lneRailName->setText(curMeaExData.railwayName);
        ui->lneStation->setText(curMeaExData.station);
    }
    ui->lneDirection->setText(curMeaExData.direction);
    ui->lnePole->setText(curMeaExData.poleid);
//    ui->lneMaxWireHeight->setText(QString::number(curMeaExData.maxdaogao));
//    ui->lneMinWireHeigh->setText(QString::number(curMeaExData.mindaogao));
//    ui->lneWireWidth->setText(QString::number(curMeaExData.maxLachuzhi));
    ui->lneABSHeight->setText(QString::number(curMeaExData.onePolePar.gaocha));
    ui->lblPicture->setPixmap(QPixmap::fromImage(QImage(QString(curMeaExData.PicPath.data()).replace("\\\\","/"))));
    update();
}

void onePoleOneForm::on_pbtnOpen_clicked()
{
    measDataFilepath = QFileDialog::getOpenFileName(this, QTextCodec::codecForName("gbk")->toUnicode("加载数据文档"), "", QTextCodec::codecForName("gbk")->toUnicode("db文档 (*.db)"));
    if (measDataFilepath.isEmpty())
    {
        return;
    }

    if(Opendatabase(measDataFilepath))
    {
        getRailInfo();
        getDBValue();
        bOpenFile=true;
    }
}
//函数功能：通过文件名，获取线路信息
void onePoleOneForm::getRailInfo(void)
{
    if(measDataFilepath.contains(QTextCodec::codecForName("gbk")->toUnicode("行")))
    {

        QString fileName=measDataFilepath.right(measDataFilepath.count()-measDataFilepath.lastIndexOf("/")-1);
        if(fileName.contains(QTextCodec::codecForName("gbk")->toUnicode("上行")))
        {
            sRailName=fileName.left(fileName.indexOf(QTextCodec::codecForName("gbk")->toUnicode("上行")));
            sDirection=QTextCodec::codecForName("gbk")->toUnicode("上行");
        }
        else if(fileName.contains(QTextCodec::codecForName("gbk")->toUnicode("下行")))
        {
            sRailName=fileName.left(fileName.indexOf(QTextCodec::codecForName("gbk")->toUnicode("下行")));
            sDirection=QTextCodec::codecForName("gbk")->toUnicode("下行");
        }
        ui->lneRailName->setText(sRailName);
        ui->lneDirection->setText(sDirection);
    }
    else
        QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("未包含线路信息..."));;
}
void onePoleOneForm::getDBValue(void)
{
    //    QList<MeasureIdEx_t> onePoleInfo;
    QSqlQuery query(m_db);
    QString sqlBuffer("select * from measureex");


    if(query.exec(sqlBuffer))
    {
        onePoleInfo.clear();

        MeasureIdEx_t dval;
        while(query.next())
        {
            dval.id=query.value(0).toInt();;
            dval.tm=query.value(1).toString();
            dval.nWalkDistance=query.value(2).toFloat();
            dval.station=query.value(3).toString();
            dval.poleid=query.value(4).toString();
//            dval.maxdaogao=query.value(5).toFloat();
//            dval.mindaogao=query.value(6).toFloat();
            dval.onePolePar.gaocha=query.value(7).toFloat();
//            dval.maxLachuzhi=query.value(8).toFloat();
            dval.locationRemark=query.value(9).toString();
            dval.PicPath=query.value(10).toByteArray();
            onePoleInfo.push_back(dval);
        }
    }
}
bool onePoleOneForm::Opendatabase(const QString  &filepath)
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
