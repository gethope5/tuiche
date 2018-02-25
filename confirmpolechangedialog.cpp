#include "confirmpolechangedialog.h"
#include "ui_confirmpolechangedialog.h"

ConFirmPoleChangeDialog::ConFirmPoleChangeDialog(QString currPole,QString topole, QWidget *parent) :
    QDialog(parent),m_topole(topole),
    ui(new Ui::ConFirmPoleChangeDialog)
{
    ui->setupUi(this);
    changePole(currPole);

    connect(ui->Ok, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->Cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

ConFirmPoleChangeDialog::~ConFirmPoleChangeDialog()
{
    delete ui;
}


void ConFirmPoleChangeDialog::changePole(QString currpole)
{

    QString str = QTextCodec::codecForName("gbk")->toUnicode("<html>是否确认将当前支柱号: "
                             "<span style=\" font-weight:600; color:#0000ff;\">%1"
                             "</span> 修改成: <span style=\" font-weight:600; color:#0000ff;\">%2"
                             "</span> ?</html>").arg(currpole).arg(m_topole);
    ui->textLabel->setText(str);
}


