#include "modelpoledlg.h"
#include "ui_modelpoledlg.h"

modelPoleDlg::modelPoleDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modelPoleDlg)
{
    ui->setupUi(this);
    initialUI(false);
    setWindowTitle(QString::fromWCharArray(L"杆号设置..."));
}
int modelPoleDlg::getDeleteID(void)
{
    return ui->lneDeleteId->text().toInt();
}
void modelPoleDlg::initialUI(bool f)
{
    if(f)
    {
        ui->gbPoleDelete->setEnabled(true);
        ui->gbPoleModify->setEnabled(false);
    }
    else
    {
        ui->gbPoleDelete->setEnabled(false);
        ui->gbPoleModify->setEnabled(true);
    }
}
modelPoleDlg::~modelPoleDlg()
{
    delete ui;
}
void modelPoleDlg::modifyPoleInfo(int &startId,int &endId,int &pole)
{
    startId=ui->lneStart->text().toInt();
    endId=ui->lneEnd->text().toInt();
    pole=ui->lneToPole->text().toInt();
}
void modelPoleDlg::on_pbtnOk_clicked()
{
    accept();
}

void modelPoleDlg::on_pbtnCancel_clicked()
{
    reject();
}
