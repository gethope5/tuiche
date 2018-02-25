#include "newrailpolefrm.h"
#include "ui_newrailpolefrm.h"
#include <QDebug>
newRailPoleFrm::newRailPoleFrm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::newRailPoleFrm)
{
    ui->setupUi(this);
    ui->wdgPoleNum->setLblName(QTextCodec::codecForName("gbk")->toUnicode("杆号:"));
    connect(ui->rdbAdd,SIGNAL(clicked(bool)),this,SLOT(slot_poleDirection(bool)));
    connect(ui->rdbReduce,SIGNAL(clicked(bool)),this,SLOT(slot_poleDirection(bool)));
//    slot_initialPole(bMeasureType,"0");
    ui->rdbAdd->setChecked(true);
    readConfig();
}
void newRailPoleFrm:: readConfig(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    if(settings.value("system/poleType", "true").toBool())
    {
        ui->pbtnAddPole->setEnabled(true);
    }
    else
    {
        ui->pbtnAddPole->setEnabled(false);
    }
}
newRailPoleFrm::~newRailPoleFrm()
{
    delete ui;
}
void newRailPoleFrm::updateNumber(QString sPoleNumber)
{
     ui->wdgPoleNum->setValue(sPoleNumber);
     MeasureDB::nPoleNum=sPoleNumber.toInt();
}
void newRailPoleFrm::initialUI(bool b)
{
    ui->wdgPoleNum->setEnabled(b);
    ui->pbtnAddPole->setEnabled(b);
    ui->rdbAdd->setEnabled(b);
    ui->rdbReduce->setEnabled(b);
//    ui->label->setEnabled(b);
}
void newRailPoleFrm::on_pbtnAddPole_clicked()
{
    poleChanged();
}
void newRailPoleFrm::slot_newPole(void)
{
    poleChanged();
}
void newRailPoleFrm::poleChanged(void)
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
void newRailPoleFrm::slot_poleDirection(bool b)
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
