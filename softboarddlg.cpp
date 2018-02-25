#include "softboarddlg.h"
#include "ui_softboarddlg.h"

softBoardDlg::softBoardDlg(QWidget *parent):QDialog(parent),
    ui(new Ui::softBoardDlg)
{
    ui->setupUi(this);
    m = new QSignalMapper(this);
    connect(ui->pbtn1,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtn2,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtn3,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtn4,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtn5,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtn6,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtn7,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtn8,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtn9,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtn0,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtnDel,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->pbtnSign,SIGNAL(clicked()),m,SLOT(map()));

    connect(ui->pbtnOK,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->pbtnCancel,SIGNAL(clicked()),this,SLOT(reject()));

    m->setMapping(ui->pbtn0,0);
    m->setMapping(ui->pbtn1,1);
    m->setMapping(ui->pbtn2,2);
    m->setMapping(ui->pbtn3,3);
    m->setMapping(ui->pbtn4,4);
    m->setMapping(ui->pbtn5,5);
    m->setMapping(ui->pbtn6,6);
    m->setMapping(ui->pbtn7,7);
    m->setMapping(ui->pbtn8,8);
    m->setMapping(ui->pbtn9,9);
    m->setMapping(ui->pbtnDel,10);
    m->setMapping(ui->pbtnSign,-1);

    connect(m,SIGNAL(mapped(int)),this,SLOT(keyPress(int)));
    ui->pbtn1->setFocus();

    connect(ui->pbtnOK,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->pbtnCancel,SIGNAL(clicked()),this,SLOT(reject()));
}
void softBoardDlg::setCustomLabel(QString labelBuffer)
{
    ui->pbtnSign->setText(labelBuffer);
}
QString softBoardDlg::customLabel(void)
{
    return ui->pbtnSign->text();
}
softBoardDlg::~softBoardDlg()
{
    delete ui;
}
void softBoardDlg::keyPress(int i)
{
    emit keyNum(i);
}
