#include "mainwindow.h"
#include <QApplication>
#include <qdebug.h>
#include <qtgui>
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QTextCodec *xcodec = QTextCodec::codecForLocale() ;
    QString exeDir = xcodec->toUnicode( QByteArray(argv[0]) ) ;
    QString BKE_CURRENT_DIR( "C:\Qt\Qt5.8.0\5.8\msvc2015\plugins\platforms");

    BKE_CURRENT_DIR = QFileInfo( exeDir ).path() ;
    //    qDebug()<<"thread"<<this->thread()->currentThread();
    QApplication::setLibraryPaths( QApplication::libraryPaths() << BKE_CURRENT_DIR) ;
    MainWindow w;
    w.show();
    return a.exec();
}
