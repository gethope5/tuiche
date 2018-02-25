#include "saveui.h"

saveUI::saveUI(QObject *parent) :
    QThread(parent)
{
    connect(this,SIGNAL(saveStatus(bool)),this,SLOT(slot_saveUIStatus(bool)));
}
void saveUI::run(void)
{

    bool cc=UIPixmap.save(saveFilePath);
//    qDebug()<<"sub thread"<<this->thread()->currentThreadId();

    emit saveStatus(cc);
}

void saveUI::slot_saveUIStatus(bool cc)
{
    saveFilePath.replace("\\","/");
    if(cc)
    {
        QMessageBox::about ( 0,QTextCodec::codecForName("gbk")->toUnicode("提示"), QTextCodec::codecForName("gbk")->toUnicode("文件存放位置：")+saveFilePath);
    }
    else
        QMessageBox::about ( 0,QTextCodec::codecForName("gbk")->toUnicode("提示"), QTextCodec::codecForName("gbk")->toUnicode("界面保存失败..."));

}
