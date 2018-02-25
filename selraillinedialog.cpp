#include "selraillinedialog.h"
#include "ui_selraillinedialog.h"

SelRailLineDialog::SelRailLineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelRailLineDialog)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    pbkGrnd = new QLabel(this);
    pbkGrnd->setPixmap(QPixmap("res/backgrnddlg.png"));
    pbkGrnd->setScaledContents(true);

    ui->setupUi(this);    
    pbkGrnd->resize(size().width(), size().height());

    connect(ui->pbtnEnter, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->pbtnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}
//idx = -1 表示现在没有选定表格
void SelRailLineDialog::setRailList(QStringList &list, int idx)
{
    qDebug()<<"I got the railist "<<list;
    ui->rlCBox->addItems(list);    
    ui->rlCBox->setCurrentIndex(idx == -1 ? 0 : idx);
}

int SelRailLineDialog::getselIndx()
{
    return ui->rlCBox->currentIndex();
}

SelRailLineDialog::~SelRailLineDialog()
{
    delete ui;
}
