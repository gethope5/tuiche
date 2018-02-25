#include "dlgdrawstly.h"
#include "ui_dlgdrawstly.h"
#include <QDebug>
dlgDrawStly::dlgDrawStly(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgDrawStly)
{
    ui->setupUi(this);
    QString strBuffer=QTextCodec::codecForName("gbk")->toUnicode("选择测量方式...");
    this->setWindowTitle(strBuffer);
    ui->rdSimulation->setChecked(true);


    QPixmap map1("./images/line.png");
    ui->rdSimulation->setIcon(QIcon(map1));
    QPixmap map("./images/dot.png");
    ui->radioButton_2->setIcon(QIcon(map));
    connect(ui->pbtnCancel,SIGNAL(clicked()),this,SLOT(reject()));
    bDrawStyle=false;
}
dlgDrawStly::~dlgDrawStly()
{
    delete ui;
}

void dlgDrawStly::on_pbtnOK_clicked()
{
    bDrawStyle=ui->radioButton_2->isChecked();
    this->accept();
}
