#include "verifysetdlg.h"
#include "ui_verifysetdlg.h"

verifySetDlg::verifySetDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::verifySetDlg)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromWCharArray(L"请选择需标定参数"));
//    ui->chbRailHeight->setCheckState(Qt::Checked);
//    ui->chbRailWidth->setCheckState(Qt::Checked);
    ui->chbWireHeight->setCheckState(Qt::Checked);
    ui->chbWireWidth->setCheckState(Qt::Checked);
    verifyValue=NO_VERIFY;
}

verifySetDlg::~verifySetDlg()
{
    delete ui;
}

void verifySetDlg::on_pbtnOk_clicked()
{
    verifyValue=0;
//    if(ui->chbRailHeight->checkState()==Qt::Checked)
//    {
//        verifyValue+=RAIL_HEIGHT;
//    }
//    if(ui->chbRailWidth->checkState()==Qt::Checked)
//    {
//        verifyValue+=RAIL_WIDTH;
//    }
    if(ui->chbWireHeight->checkState()==Qt::Checked)
    {
        verifyValue+=WIRE_HEIGHT;
    }
    if(ui->chbWireWidth->checkState()==Qt::Checked)
    {
        verifyValue+=WIRE_WIDTH;
    }
    if(ui->chbSide->checkState()==Qt::Checked)
    {
        verifyValue+=RAIL_SIDE;
    }
    accept();
}

void verifySetDlg::on_chbCancel_clicked()
{
    verifyValue=NO_VERIFY;
    reject();

}
