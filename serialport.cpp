#include "serialport.h"
static bool bCom1=false;
int SerialPort::preDistanceCount=-1;
//激光雷达继电器控制命令
//波特率：115200
//下发命令：XJXT_Q2_1_end
//反馈命令：XJXT_Q1_0_Sta

SerialPort::SerialPort(COMTYPE type,QObject *parent) :
    /* QObject(parent),*/QThread(parent)
  ,disCount(0)
  ,bFirstWalk(true)
  ,directFlag(1)
  ,m_bOpen(false)
  ,bOk(false)
  ,timerDeviceInfo(NULL)
{
    nComType=type;
    walkStepCounts=0;
#if SYS_COM
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &SerialPort::readComData);
#else
    RS232=NULL;
    readTimerCom=new QTimer(this);
    connect(readTimerCom,SIGNAL(timeout()),this,SLOT(readComData()));
#endif
    readBasePar();
    enumToString();
    getComConfigInfo();
    connectDevice();
    //    if(nComType==RADAR_COM)
    //        qDebug()<<"comm name= rador "<<commInfo.com;
#if DEBUG_COM_0428
    file=NULL;
    if(nComType==RAIL_MCU_COM)
    {
        file=new QFile("originalData.txt");
        if(!file->isOpen())
        {
            file->open(QIODevice::ReadWrite|QIODevice::Text | QIODevice::WriteOnly);
            qDebug()<<"file is ok okok";
        }
        else
        {
            qDebug()<<"file is ok";
        }
    }
#endif
    curDeviceInfo.dianliang="";
    curDeviceInfo.GPSJ="";
    curDeviceInfo.GPSW="";
    curDeviceInfo.wAndS="";
}
#if DEBUG_COM_0428
void SerialPort::run(void)
{
#if SYS_COM
    parse_comData();
#else
    readTimerCom->stop();
    parse_comData();
    readTimerCom->start(200);
#endif
}
#endif
void SerialPort:: parse_comData(void)
{
    //    qDebug()<<"c"<<receiveDataBuffer;
    //带雷达推车的雷达供电继电器控制
    if(nComType==RADAR_COM)
    {
//        qDebug()<<"rador com="<<receiveDataBuffer<<bOk;

        //        emit updateShowInfo(QString(receiveDataBuffer));
        //        qDebug()<<"rador com value="<<receiveDataBuffer;//.contains("XJXT_Q1_0_Sta")<<"q1 open"<<receiveDataBuffer.contains("XJXT_Q1_1_Sta");
//        "XJXT_Q2_0_Sta"
        if(receiveDataBuffer.contains("Q2_0_Sta"))
        {
            bOk=true;
            emit com_replay(RADAR_CLOSED);
        }
//        "XJXT_Q2_1_Sta"
        if(receiveDataBuffer.contains("Q2_1_Sta"))
        {
            bOk=true;
            emit com_replay(RADAR_OPENED);
        }
//        "XJXT_Q1_0_Sta"
        if(receiveDataBuffer.contains("Q1_0_Sta"))
        {
            //qDebug()<<"q1 close";
            bOk=true;
            emit com_replay(WIFI_CLOSED);
        }
//        "XJXT_Q1_1_Sta"
        if(receiveDataBuffer.contains("Q1_1_Sta"))
        {
            //qDebug()<<"q1 open";
            bOk=true;
            emit com_replay(WIFI_OPENED);
        }
    }
    //带雷达推车底盘串口，获取里程、轨距、超高等信息
    else if(nComType==RAIL_MCU_COM)
    {
//        qDebug()<<"rail com="<<receiveDataBuffer<<bOk;
        //        qDebug()<<"rail com value="<<receiveDataBuffer;//.contains("XJXT_Q1_0_Sta")<<"q1 open"<<receiveDataBuffer.contains("XJXT_Q1_1_Sta");

        //帧完整性判别
        QString output;
        //里程
        if(getParData(receiveDataBuffer,"T",output))
        {
//            qDebug()<<"-----------------------------walk "<<output;
            parse_RailSensor(output);
            output.clear();
            bOk=true;
        }
        //gps error
        if(getParData(receiveDataBuffer,"UM220",output))
        {
//            qDebug()<<"-----------------------------gps is error "<<output;
            curDeviceInfo.GPSJ="NULL";
            curDeviceInfo.GPSW="NULL";
            output.clear();
            bOk=true;
        }
        //GPS
        if(getParData(receiveDataBuffer,"G",output))
        {
            QStringList tmp=output.split(",");

            if((tmp.at(0)=="GA")&&(tmp.count()>4))
            {
                curDeviceInfo.GPSW=tmp.at(1);
                curDeviceInfo.GPSJ=tmp.at(3);
            }
            else
            {

                curDeviceInfo.GPSJ="invalid";
                curDeviceInfo.GPSW="invalid";
            }
            output.clear();
            bOk=true;

//            qDebug()<<"-----------------------------gps is ok "<<output;
        }
        //device wendu /shidu ,voltage
        if(getParData(receiveDataBuffer,"W",output))
        {
//            qDebug()<<"-----------------------------wendu "<<output<<output.count();
//            QString wendu;
            if(output.count()==13)
            {
                curDeviceInfo.wAndS=QString::number(output.mid(output.indexOf("W")+1,4).toInt()/10.0,'f',1)+" "+QString::number(output.mid(output.indexOf("W")+5,3).toInt()/10.0,'f',1)+"%";
                curDeviceInfo.dianliang=QString::number(output.mid(output.indexOf("W")+8,3).toInt()/10.0,'f',1);
//                qDebug()<<"wendu"<<output<<QString::number(output.mid(output.indexOf("W")+1,4).toInt()/10.0,'f',1);
//                qDebug()<<"wendu"<<output<<QString::number(output.mid(output.indexOf("W")+5,3).toInt()/10.0,'f',1);
//                qDebug()<<"wendu"<<output<<QString::number(output.mid(output.indexOf("W")+8,3).toInt()/10.0,'f',1);
            }

            output.clear();
            bOk=true;
        }

        //打开夜间照明
        if(receiveDataBuffer.contains("OK"))
        {
            if(receiveDataBuffer.contains("OK1"))
            {
                qDebug()<<"led is ok";
                bOk=true;
                emit com_replay(LED_OPENED);
            }
            //关闭夜间照明
            else if(receiveDataBuffer.contains("OK2"))
            {
                qDebug()<<"led is not ok";
                bOk=true;
                emit com_replay(LED_CLOSED);
            }
            //开始轨距、超高及里程测量
            else if(receiveDataBuffer.contains("OK3"))
            {

            }
            //停止轨距、超高及里程测量
            else if(receiveDataBuffer.contains("OK4"))
            {

            }
            //里程清零
            else if(receiveDataBuffer.contains("OK5"))
            {
                walkStepCounts=0;
                preDistanceCount=0;
                bOk=true;
            }
        }
    }
    //    receiveDataBuffer.clear();
}
//函数功能：orignal表示接受到的原始数据，feature表示特征值，output返回包含了特征值数据的字符串
bool SerialPort::getParData(QString orignal,QString feature,QString &Output)
{
    if(orignal.contains(feature))
    {
        int startIndex=orignal.indexOf(feature);
        QString tmp=orignal.right(orignal.count()-startIndex);
        int endIndex=tmp.indexOf("\n");
        if(endIndex!=-1)
        {
            startIndex=tmp.indexOf(feature);
            int i=endIndex-startIndex+1;
            Output=tmp.mid(startIndex,i);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}
void SerialPort::readComData()
{
#if SYS_COM
    if(bOk)
    {
        bOk=false;
        receiveDataBuffer = serial->readAll();
    }
    else
    {
        receiveDataBuffer.append(serial->readAll());
    }
    //    if(bOk)
    //    {
    //        receiveDataBuffer = serial->readAll();
    //    }
    //    else
    //    {
    //        receiveDataBuffer.append(serial->readAll());
    //    }
    //    qDebug()<<"serial value"<<receiveDataBuffer;
#if DEBUG_COM_0428

    if(nComType==RAIL_MCU_COM)
    {
        if(file->isOpen())
        {
            QTextStream outfile(file);
            outfile<<receiveDataBuffer <<endl;
        }
    }
#endif
    //run();
    parse_comData();
#else
    char *databuffer=NULL;
    qint64 ByteCount = RS232->bytesAvailable();
    if(ByteCount)
    {
        databuffer= (char *)malloc(ByteCount );
        RS232->readData(databuffer,ByteCount);
        //        QByteArray dd(RS232->readAll());
        receiveDataBuffer=QString(databuffer).left(ByteCount);
        //        qDebug()<<"ByteCount="<<ByteCount<<receiveDataBuffer;//*/<<dd.toHex()<<dd.data();
        //        readTimerCom->start(100);
        //        run();
        parse_comData();
        //        }
        free(databuffer);
    }
#endif
}
void SerialPort::getComConfigInfo( void )
{
    //系统通过设置文件对串口号、波特率、数据位等参数进行设置
    QSettings  settings("Config.ini", QSettings::IniFormat);
#if SYS_COM
    if(nComType==RAIL_MCU_COM)
    {
        comSet.name =settings.value("ComInfo/RailCom", "COM1").toString();
        comSet.baudRate = QSerialPort::Baud115200;
    }
    else if(nComType==RADAR_COM)
    {
        comSet.name=settings.value("ComInfo/RadarCom", "COM4").toString();
        comSet.baudRate = QSerialPort::Baud115200;
    }
    else if(nComType== CLOUD_COM)
    {
        comSet.name=settings.value("ComInfo/CloudCom", "COM4").toString();
        comSet.baudRate = QSerialPort::Baud2400;
    }
    else if(nComType==TS_RAIL_COM)
    {
        comSet.name=settings.value("ComInfo/tsRailCom", "COM5").toString();
        comSet.baudRate = QSerialPort::Baud9600;
    }
    else if(nComType==TS_ANGLE_COM)
    {
        comSet.name=settings.value("ComInfo/tsAngleCom", "COM6").toString();
        comSet.baudRate = QSerialPort::Baud115200;
    }
    comSet.dataBits = QSerialPort::Data8;
    comSet.parity = QSerialPort::NoParity;
    comSet.stopBits = QSerialPort::OneStop;
    comSet.flowControl = QSerialPort::NoFlowControl;
    //                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    //    currentSettings.stringFlowControl = ui->flowControlBox->currentText();
#else
    //    settings.beginGroup("ComInfo");
    if(nComType==RAIL_MCU_COM)
    {
        //        QString baurte=settings.value("ComInfo/braudrate", "BAUD115200").toString();
        //        commInfo.braudrate=BaudRateType(braudMapToInt.find(baurte).value());
        commInfo.com=settings.value("ComInfo/RailCom", "COM1").toString();
        //        commInfo.braudrate=BAUD9600;
    }/*RailCom=COM4
            MagneticCom=COM1*/
    else if(nComType==RADAR_COM)
    {
        commInfo.com=settings.value("ComInfo/RadarCom", "COM4").toString();
    }
    commInfo.braudrate=BAUD115200;
    commInfo.stopbit=STOP_1;
    commInfo.databit=DATA_8;
    commInfo.parity=PAR_NONE;
#endif
    walkStepCounts=settings.value("system/preDisCount",0).toLongLong();
    constDiameter=settings.value("system/diameter",10).toDouble();
    constRound1Pulses=settings.value("system/roundPulses",360).toInt();
    distCoefficients=double(PI*constDiameter/constRound1Pulses/100.0);
    if(nComType==RAIL_MCU_COM)
    {
        qDebug()<<"walk initial par,pre walkStepCounts ="<<walkStepCounts<<"diameter="<<constDiameter<<"one round pulses="<<constRound1Pulses<<"distance coeifficient="<<distCoefficients<<PI/3600.0;
    }
}
void SerialPort::setPreDistanceCount(qint64 tmp)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    QVariant dd(QVariant::LongLong);
    dd.setValue(tmp);
    //    qDebug()<<"dd value"<<dd.toLongLong();
    settings.setValue("system/preDisCount",dd);
}
bool SerialPort::connectDevice( )
{
    if(m_bOpen)
    {
        return true;
    }
    else
    {
#if SYS_COM
        serial->setPortName(comSet.name);
        serial->setBaudRate(comSet.baudRate);
        serial->setDataBits(comSet.dataBits);
        serial->setParity(comSet.parity);
        serial->setStopBits(comSet.stopBits);
        serial->setFlowControl(comSet.flowControl);
        m_bOpen=serial->open(QIODevice::ReadWrite);
        //        serial->set
        timerDeviceInfo=new QTimer;
        connect(timerDeviceInfo,SIGNAL(timeout()),this,SLOT(slot_updateDevice()));
        timerDeviceInfo->start(2000);
        if(m_bOpen)
        {
            if(nComType==RAIL_MCU_COM)
            {
                qDebug()<<"rail com is ok,com name="<<serial->portName();
            }
            else if(nComType==RADAR_COM)
            {
                qDebug()<<"radar com is ok,com name="<<serial->portName();
            }
        }
        else
        {
            qDebug()<<"com is not opened";
        }
#else
        if(RS232 && RS232->isOpen())
            CommDeviceClose();
        //串口参数设置
        //        qDebug()<<"com info :"<<RS232->portName();
        RS232=new Win_QextSerialPort(commInfo.com);
        RS232->setBaudRate(commInfo.braudrate);
        RS232->setFlowControl(FLOW_OFF);
        RS232->setParity(commInfo.parity);
        RS232->setDataBits(commInfo.databit);
        RS232->setStopBits(commInfo.stopbit);
        m_bOpen=RS232->open();

        if(nComType==RAIL_MCU_COM)
        {
            qDebug()<<"rail com,com info :"<<commInfo.com
                   <<"brand"<<braudMapToString.find(RS232->baudRate()).value()<<
                     "data="<<DataBitsType(RS232->dataBits())<<
                     "stop"<<RS232->stopBits()<<
                     "partiy="<<RS232->parity();
        }
        else if(nComType==RADAR_COM)
        {
            qDebug()<<"radar com info :"<<commInfo.com
                   <<"brand"<<braudMapToString.find(RS232->baudRate()).value()<<
                     "data="<<DataBitsType(RS232->dataBits())<<
                     "stop"<<RS232->stopBits()<<
                     "partiy="<<RS232->parity();
        }
        //        if(nComType==1)
        {
            readTimerCom->start(100);
        }
        //        qDebug()<<"comm name= rador "<<commInfo.com<<"connect status="<<m_bOpen;
        //        if(nComType==1)
        {
            //            readTimerCom->start(100);
        }
#endif
    }
    return m_bOpen;
}
bool  SerialPort::openStatus( void)
{
    return (m_bOpen);
}
bool SerialPort::comClose()
{
#if SYS_COM
    if (serial->isOpen())
        serial->close();
#else
    RS232->close();
#endif
    m_bOpen=false;
    return true;
}
void  SerialPort::CommDeviceClose( )
{
#if SYS_COM
    comClose();
#else
    if(RS232->isOpen() )
    {
        if( readTimerCom->isActive())
            readTimerCom->stop();
        RS232->close();

    }
    else
        return;
#endif
}
void SerialPort::SendCommString(QString strSend)
{
#if SYS_COM
    //    serial.isBreakEnabled();
    serial->write(strSend.toLocal8Bit());
#else
    if(RS232->isOpen()&&RS232)
        RS232->writeData(strSend.toLocal8Bit().constData(),strSend.toLocal8Bit().count());
#endif
}
SerialPort::~SerialPort(void)
{
    //    setPreDistanceCount(walkDistance);
#if DEBUG_COM_0428
    if(file)
        if(file->isOpen())
            file->close();
#endif
}

//函数功能:获取当前系统的所有com口，并返回串口信息
QStringList SerialPort::getComm(void)
{
    QSettings reg(QString("HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM\\"),QSettings::NativeFormat);
    //    qDebug()<<"comm"<<reg.allKeys();
    QStringList key = reg.allKeys();
    int num = (int)key.size();
    QString value;
    QStringList listCommName;
    for(int i=0; i<num; i++)
    {
        value = MakeSingleCom(i, "value");
        listCommName<<value;
    }
    //    qDebug()<<"listCommName"<<listCommName;
    return listCommName;
}
//函数功能：读取注册表，获取单个串口的具体信息
QString SerialPort::MakeSingleCom(int index,QString keyorvalue)
//index为索引号，keyorvalue为选择返回值应该为key还是value
{
    QString commresult;
#if 0

    HKEY hKey;
    wchar_t keyname[256]; //键名数组
    char keyvalue[256];  //键值数组
    DWORD keysize,type,valuesize;
    int indexnum;

    if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),0,KEY_READ,&hKey)!=0)
    {
        QString error="Cannot open regedit!";//无法打开注册表时返回error
        return error;
    }//if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ,&hKey)!=0)
    QString keymessage,message,valuemessage;//键名,,键值
    indexnum = index;//要读取键值的索引号
    keysize=sizeof(keyname);
    valuesize=sizeof(keyvalue);
    if(::RegEnumValue(hKey,indexnum,keyname,&keysize,0,&type,(BYTE*)keyvalue,&valuesize)==0)//列举键名和值
    {
        //读取键名
        for(int i=0;i<keysize;i++)
        {
            message=keyname[i];
            keymessage.append(message);
        }
        //读取键值
        for(int j=0;j<valuesize;j++)
        {
            if(keyvalue[j]!=0x00)
                valuemessage.append(keyvalue[j]);
        }
        if(keyorvalue=="key")
            commresult=keymessage;
        else if(keyorvalue=="value")
            commresult=valuemessage;
    }
    else
    {
        commresult="nokey";
    }
    ::RegCloseKey(hKey);//关闭注册表
#endif
    return commresult;
}
void SerialPort:: parse_RailSensor(QString strBuffer)
{
    //08-10 上海
    //T+14140000+00471
    //      T-00060016+17853
    //angle  width distance
    //T+0083 064   10818
    //7,8
    //qDebug()<<"get rail sensor"<<    strBuffer<<strBuffer.mid(1,5)<<strBuffer.mid(6,3)<<strBuffer.mid(9,5);
    //qDebug()<<"count="<<strBuffer.mid(9,5).count()<<strBuffer.mid(6,3).count()<<strBuffer.mid(1,5).count();
    //    qDebug()<<"value"<<strBuffer;
    if(strBuffer.count()<16)
        return;
    curDistanceCount=abs(strBuffer.mid(10,6).toInt());
    //    qDebug()<<""
    if(bFirstWalk)
    {
        preDistanceCount=curDistanceCount;
        bFirstWalk=false;
        return;
    }
    else
    {
        int tmp=curDistanceCount-preDistanceCount;
        if(abs(tmp)>=MAXABS_DISTANCE)
        {
            if(tmp>0)
            {
                tmp-=MAX_DISTANCE;
            }
            else
            {
                tmp+=MAX_DISTANCE;
            }
//            qDebug()<<"see flag";
            //        curDistanceCount+=walkStepCounts;
        }
        tmp*=directFlag;
#if DEBUG_COM_0428
        static  int preWalk=0;
        preWalk=walkStepCounts;
#endif
        walkStepCounts+=tmp;
        //        walkStepCounts*=directFlag;
        //    qDebug()<<"rail com data:"<<strBuffer.mid(1,5)<<strBuffer.mid(6,4)<<strBuffer.mid(10,6);
        //检查数据的完整性
        //    if((strBuffer.mid(10,6).count()!=6)|(strBuffer.mid(6,4).count()!=3)|(strBuffer.mid(1,5).count()!=5))
        //    {
        //        qDebug()<<"std distance=5 "<<strBuffer.mid(1,5)<<"3="<<QString(strBuffer.mid(6,3))<<"5="<<QString(strBuffer.mid(9,5));
        //        return;
        //    }
        //        qDebug()<<"distance tmp"<<walkStepCounts<<curDistanceCount<<preDistanceCount<<tmp;
        m_comDataType.walkDistance=QString::number(walkStepCounts*distCoefficients,'f',2).toFloat();//mm
        //    printf(m_comDataType.walkDistance,"%.1f",walkStepCounts*PI/36.0);
        m_comDataType.angle=strBuffer.mid(1,5).toInt()/100.0;
        //            qDebug()<<"distance value"<<distCoefficients*walkStepCounts<<m_comDataType.walkDistance;
        curRailWidth=strBuffer.mid(6,4).toInt();
        //    QString sDistance=strBuffer.mid(10,6);

        //2015-07-12:解决了里程溢出的问题.

        //        m_comDataType.walkDistance=nDistance*PI/360/*/1000.000*/;//米 变 公里
        m_comDataType.nRailHeight=curRailWidth*sin(m_comDataType.angle*3.1415/180)+nRailHeightC;
        m_comDataType.nRailWidth=curRailWidth*cos(m_comDataType.angle*3.1415/180)+nRailWidthC;

#if SYS_COM

#else
        readTimerCom->stop();
#endif
#if DEBUG_COM_0428
        qDebug()<<"rail data run here"<<preDistanceCount<<curDistanceCount<<tmp<<walkStepCounts<< walkStepCounts-preWalk;
#endif
        //qDebug()<<"rail data"<<m_comDataType.nRailHeight<<m_comDataType.nRailWidth<<m_comDataType.walkDistance;
        preDistanceCount=curDistanceCount;

        setPreDistanceCount(walkStepCounts);
        bOk=true;
        //        receiveDataBuffer .clear();
        //        qDebug()<<"walk"<<m_comDataType.walkDistance;
        emit getComData(m_comDataType);
        emit baseAngle(m_comDataType.angle);
    }
}
void SerialPort::setDirectionFlag(int tmp)
{
    directFlag=tmp;
    //    qDebug()<<"++++++++++++++++++++++++directFlag"<<directFlag;
}
void SerialPort::railMeasure(void)
{
    //    if(nComType==1)
    //    {
    //        clearDistance();
    //    }
    //    qDebug()<<"--------------rail distance";
#if SYS_COM
#else
    readTimerCom->start(200);
#endif
}
void SerialPort::slot_baseRailParChanged()
{
    readBasePar();
}
void SerialPort::readBasePar()
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    nRailHeightC=settings.value("initialPar/RailHeightC").toInt();
    nRailWidthC=settings.value("initialPar/RailWidthC").toInt();
    nSideWidthConst=settings.value("initialPar/sideWidthConst").toFloat();
    nSideDisConst=settings.value("initialPar/sideDisConst").toInt();

    //    qDebug()<<"read"<<
    //    qDebug()<<"base par is changed"<<nSideDisConst<<nSideWidthConst<<nRailHeightC<<nRailWidthC;
}
void SerialPort::radarOn(void)
{
#if ZHENG_RADAR_COM
    if(serial->isOpen())
    {
        qDebug()<<"radar power on"<<comSet.name;
    }
    else
    {
        qDebug()<<"radar power off"<<comSet.name;
    }
//    SendCommString("XJXT_Q2_1_end\r\n");
    char p[30]="XJXT_Q2_1_end";
    p[13]=0x0d;
    p[14]=0x0a;
    p[15]=0;
    serial->write(p,15);
#endif
}
void SerialPort::radarOff(void)
{
#if ZHENG_RADAR_COM
    qDebug()<<"radar power off"<<serial->isOpen();
//    SendCommString("XJXT_Q2_0_end\r\n");

    char p[30]="XJXT_Q2_0_end";
    p[13]=0x0d;
    p[14]=0x0a;
    p[15]=0;
    serial->write(p,15);
#endif
}
void SerialPort::wifiPower(bool f)
{
    if(f)
    {
        //打开wifi电源
        SendCommString("XJXT_Q1_1_end\r\n");
    }
    else
    {
        //关闭wifi电源
        SendCommString("XJXT_Q1_0_end\r\n");
    }
}
void SerialPort::startRail(void)
{
#if ZHENG_RAIL_COM
    char datbuffer[]="XJXT_GJ_P_end\r\n";
    RS232->writeData(datbuffer,15);
#endif
#if LIU_RAIL_COM
    SendCommString("C103\r\n");
#endif
}
void SerialPort::stopRail(void)
{
#if ZHENG_RAIL_COM
    char datbuffer[]="XJXT_GJ_S_end\r\n";
    RS232->writeData(datbuffer,15);
#endif
#if LIU_RAIL_COM
    SendCommString("C104\r\n");
#endif
}
void SerialPort::clearDistance(void)
{
#if ZHENG_RAIL_COM
    //    char datbuffer[]="CLR\n";
    //    RS232->writeData(datbuffer,4);
    char datbuffer[]="XJXT_GJ_0_end\r\n";
    RS232->writeData(datbuffer,15);
#endif
#if LIU_RAIL_COM
    SendCommString("C105\r\n");
#endif
}
void SerialPort::slot_deviceControl(COM_COMMAND_TYPE commandType)
{
    //    qDebug()<<"comand type="<<commandType;
    if(commandType==CLOSE_RADAR)
    {
        radarOff();
    }
    else if(commandType==OPEN_RADAR)
    {
        radarOn();
    }
    else if(commandType==OPEN_WIFI)
    {
        wifiPower(true);
    }
    else if(commandType==CLOSE_WIFI)
    {
        wifiPower(false);
    }
    else if(commandType==CLEAR_WALK)
    {
        clearDistance();
    }
    else if(commandType==OPEN_LED)
    {
#if SYS_COM
        //        qDebug()<<"open led"<<readTimerCom->isActive();
#else
        qDebug()<<"open led"<<RS232->isOpen()<<readTimerCom->isActive();
#endif
        SendCommString("C101\r\n");
    }
    else if(commandType==CLOSE_LED)
    {
        //        qDebug()<<"close led";
        SendCommString("C102\r\n");
    }
    //    else if(commandType.contains("updateStatus"))
    {
    }
}
void SerialPort::enumToString (void)
{
    QString cc("BAUD50;BAUD75;BAUD110;BAUD134;BAUD150;\
               BAUD200;BAUD300;BAUD600;BAUD1200;BAUD1800;\
    BAUD2400;BAUD4800;BAUD9600;BAUD14400;\
    BAUD19200;BAUD38400;BAUD56000;BAUD57600;\
    BAUD76800;BAUD115200;BAUD128000;BAUD256000");
            for(int i=0;i<cc.split(";").count();i++)
    {
        braudMapToInt.insert(cc.split(";").at(i).trimmed(),i);
        braudMapToString.insert(i,cc.split(";").at(i).trimmed());
    }
    databitsMapToInt.insert("DATA4",0);
    databitsMapToInt.insert("DATA5",1);
    databitsMapToInt.insert("DATA6",2);
    databitsMapToInt.insert("DATA7",3);
    databitsMapToInt.insert("DATA8",4);
    databitsMapToString.insert(0,"DATA4");
    databitsMapToString.insert(1,"DATA5");
    databitsMapToString.insert(2,"DATA6");
    databitsMapToString.insert(3,"DATA7");
    databitsMapToString.insert(4,"DATA8");
}
void SerialPort::slot_updateDevice(void)
{
    emit updateDeviceInfos(curDeviceInfo);
}
void SerialPort::initialDistance(void)
{
    bFirstWalk=true;
}
