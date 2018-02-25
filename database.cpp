/*
 * operator here for database
*/

#include <QtSql>
#include "database.h"
QString CDataBase::railTitle=QString::fromWCharArray(L"线路名1,上行0,下行0,简称1");
QString CDataBase::railTableName="railList";
CDataBase::CDataBase()
{
    beConnected = false;
}


CDataBase::~CDataBase()
{
    m_db.close();
    qDebug()<<"~~~~";
    QSqlDatabase::removeDatabase(m_db.connectionName());
}
void CDataBase::initDb(void)
{
    //search SD card
    addConnection("fieldrailway.db", QString(""), QString(""), QString(""));
    createTable();
}

bool CDataBase::addConnection(const QString &dbName, const QString &host,
                              const QString &user, const QString &passwd)
{

    QSqlError err;
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);
    m_db.setHostName(host);
    if (!m_db.open(user, passwd)) {
        err = m_db.lastError();
        m_db = QSqlDatabase();
    }

    if (err.type() != QSqlError::NoError)
    {
        QMessageBox::warning(NULL, QString::fromWCharArray(L"无法打开数据库"), QString::fromWCharArray(L"数据库操作错误: ") + err.text());
        qDebug()<<"can not open the database! "<<dbName;
        beConnected = false;
    }
    else
    {
        qDebug()<<"I can get the database\n";
        //        QMessageBox::warning(NULL, "info","database is ok");
    }
    beConnected =  true;
    return beConnected;
}

QSqlDatabase CDataBase::currDatabase() const
{
    return m_db;
}


void CDataBase::dropTables(void)
{
    QSqlQuery query(currDatabase());
    query.exec(QString("DROP TABLE %1").arg(railTableName));
    query.exec("DROP TABLE railLine");
    query.exec("DROP TABLE user");
}

bool CDataBase::createRaiList(QString name, QString sxName, QString xxName)
{
    bool bCreate;
#if 0
    QSqlQuery query(currDatabase());
    query.prepare(QString("INSERT INTO %1 VALUES(:name, :sx, :xx)").arg(railTableName));
    query.bindValue(":name", name);
    query.bindValue(":sx", sxName);
    query.bindValue(":xx", xxName);
    return query.exec();
#else
    QSqlQuery query(currDatabase());
    QString insertBuffer=QString("insert into %4 values(\'%1\',\'%2\',\'%3\','%1')").arg(name).arg(sxName).arg(xxName).arg(railTableName);
    bCreate=query.exec(insertBuffer);
    qDebug()<<"create rail line ,sql="<<insertBuffer<<bCreate;
    return bCreate;
#endif
}

void CDataBase::createRailLine(QString name)
{
    QSqlQuery query(currDatabase());
    qDebug()<<"the database name = "<<name;
    QString strval =  QString("CREATE TABLE %1 (ID INT NOT NULL,"
                              "station varchar(30),"           //站区名称
                              "tunnel varchar(30),"            //隧道名称
                              "poleID varchar(6) NOT NULL,"             //支柱号
                              "poleDist float,"                //跨距
                              "structFlag int(1),"              //结构标志
                              "curveFlag INT,"               //取消标志
                              "distFlag varchar(20),"          //公里标
                              "powerSupply varchar(24),"       //供电段
                              "factory varchar(24),"           //车间名称
                              "workArea varchar(24),"          //工区
                              "wireHight INT,"                 //导高标准值
                              "offset INT)"                    //拉出值标准值
                              ).arg(name);

    bool ret = query.exec(strval);
    QSqlError err;
    err = query.lastError();

    if (err.type() != QSqlError::NoError)
    {
        qDebug()<<"result: "<<err.text();
    }
    qDebug()<<"the create table ret = " << ret;
}

bool CDataBase::insertRailLine(QString tbName, oneLineData_t & dval)
{
    QSqlQuery query(currDatabase());
    //    static int id=0;
    /* 1 ID INT                     2 station varchar//站区名称
   3 tunnel varchar //隧道名称   4 poleID varchar(6)//支柱号
   5 poleDist float //跨距      6 structFlag int(1)//结构标志
   7 curveFlag INT  //取消标志   8 distFlag varchar(20)//公里标
   9 powerSupply char//供电段   10 factory char  //车间名称
  11 workArea varchar//工区     12 wireHight INT//导高标准值
  13 offset INT//拉出值标准值*/
#if 0
    QString strval = QString("INSERT INTO %1 VALUES(:id1, :id2, :id3, :id4, :id5, :id6, :id7,:id8, :id9, "
                             ":id10, :id11, :id12, :id13)").arg(tbName);
    query.prepare(strval);
    query.bindValue(":id1:", dval.id);
    query.bindValue(":id2:", dval.station);
    query.bindValue(":id3:", dval.tunnel);
    query.bindValue(":id4", dval.poleID);
    query.bindValue(":id5", dval.poleDist);
    query.bindValue(":id6", dval.structFlag);
    query.bindValue(":id7", dval.curveFlag);
    query.bindValue(":id8", dval.distFlag);
    query.bindValue(":id9", dval.powerSupply);
    query.bindValue(":id10", dval.factory);
    query.bindValue(":id11", dval.workArea);
    query.bindValue(":id12", dval.wireHight);
    query.bindValue(":id13", dval.offset);
    return query.exec();
#else
    QString strQuery=QString("Insert into %1 VALUES(%2,\'%3\',\'%4\',\'%5\',%6,%7,%8,\'%9\',\'%10\',\'%11\',\'%12\',%13,%14)")
            .arg(tbName)
            .arg(dval.id)
            .arg(dval.station)
            .arg(dval.tunnel)
            .arg(dval.poleID)
            .arg(dval.poleDist)
            .arg(dval.structFlag)
            .arg(dval.curveFlag)
            .arg(dval.distFlag)
            .arg(dval.powerSupply)
            .arg(dval.factory)
            .arg(dval.workArea)
            .arg(dval.wireHight)
            .arg(dval.offset);
    //            qDebug()<<"query string buffer:"<<strQuery;
    return query.exec(strQuery);
#endif
}
void CDataBase::Transaction(void)
{
    m_db.transaction();
}
void CDataBase::Commit(void)
{
    m_db.commit();
}

void CDataBase::Rollback(void)
{
    m_db.rollback();
}

/*
 *   判断路线是否存在
 */
bool CDataBase::isLineExist(QString railName)
{
    QSqlQuery query(currDatabase());
    QString cmdbuf = QString("SELECT * FROM %1 WHERE name = \"" + railName + "\"").arg(railTableName);
    query.exec(cmdbuf);
    bool ret = query.first();
    qDebug()<<"query railName : "<<query.value(0).toString();
    query.finish();
    return ret;
}
bool CDataBase::emptyTable(QString tableName)
{
    QString buffer("delete from ");
    buffer.append(tableName);
    QSqlQuery query(currDatabase());
    return query.exec(buffer);
}
bool CDataBase::updateRailList(QString railName,QString tableName,bool bDirection)
{
    QString buffer;
    if(bDirection)
    {
        buffer=QString("update %1 set xx=\'%2\' where name=\'%3\'").arg(railTableName).arg(tableName).arg(railName);
    }
    else
    {
        buffer=QString("update %1 set sx=\'%2\' where name=\'%3\'").arg(railTableName).arg(tableName).arg(railName);
    }
    qDebug()<<"update table ="<<buffer;
    QSqlQuery query(currDatabase());
    return query.exec(buffer);
}
/*
 *   删除路线及其相关的上行行表
 */
void CDataBase::dropLine(QString railName)
{
    QSqlQuery query(currDatabase());
    QSqlQuery delQuery(currDatabase());
    QString cmdbuf = QString("SELECT * FROM %1 WHERE name = \"" + railName + "\"").arg(railTableName);
    qDebug()<<"delete raillist table record "<<query.exec(cmdbuf);
    QString tb1, tb2;
    if (query.next())
    {
        tb1 = query.value(1).toString();
        tb2 = query.value(2).toString();
        query.finish();
    }
    if (!delQuery.exec("DROP TABLE " + tb1))
    {
        QSqlError err;
        err = delQuery.lastError();

        if (err.type() != QSqlError::NoError)
        {
            qDebug()<<"result："<<err.text();
        }
        qDebug()<<"can nont drop"<<"DROP TABLE " + tb1;
    }
    else
        qDebug()<<"DROP TABLE " + tb1+" is ok";
    if (!delQuery.exec("DROP TABLE " + tb2))
    {
        QSqlError err;
        err = delQuery.lastError();

        if (err.type() != QSqlError::NoError)
        {
            qDebug()<<"result："<<err.text();
        }
        qDebug()<<"can nont drop"<<"DROP TABLE " + tb2;
    }
    else
        qDebug()<<"DROP TABLE " + tb2 +" is ok";

}

void CDataBase::createTable(void)
{
    QSqlQuery query(currDatabase());

    query.exec(QString("CREATE TABLE %1 (name char(30) NOT NULL UNIQUE, sx varchar(20), xx varchar(20),railAbbr varchar(30))").arg(railTableName));
    query.exec("CREATE TABLE user (name char(30) NOT NULL UNIQUE, passwd varchar(12), level INT NOT NULL)");
}
void CDataBase::createNullRail(void)
{
    if(!getRailList().contains(QString::fromWCharArray(L"临时线")))
    {
        QSqlQuery query(currDatabase());
        QString cc=QString("insert into raillist(name,railAbbr) values('%1','%2')").arg(QString::fromWCharArray(L"临时线")).arg(QString::fromWCharArray(L"临时线"));
        qDebug()<<QString::fromWCharArray(L"临时线路添加，=")<<query.exec(cc)<<"sql="<<cc;
    }
    else
    {
        qDebug()<<QString::fromWCharArray(L"已包含临时线路...");
    }
}
QStringList CDataBase::getRailList(void)
{
    QStringList selres;
    int cnt;
    QSqlQuery query(currDatabase());
    query.exec(QString("SELECT railabbr FROM %1").arg(railTableName));
    cnt = query.record().count();
    //qDebug()<<"the cnt is "<<cnt;
    while (query.next())
    {
        selres<<query.value(0).toString();
    }
    query.finish();
    qDebug()<<"cur rail list="<<selres;
    return selres;
}
QStringList CDataBase::getRailListALL(void)
{
    QStringList selres;
    int cnt;
    QSqlQuery query(currDatabase());
    query.exec(QString("SELECT * FROM %1").arg(railTableName));
    cnt = query.record().count();
    //qDebug()<<"the cnt is "<<cnt;
    QString oneRailInfo;
    while (query.next())
    {
        oneRailInfo=query.value(0).toString().append(",");
        oneRailInfo+=query.value(1).toString().append(",");
        oneRailInfo+=query.value(2).toString();
        selres<<oneRailInfo;
    }
    query.finish();
    return selres;
}
QStringList CDataBase::getUserList(void)
{
    QStringList users;
    int cnt;
    QSqlQuery query(currDatabase());
    query.exec("SELECT name FROM user");

    while (query.next())
    {
        users<<query.value(0).toString();
    }
    query.finish();
    return users;
}

QString CDataBase::getPasswd(QString user, bool *bok)
{
    QString passwd;
    QSqlQuery query(currDatabase());
    if (query.exec("SELECT passwd FROM user where name = \"" + user + "\""))
    {
        *bok = true;
        query.next();
        passwd = query.value(0).toString();
    }
    else
    {
        *bok = false;
    }
    query.finish();
    return passwd;
}

int CDataBase::getLoginLevel(QString user, bool *bok)
{
    int level = -1;
    QSqlQuery query(currDatabase());
    if (query.exec("SELECT level FROM user where name = \"" + user + "\""))
    {
        *bok = true;
        query.next();
        level = query.value(0).toString().toInt();
    }
    else
    {
        *bok = false;
    }
    query.finish();
    return level;
}


/*another database class */
bool MeasureDB::overAlarm=false;
bool MeasureDB::bMeasure=false;
bool MeasureDB::bSimDraw=false;
bool MeasureDB::bSimUI=false;
bool MeasureDB::moving=false;
QString MeasureDB::detailDataTb=QString("debugMeasure");
QString MeasureDB::overDataTb=QString("overPars");
QString MeasureDB::simpleDataTb=QString("measureEx");
QString MeasureDB::verifyTb=QString("verifyPar");
bool MeasureDB::bRadarStatus=false;
bool MeasureDB::bRadarPower=false;
bool MeasureDB::bVerify=true;
bool MeasureDB::oneTableData=true;
bool MeasureDB::bPoleDir=true;       //杆号变化方向,true，增加;false,减少
QString MeasureDB::nPoleNum="0";
bool MeasureDB::bRealSim=false;
QString MeasureDB::strFileData="";

QString MeasureDB::detailDataTitle="";//QString::fromWCharArray(L"ID1,时间1,里程(米)1,公里标(Km)1,站区0,杆号1,轨距1,超高1,拉出值1,导高1,拉出值11,导高11,平行距离1,垂直距离1,承力索1,结构高度1,500高差1,侧面界限1,数据类型1");
QString MeasureDB::detailTableFields="";//QString("id,tm,dist,km,station,PoleID,railWidth,railHeight,wireWidth,wireHeight,wireWidth1,wireHeight1,parallelDist,verticalDist,carrier,structureHeight,wire500,side,dataType");

//2017-11-13:数据结构调整
//                                                            0    1   2        3         4          5    6    7     8    9     10    11     12       13    14      15    16      17      18       19  20
QString MeasureDB::detailNewTitle=QString::fromWCharArray(L"ID1,时间1,里程(米)1,公里标(Km)1,速度(Km/h)1,站区0,杆号1,轨距1,超高1,拉出值1,导高1,拉出值11,导高11,平行距离1,垂直距离1,承高1,承拉1,结构高度1,500高差1,侧面界限1,数据类型1");
//                                             0  1  2 3  4       5      6         7          8         9          10         11          12         13          14         15           16         17          18     19   20
QString MeasureDB::detailNewFields=QString("id,tm,dist,km,speed,station,PoleID,railWidth,railHeight,wireWidth,wireHeight,wireWidth1,wireHeight1,parallelDist,verticalDist,carrier,carrierWidth,structureHeight,wire500,side,dataType");


QString MeasureDB::detailOldTitle=QString::fromWCharArray(L"ID1,时间1,里程(米)1,公里标(Km)1,站区0,杆号1,轨距1,超高1,拉出值1,导高1,拉出值11,导高11,承力索1,结构高度1,原始数据0");
QString MeasureDB::detailOldFields=QString("id,tm,dist,km,station,PoleID,railWidth,railHeight,wireWidth,wireHeight,wireWidth1,wireHeight1,fXangle,fYangle,orignalData");


QString MeasureDB::verifyTitle=QString::fromWCharArray(L"ID1,时间1,里程(米)1,公里标(Km)0,站区0,杆号1,轨距0,超高0,拉出值1,导高1,拉出值10,导高10");
QString MeasureDB::verifyFields=QString("id,tm,dist,km,station,PoleID,railWidth,railHeight,wireWidth,wireHeight,wireWidth1,wireHeight1");

QString MeasureDB::overDataTitle=QString::fromWCharArray(L"ID1,时间1,里程(m)1,公里标(Km)1,站区0,杆号1,轨距0,超高0,拉出值1,导高1,拉出值10,导高10");
QString MeasureDB::overTableField=QString("id,tm,dist,km,station,PoleID,railWidth,railHeight,wireWidth,wireHeight,wireWidth1,wireHeight1)");

//                                                            1   2     3       4           5   6   7       8   9       10  11        12    13  14     15   16   17     18          19      20    21        22      23          24
QString MeasureDB::simpleDataTitle=QString::fromWCharArray(L"ID1,时间1,里程(m)1,公里标(Km)1,站区0,杆号1,轨距0,超高0,侧面界限1,红线0,结构高度1,拉出值1,导高1,坡度1,高差1,中高0,中拉0,非支拉出值1,非支导高11,水平距离0,垂直距离0,500高差1,数据类型1,巡视图片0");
//                                             0 1  2    3  4       5       6        7          8                   9           11              12      13          14      15  16              17          18          19          20          21          22           23             24
QString MeasureDB::simpleTableFields=QString("id,tm,dist,km,station,poleid,railwidth,railheight,sidelimitWidth,sidelimitHeight,structHeight,dWireWidth,dWireHeight,slope,gaocha,midWireHeight,midWireWidht,wirewidth1,wireheight1,wiresWidth,wiresHeight,wireCrossHeight,LocationRemark,picPath");
int MeasureDB::dbCount=0;
MeasureDB::MeasureDB(void)
{

}
void MeasureDB::createDb( const QString& dbName)
{
    QSqlError err;
    QString connectName= QString("mdb_connection%1").arg(dbCount++);
    m_db = QSqlDatabase::addDatabase("QSQLITE",connectName);
    m_db.setDatabaseName(dbName);
    if (!m_db.open())
    {
        qDebug()<<"can not open db..........";
        err = m_db.lastError();
        m_db = QSqlDatabase();
    }
    else
    {
        qDebug()<<"measure db is opened..........";
    }
    qDebug()<<"connect name="<<connectName;
    curDataName=dbName;
}
void MeasureDB::isOpenDB(void)
{
    if(!m_db.isOpen())
        m_db.open();
}
void MeasureDB::isCloseDB(void)
{
    Commit();
    m_db.close();
}
QStringList MeasureDB::tables(void)
{
    return m_db.tables();
}
bool MeasureDB::createTables(void)
{    
    isOpenDB();
    QSqlQuery query(m_db);


    //                                                            0    1   2        3           4   5    6       7   8     9       10   11     12       13       14     15      16      17
    //                                      0 ID1                               1 时间          2里程(米)   3公里标(Km)         4速度        5站区             6杆号                 7轨距         8超高         9拉出值        10导高           11拉出值1         12导高1           13平行距离       14垂直距离         15承力索   16承力索拉出值，      17结构高度          18 500高差   19侧面界限  20数据类型1");
    QString strDetailSql=QString("CREATE TABLE %1(id INTEGER PRIMARY KEY AUTOINCREMENT, tm varchar(30),dist float,km float,speed float,station varchar(20),PoleID varchar(20),railWidth int,railHeight int,wireWidth int,wireHeight int,wireWidth1 int,wireHeight1 int,parallelDist int,verticalDist int,carrier int,carrierWidth int,structureHeight int,wire500 int,side int,dataType varchar(100))").arg(detailDataTb);
    //                                                  1ID,                                2 时间,         3 里程(米), 4公里标(千米),    5站区,            6杆号,         7轨距,           8超高,            9侧面界限,            10 红线                11 结构高度          12定拉            13定高,            14坡度,      15高差,      16中高,              17中拉,              18非支拉出值,      19非支导高1,       20 水平距离,          21垂直距离,      22 500高差,             23 定位器描述,               24 巡视图片";
    QString str2=QString("CREATE TABLE %1(id INTEGER, tm varchar(30),dist float(0,2),km float,station varchar(30),poleid varchar(10),railwidth float,railheight float,sidelimitWidth float,sidelimitHeight float,structHeight float,dWireWidth float,dWireHeight float,slope float,gaocha float,midWireHeight float,midWireWidht float,  wirewidth1 float,wireheight1 float,wiresWidth float,wiresHeight float,wireCrossHeight float,LocationRemark varchar(100),picPath varchar(200))").arg(simpleDataTb);
    //                                                  1ID,                        2时间         3里程     4公里标        5站区         6杆号                 7轨距         8超高         9拉出值             10导高       11拉出值1      12导高1
    QString str3=QString("CREATE TABLE %1(id INTEGER PRIMARY KEY AUTOINCREMENT, tm varchar(30),dist float,km float,station varchar(20),PoleID varchar(20),railWidth int,railHeight int,wireWidth int,wireHeight int,wireWidth1 int,wireHeight1 int)").arg(overDataTb);
    //    isCloseDB();
#if 0
    QString str4=QString("create table %2 as select * from %1 where 1=2").arg(detailDataTb).arg(verifyTb);
#else
    QString strVerifyTSql=QString("CREATE TABLE %1(id INTEGER PRIMARY KEY AUTOINCREMENT, tm varchar(30),dist float,km float,station varchar(20),PoleID varchar(20),railWidth int,railHeight int,wireWidth int,wireHeight int,wireWidth1 int,wireHeight1 int )").arg(verifyTb);
#endif
    query.exec(strDetailSql);

    //    qDebug()<<"verify table sql"<<str4<<
    query.exec(strVerifyTSql);
    return query.exec(str2)&&query.exec(str3);
}
bool MeasureDB::modifyRecord(int id, const QString& station,
                             const QString& pid,
                             const int structFlg )
{
    isOpenDB();
    QSqlQuery query(m_db);
    QString strval = QString("UPDATE measures SET station='%1',"
                             "poleID='%2',structFlag =%3 WHERE id>=%4")
            .arg(station).arg(pid).arg(structFlg).arg(id);
    bool modifiedrecord=query.exec(strval);
    //    qDebug()<< "modify val "<<strval;
    //      qDebug()<<"exec: "<<modifiedrecord;
    //    qDebug()<<query.lastError();
    isCloseDB();
    return true;
}
bool MeasureDB::insertRecord_overPars(MeasureDebugData & dval)
{
    isOpenDB();
    QSqlQuery query(m_db);
    //                                                  1ID,  2时间  3里程  4公里标 5站区  6杆号  7轨距 8超高  9拉出值  10导高 11拉出值1  12导高1
    QString strval = QString("INSERT INTO %1 VALUES(:id1, :id2, :id3,  :id4,  :id5, :id6, :id7, :id8, :id9,   :id10, :id11,    :id12)").arg(overDataTb);
    query.prepare(strval);
    //1    2    3   4   5        6
    //id, tm ,dist ,km ,station ,PoleID ,
    //7          8              9       10
    //railWidth ,railHeight ,wireWidth ,wireHeight
    //11            12         13      14       15
    //wireWidth1,wireHeight1,carrier,structureHeight,orignalData
    //    query.bindValue(":id1",dval.id);
    query.bindValue(":id2",dval.tm);
    query.bindValue(":id3",dval.nWalkDistance);
    query.bindValue(":id4",QString::number(dval.dKm,'f',3).toFloat());
    query.bindValue(":id5", dval.station);
    query.bindValue(":id6", dval.poleID);
    query.bindValue(":id7", dval.nRailWidth);
    query.bindValue(":id8", dval.nRailHeight);
    query.bindValue(":id9",dval.wire1.x());
    query.bindValue(":id10",dval.wire1.y());
    query.bindValue(":id11",dval.wire2.x());
    query.bindValue(":id12",dval.wire2.y());
    bool flag=query.exec();

    isCloseDB();
    return flag;
}
//函数功能：向详细数据库表格和校验数据表格插入某一点的测量数据，参数说明：测量数据、侧面界限、测量数据及标定数据选择
//flag=true，向详细数据表格插入数据；false，向校验数据表格插入数据
bool MeasureDB::insertRecord_detailAndVerify(MeasureDebugData & dval,int nSide ,bool flag)
{
    isOpenDB();
    QSqlQuery query(m_db);
    //数据准备，根据wire1及wire2的值，获取1平行线间距、2垂直距离及3结构高度
    if(dval.wire1.y()&&dval.carrier.y())
    {
        //结构高度
        dval.structureHeight=abs(dval.wire1.y()-dval.carrier.y());
    }
    else
    {
        dval.structureHeight=0;
    }
    if(dval.wire1.y()&&dval.wire2.y())
    {
        //平行线间距
        dval.parallelDist=abs(dval.wire1.x()-dval.wire2.x());
        //垂直距离
        dval.verticalDist=abs(dval.wire1.y()-dval.wire2.y());
    }
    else
    {
        dval.parallelDist=0;
        dval.verticalDist=0;
    }

    QString strval;
    if(flag)
    {
        //                                       0    1   2        3          4   5    6       7   8     9    10   11     12       13       14     15      16         17      18    19       20
        //                                      ID1,时间1,里程(米)1,公里标(Km)1,速度,站区0,杆号1,轨距1,超高1,拉出值1,导高1,拉出值11,导高11,平行距离1,垂直距离1,承力索1,承力索拉出值1,结构高度1,500高差1,侧面界限1,数据类型1");

        strval= QString("INSERT INTO %1 VALUES(:id1, :id2, :id3, :id4, :id5, :id6, :id7,:id8, :id9,:id10,:id11,:id12,:id13,:id14,:id15,:id16,:id17,:id18,:id19,:id20,:id21)").arg(detailDataTb);
    }
    else
    {
        strval= QString("INSERT INTO %1 VALUES(:id1, :id2, :id3, :id4, :id5, :id6, :id7,:id8, :id9,:id10,:id11,:id12,:id13,:id14,:id15)").arg(verifyTb);
        // 1    2   3       4     5         6          7           8           9       10          11      12
        //(id , tm, dist,   km, station,PoleID,railWidth,railHeight,wireWidth,wireHeight,wireWidth1,wireHeight1)")
    }
    query.prepare(strval);
    //1    2    3   4   5        6
    //id, tm ,dist ,km ,station ,PoleID ,
    //7          8              9       10
    //railWidth ,railHeight ,wireWidth ,wireHeight
    //11            12         13      14       15
    //wireWidth1,wireHeight1,carrier,structureHeight,orignalData
    //    query.bindValue(":id1",dval.id);
    //    qDebug()<<"walk diatance "<<dval.nWalkDistance<<QString::number(dval.dKm,'f',3).toFloat();

    query.bindValue(":id2",dval.tm);                                    //2时间1
    query.bindValue(":id3",dval.nWalkDistance);                         //3里程(米)
    query.bindValue(":id4",QString::number(dval.dKm,'f',3).toFloat());  //4公里标(Km)
    query.bindValue(":id5", dval.speed);                                //5速度
    query.bindValue(":id6", dval.station);                              //6站区
    query.bindValue(":id7", dval.poleID);                               //7杆号
    query.bindValue(":id8", dval.nRailWidth);                           //8轨距
    query.bindValue(":id9", dval.nRailHeight);                          //9超高
    query.bindValue(":id10",dval.wire1.x());                             //10拉出值
    query.bindValue(":id11",dval.wire1.y());                            //11导高
    query.bindValue(":id12",dval.wire2.x());                            //12拉出值1
    query.bindValue(":id13",dval.wire2.y());                            //13导高1
    if(flag)
    {
        query.bindValue(":id14",dval.parallelDist);                         //15平行距离
        query.bindValue(":id15",dval.verticalDist);                         //16垂直距离
        query.bindValue(":id16",dval.carrier.y());                              //17承力索
        query.bindValue(":id17",dval.carrier.x());                         //18承力索拉出值
        query.bindValue(":id18",dval.structureHeight);                      //19结构高度
        query.bindValue(":id19",dval.wire500);                              //20 500高差
        query.bindValue(":id20",nSide);                                     //21    侧面界限
        query.bindValue(":id21",dval.dataType);                             //19数据类型
    }
    bool flag1=query.exec();
    //    qDebug()<<"insert measure record="<<flag1;
    isCloseDB();
    return flag1;
}
//函数功能：根据对详细数据表格中的跨中及定位点数据进行标注
bool MeasureDB::updateDetailTb(MeasureDebugData dval,int nSide,QString dataType)
{
    isOpenDB();
    QSqlQuery query(m_db);
    QString strSql;
    int nIndex=-1;
    //避免在满足同意时刻和里程的情况下，出现多条记录，出现同一个定位点，有两条记录。应用了round(dist,2)，对里程保留两位小数
    strSql= QString("select id,wirewidth,wireheight from %3 where round(dist,2)=%1 and tm= \'%2\'" ).arg(dval.nWalkDistance).arg(dval.tm).arg(detailDataTb);

    if(query.exec(strSql))
    {
        while(query.next())
        {
            nIndex=query.value(0).toInt();
            break;
        }
        if(nIndex==-1)
            return false;
    }
    else
        return false;

    if(nSide)
    {
        strSql= QString("update %1 set dataType=\'%2\',side=%3 where id=%4 " ).arg(detailDataTb).arg(dataType).arg(nSide).arg(nIndex);
    }
    else
    {
        strSql= QString("update %1 set dataType=\'%2\' where id=%3" ).arg(detailDataTb).arg(dataType).arg(nIndex);
    }
    //    qDebug()<<"update detail table sql="<<strSql;
    return query.exec(strSql);
}
bool MeasureDB::insertRecord_MeasureEx(MeasureIdEx_t & dval)
{
    isOpenDB();
    QSqlQuery query(m_db);
    //    query.exec("select max(id) from MeasureEx");
    //    while(query.next())
    //    {
    //        dval.id=query.value(0).toInt();
    //    }

    QString strval = QString("INSERT INTO MeasureEx VALUES(:id1, :id2, :id3, :id4, :id5, :id6, :id7,:id8, :id9,:id10,:id11,:id12,:id13,:id14,:id15,:id16,:id17,:id18,:id19,:id20,:id121,:id22,:id23,:id24)");
    query.prepare(strval);
    //    query.bindValue(":id1",dval.id);
    //ID, 2 时间, 3 里程(米), 4公里标(千米),  5站区,   6杆号,  7轨距,       8超高,
    //id, tm,    dist       ,km ,         station, poleid,railwidth ,railheight
    //9侧面界限,  10 红线 11 结构高度   12定拉       13定高,       14坡度,      高差,    16中高,
    //sideWidth,Height,structHeight ,dWireWidth ,dWireHeight ,slope float,gaocha, midWireHeight,
    //17中拉,       18非支拉出值,19非支导高1, 20 水平距离,21垂直距离,   22 500高差,      23 定位器描述,   24 巡视图片;
    //midWireWidht,wirewidth1,wireheight1,wiresWidth,wiresHeight,wire500Height,LocationRemark,picPath");

    query.bindValue(":id2",dval.tm);
    query.bindValue(":id3",dval.nWalkDistance);
    query.bindValue(":id4",QString::number(dval.dKm,'f',3).toFloat());
    //    query.bindValue(":id4",dval.dKm);

    query.bindValue(":id5", dval.station);
    query.bindValue(":id6", dval.poleid);
    query.bindValue(":id7", dval.onePolePar.nRailWidth);
    query.bindValue(":id8", dval.onePolePar.nRailHeight);
    query.bindValue(":id9", dval.sideLimit);
    query.bindValue(":id10",dval.onePolePar.sideHeight);
    query.bindValue(":id11",dval.onePolePar.structHeight);
    query.bindValue(":id12",dval.onePolePar.dWireWidth);
    query.bindValue(":id13",dval.onePolePar.dWireHeight);
    query.bindValue(":id14",dval.onePolePar.slope);
    query.bindValue(":id15",dval.onePolePar.gaocha);
    query.bindValue(":id16",dval.onePolePar.midWireHeight);
    query.bindValue(":id17",dval.onePolePar.midWireWidth);
    query.bindValue(":id18", dval.onePolePar.wireWidth1);
    query.bindValue(":id19", dval.onePolePar.wireHeight1);
    query.bindValue(":id20", dval.onePolePar.wiresWidth);
    query.bindValue(":id21", dval.onePolePar.wiresHeight);
    query.bindValue(":id22",dval.onePolePar.wire500Height);
    query.bindValue(":id23",dval.locationRemark);
    query.bindValue(":id24",dval.PicPath);
    bool flag=query.exec();
    isCloseDB();
    return flag;
}
void MeasureDB::Transaction(void)
{
    if(m_db.isOpen())
        m_db.transaction();
}

void MeasureDB::Commit(void)
{
    if(m_db.isOpen())
        m_db.commit();
}

MeasureDB::~MeasureDB()
{
    if(m_db.isOpen())
        m_db.close();
    if(m_db.isOpen())
    {
        qDebug()<<"~~~~"<<m_db.connectionName();
        QSqlDatabase::removeDatabase(m_db.connectionName());
    }
}
QSqlDatabase MeasureDB::currDatabase(void)
{
    return m_db;
}
void MeasureDB::closeDb(void)
{
    m_db.close();
    m_db.removeDatabase("mdb_connection");
}
