#include "prenewrailfrm.h"
#include "ui_prenewrailfrm.h"

preNewRailFrm::preNewRailFrm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::preNewRailFrm)
{
    ui->setupUi(this);
    ui->cmbDirection->addItem(QTextCodec::codecForName("gbk")->toUnicode("上行"));
    ui->cmbDirection->addItem(QTextCodec::codecForName("gbk")->toUnicode("下行"));
//    ui->wdgPoleNum->setLblName(QTextCodec::codecForName("gbk")->toUnicode("初始杆号:"));//    ui->lneName->setText(QTextCodec::codecForName("gbk")->toUnicode("临时1"));
    ui->spbRailName->setPrefix(QTextCodec::codecForName("gbk")->toUnicode("临时"));
    ui->spbRailName->setMinimum(0);
    ui->spbStationName->setPrefix(QTextCodec::codecForName("gbk")->toUnicode("站区"));
    ui->spbStationName->setMinimum(1);

//    readSetting();
//    ui->wdgPoleNum->setValue("0");
}
void preNewRailFrm::newRailInfo(RunProfile &profile)
{
    profile.direction=ui->cmbDirection->currentIndex();//->currentText();
//    profile.
    profile.lineName=ui->spbRailName->text();
    profile.station=ui->spbStationName->text();
//    profile.prevPole=ui->wdgPoleNum->getValue().toInt();
//    qDebug()<<"check status="<<ui->rdbNewRail->isChecked()<<ui->rdbOldRail->isChecked();
//    newRailInitialPole=ui->wdgPoleNum->getValue();
}
preNewRailFrm::~preNewRailFrm()
{
    delete ui;
}
QString preNewRailFrm::railName(void)
{
    return ui->spbRailName->prefix()+QString::number(ui->spbRailName->value());
}
QString preNewRailFrm::railDirection(void)
{
    return ui->cmbDirection->currentText();
}
 void preNewRailFrm::on_spbRailName_valueChanged(const QString &arg1)
{
    emit railInfoChanged(true);
}

void preNewRailFrm::on_cmbDirection_activated(int index)
{
    emit railInfoChanged(true);
}
