#include "subexcelsave.h"

SubExcelSave::SubExcelSave(QObject *parent) :
    QThread(parent)
{
    //    dataBase=NULL;
}
void SubExcelSave::run(void)
{
    bool bSave=SaveCSVFile();
    emit saveOK(bSave);
}
void SubExcelSave::railData(void)
{
    QFile csvFile("c:\\11.csv");
    QStringList CSVList;
    QElapsedTimer timer;
    timer.start();
    if(csvFile.open(QIODevice::ReadOnly))

    {
        QTextStream stream(&csvFile);
        while (!stream.atEnd())
        {
            CSVList.push_back(stream.readLine());
        }
        csvFile.close();
        qDebug()<<"open file is ok"<<CSVList.count();
    }
    else
        qDebug()<<"open file is not ok";

    qDebug() << "The slow operation took" << timer.elapsed() ;
    csvFile.close();
}
bool SubExcelSave::SaveCSVFile(void)
{
    if(dataBase.isValid())
    {
        QSqlQuery query(dataBase);
        QFile csvFile(sSavePath);
        if (csvFile.open(QIODevice::ReadWrite))
        {
            qDebug()<<"export xls="<<query.exec(QString("select * from %1").arg(tableName))/*<<"tableName"<<tableName*/;

            int columnCount=query.record().count();
            if(tableTitle.isEmpty())
                return false;
            tableTitle/*.append("\r\n")*/;
            // 1 标题处理            //          写进标题
            //            qDebug()<<"1"/*<<tableTitle.toAscii().toHex()*/<<tableTitle;
            //            qDebug()<<"table value="<<tableTitle<<columnCount;

            QString tmp;
            for(int i=0;i<tableTitle.split(',').count();i++)
            {
                if(tableTitle.split(',').at(i).right(1).toInt())
                    tmp+=tableTitle.split(',').at(i).left(tableTitle.split(',').at(i).length()-1)+",";
            }

            tmp.append("\r\n");
            qDebug()<<"title name"<<tmp;
            if(csvFile.write(QTextCodec::codecForName("system")->fromUnicode(tmp))==-1)
                return false;
            while(query.next())
            {
                //2 处理读写内容
                QString rowBuffer;
                //      for(int i=0;i<columnCount;i++)
                //      {
                //          qDebug()<<"title "<<i<<tableTitle.split(',').at(i).right(1).toInt()<<tableTitle.split(',').at(i);
                //      }
                for(int i=0;i<columnCount;i++)
                {
                    QString tmp,tmp1=query.value(i).toString();
                    if(tmp1.contains(","))
                    {
//                        tmp.replace(","," ");
                        bool *ok;
                        for(int j=0;j<tmp1.split(",").count();j++)
                        {
                            tmp+=QString::number(tmp1.split(",").at(j).toInt(ok,16))+" ";
                        }
//                        qDebug()<<"before and after"<<tmp1<<tmp;
                    }
                    else
                        tmp=tmp1;
                    if(tableTitle.split(',').at(i).right(1).toInt())
                        rowBuffer+=tmp.append(",");
                }
                rowBuffer.append("\r\n");
                if(csvFile.write(QTextCodec::codecForName("system")->fromUnicode(rowBuffer))==-1)
                    return false;
            }
            //3 导出完毕，关闭文件
            csvFile.close();
            return true;
        }
        else
            qDebug()<<"file is not ok";
    }
}
