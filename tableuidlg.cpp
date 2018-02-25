#include "tableuidlg.h"
#include "ui_tableuidlg.h"

tableUIDlg::tableUIDlg(QString title,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tableUIDlg)
{
    ui->setupUi(this);
    titleName=title;
    check.clear();
    check<<ui->chb1;
    check<<ui->chb2;
    check<<ui->chb3;
    check<<ui->chb4;
    check<<ui->chb5;
    check<<ui->chb6;
    check<<ui->chb7;
    check<<ui->chb8;
    check<<ui->chb9;
    check<<ui->chb10;
    check<<ui->chb11;
    check<<ui->chb12;
    for(int i=1;i<titleName.split(",").count();i++)
    {
        QString tmp=titleName.split(",").at(i);
        if(tmp.right(1).toInt())
        {
            check.at(i-1)->setCheckState(Qt::Checked);
        }
        else
        {
            check.at(i-1)->setCheckState(Qt::Unchecked);
        }
    }
}

tableUIDlg::~tableUIDlg()
{
    delete ui;
}
void tableUIDlg::on_pbtnOk_clicked()
{
    QStringList titleList=titleName.split(",");
    for(int i=1;i<titleList.count();i++)
    {
        QString tmp=titleList.at(i);
        if(check.at(i-1)->checkState()==Qt::Checked)
        {
            tmp.replace(tmp.count()-1,1,"1");
            titleList.replace(i,tmp);
        }
        else
        {
            tmp.replace(tmp.count()-1,1,"0");
            titleList.replace(i,tmp);
        }
    }
    titleName=titleList.join(",");
    accept();
}

void tableUIDlg::on_chbCancel_clicked()
{
    reject();
}
