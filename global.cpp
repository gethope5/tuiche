#include "global.h"


Global *Global:: pinstance = NULL;


Global::Global()
{
//    thread = NULL;
}



Global* Global::getInstance()
{
    //如果pinstance为空,表明是第一次打开数据库,在从新进行数据库的初始化,否则打开以前的数据库
    if (!pinstance)
    {
        pinstance = new Global;
        pinstance->gbase.initDb();
    }
    pinstance->gbase.createNullRail();
    return pinstance;
}

//void Global::setVelocityThread(GetVelocity *pthread)
//{
//    thread = pthread;
//}

Global::~Global()
{

}

//函数功能：获得线路的基础信息
void Global::setPostionAsProfile(RunProfile& profile)
{
    polesets.clear();
    calcPolesets.clear();
    changedPos.clear();
    QSqlQuery dataset(gbase.currDatabase());
    QString cc=QString("SELECT * FROM %1") .arg(profile.tableName);
    qDebug()<<"sql cmd="<<cc;
    if (dataset.exec(cc))
    {
        QSqlRecord rec = dataset.record();
        int stationid = rec.indexOf("station");
        int tunid = rec.indexOf("tunnel");
        int poleid = rec.indexOf("poleID");
        int distid = rec.indexOf("poleDist");
        int structid = rec.indexOf("structFlag");

        profile.m_tinyspacer = profile.position;
        double sumdist = 0;
        double lastdist = 0;

        qDebug()<<"sql "<<dataset.isSelect();//<<" I found recodes: " << dataset.size();
        int i=0;
        while (dataset.next())
        {
            PoleBase b;
            b.station = dataset.value(stationid).toString();
            b.tunnel = dataset.value(tunid).toString();
            b.poleID = dataset.value(poleid).toString();
            b.dist = dataset.value(distid).toDouble();
            b.structFlag = dataset.value(structid).toInt();
            sumdist += lastdist;
            lastdist = b.dist;
            b.sum_dist = sumdist;
//            qDebug()<<"poleset"<<polesets.count()<<b.station<<b.tunnel
//                   <<b.poleID<<b.dist<<b.sum_dist;
            polesets.push_back(b);
        }
    }
    dataset.finish();
}


