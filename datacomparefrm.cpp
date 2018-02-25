#include "datacomparefrm.h"
#include "ui_datacomparefrm.h"

dataCompareFrm::dataCompareFrm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dataCompareFrm)
{
    ui->setupUi(this);
}

dataCompareFrm::~dataCompareFrm()
{
    delete ui;
}
