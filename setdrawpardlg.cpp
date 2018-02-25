#include "setdrawpardlg.h"
#include "ui_setdrawpardlg.h"

setDrawParDlg::setDrawParDlg(QWidget *parent) :
    QDialog(parent),
    bDrawStyle(false),
    ui(new Ui::setDrawParDlg)
{
    ui->setupUi(this);
    ui->wdgWireWidthMax->setLblName(QTextCodec::codecForName("gbk")->toUnicode("拉出值上限："));
    ui->wdgWireWidthMin->setLblName(QTextCodec::codecForName("gbk")->toUnicode("拉出值下限："));

    ui->wdgWireHeightMax->setLblName(QTextCodec::codecForName("gbk")->toUnicode("导高值上限："));
    ui->wdgWireHeightMin->setLblName(QTextCodec::codecForName("gbk")->toUnicode("导高值下限："));
    ui->wdgWalkMax->setLblName(QTextCodec::codecForName("gbk")->toUnicode("里程值上限："));
    ui->wdgWalkMin->setLblName(QTextCodec::codecForName("gbk")->toUnicode("里程值下限："));

    connect(ui->pbtnCancel,SIGNAL(clicked()),this,SLOT(reject()));
    initialPar();
    ui->rdbAxis->setChecked(true);
    setWindowTitle(QTextCodec::codecForName("gbk")->toUnicode("显示界面设置..."));


    getDisplayInitial();

    connect(ui->chbAlarm,SIGNAL(pressed()),this,SLOT(slot_curveVisiable()));
    connect(ui->chbAlarm_2,SIGNAL(pressed()),this,SLOT(slot_curveVisiable()));
    connect(ui->chbCarrier,SIGNAL(pressed()),this,SLOT(slot_curveVisiable()));
    connect(ui->chbDropper,SIGNAL(pressed()),this,SLOT(slot_curveVisiable()));
    connect(ui->chbPole,SIGNAL(pressed()),this,SLOT(slot_curveVisiable()));
    connect(ui->chbWire2,SIGNAL(pressed()),this,SLOT(slot_curveVisiable()));

    ui->chbWire2->setChecked(true);

    ui->chbAlarm_2->setEnabled(false);
    this->setFixedSize(this->size());
}
//函数功能：从配置文件中获取当前显示类型的状态，并对显示类型按钮进行初始化
void setDrawParDlg::getDisplayInitial(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    if(settings.value("ScanDiaplayColor/alarm1Display",1).toBool())
    {
        ui->chbAlarm->setCheckState(Qt::Checked);
    }
    else
    {
        ui->chbAlarm->setCheckState(Qt::Unchecked);
    }
    if(settings.value("ScanDiaplayColor/alarm2Display",1).toBool())
    {
        ui->chbAlarm_2->setCheckState(Qt::Checked);
    }
    else
    {
        ui->chbAlarm_2->setCheckState(Qt::Unchecked);
    }
    if(settings.value("ScanDiaplayColor/dropperDisplay",1).toBool())
    {
        ui->chbDropper->setCheckState(Qt::Checked);
    }
    else
    {
        ui->chbDropper->setCheckState(Qt::Checked);
    }
    if(settings.value("ScanDiaplayColor/poleDisplay",1).toBool())
    {
        ui->chbPole->setCheckState(Qt::Checked);
    }
    else
    {
        ui->chbPole->setCheckState(Qt::Unchecked);
    }
    if(settings.value("ScanDiaplayColor/carrierDisplay",1).toBool())
    {
        ui->chbCarrier->setCheckState(Qt::Checked);
    }
    else
    {
        ui->chbCarrier->setCheckState(Qt::Unchecked);
    }
}
setDrawParDlg::~setDrawParDlg()
{
    delete ui;
}
void setDrawParDlg::initialPar(bool flag)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    if(flag)
    {
        //载入坐标默认参数
        ui->wdgWireWidthMax->setValue(settings.value("axisPar/WireWidthMax").toString());
        ui->wdgWireWidthMin->setValue(settings.value("axisPar/WireWidthMin").toString());
        ui->wdgWireHeightMax->setValue(settings.value("axisPar/WireHeightMax").toString());
        ui->wdgWireHeightMin->setValue(settings.value("axisPar/WireHeightMin").toString());
        ui->wdgWalkMax->setValue(settings.value("axisPar/WalkMax").toString());
        ui->wdgWalkMin->setValue(settings.value("axisPar/WalkMin").toString());
    }
    else
    {
        ui->wdgWireWidthMax->setValue(settings.value("axisPar/defWireWidthMax").toString());
        ui->wdgWireWidthMin->setValue(settings.value("axisPar/defWireWidthMin").toString());
        ui->wdgWireHeightMax->setValue(settings.value("axisPar/defWireHeightMax").toString());
        ui->wdgWireHeightMin->setValue(settings.value("axisPar/defWireHeightMin").toString());
        ui->wdgWalkMax->setValue(settings.value("axisPar/defWalkMax").toString());
        ui->wdgWalkMin->setValue(settings.value("axisPar/defWalkMin").toString());
    }
}
void setDrawParDlg::on_pbtnSetWireColor_clicked()
{
    if(ui->rdbDisplayStyle->isChecked())
    {
        dlgDrawStly dlgSetDrawStlye(this);
        if(dlgSetDrawStlye.exec())
        {
            bDrawStyle=dlgSetDrawStlye.bDrawStyle;
        }
    }
    else
    {
        QColorDialog *setColor=new QColorDialog(ui->pbtnSetWireColor);
        setColor->show();
        connect(setColor,SIGNAL(colorSelected(QColor)),this,SLOT(setScanColor_solt(QColor)));
    }
}
//函数功能：颜色设置
void setDrawParDlg::setScanColor_solt(QColor curColor)
{
    curveType type;
    //1
    if(ui->rdbAxis->isChecked())
    {
        type=curveType::axisColor;
    }
    //2
    else if(ui->rdbBackground->isChecked())
    {
        type=curveType::backgroundColor;
    }
    //3 接触线1颜色
    else if(ui->rdbCurve->isChecked())
    {
        type=curveType::wire1Color;
    }
    //4 接触线2颜色
    else if(ui->rdbAlarm->isChecked())
    {
        //type=curveType::Alarm1;
        type=curveType::wire2Color;
    }
    //5 杆号颜色
    else if (ui->rdbAlarm2->isChecked())
    {
//        type=curveType::Alarm2;
        type=curveType::poleColor;
    }
    //6 吊弦颜色
    else if (ui->rdbDropper->isChecked())
    {
        type=curveType::dropperColor;
    }
    //7 承力索颜色
    else if (ui->rdbCarrier->isChecked())
    {
        type=curveType::carrierColor;
    }

    emit colorChanged(type,curColor);
}
void setDrawParDlg::on_pbtnSaveBasePar_clicked()
{
    if(QMessageBox::question(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("确定保存参数?"),QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok);
    {
        QSettings  settings("Config.ini", QSettings::IniFormat);
        //3坐标系统参数设置
        //3.1
        int nBuffer=ui->wdgWireWidthMax->getValue().toInt();
        if(nBuffer)
        {
            settings.setValue("axisPar/WireWidthMax", nBuffer);
        }
        //3.2
        nBuffer=ui->wdgWireWidthMin->getValue().toInt();
        if(nBuffer)
        {
            settings.setValue("axisPar/WireWidthMin", nBuffer);
        }
        //3.3
        nBuffer=ui->wdgWireHeightMax->getValue().toInt();
        if(nBuffer)
        {
            settings.setValue("axisPar/WireHeightMax", nBuffer);
        }
        //3.4
        nBuffer=ui->wdgWireHeightMin->getValue().toInt();
        if(nBuffer)
        {
            settings.setValue("axisPar/WireHeightMin", nBuffer);
        }
        //3.5
        nBuffer=ui->wdgWalkMax->getValue().toInt();
        if(nBuffer)
        {
            settings.setValue("axisPar/WalkMax", nBuffer);
        }
        //3.6
        nBuffer=ui->wdgWalkMin->getValue().toInt();
        //        if(nBuffer)
        {
            settings.setValue("axisPar/WalkMin", nBuffer);
        }
        //1
        if(ui->chbAlarm->isChecked())
        {
            settings.setValue("ScanDiaplayColor/alarm1Display",1);
        }
        else
        {
            settings.setValue("ScanDiaplayColor/alarm1Display",0);
        }
        //2
        if(ui->chbAlarm_2->isChecked())
        {
            settings.setValue("ScanDiaplayColor/alarm2Display",1);
        }
        else
        {
            settings.setValue("ScanDiaplayColor/alarm2Display",0);
        }
        //3
        if(ui->chbCarrier->isChecked())
        {
            settings.setValue("ScanDiaplayColor/carrierDisplay",1);
        }
        else
        {
            settings.setValue("ScanDiaplayColor/carrierDisplay",0);
        }
        //4
        if(ui->chbDropper->isChecked())
        {
            settings.setValue("ScanDiaplayColor/dropperDisplay",1);
        }
        else
        {
            settings.setValue("ScanDiaplayColor/dropperDisplay",0);
        }
        //5
        if(ui->chbPole->isChecked())
        {
            settings.setValue("ScanDiaplayColor/poleDisplay",1);
        }
        else
        {
            settings.setValue("ScanDiaplayColor/poleDisplay",0);
        }

        emit curveUIParChanged();
        accept();
    }
}
void setDrawParDlg::slot_curveVisiable(void)
{
    QCheckBox *chb=(QCheckBox *)this->sender();
    curveType type;
    bool flag;
    if(chb==ui->chbAlarm)
    {
        if(ui->chbAlarm->checkState()==Qt::Unchecked)
        {
            flag=true;
        }
        else
        {
            flag=false;
        }
        type=curveType::Alarm1;
    }
    else if(chb==ui->chbAlarm_2)
    {
        if(ui->chbAlarm_2->checkState()==Qt::Unchecked)
        {
            flag=true;
        }
        else
        {
            flag=false;
        }
        type=curveType::Alarm2;
    }
    else if(chb==ui->chbDropper)
    {
        if(ui->chbDropper->checkState()==Qt::Unchecked)
        {
            flag=true;
        }
        else
        {
            flag=false;
        }
        type=curveType::Dropper;
    }
    else if(chb==ui->chbCarrier)
    {
        if(ui->chbCarrier->checkState()==Qt::Unchecked)
        {
            flag=true;
        }
        else
        {
            flag=false;
        }
        type=curveType::Carrier;
    }
    else if(chb==ui->chbPole)
    {
        if(ui->chbPole->checkState()==Qt::Unchecked)
        {
            flag=true;
        }
        else
        {
            flag=false;
        }
        type=curveType::pole;
    }
    else if(chb==ui->chbWire2)
    {
        if(ui->chbWire2->checkState()==Qt::Unchecked)
        {
            flag=true;
        }
        else
        {
            flag=false;
        }
        type=curveType::wire2;
    }

    emit curveVisiable(type,flag);
}

void setDrawParDlg::on_pbtnGetDefault_clicked()
{
    initialPar(false);
}
