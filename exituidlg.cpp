#include "exituidlg.h"
#include "ui_exituidlg.h"

exitUIDlg::exitUIDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::exitUIDlg)
{
    ui->setupUi(this);
    QString strBuffer=QTextCodec::codecForName("gbk")->toUnicode("提示");
    this->setWindowTitle(strBuffer);
    this->setStyleSheet("font:900 12pt \"新宋体\";");
    QImage dd("./images/warning.png");

    ui->label_2->setPixmap(QPixmap::fromImage(dd.scaled(ui->label_2->size())));
}
exitUIDlg::~exitUIDlg()
{
    delete ui;
}
bool exitUIDlg::sleepSys(void)
{
    if(ui->checkBox->checkState()==Qt::Checked)
        return true;
    else
        return false;
}

void exitUIDlg::on_checkBox_clicked()
{
    if(ui->checkBox->checkState()==Qt::Checked)
    {
        QString strBuffer=QTextCodec::codecForName("gbk")->toUnicode("提示...");
        if(QMessageBox::Ok==QMessageBox::information(this,strBuffer,QTextCodec::codecForName("gbk")->toUnicode("选中该项，设备传感器将进入待机模块，确定选择?"),QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel))
        {
            ui->checkBox->setCheckState(Qt::Checked);
        }
        else
            ui->checkBox->setCheckState(Qt::Unchecked);
    }
}
