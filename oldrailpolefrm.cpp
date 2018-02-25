#include "oldrailpolefrm.h"
#include "ui_oldrailpolefrm.h"

oldRailPoleFrm::oldRailPoleFrm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::oldRailPoleFrm)
{
    ui->setupUi(this);
    connect(ui->listViewPolelist, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slots_railPoleClicked(QListWidgetItem*)));
    this->setFocusPolicy(Qt::NoFocus);
//    ui->listViewPolelist->setEnabled(false);
}

oldRailPoleFrm::~oldRailPoleFrm()
{
    delete ui;
}
void oldRailPoleFrm::updateNumber(QString sPoleNumber)
{
//     ui->wdgPoleNum->setValue(sPoleNumber);
}
void oldRailPoleFrm::fillPoleSelList(int id )
{
    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
    m_currIdx=id;
    int cnt = 0;
    clearOldPoleDipslay();
    //    qDebug()<<"`````````````the id = "<<id;
    for (int i = id-2; i < id+3 && i < polesets.size(); i++)
    {
        if (i < 0)
            continue;
        curDisplayPoles.append(polesets[i].poleID);
        ui->listViewPolelist->addItem(polesets[i].poleID);
        if (id == i)
        {
            ui->listViewPolelist->item(cnt)->setBackground(QBrush(QColor(0,255,0)));
            curDisplayPole=ui->listViewPolelist->item(cnt)->text();
//            qDebug()<<"the cnt = "<<cnt<<curDisplayPole;
        }
        cnt++;
    }
//    emit passedPoleId(polesets[m_currIdx].poleID);
}
void oldRailPoleFrm::slots_railPoleClicked(QListWidgetItem*item)
{
    QString toPole = item->text();
    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
    //labelPoleInfo->setText(item->text());
    ConFirmPoleChangeDialog dlg(polesets[m_currIdx].poleID, toPole, this);
//    connect(this, SIGNAL(passedPoleId(QString)), &dlg, SLOT(changePole(QString)));
    if (dlg.exec())
    {
//        qDebug()<<" I should change !"<<m_currIdx;
        slot_changePoleId( toPole);
        emit changingPole(QString::number(m_currIdx));
    }
}
//函数功能：手动干预杆号信息，重载listwidget中的杆号信息，快捷键
void oldRailPoleFrm::slots_railPoleClicked(QString toPole)
{
    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
    //labelPoleInfo->setText(item->text());
    ConFirmPoleChangeDialog dlg(polesets[m_currIdx].poleID, toPole, this);
//    connect(this, SIGNAL(passedPoleId(QString)), &dlg, SLOT(changePole(QString)));
    if (dlg.exec())
    {
//        qDebug()<<" I should change !"<<m_currIdx;
        slot_changePoleId( toPole);
        emit changingPole(QString::number(m_currIdx));
    }
}
void oldRailPoleFrm::slot_changePoleId(QString toPole)
{
    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
    int minval = (m_currIdx-20) > 0 ? (m_currIdx-20) : 0;
    qDebug()<<"before currid"<<m_currIdx;
    for (int i = minval; i < m_currIdx+20 && i < polesets.size(); i++)
    {
        if (polesets[i].poleID == toPole)
        {
            if (i != m_currIdx)
            {
                m_currIdx = i;
                Global::getInstance()->changedPos.push_back(walkDistance );
                qDebug()<<"changed pos: "<<walkDistance <<m_currIdx;
//                stationChanged();
                fillPoleSelList(m_currIdx);
                return;
            }
            break;
        }
    }
}
void oldRailPoleFrm::clearOldPoleDipslay(void)
{
    curDisplayPoles.clear();
    ui->listViewPolelist->clear();
}

