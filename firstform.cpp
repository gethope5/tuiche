#include "firstform.h"
#include "ui_firstform.h"

firstForm::firstForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::firstForm)
{
    ui->setupUi(this);

    //    this->setStyleSheet("background-color: rgb(85, 170, 255);");
    ui->lblTitle->setAttribute(Qt::WA_TranslucentBackground, true);//设置透明2-窗体标题栏不透明,背景透明
    readConfigerFile();
//    qDebug()<<"first ui size"<<this->size();
}
firstForm::~firstForm()
{
    delete ui;
}
void firstForm::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    e->accept();
    QImage imgBackGround;
    imgBackGround.load("./images/uiMain.png");
    painter.drawImage(this->rect(),imgBackGround);
}
void firstForm::readConfigerFile(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);

    //设置软件中文标题
    QByteArray titleBuffer=settings.value("system/title").toByteArray();

    if(titleBuffer.isEmpty())
    {
        //        qDebug()<<"cc3";
        return;
    }
    else
    {
        QString  strutf = QTextCodec::codecForName("gbk")->toUnicode(settings.value("system/title").toString().toLatin1());
        //        QString   labelName=QTextCodec::codecForName("system")->toUnicode(settings.value("System/labelName",tr("时代f铁人")).toByteArray());
        //        this->setWindowTitle(/*QString("guiyang")+*/strutf);
        //        setTitle(strutf);
//        ui->lblTitle->setText(QString::fromWCharArray(L"高速铁路接触网智能巡检系统"));
        ui->lblTitle->setText(strutf);
    }
}
void  firstForm::resizeEvent(QResizeEvent *event)
{
    QSize dd=this->size();
    ui->lblTitle->move((dd.width()-ui->lblTitle->width())/2,dd.height()*0.2);
}
